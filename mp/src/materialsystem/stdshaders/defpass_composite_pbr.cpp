#include "deferred_includes.h"

// Includes for PS30
#include "composite_pbr_vs30.inc"
#include "composite_pbr_ps30.inc"

// Defining samplers
const Sampler_t SAMPLER_BASETEXTURE = SHADER_SAMPLER0;
const Sampler_t SAMPLER_NORMAL = SHADER_SAMPLER1;
const Sampler_t SAMPLER_ENVMAP = SHADER_SAMPLER2;
const Sampler_t SAMPLER_LIGHTACCUM = SHADER_SAMPLER3;
const Sampler_t SAMPLER_LIGHTMAP = SHADER_SAMPLER4;
const Sampler_t SAMPLER_MRAO = SHADER_SAMPLER5;
const Sampler_t SAMPLER_EMISSIVE = SHADER_SAMPLER6;
const Sampler_t SAMPLER_SPECULAR = SHADER_SAMPLER7;

// Convars
extern ConVar mat_fullbright;
static ConVar mat_specular( "mat_specular", "1", FCVAR_CHEAT );
static ConVar mat_pbr_parallaxmap( "mat_pbr_parallaxmap", "1" );

void InitParmsCompositePBR( const PBR_Vars_t &info, CBaseVSShader *pShader, IMaterialVar **params )
{
    // Set a good default mrao texture
    // if ( !params[info.mraoTexture]->IsDefined() ) params[info.mraoTexture]->SetStringValue( "dev/pbr_mraotexture" );

    // PBR relies heavily on envmaps
    // if ( !params[info.envMap]->IsDefined() ) params[info.envMap]->SetStringValue( "env_cubemap" );

    // Check if the hardware supports flashlight border color
    if ( g_pHardwareConfig->SupportsBorderColor() )
    {
        params[FLASHLIGHTTEXTURE]->SetStringValue( "effects/flashlight_border" );
    }
    else
    {
        params[FLASHLIGHTTEXTURE]->SetStringValue( "effects/flashlight001" );
    }
}

void InitPassCompositePBR( const PBR_Vars_t &info, CBaseVSShader *pShader, IMaterialVar **params )
{
    Assert( info.envMap >= 0 );
    int envMapFlags = g_pHardwareConfig->GetHDRType() == HDR_TYPE_NONE ? TEXTUREFLAGS_SRGB : 0;
    envMapFlags |= TEXTUREFLAGS_ALL_MIPS;
    pShader->LoadCubeMap( info.envMap, envMapFlags );

    if ( info.emissionTexture >= 0 && PARM_DEFINED( info.emissionTexture ) )
        pShader->LoadTexture( info.emissionTexture, TEXTUREFLAGS_SRGB );

    Assert( info.mraoTexture >= 0 );
    pShader->LoadTexture( info.mraoTexture, 0 );

    if ( PARM_DEFINED( info.baseTexture ) )
    {
        pShader->LoadTexture( info.baseTexture, TEXTUREFLAGS_SRGB );
    }

    if ( PARM_DEFINED( info.specularTexture ) )
    {
        pShader->LoadTexture( info.specularTexture, TEXTUREFLAGS_SRGB );
    }

    if ( info.bModel )  // Set material var2 flags specific to models
    {
        SET_FLAGS2( MATERIAL_VAR2_SUPPORTS_HW_SKINNING );            // Required for skinning
        SET_FLAGS2( MATERIAL_VAR2_DIFFUSE_BUMPMAPPED_MODEL );        // Required for dynamic lighting
        SET_FLAGS2( MATERIAL_VAR2_NEEDS_TANGENT_SPACES );            // Required for dynamic lighting
        SET_FLAGS2( MATERIAL_VAR2_LIGHTING_VERTEX_LIT );             // Required for dynamic lighting
        SET_FLAGS2( MATERIAL_VAR2_NEEDS_BAKED_LIGHTING_SNAPSHOTS );  // Required for ambient cube
    }
    else  // Set material var2 flags specific to brushes
    {
        SET_FLAGS2( MATERIAL_VAR2_LIGHTING_LIGHTMAP );         // Required for lightmaps
        SET_FLAGS2( MATERIAL_VAR2_LIGHTING_BUMPED_LIGHTMAP );  // Required for lightmaps
    }
}

