
#include "deferred_includes.h"

#include "tier0/memdbgon.h"

// DEFINE_FALLBACK_SHADER( VertexLitGeneric, DEFERRED_MODEL )
// DEFINE_FALLBACK_SHADER( UnlitGeneric, DEFERRED_MODEL )

BEGIN_VS_SHADER( DEFERRED_MODEL, "" )
BEGIN_SHADER_PARAMS
SHADER_PARAM( MRAOTEXTURE, SHADER_PARAM_TYPE_TEXTURE, "",
              "Texture with metalness in R, roughness in G, ambient occlusion in B." );
SHADER_PARAM( EMISSIONTEXTURE, SHADER_PARAM_TYPE_TEXTURE, "", "Emission texture" );
SHADER_PARAM( SPECULARTEXTURE, SHADER_PARAM_TYPE_TEXTURE, "", "Specular F0 RGB map" );

SHADER_PARAM( BUMPMAP, SHADER_PARAM_TYPE_TEXTURE, "", "" )
SHADER_PARAM( NORMALMAP, SHADER_PARAM_TYPE_TEXTURE, "", "" )

SHADER_PARAM( LITFACE, SHADER_PARAM_TYPE_BOOL, "0", "" )

SHADER_PARAM( PHONG_SCALE, SHADER_PARAM_TYPE_FLOAT, "", "" )
SHADER_PARAM( PHONG_EXP, SHADER_PARAM_TYPE_FLOAT, "", "" )
SHADER_PARAM( PHONG_MAP, SHADER_PARAM_TYPE_TEXTURE, "", "" )
SHADER_PARAM( PHONG_FRESNEL, SHADER_PARAM_TYPE_BOOL, "", "" )

SHADER_PARAM( FRESNELRANGES, SHADER_PARAM_TYPE_VEC3, "", "" )
SHADER_PARAM( ALPHATESTREFERENCE, SHADER_PARAM_TYPE_FLOAT, "0.5", "" )

SHADER_PARAM( ENVMAP, SHADER_PARAM_TYPE_TEXTURE, "shadertest/shadertest_env", "envmap" )
SHADER_PARAM( ENVMAPTINT, SHADER_PARAM_TYPE_COLOR, "[1 1 1]", "envmap tint" )
SHADER_PARAM( ENVMAPCONTRAST, SHADER_PARAM_TYPE_FLOAT, "0.0", "contrast 0 == normal 1 == color*color" )
SHADER_PARAM( ENVMAPSATURATION, SHADER_PARAM_TYPE_FLOAT, "1.0", "saturation 0 == greyscale 1 == normal" )
SHADER_PARAM( ENVMAPFRESNEL, SHADER_PARAM_TYPE_BOOL, "", "" )
SHADER_PARAM( ENVMAPMASK, SHADER_PARAM_TYPE_TEXTURE, "shadertest/shadertest_envmask", "envmap mask" )

SHADER_PARAM( RIMLIGHT, SHADER_PARAM_TYPE_BOOL, "0", "enables rim lighting" )
SHADER_PARAM( RIMLIGHTEXPONENT, SHADER_PARAM_TYPE_FLOAT, "4.0", "Exponent for rim lights" )
SHADER_PARAM( RIMLIGHTALBEDOSCALE, SHADER_PARAM_TYPE_FLOAT, "0.0", "Albedo influence on rim light" )
SHADER_PARAM( RIMLIGHTTINT, SHADER_PARAM_TYPE_VEC3, "[1 1 1]", "Tint for rim lights" )
SHADER_PARAM( RIMLIGHT_MODULATE_BY_LIGHT, SHADER_PARAM_TYPE_BOOL, "0", "Modulate rimlight by lighting." )

SHADER_PARAM( SELFILLUMTINT, SHADER_PARAM_TYPE_COLOR, "[1 1 1]", "Self-illumination tint" )
SHADER_PARAM( SELFILLUM_ENVMAPMASK_ALPHA, SHADER_PARAM_TYPE_BOOL, "0",
              "defines that self illum value comes from env map mask alpha" )
