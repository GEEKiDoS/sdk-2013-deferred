#ifndef DEFPASS_COMPOSITE_H
#define DEFPASS_COMPOSITE_H

class CDeferredPerMaterialContextData;

// Variables for this shader
struct PBR_Vars_t
{
    PBR_Vars_t() 
	{ 
		memset( this, 0xFF, sizeof( *this ) );
        bModel = false;
	}

    int baseTexture;
    int baseColor;
    int bumpMap;
    int envMap;
    int baseTextureFrame;
    int baseTextureTransform;
    int useParallax;
    int parallaxDepth;
    int parallaxCenter;
    int alphaTestReference;
    int flashlightTexture;
    int flashlightTextureFrame;
    int emissionTexture;
    int mraoTexture;
    int useEnvAmbient;
    int specularTexture;

	bool bModel;
};

void InitParmsCompositePBR( const PBR_Vars_t &info, CBaseVSShader *pShader, IMaterialVar **params );
void InitPassCompositePBR( const PBR_Vars_t &info, CBaseVSShader *pShader, IMaterialVar **params );
void DrawPassCompositePBR( const PBR_Vars_t &info, CBaseVSShader *pShader, IMaterialVar **params,
                        IShaderShadow *pShaderShadow, IShaderDynamicAPI *pShaderAPI,
                        VertexCompressionType_t vertexCompression, CDeferredPerMaterialContextData *pDeferredContext );


#endif