// ALL SKIP STATEMENTS THAT AFFECT THIS SHADER!!!
// ($PIXELFOGTYPE == 0) && ($WRITEWATERFOGTODESTALPHA != 0)
// defined $HDRTYPE && defined $HDRENABLED && !$HDRTYPE && $HDRENABLED
// defined $PIXELFOGTYPE && defined $WRITEWATERFOGTODESTALPHA && ( $PIXELFOGTYPE != 1 ) && $WRITEWATERFOGTODESTALPHA
// defined $LIGHTING_PREVIEW && defined $HDRTYPE && $LIGHTING_PREVIEW && $HDRTYPE != 0
// defined $LIGHTING_PREVIEW && defined $FASTPATHENVMAPTINT && $LIGHTING_PREVIEW && $FASTPATHENVMAPTINT
// defined $LIGHTING_PREVIEW && defined $FASTPATHENVMAPCONTRAST && $LIGHTING_PREVIEW && $FASTPATHENVMAPCONTRAST
// defined $LIGHTING_PREVIEW && defined $FASTPATH && $LIGHTING_PREVIEW && $FASTPATH
// ($FLASHLIGHT || $FLASHLIGHTSHADOWS) && $LIGHTING_PREVIEW
// defined $HDRTYPE && defined $HDRENABLED && !$HDRTYPE && $HDRENABLED
// defined $PIXELFOGTYPE && defined $WRITEWATERFOGTODESTALPHA && ( $PIXELFOGTYPE != 1 ) && $WRITEWATERFOGTODESTALPHA
// defined $LIGHTING_PREVIEW && defined $HDRTYPE && $LIGHTING_PREVIEW && $HDRTYPE != 0
// defined $LIGHTING_PREVIEW && defined $FASTPATHENVMAPTINT && $LIGHTING_PREVIEW && $FASTPATHENVMAPTINT
// defined $LIGHTING_PREVIEW && defined $FASTPATHENVMAPCONTRAST && $LIGHTING_PREVIEW && $FASTPATHENVMAPCONTRAST
// defined $LIGHTING_PREVIEW && defined $FASTPATH && $LIGHTING_PREVIEW && $FASTPATH
// ($FLASHLIGHT || $FLASHLIGHTSHADOWS) && $LIGHTING_PREVIEW

#pragma once
#include "shaderlib/cshader.h"
class composite_pbr_ps30_Static_Index
{
	unsigned int m_nLIGHTMAPPED : 2;
	unsigned int m_nUSEENVAMBIENT : 2;
	unsigned int m_nEMISSIVE : 2;
	unsigned int m_nSPECULAR : 2;
	unsigned int m_nPARALLAXOCCLUSION : 2;
	unsigned int m_nBUMPED : 2;
#ifdef _DEBUG
	bool m_bLIGHTMAPPED : 1;
	bool m_bUSEENVAMBIENT : 1;
	bool m_bEMISSIVE : 1;
	bool m_bSPECULAR : 1;
	bool m_bPARALLAXOCCLUSION : 1;
	bool m_bBUMPED : 1;
#endif	// _DEBUG
public:
	void SetLIGHTMAPPED( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nLIGHTMAPPED = i;
#ifdef _DEBUG
		m_bLIGHTMAPPED = true;
#endif	// _DEBUG
	}

	void SetUSEENVAMBIENT( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nUSEENVAMBIENT = i;
#ifdef _DEBUG
		m_bUSEENVAMBIENT = true;
#endif	// _DEBUG
	}

	void SetEMISSIVE( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nEMISSIVE = i;
#ifdef _DEBUG
		m_bEMISSIVE = true;
#endif	// _DEBUG
	}

	void SetSPECULAR( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nSPECULAR = i;
#ifdef _DEBUG
		m_bSPECULAR = true;
#endif	// _DEBUG
	}

	void SetPARALLAXOCCLUSION( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nPARALLAXOCCLUSION = i;
#ifdef _DEBUG
		m_bPARALLAXOCCLUSION = true;
#endif	// _DEBUG
	}

	void SetBUMPED( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nBUMPED = i;
#ifdef _DEBUG
		m_bBUMPED = true;
#endif	// _DEBUG
	}

	composite_pbr_ps30_Static_Index( IShaderShadow* pShaderShadow, IMaterialVar** params )
	{
		m_nLIGHTMAPPED = 0;
		m_nUSEENVAMBIENT = 0;
		m_nEMISSIVE = 0;
		m_nSPECULAR = 0;
		m_nPARALLAXOCCLUSION = 0;
		m_nBUMPED = 0;
#ifdef _DEBUG
		m_bLIGHTMAPPED = false;
		m_bUSEENVAMBIENT = false;
		m_bEMISSIVE = false;
		m_bSPECULAR = false;
		m_bPARALLAXOCCLUSION = false;
		m_bBUMPED = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bLIGHTMAPPED && m_bUSEENVAMBIENT && m_bEMISSIVE && m_bSPECULAR && m_bPARALLAXOCCLUSION && m_bBUMPED );
		return ( 32 * m_nLIGHTMAPPED ) + ( 64 * m_nUSEENVAMBIENT ) + ( 128 * m_nEMISSIVE ) + ( 256 * m_nSPECULAR ) + ( 512 * m_nPARALLAXOCCLUSION ) + ( 1024 * m_nBUMPED ) + 0;
	}
};

