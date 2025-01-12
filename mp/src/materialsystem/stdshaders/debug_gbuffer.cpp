
#include "deferred_includes.h"

#include "screenspace_vs30.inc"
#include "debug_gbuffer_ps30.inc"

BEGIN_VS_SHADER( DEBUG_GBUFFER, "" )
BEGIN_SHADER_PARAMS

END_SHADER_PARAMS

SHADER_INIT_PARAMS() {}

SHADER_INIT {}

SHADER_FALLBACK { return 0; }

SHADER_DRAW
{
    SHADOW_STATE
    {
        pShaderShadow->EnableDepthWrites( false );

        pShaderShadow->EnableTexture( SHADER_SAMPLER0, true );
        pShaderShadow->EnableTexture( SHADER_SAMPLER1, true );
        pShaderShadow->EnableTexture( SHADER_SAMPLER2, true );

        pShaderShadow->VertexShaderVertexFormat( VERTEX_POSITION, 1, NULL, 0 );

        DECLARE_STATIC_VERTEX_SHADER( screenspace_vs30 );
        SET_STATIC_VERTEX_SHADER( screenspace_vs30 );

        DECLARE_STATIC_PIXEL_SHADER( debug_gbuffer_ps30 );
        SET_STATIC_PIXEL_SHADER( debug_gbuffer_ps30 );
    }
    DYNAMIC_STATE
    {
        BindTexture( SHADER_SAMPLER0, GetDeferredExt()->GetTexture_Normals() );
        BindTexture( SHADER_SAMPLER1, GetDeferredExt()->GetTexture_ShadowDepth_Ortho(0) );
        BindTexture( SHADER_SAMPLER2, GetDeferredExt()->GetTexture_SpecularRoughness() );

        DECLARE_DYNAMIC_VERTEX_SHADER( screenspace_vs30 );
        SET_DYNAMIC_VERTEX_SHADER( screenspace_vs30 );

        DECLARE_DYNAMIC_PIXEL_SHADER( debug_gbuffer_ps30 );
        SET_DYNAMIC_PIXEL_SHADER( debug_gbuffer_ps30 );
    }

    Draw();
}

END_SHADER