SHADER_PARAM( SELFILLUMFRESNEL, SHADER_PARAM_TYPE_BOOL, "0", "Self illum fresnel" )
SHADER_PARAM( SELFILLUMMASK, SHADER_PARAM_TYPE_TEXTURE, "shadertest/BaseTexture",
              "If we bind a texture here, it overrides base alpha (if any) for self illum" )

SHADER_PARAM( TREESWAY, SHADER_PARAM_TYPE_INTEGER, "0", "" )
SHADER_PARAM( TREESWAYHEIGHT, SHADER_PARAM_TYPE_FLOAT, "1000", "" )
SHADER_PARAM( TREESWAYSTARTHEIGHT, SHADER_PARAM_TYPE_FLOAT, "0.2", "" )
SHADER_PARAM( TREESWAYRADIUS, SHADER_PARAM_TYPE_FLOAT, "300", "" )
SHADER_PARAM( TREESWAYSTARTRADIUS, SHADER_PARAM_TYPE_FLOAT, "0.1", "" )
SHADER_PARAM( TREESWAYSPEED, SHADER_PARAM_TYPE_FLOAT, "1", "" )
SHADER_PARAM( TREESWAYSPEEDHIGHWINDMULTIPLIER, SHADER_PARAM_TYPE_FLOAT, "2", "" )
SHADER_PARAM( TREESWAYSTRENGTH, SHADER_PARAM_TYPE_FLOAT, "10", "" )
SHADER_PARAM( TREESWAYSCRUMBLESPEED, SHADER_PARAM_TYPE_FLOAT, "0.1", "" )
SHADER_PARAM( TREESWAYSCRUMBLESTRENGTH, SHADER_PARAM_TYPE_FLOAT, "0.1", "" )
SHADER_PARAM( TREESWAYSCRUMBLEFREQUENCY, SHADER_PARAM_TYPE_FLOAT, "0.1", "" )
SHADER_PARAM( TREESWAYFALLOFFEXP, SHADER_PARAM_TYPE_FLOAT, "1.5", "" )
SHADER_PARAM( TREESWAYSCRUMBLEFALLOFFEXP, SHADER_PARAM_TYPE_FLOAT, "1.0", "" )
SHADER_PARAM( TREESWAYSPEEDLERPSTART, SHADER_PARAM_TYPE_FLOAT, "3", "" )
SHADER_PARAM( TREESWAYSPEEDLERPEND, SHADER_PARAM_TYPE_FLOAT, "6", "" )

SHADER_PARAM( USEENVAMBIENT, SHADER_PARAM_TYPE_BOOL, "0", "Use the cubemaps to compute ambient light." )

SHADER_PARAM( PARALLAX, SHADER_PARAM_TYPE_BOOL, "0", "Use Parallax Occlusion Mapping." );
SHADER_PARAM( PARALLAXDEPTH, SHADER_PARAM_TYPE_FLOAT, "0.0030", "Depth of the Parallax Map" );
SHADER_PARAM( PARALLAXCENTER, SHADER_PARAM_TYPE_FLOAT, "0.5", "Center depth of the Parallax Map" );

END_SHADER_PARAMS

void SetupParmsGBuffer( defParms_gBuffer &p )
{
    p.bModel = true;

    p.iAlbedo = BASETEXTURE;
    p.iBumpmap = BUMPMAP;

    p.iMraoTexture = MRAOTEXTURE;
    p.iSpecularTexture = SPECULARTEXTURE;

    p.iAlphatestRef = ALPHATESTREFERENCE;
    p.iLitface = LITFACE;
    p.iPhongExp = PHONG_EXP;

    p.iTreeSway = TREESWAY;
    p.iTreeSwayHeight = TREESWAYHEIGHT;
    p.iTreeSwayStartHeight = TREESWAYSTARTHEIGHT;
    p.iTreeSwayRadius = TREESWAYRADIUS;
    p.iTreeSwayStartRadius = TREESWAYSTARTRADIUS;
    p.iTreeSwaySpeed = TREESWAYSPEED;
    p.iTreeSwaySpeedHighWindMultiplier = TREESWAYSPEEDHIGHWINDMULTIPLIER;
    p.iTreeSwayStrength = TREESWAYSTRENGTH;
    p.iTreeSwayScrumbleSpeed = TREESWAYSCRUMBLESPEED;
    p.iTreeSwayScrumbleStrength = TREESWAYSCRUMBLESTRENGTH;
    p.iTreeSwayScrumbleFrequency = TREESWAYSCRUMBLEFREQUENCY;
    p.iTreeSwayFalloffExp = TREESWAYFALLOFFEXP;
    p.iTreeSwayScrumbleFalloffExp = TREESWAYSCRUMBLEFALLOFFEXP;
    p.iTreeSwaySpeedLerpStart = TREESWAYSPEEDLERPSTART;
    p.iTreeSwaySpeedLerpEnd = TREESWAYSPEEDLERPEND;
}

