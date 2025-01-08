#include "cbase.h"
#include "viewrender_deferred_helper.h"

#include "tier0/valve_minmax_off.h"
#include <vector>
#include "tier0/valve_minmax_on.h"
#include "winlite.h"
#undef GetObject
#define SUBHOOK_STATIC

#ifdef __linux__
#include "../subhook/subhook/subhook.h"
#include <dlfcn.h>
#include <libgen.h>
#else
#include "subhook.h"
#include "Psapi.h"
#pragma comment(lib, "Psapi.lib")
#endif

#include <xmmintrin.h>
#include <immintrin.h>

#include "tier0/memdbgon.h"

namespace Memory
{
	struct BytePattern
	{
		struct Entry
		{
			uint8 Value;
			bool Unknown;
		};

		std::vector<Entry> Bytes;
	};

	BytePattern GetPatternFromString( const char* input )
	{
		BytePattern ret;

		while ( *input )
		{
			if ( *input == ' ' )
			{
				++input;
			}

			if ( isxdigit( *input ) )
			{
				ret.Bytes.push_back( { static_cast<unsigned char>(strtol( input, NULL, 16 )), false } );

				input += 2;
			}
			else
			{
				ret.Bytes.push_back( { 0, true } );
				input += 2;
			}
		}

		return ret;
	}

	/*
	Not accessing the STL iterators in debug mode makes this run >10x faster, less sitting around waiting for nothing.
	*/
	inline bool DataCompare( const uint8* data, const BytePattern::Entry* pattern, size_t patternlength )
	{
		uint index = 0;

		for ( size_t i = 0; i < patternlength; i++ )
		{
			const BytePattern::Entry& byte = *pattern;

			if ( !byte.Unknown && *data != byte.Value )
			{
				return false;
			}

			++data;
			++pattern;
			++index;
		}

		return index == patternlength;
	}

	void* FindPattern( void* start, size_t searchlength, const BytePattern& pattern )
	{
		const BytePattern::Entry* patternstart = pattern.Bytes.data();
		const size_t length = pattern.Bytes.size();

		for ( size_t i = 0; i <= searchlength - length; ++i )
		{
			const uint8* addr = static_cast<const uint8*>( start ) + i;

			if ( DataCompare( addr, patternstart, length ) )
			{
				return static_cast<void*>( const_cast<uint8*>( addr ) );
			}
		}

		return NULL;
	}
}

// taken from https://github.com/momentum-mod/game
#ifdef __linux__
int GetModuleInformation( const char *name, void **base, size_t *length )
{
	// this is the only way to do this on linux, lol
	FILE *f = fopen( "/proc/self/maps", "r" );
	if (!f)
		return 1;

	char buf[PATH_MAX+100];
	while ( !feof( f ) )
	{
		if ( !fgets( buf, sizeof( buf ), f ) )
			break;

		char *tmp = strrchr( buf, '\n' );
		if ( tmp )
			*tmp = '\0';

		char *mapname = strchr( buf, '/' );
		if ( !mapname )
			continue;

		char perm[5];
		unsigned long begin, end;
		sscanf( buf, "%lx-%lx %4s", &begin, &end, perm );

		if ( strcmp( basename( mapname ), name ) == 0 && perm[0] == 'r' && perm[2] == 'x' )
		{
			*base = (void*)begin;
			*length = (size_t)end-begin;
			fclose( f );
			return 0;
		}
	}

	fclose( f );
	return 2;
}
#endif

namespace helper
{
	bool bDisableDecalRendering = false;
}

static subhook::Hook* DecalSurfaceDrawHook = NULL;
void( *DecalSurfaceDrawOriginal )( IMatRenderContext* pRenderContext, int renderGroup, float a3 );
void DecalSurfaceDraw( IMatRenderContext *pRenderContext, int renderGroup, float a3 )
{
	if ( helper::bDisableDecalRendering )
		return;
	return DecalSurfaceDrawOriginal( pRenderContext, renderGroup, a3 );
}

static subhook::Hook* DispInfo_DrawDecalsGroupHook = NULL;
void (*DispInfo_DrawDecalsGroupOriginal)( int iGroup, int iTreeType );
void DispInfo_DrawDecalsGroup( int iGroup, int iTreeType )
{
	if ( helper::bDisableDecalRendering )
		return;
	return DispInfo_DrawDecalsGroupOriginal( iGroup, iTreeType );
}

namespace LightmapHook
{
	using lightmap_alpha_t = uint8_t[4];
	using blocklights_t = Vector4D[MAX_LIGHTMAP_DIM_INCLUDING_BORDER * MAX_LIGHTMAP_DIM_INCLUDING_BORDER];

	blocklights_t* blocklights;

	static subhook::Hook* AccumulateLightstylesHook = nullptr;
	static void AccumulateLightstyles(ColorRGBExp32* pLightmap, int lightmapSize, float scalar)
	{
		lightmap_alpha_t* pSunAmount = reinterpret_cast<lightmap_alpha_t*>(pLightmap + lightmapSize);

		for (int i = 0; i < lightmapSize; ++i)
		{
			blocklights[0][i][0] += scalar * TexLightToLinear(pLightmap[i].r, pLightmap[i].exponent);
			blocklights[0][i][1] += scalar * TexLightToLinear(pLightmap[i].g, pLightmap[i].exponent);
			blocklights[0][i][2] += scalar * TexLightToLinear(pLightmap[i].b, pLightmap[i].exponent);
			blocklights[0][i][3] += scalar * pSunAmount[i][0] / 255.0f;
		}
	}