void DrawPassCompositePBR( const PBR_Vars_t &info, CBaseVSShader *pShader, IMaterialVar **params,
                           IShaderShadow *pShaderShadow, IShaderDynamicAPI *pShaderAPI,
                           VertexCompressionType_t vertexCompression,
                           CDeferredPerMaterialContextData *pDeferredContext )
{
    // Setting up booleans
    const bool bModel = info.bModel;
    const bool bHasBaseTexture = ( info.baseTexture != -1 ) && params[info.baseTexture]->IsTexture();
    const bool bHasNormalTexture = ( info.bumpMap != -1 ) && params[info.bumpMap]->IsTexture();
    const bool bHasMraoTexture = ( info.mraoTexture != -1 ) && params[info.mraoTexture]->IsTexture();
    const bool bHasEmissionTexture = ( info.emissionTexture != -1 ) && params[info.emissionTexture]->IsTexture();
    const bool bHasEnvTexture = ( info.envMap != -1 ) && params[info.envMap]->IsTexture();
    const bool bIsAlphaTested = IS_FLAG_SET( MATERIAL_VAR_ALPHATEST ) != 0;
    const bool bHasColor = ( info.baseColor != -1 ) && params[info.baseColor]->IsDefined();
    const bool bLightMapped = !info.bModel;
    const bool bUseEnvAmbient = ( info.useEnvAmbient != -1 ) && ( params[info.useEnvAmbient]->GetIntValue() == 1 );
    const bool bSpecular = !mat_specular.GetBool();
    const bool bTranslucent = IS_FLAG_SET( MATERIAL_VAR_TRANSLUCENT );

    // Determining whether we're dealing with a fully opaque material
    BlendType_t nBlendType = pShader->EvaluateBlendRequirements( info.baseTexture, true );
    bool bFullyOpaque = ( nBlendType != BT_BLENDADD ) && ( nBlendType != BT_BLEND ) && !bIsAlphaTested && !bTranslucent;

    if ( pShader->IsSnapshotting() )
    {
        // If alphatest is on, enable it
        pShaderShadow->EnableAlphaTest( bIsAlphaTested );

        if ( info.alphaTestReference != -1 && params[info.alphaTestReference]->GetFloatValue() > 0.0f )
        {
            pShaderShadow->AlphaFunc( SHADER_ALPHAFUNC_GEQUAL, params[info.alphaTestReference]->GetFloatValue() );
        }

        // Setting up samplers
        pShaderShadow->EnableTexture( SAMPLER_BASETEXTURE, true );   // Basecolor texture
        pShaderShadow->EnableSRGBRead( SAMPLER_BASETEXTURE, true );  // Basecolor is sRGB
        pShaderShadow->EnableTexture( SAMPLER_EMISSIVE, true );      // Emission texture
        pShaderShadow->EnableSRGBRead( SAMPLER_EMISSIVE, true );     // Emission is sRGB
        pShaderShadow->EnableTexture( SAMPLER_LIGHTACCUM, true );    // Lightaccum texture
        pShaderShadow->EnableSRGBRead( SAMPLER_LIGHTACCUM, false );  // Lightaccum aren't sRGB
        pShaderShadow->EnableTexture( SAMPLER_LIGHTMAP, true );      // Lightmap texture
        pShaderShadow->EnableSRGBRead( SAMPLER_LIGHTMAP, false );    // Lightmaps aren't sRGB
        pShaderShadow->EnableTexture( SAMPLER_MRAO, true );          // MRAO texture
        pShaderShadow->EnableSRGBRead( SAMPLER_MRAO, false );        // MRAO isn't sRGB
        pShaderShadow->EnableTexture( SAMPLER_NORMAL, true );        // Normal texture
        pShaderShadow->EnableSRGBRead( SAMPLER_NORMAL, false );      // Normals aren't sRGB
        pShaderShadow->EnableTexture( SAMPLER_SPECULAR, true );      // Specular F0 texture
        pShaderShadow->EnableSRGBRead( SAMPLER_SPECULAR, true );     // Specular F0 is sRGB

        // Setting up envmap
        if ( bHasEnvTexture )
        {
            pShaderShadow->EnableTexture( SAMPLER_ENVMAP, true );  // Envmap
            if ( g_pHardwareConfig->GetHDRType() == HDR_TYPE_NONE )
            {
                pShaderShadow->EnableSRGBRead( SAMPLER_ENVMAP, true );  // Envmap is only sRGB with HDR disabled?
            }
        }

        // Enabling sRGB writing
        // See common_ps_fxc.h line 349
        // PS2b shaders and up write sRGB
        pShaderShadow->EnableSRGBWrite( true );

        if ( bModel )
        {
            // We only need the position and surface normal
            // and... static prop lighting
            unsigned int flags = VERTEX_POSITION | VERTEX_NORMAL | VERTEX_FORMAT_COMPRESSED;
            // We need three texcoords, all in the default float2 size
            pShaderShadow->VertexShaderVertexFormat( flags, 1, 0, 0 );
        }
        else
        {
            // We need the position, surface normal, and vertex compression format
            unsigned int flags = VERTEX_POSITION | VERTEX_NORMAL;
            // We only need one texcoord, in the default float2 size
            pShaderShadow->VertexShaderVertexFormat( flags, 3, 0, 0 );
        }

        int useParallax = params[info.useParallax]->GetIntValue();
        if ( !mat_pbr_parallaxmap.GetBool() )
        {
            useParallax = 0;
        }

        // Setting up static vertex shader
        DECLARE_STATIC_VERTEX_SHADER( composite_pbr_vs30 );
        SET_STATIC_VERTEX_SHADER_COMBO( LIGHTMAPPED, bLightMapped ? 1 : 0 );
        SET_STATIC_VERTEX_SHADER( composite_pbr_vs30 );

        // Setting up static pixel shader
        DECLARE_STATIC_PIXEL_SHADER( composite_pbr_ps30 );
        SET_STATIC_PIXEL_SHADER_COMBO( LIGHTMAPPED, bLightMapped ? 1 : 0 );
        SET_STATIC_PIXEL_SHADER_COMBO( BUMPED, bHasNormalTexture );
        SET_STATIC_PIXEL_SHADER_COMBO( USEENVAMBIENT, bUseEnvAmbient );
        SET_STATIC_PIXEL_SHADER_COMBO( EMISSIVE, bHasEmissionTexture );
        SET_STATIC_PIXEL_SHADER_COMBO( PARALLAXOCCLUSION, useParallax );
        SET_STATIC_PIXEL_SHADER( composite_pbr_ps30 );

        // Setting up fog
        pShader->DefaultFog();  // I think this is correct

        // HACK HACK HACK - enable alpha writes all the time so that we have them for underwater stuff
        pShaderShadow->EnableAlphaWrites( bFullyOpaque );
    }
    else  // Not snapshotting -- begin dynamic state
    {
        LightState_t lightState = { 0, false, false, false };
        pShaderAPI->GetDX9LightState( &lightState );

        bool bLightingOnly = mat_fullbright.GetInt() == 2 && !IS_FLAG_SET( MATERIAL_VAR_NO_DEBUG_OVERRIDE );

        // Setting up albedo texture
        if ( bHasBaseTexture )
        {
            pShader->BindTexture( SAMPLER_BASETEXTURE, info.baseTexture, info.baseTextureFrame );
        }
        else
        {
            pShaderAPI->BindStandardTexture( SAMPLER_BASETEXTURE, TEXTURE_GREY );
        }

        // Setting up vmt color
        Vector color;
        if ( bHasColor )
        {
            params[info.baseColor]->GetVecValue( color.Base(), 3 );
        }
        else
        {
            color = Vector{ 1.f, 1.f, 1.f };
        }

        // g_BaseColor
        pShaderAPI->SetPixelShaderConstant( 4, color.Base() );

        // Setting up environment map
        if ( bHasEnvTexture )
        {
            pShader->BindTexture( SAMPLER_ENVMAP, info.envMap, 0 );
        }
        else
        {
            pShaderAPI->BindStandardTexture( SAMPLER_ENVMAP, TEXTURE_BLACK );
        }

        // Setting up emissive texture
        if ( bHasEmissionTexture )
        {
            pShader->BindTexture( SAMPLER_EMISSIVE, info.emissionTexture, 0 );
        }
        else
        {
            pShaderAPI->BindStandardTexture( SAMPLER_EMISSIVE, TEXTURE_BLACK );
        }

        // Setting up normal map
        if ( bHasNormalTexture )
        {
            pShader->BindTexture( SAMPLER_NORMAL, info.bumpMap );
        }
        else
        {
            pShaderAPI->BindStandardTexture( SAMPLER_NORMAL, TEXTURE_NORMALMAP_FLAT );
        }

        // Setting up mrao map
        if ( bHasMraoTexture )
        {
            pShader->BindTexture( SAMPLER_MRAO, info.mraoTexture, 0 );
        }
        else
        {
            pShaderAPI->BindStandardTexture( SAMPLER_MRAO, TEXTURE_WHITE );
        }

        pShader->BindTexture( SAMPLER_SPECULAR, GetDeferredExt()->GetTexture_SpecularRoughness() );

        // Getting fog info
        MaterialFogMode_t fogType = pShaderAPI->GetSceneFogMode();
        int fogIndex = ( fogType == MATERIAL_FOG_LINEAR_BELOW_FOG_Z ) ? 1 : 0;

        // Getting skinning info
        int numBones = pShaderAPI->GetCurrentNumBones();

        // Some debugging stuff
        bool bWriteDepthToAlpha = false;
        bool bWriteWaterFogToAlpha = false;
        if ( bFullyOpaque )
        {
            bWriteDepthToAlpha = pShaderAPI->ShouldWriteDepthToDestAlpha();
            bWriteWaterFogToAlpha = ( fogType == MATERIAL_FOG_LINEAR_BELOW_FOG_Z );
            AssertMsg( !( bWriteDepthToAlpha && bWriteWaterFogToAlpha ),
                       "Can't write two values to alpha at the same time." );
        }

        float vEyePos_SpecExponent[4];
        pShaderAPI->GetWorldSpaceCameraPosition( vEyePos_SpecExponent );

        // Determining the max level of detail for the envmap
        int iEnvMapLOD = 6;
        auto envTexture = params[info.envMap]->GetTextureValue();
        if ( envTexture )
        {
            // Get power of 2 of texture width
            int width = envTexture->GetMappingWidth();
            int mips = 0;
            while ( width >>= 1 ) ++mips;

            // Cubemap has 4 sides so 2 mips less
            iEnvMapLOD = mips;
        }

        // Dealing with very high and low resolution cubemaps
        if ( iEnvMapLOD > 12 ) iEnvMapLOD = 12;
        if ( iEnvMapLOD < 4 ) iEnvMapLOD = 4;

        // This has some spare space
        vEyePos_SpecExponent[3] = iEnvMapLOD;
        // g_EyePos
        pShaderAPI->SetPixelShaderConstant( 2, vEyePos_SpecExponent, 1 );

        // Setting lightmap texture
        if ( bLightMapped || lightState.m_bStaticLightTexel )
            pShaderAPI->BindStandardTexture( SAMPLER_LIGHTMAP, TEXTURE_LIGHTMAP );

        // Setting light accum texture
        pShader->BindTexture( SAMPLER_LIGHTACCUM, GetDeferredExt()->GetTexture_LightAccum() );

        // Setting up dynamic vertex shader
        DECLARE_DYNAMIC_VERTEX_SHADER( composite_pbr_vs30 );
        SET_DYNAMIC_VERTEX_SHADER_COMBO( DOWATERFOG, fogIndex );
        SET_DYNAMIC_VERTEX_SHADER_COMBO( SKINNING, numBones > 0 );
        SET_DYNAMIC_VERTEX_SHADER_COMBO(
            LIGHTING_PREVIEW, pShaderAPI->GetIntRenderingParameter( INT_RENDERPARM_ENABLE_FIXED_LIGHTING ) != 0 );
        SET_DYNAMIC_VERTEX_SHADER_COMBO( COMPRESSED_VERTS, (int) vertexCompression );
        SET_DYNAMIC_VERTEX_SHADER_COMBO( STATIC_LIGHT_VERTEX, lightState.m_bStaticLightVertex ? 1 : 0 );
        SET_DYNAMIC_VERTEX_SHADER( composite_pbr_vs30 );

        // Setting up dynamic pixel shader
        DECLARE_DYNAMIC_PIXEL_SHADER( composite_pbr_ps30 );
        SET_DYNAMIC_PIXEL_SHADER_COMBO( WRITEWATERFOGTODESTALPHA, bWriteWaterFogToAlpha );
        SET_DYNAMIC_PIXEL_SHADER_COMBO( WRITE_DEPTH_TO_DESTALPHA, bWriteDepthToAlpha );
        SET_DYNAMIC_PIXEL_SHADER_COMBO( PIXELFOGTYPE, pShaderAPI->GetPixelFogCombo() );
        SET_DYNAMIC_PIXEL_SHADER_COMBO( STATIC_LIGHT_LIGHTMAP, lightState.m_bStaticLightTexel );
        SET_DYNAMIC_PIXEL_SHADER_COMBO( STATIC_LIGHT_VERTEX, lightState.m_bStaticLightVertex ? 1 : 0 );
        SET_DYNAMIC_PIXEL_SHADER_COMBO( ENABLE_SPECULAR, bSpecular );
        SET_DYNAMIC_PIXEL_SHADER( composite_pbr_ps30 );

        // Setting up base texture transform
        pShader->SetVertexShaderTextureTransform( VERTEX_SHADER_SHADER_SPECIFIC_CONST_0, info.baseTextureTransform );

        // This is probably important
        pShader->SetModulationPixelShaderDynamicState_LinearColorSpace( 1 );

        if ( bModel )
        {
            // cAmbientCubeXYZ in vs
            pShader->SetAmbientCubeDynamicStateVertexShader();

            // Send ambient cube to the pixel shader, force to black if not available
            // cAmbientCube
            pShaderAPI->SetPixelShaderStateAmbientLightCube( 8, false );
        }

        // Handle mat_fullbright 2 (diffuse lighting only)
        if ( bLightingOnly )
        {
            pShaderAPI->BindStandardTexture( SAMPLER_BASETEXTURE, TEXTURE_GREY );  // Basecolor
        }

        // Handle mat_specular 0 (no envmap reflections)
        if ( bSpecular )
        {
            pShaderAPI->BindStandardTexture( SAMPLER_ENVMAP, TEXTURE_BLACK );  // Envmap
        }

        // Sending fog info to the pixel shader
        // g_FogParams
        pShaderAPI->SetPixelShaderFogParams( 3 );

        // Set up shader modulation color
        float modulationColor[4] = { 1.0, 1.0, 1.0, 1.0 };
        pShader->ComputeModulationColor( modulationColor );
        float flLScale = pShaderAPI->GetLightMapScaleFactor();
        modulationColor[0] *= flLScale;
        modulationColor[1] *= flLScale;
        modulationColor[2] *= flLScale;
        // g_DiffuseModulation
        pShaderAPI->SetPixelShaderConstant( 1, modulationColor );

        float flParams[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
        // Parallax Depth (the strength of the effect)
        flParams[0] = GetFloatParam( info.parallaxDepth, params, 3.0f );
        // Parallax Center (the height at which it's not moved)
        flParams[1] = GetFloatParam( info.parallaxCenter, params, 3.0f );
        // g_ParallaxParms
        pShaderAPI->SetPixelShaderConstant( 6, flParams, 1 );

        ShaderViewport_t viewport;
        pShaderAPI->GetViewports( &viewport, 1 );
        float fl1[4] = { 1.0f / viewport.m_nWidth, 1.0f / viewport.m_nHeight, 0, 0 };

        // g_vecFullScreenTexel
        pShaderAPI->SetPixelShaderConstant( 7, fl1 );

        // g_sunColor
        pShaderAPI->SetPixelShaderConstant( 5, GetDeferredExt()->GetLightData_Global().diff.Base() );
    }

    // Actually draw the shader
    pShader->Draw();
}