void SetupParmsShadow( defParms_shadow &p )
{
    p.bModel = true;
    p.iAlbedo = BASETEXTURE;
    p.iAlphatestRef = ALPHATESTREFERENCE;

    p.iTreeSway = TREESWAY;
    p.iTreeSwayHeight = TREESWAYHEIGHT;
    p.iTreeSwayStartHeight = TREESWAYSTARTHEIGHT;
    p.iTreeSwayRadius = TREESWAYRADIUS;
    p.iTreeSwayStartRadius = TREESWAYSTARTRADIUS;
    p.iTreeSwaySpeed = TREESWAYSPEED;
    p.iTreeSwaySpeedHighWindMultiplier = TREESWAYSPEEDHIGHWINDMULTIPLIER;
    p.iTreeSwayStrength = TREESWAYSTRENGTH;
    p.iTreeSwayScrumbleSpeed = TREESWAYSCRUMBLESPEED;
    p.iTreeSwayScrumbleStrength = TREESWAYSCRUMBLESTRENGTH;
    p.iTreeSwayScrumbleFrequency = TREESWAYSCRUMBLEFREQUENCY;
    p.iTreeSwayFalloffExp = TREESWAYFALLOFFEXP;
    p.iTreeSwayScrumbleFalloffExp = TREESWAYSCRUMBLEFALLOFFEXP;
    p.iTreeSwaySpeedLerpStart = TREESWAYSPEEDLERPSTART;
    p.iTreeSwaySpeedLerpEnd = TREESWAYSPEEDLERPEND;
}

void SetupParmsComposite( PBR_Vars_t &p )
{
    p.bModel = true;

    p.baseTexture = BASETEXTURE;
    p.baseColor = COLOR;
    p.bumpMap = NORMALMAP;
    p.baseTextureFrame = FRAME;
    p.baseTextureTransform = BASETEXTURETRANSFORM;
    p.alphaTestReference = ALPHATESTREFERENCE;
    p.flashlightTexture = FLASHLIGHTTEXTURE;
    p.flashlightTextureFrame = FLASHLIGHTTEXTUREFRAME;
    p.envMap = ENVMAP;
    p.emissionTexture = EMISSIONTEXTURE;
    p.mraoTexture = MRAOTEXTURE;
    p.useEnvAmbient = USEENVAMBIENT;
    p.specularTexture = SPECULARTEXTURE;
    p.useParallax = PARALLAX;
    p.parallaxDepth = PARALLAXDEPTH;
    p.parallaxCenter = PARALLAXCENTER;
}

bool DrawToGBuffer( IMaterialVar **params )
{
#if DEFCFG_DEFERRED_SHADING == 1
    return true;
#else
    const bool bIsDecal = IS_FLAG_SET( MATERIAL_VAR_DECAL );
    const bool bTranslucent = IS_FLAG_SET( MATERIAL_VAR_TRANSLUCENT );

    return !bTranslucent && !bIsDecal;
#endif
}