	static subhook::Hook* AccumulateBumpedLightstylesHook = nullptr;
	static void AccumulateBumpedLightstyles(ColorRGBExp32* pLightmap, int lightmapSize, float scalar)
	{
		ColorRGBExp32* pBumpedLightmaps[3];
		pBumpedLightmaps[0] = pLightmap + lightmapSize;
		pBumpedLightmaps[1] = pLightmap + 2 * lightmapSize;
		pBumpedLightmaps[2] = pLightmap + 3 * lightmapSize;

		lightmap_alpha_t* pSunAmount = reinterpret_cast<lightmap_alpha_t*>(pLightmap + 4 * lightmapSize);

		for (int j = 0; j < NUM_BUMP_VECTS + 1; j++)
		{
			for (int i = 0; i < lightmapSize; ++i)
			{
				blocklights[j][i][0] += scalar * TexLightToLinear(pLightmap[i].r, pLightmap[i].exponent);
				blocklights[j][i][1] += scalar * TexLightToLinear(pLightmap[i].g, pLightmap[i].exponent);
				blocklights[j][i][2] += scalar * TexLightToLinear(pLightmap[i].b, pLightmap[i].exponent);
				blocklights[j][i][3] += scalar * pSunAmount[i][j] / 255.0f;
			}
		}
	}
}

static class AutoHook : public CAutoGameSystem
{
public:
	bool Init() OVERRIDE
	{
		using namespace Memory;
#ifdef WIN32
		CSysModule* engineDll = Sys_LoadModule( "engine" DLL_EXT_STRING );
		const BytePattern& decalSurfaceDrawPattern			= GetPatternFromString( "55 8B EC A1 ?? ?? ?? ?? 83 EC 24" );
		const BytePattern& dispInfo_DrawDecalsGroupPattern	= GetPatternFromString( "55 8B EC 81 EC ?? ?? ?? ?? 8B 0D ?? ?? ?? ?? 56 8B 01 FF 90 ?? ?? ?? ?? 8B F0 89 75 E0" );
		MODULEINFO info;
		GetModuleInformation( GetCurrentProcess(), reinterpret_cast<HMODULE>( engineDll ), &info, sizeof( info ) );
#define FUNC_FROM_PATTERN( name )	void* name##Func = FindPattern( info.lpBaseOfDll, info.SizeOfImage, name##Pattern )
#elif __linux__
		const BytePattern& decalSurfaceDrawPattern			= GetPatternFromString( "55 89 E5 57 56 53 31 DB 83 EC ?? C7 45 ?? ?? ?? ?? ?? A1 ?? ?? ?? ?? C7 45 ?? ?? ?? ?? ?? 8B 75 ?? C7 45 ?? ?? ?? ?? ?? 8B 7D ??" );
		const BytePattern& dispInfo_DrawDecalsGroupPattern	= GetPatternFromString( "55 89 E5 57 56 53 81 EC ?? ?? ?? ?? A1 ?? ?? ?? ?? 8B 5D ?? 8B 10 89 04 24 FF 92 ?? ?? ?? ?? 85 C0 89 85 ?? ?? ?? ??" );
		void* pBase;
		size_t length;
		GetModuleInformation( "engine" DLL_EXT_STRING, &pBase, &length );
#define FUNC_FROM_PATTERN( name )	void* name##Func = FindPattern( pBase, length, name##Pattern )
#else
	#error	"Not supported platform!"
#endif

		FUNC_FROM_PATTERN( decalSurfaceDraw );
		FUNC_FROM_PATTERN( dispInfo_DrawDecalsGroup );

#undef FUNC_FROM_PATTERN

#define HOOK_FUNC( funcAddr, funcRepl ) { funcRepl##Hook = new subhook::Hook( funcAddr##Func, reinterpret_cast<void*>( &funcRepl ) ); Msg("Hooking " V_STRINGIFY( funcRepl ) " %ssuccessful\n", funcRepl##Hook->Install() ? "" : "un" ); *reinterpret_cast<void**>( &funcRepl##Original ) = funcRepl##Hook->GetTrampoline(); }

		HOOK_FUNC( decalSurfaceDraw, DecalSurfaceDraw );
		HOOK_FUNC( dispInfo_DrawDecalsGroup, DispInfo_DrawDecalsGroup );

#undef HOOK_FUNC

		// it is expected not able to load the map after this...
		// or maybe, but with broken lightmaps...
		{
			auto engineBase = reinterpret_cast<uint8_t*>(engineDll);
			LightmapHook::AccumulateLightstylesHook = new subhook::Hook(engineBase + 0xF2690, LightmapHook::AccumulateLightstyles);
			LightmapHook::AccumulateBumpedLightstylesHook = new subhook::Hook(engineBase + 0xF2430, LightmapHook::AccumulateBumpedLightstyles);

			if (LightmapHook::AccumulateLightstylesHook->Install() &&
				LightmapHook::AccumulateBumpedLightstylesHook->Install())
			{
				Msg("successfully installed lightmap hooks\n");
			}
		}

	
		//g_WorldStaticMeshes = reinterpret_cast<CUtlVector<IMesh*>*>(reinterpret_cast<byte*>( engineDll ) + 0x5AD2A8);

		return true;
	}

	void Shutdown() OVERRIDE
	{
		delete DecalSurfaceDrawHook;
		delete DispInfo_DrawDecalsGroupHook;
	}
} autoHook;
