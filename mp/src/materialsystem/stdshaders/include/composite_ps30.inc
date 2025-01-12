// ALL SKIP STATEMENTS THAT AFFECT THIS SHADER!!!
// defined $HDRTYPE && defined $HDRENABLED && !$HDRTYPE && $HDRENABLED
// defined $PIXELFOGTYPE && defined $WRITEWATERFOGTODESTALPHA && ( $PIXELFOGTYPE != 1 ) && $WRITEWATERFOGTODESTALPHA
// defined $LIGHTING_PREVIEW && defined $HDRTYPE && $LIGHTING_PREVIEW && $HDRTYPE != 0
// defined $LIGHTING_PREVIEW && defined $FASTPATHENVMAPTINT && $LIGHTING_PREVIEW && $FASTPATHENVMAPTINT
// defined $LIGHTING_PREVIEW && defined $FASTPATHENVMAPCONTRAST && $LIGHTING_PREVIEW && $FASTPATHENVMAPCONTRAST
// defined $LIGHTING_PREVIEW && defined $FASTPATH && $LIGHTING_PREVIEW && $FASTPATH
// ($FLASHLIGHT || $FLASHLIGHTSHADOWS) && $LIGHTING_PREVIEW
// !$MODEL && $MORPHING_VTEX
// !$MODEL && $COMPRESSED_VERTS
// !$MODEL && $SKINNING
// !$MODEL && $MORPHING_VTEX
// defined $HDRTYPE && defined $HDRENABLED && !$HDRTYPE && $HDRENABLED
// defined $PIXELFOGTYPE && defined $WRITEWATERFOGTODESTALPHA && ( $PIXELFOGTYPE != 1 ) && $WRITEWATERFOGTODESTALPHA
// defined $LIGHTING_PREVIEW && defined $HDRTYPE && $LIGHTING_PREVIEW && $HDRTYPE != 0
// defined $LIGHTING_PREVIEW && defined $FASTPATHENVMAPTINT && $LIGHTING_PREVIEW && $FASTPATHENVMAPTINT
// defined $LIGHTING_PREVIEW && defined $FASTPATHENVMAPCONTRAST && $LIGHTING_PREVIEW && $FASTPATHENVMAPCONTRAST
// defined $LIGHTING_PREVIEW && defined $FASTPATH && $LIGHTING_PREVIEW && $FASTPATH
// ($FLASHLIGHT || $FLASHLIGHTSHADOWS) && $LIGHTING_PREVIEW

#pragma once
#include "shaderlib/cshader.h"
class composite_ps30_Static_Index
{
	unsigned int m_nBLENDMODULATE : 2;
	unsigned int m_nLIGHTMAPPED : 2;
	unsigned int m_nUSEENVAMBIENT : 2;
	unsigned int m_nEMISSIVE : 2;
#ifdef _DEBUG
	bool m_bBLENDMODULATE : 1;
	bool m_bLIGHTMAPPED : 1;
	bool m_bUSEENVAMBIENT : 1;
	bool m_bEMISSIVE : 1;
#endif	// _DEBUG
public:
	void SetBLENDMODULATE( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nBLENDMODULATE = i;
#ifdef _DEBUG
		m_bBLENDMODULATE = true;
#endif	// _DEBUG
	}

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

	composite_ps30_Static_Index( IShaderShadow* pShaderShadow, IMaterialVar** params )
	{
		m_nBLENDMODULATE = 0;
		m_nLIGHTMAPPED = 0;
		m_nUSEENVAMBIENT = 0;
		m_nEMISSIVE = 0;
#ifdef _DEBUG
		m_bBLENDMODULATE = false;
		m_bLIGHTMAPPED = false;
		m_bUSEENVAMBIENT = false;
		m_bEMISSIVE = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bBLENDMODULATE && m_bLIGHTMAPPED && m_bUSEENVAMBIENT && m_bEMISSIVE );
		return ( 1 * m_nBLENDMODULATE ) + ( 2 * m_nLIGHTMAPPED ) + ( 4 * m_nUSEENVAMBIENT ) + ( 8 * m_nEMISSIVE ) + 0;
	}
};

#define shaderStaticTest_composite_ps30 psh_forgot_to_set_static_BLENDMODULATE + psh_forgot_to_set_static_LIGHTMAPPED + psh_forgot_to_set_static_USEENVAMBIENT + psh_forgot_to_set_static_EMISSIVE


class composite_ps30_Dynamic_Index
{
public:
	composite_ps30_Dynamic_Index( IShaderDynamicAPI* pShaderAPI )
	{
	}

	int GetIndex() const
	{
		return 0;
	}
};

#define shaderDynamicTest_composite_ps30 1