SHADER_INIT_PARAMS()
{
    SET_FLAGS2( MATERIAL_VAR2_SUPPORTS_HW_SKINNING );

    if ( g_pHardwareConfig->HasFastVertexTextures() ) SET_FLAGS2( MATERIAL_VAR2_USES_VERTEXID );

    if (params[BUMPMAP]->IsDefined())
    {
        params[NORMALMAP]->SetStringValue( params[BUMPMAP]->GetStringValue() );
        params[BUMPMAP]->SetUndefined();
    }

    const bool bDrawToGBuffer = DrawToGBuffer( params );

    if ( bDrawToGBuffer )
    {
        defParms_gBuffer parms_gbuffer;
        SetupParmsGBuffer( parms_gbuffer );
        InitParmsGBuffer( parms_gbuffer, this, params );

        defParms_shadow parms_shadow;
        SetupParmsShadow( parms_shadow );
        InitParmsShadowPass( parms_shadow, this, params );
    }

    PBR_Vars_t parms_composite;
    SetupParmsComposite( parms_composite );
    InitParmsCompositePBR( parms_composite, this, params );
}

SHADER_INIT
{
    const bool bDrawToGBuffer = DrawToGBuffer( params );

    if ( bDrawToGBuffer )
    {
        defParms_gBuffer parms_gbuffer;
        SetupParmsGBuffer( parms_gbuffer );
        InitPassGBuffer( parms_gbuffer, this, params );

        defParms_shadow parms_shadow;
        SetupParmsShadow( parms_shadow );
        InitPassShadowPass( parms_shadow, this, params );
    }

    PBR_Vars_t parms_composite;
    SetupParmsComposite( parms_composite );
    InitPassCompositePBR( parms_composite, this, params );
}

SHADER_FALLBACK
{
    const bool bIsDecal = IS_FLAG_SET( MATERIAL_VAR_DECAL );

    if ( bIsDecal ) return "DEFERRED_DECALMODULATE";

    return nullptr;
}

SHADER_DRAW
{
    if ( pShaderAPI != NULL && *pContextDataPtr == NULL ) *pContextDataPtr = new CDeferredPerMaterialContextData();

    CDeferredPerMaterialContextData *pDefContext =
        reinterpret_cast<CDeferredPerMaterialContextData *>( *pContextDataPtr );

    const int iDeferredRenderStage = pShaderAPI
                                         ? pShaderAPI->GetIntRenderingParameter( INT_RENDERPARM_DEFERRED_RENDER_STAGE )
                                         : DEFERRED_RENDER_STAGE_INVALID;

    const bool bDrawToGBuffer = DrawToGBuffer( params );

    Assert( pShaderAPI == NULL || iDeferredRenderStage != DEFERRED_RENDER_STAGE_INVALID );

    if ( bDrawToGBuffer )
    {
        if ( pShaderShadow != NULL || iDeferredRenderStage == DEFERRED_RENDER_STAGE_GBUFFER )
        {
            defParms_gBuffer parms_gbuffer;
            SetupParmsGBuffer( parms_gbuffer );
            DrawPassGBuffer( parms_gbuffer, this, params, pShaderShadow, pShaderAPI, vertexCompression, pDefContext );
        }
        else
            Draw( false );

        if ( pShaderShadow != NULL || iDeferredRenderStage == DEFERRED_RENDER_STAGE_SHADOWPASS )
        {
            defParms_shadow parms_shadow;
            SetupParmsShadow( parms_shadow );
            DrawPassShadowPass( parms_shadow, this, params, pShaderShadow, pShaderAPI, vertexCompression, pDefContext );
        }
        else
            Draw( false );
    }

#if ( DEFCFG_DEFERRED_SHADING == 0 )
    if ( pShaderShadow != NULL || iDeferredRenderStage == DEFERRED_RENDER_STAGE_COMPOSITION )
    {
        PBR_Vars_t parms_composite;
        SetupParmsComposite( parms_composite );
        DrawPassCompositePBR( parms_composite, this, params, pShaderShadow, pShaderAPI, vertexCompression, pDefContext );
    }
    else
        Draw( false );
#endif

    if ( pShaderAPI != NULL && pDefContext->m_bMaterialVarsChanged ) pDefContext->m_bMaterialVarsChanged = false;
}

END_SHADER