#define shaderStaticTest_composite_pbr_ps30 psh_forgot_to_set_static_LIGHTMAPPED + psh_forgot_to_set_static_USEENVAMBIENT + psh_forgot_to_set_static_EMISSIVE + psh_forgot_to_set_static_SPECULAR + psh_forgot_to_set_static_PARALLAXOCCLUSION + psh_forgot_to_set_static_BUMPED


class composite_pbr_ps30_Dynamic_Index
{
	unsigned int m_nWRITEWATERFOGTODESTALPHA : 2;
	unsigned int m_nPIXELFOGTYPE : 2;
	unsigned int m_nWRITE_DEPTH_TO_DESTALPHA : 2;
	unsigned int m_nSTATIC_LIGHT_VERTEX : 2;
	unsigned int m_nSTATIC_LIGHT_LIGHTMAP : 2;
#ifdef _DEBUG
	bool m_bWRITEWATERFOGTODESTALPHA : 1;
	bool m_bPIXELFOGTYPE : 1;
	bool m_bWRITE_DEPTH_TO_DESTALPHA : 1;
	bool m_bSTATIC_LIGHT_VERTEX : 1;
	bool m_bSTATIC_LIGHT_LIGHTMAP : 1;
#endif	// _DEBUG
public:
	void SetWRITEWATERFOGTODESTALPHA( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nWRITEWATERFOGTODESTALPHA = i;
#ifdef _DEBUG
		m_bWRITEWATERFOGTODESTALPHA = true;
#endif	// _DEBUG
	}

	void SetPIXELFOGTYPE( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nPIXELFOGTYPE = i;
#ifdef _DEBUG
		m_bPIXELFOGTYPE = true;
#endif	// _DEBUG
	}

	void SetWRITE_DEPTH_TO_DESTALPHA( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nWRITE_DEPTH_TO_DESTALPHA = i;
#ifdef _DEBUG
		m_bWRITE_DEPTH_TO_DESTALPHA = true;
#endif	// _DEBUG
	}

	void SetSTATIC_LIGHT_VERTEX( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nSTATIC_LIGHT_VERTEX = i;
#ifdef _DEBUG
		m_bSTATIC_LIGHT_VERTEX = true;
#endif	// _DEBUG
	}

	void SetSTATIC_LIGHT_LIGHTMAP( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nSTATIC_LIGHT_LIGHTMAP = i;
#ifdef _DEBUG
		m_bSTATIC_LIGHT_LIGHTMAP = true;
#endif	// _DEBUG
	}

	composite_pbr_ps30_Dynamic_Index( IShaderDynamicAPI* pShaderAPI )
	{
		m_nWRITEWATERFOGTODESTALPHA = 0;
		m_nPIXELFOGTYPE = 0;
		m_nWRITE_DEPTH_TO_DESTALPHA = 0;
		m_nSTATIC_LIGHT_VERTEX = 0;
		m_nSTATIC_LIGHT_LIGHTMAP = 0;
#ifdef _DEBUG
		m_bWRITEWATERFOGTODESTALPHA = false;
		m_bPIXELFOGTYPE = false;
		m_bWRITE_DEPTH_TO_DESTALPHA = false;
		m_bSTATIC_LIGHT_VERTEX = false;
		m_bSTATIC_LIGHT_LIGHTMAP = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bWRITEWATERFOGTODESTALPHA && m_bPIXELFOGTYPE && m_bWRITE_DEPTH_TO_DESTALPHA && m_bSTATIC_LIGHT_VERTEX && m_bSTATIC_LIGHT_LIGHTMAP );
		AssertMsg( !( ( m_nPIXELFOGTYPE == 0 ) && ( m_nWRITEWATERFOGTODESTALPHA != 0 ) ), "Invalid combo combination ( ( PIXELFOGTYPE == 0 ) && ( WRITEWATERFOGTODESTALPHA != 0 ) )" );
		AssertMsg( !( 1 && ( 1 && ( ( m_nPIXELFOGTYPE != 1 ) && m_nWRITEWATERFOGTODESTALPHA ) ) ), "Invalid combo combination ( 1 && ( 1 && ( ( PIXELFOGTYPE != 1 ) && WRITEWATERFOGTODESTALPHA ) ) )" );
		AssertMsg( !( 1 && ( 1 && ( ( m_nPIXELFOGTYPE != 1 ) && m_nWRITEWATERFOGTODESTALPHA ) ) ), "Invalid combo combination ( 1 && ( 1 && ( ( PIXELFOGTYPE != 1 ) && WRITEWATERFOGTODESTALPHA ) ) )" );
		return ( 1 * m_nWRITEWATERFOGTODESTALPHA ) + ( 2 * m_nPIXELFOGTYPE ) + ( 4 * m_nWRITE_DEPTH_TO_DESTALPHA ) + ( 8 * m_nSTATIC_LIGHT_VERTEX ) + ( 16 * m_nSTATIC_LIGHT_LIGHTMAP ) + 0;
	}
};

#define shaderDynamicTest_composite_pbr_ps30 psh_forgot_to_set_dynamic_WRITEWATERFOGTODESTALPHA + psh_forgot_to_set_dynamic_PIXELFOGTYPE + psh_forgot_to_set_dynamic_WRITE_DEPTH_TO_DESTALPHA + psh_forgot_to_set_dynamic_STATIC_LIGHT_VERTEX + psh_forgot_to_set_dynamic_STATIC_LIGHT_LIGHTMAP

