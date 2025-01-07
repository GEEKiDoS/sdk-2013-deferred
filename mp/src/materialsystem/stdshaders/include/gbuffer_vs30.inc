// ALL SKIP STATEMENTS THAT AFFECT THIS SHADER!!!
// !$BUMPMAP2 && $BLENDMODULATE
// $MODEL && $BUMPMAP2
// defined $LIGHTING_PREVIEW && defined $FASTPATH && $LIGHTING_PREVIEW && $FASTPATH
// !$MODEL && $MORPHING_VTEX
// !$MODEL && $COMPRESSED_VERTS
// !$MODEL && $SKINNING
// !$MODEL && $MORPHING_VTEX

#pragma once
#include "shaderlib/cshader.h"
class gbuffer_vs30_Static_Index
{
	unsigned int m_nMODEL : 2;
	unsigned int m_nTANGENTSPACE : 2;
	unsigned int m_nMORPHING_VTEX : 2;
	unsigned int m_nBUMPMAP2 : 2;
	unsigned int m_nBLENDMODULATE : 2;
	unsigned int m_nTREESWAY : 2;
#ifdef _DEBUG
	bool m_bMODEL : 1;
	bool m_bTANGENTSPACE : 1;
	bool m_bMORPHING_VTEX : 1;
	bool m_bBUMPMAP2 : 1;
	bool m_bBLENDMODULATE : 1;
	bool m_bTREESWAY : 1;
#endif	// _DEBUG
public:
	void SetMODEL( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nMODEL = i;
#ifdef _DEBUG
		m_bMODEL = true;
#endif	// _DEBUG
	}

	void SetTANGENTSPACE( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nTANGENTSPACE = i;
#ifdef _DEBUG
		m_bTANGENTSPACE = true;
#endif	// _DEBUG
	}

	void SetMORPHING_VTEX( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nMORPHING_VTEX = i;
#ifdef _DEBUG
		m_bMORPHING_VTEX = true;
#endif	// _DEBUG
	}

	void SetBUMPMAP2( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nBUMPMAP2 = i;
#ifdef _DEBUG
		m_bBUMPMAP2 = true;
#endif	// _DEBUG
	}

	void SetBLENDMODULATE( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nBLENDMODULATE = i;
#ifdef _DEBUG
		m_bBLENDMODULATE = true;
#endif	// _DEBUG
	}

	void SetTREESWAY( int i )
	{
		Assert( i >= 0 && i <= 2 );
		m_nTREESWAY = i;
#ifdef _DEBUG
		m_bTREESWAY = true;
#endif	// _DEBUG
	}

	gbuffer_vs30_Static_Index( IShaderShadow* pShaderShadow, IMaterialVar** params )
	{
		m_nMODEL = 0;
		m_nTANGENTSPACE = 0;
		m_nMORPHING_VTEX = 0;
		m_nBUMPMAP2 = 0;
		m_nBLENDMODULATE = 0;
		m_nTREESWAY = 0;
#ifdef _DEBUG
		m_bMODEL = false;
		m_bTANGENTSPACE = false;
		m_bMORPHING_VTEX = false;
		m_bBUMPMAP2 = false;
		m_bBLENDMODULATE = false;
		m_bTREESWAY = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bMODEL && m_bTANGENTSPACE && m_bMORPHING_VTEX && m_bBUMPMAP2 && m_bBLENDMODULATE && m_bTREESWAY );
		AssertMsg( !( !m_nBUMPMAP2 && m_nBLENDMODULATE ), "Invalid combo combination ( !BUMPMAP2 && BLENDMODULATE )" );
		AssertMsg( !( m_nMODEL && m_nBUMPMAP2 ), "Invalid combo combination ( MODEL && BUMPMAP2 )" );
		AssertMsg( !( !m_nMODEL && m_nMORPHING_VTEX ), "Invalid combo combination ( !MODEL && MORPHING_VTEX )" );
		AssertMsg( !( !m_nMODEL && m_nMORPHING_VTEX ), "Invalid combo combination ( !MODEL && MORPHING_VTEX )" );
		return ( 8 * m_nMODEL ) + ( 16 * m_nTANGENTSPACE ) + ( 32 * m_nMORPHING_VTEX ) + ( 64 * m_nBUMPMAP2 ) + ( 128 * m_nBLENDMODULATE ) + ( 256 * m_nTREESWAY ) + 0;
	}
};

#define shaderStaticTest_gbuffer_vs30 vsh_forgot_to_set_static_MODEL + vsh_forgot_to_set_static_TANGENTSPACE + vsh_forgot_to_set_static_MORPHING_VTEX + vsh_forgot_to_set_static_BUMPMAP2 + vsh_forgot_to_set_static_BLENDMODULATE + vsh_forgot_to_set_static_TREESWAY


class gbuffer_vs30_Dynamic_Index
{
	unsigned int m_nCOMPRESSED_VERTS : 2;
	unsigned int m_nSKINNING : 2;
	unsigned int m_nMORPHING : 2;
#ifdef _DEBUG
	bool m_bCOMPRESSED_VERTS : 1;
	bool m_bSKINNING : 1;
	bool m_bMORPHING : 1;
#endif	// _DEBUG
public:
	void SetCOMPRESSED_VERTS( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nCOMPRESSED_VERTS = i;
#ifdef _DEBUG
		m_bCOMPRESSED_VERTS = true;
#endif	// _DEBUG
	}

	void SetSKINNING( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nSKINNING = i;
#ifdef _DEBUG
		m_bSKINNING = true;
#endif	// _DEBUG
	}

	void SetMORPHING( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nMORPHING = i;
#ifdef _DEBUG
		m_bMORPHING = true;
#endif	// _DEBUG
	}

	gbuffer_vs30_Dynamic_Index( IShaderDynamicAPI* pShaderAPI )
	{
		m_nCOMPRESSED_VERTS = 0;
		m_nSKINNING = 0;
		m_nMORPHING = 0;
#ifdef _DEBUG
		m_bCOMPRESSED_VERTS = false;
		m_bSKINNING = false;
		m_bMORPHING = false;
#endif	// _DEBUG
	}

	int GetIndex() const
	{
		Assert( m_bCOMPRESSED_VERTS && m_bSKINNING && m_bMORPHING );
		return ( 1 * m_nCOMPRESSED_VERTS ) + ( 2 * m_nSKINNING ) + ( 4 * m_nMORPHING ) + 0;
	}
};

#define shaderDynamicTest_gbuffer_vs30 vsh_forgot_to_set_dynamic_COMPRESSED_VERTS + vsh_forgot_to_set_dynamic_SKINNING + vsh_forgot_to_set_dynamic_MORPHING

