

//*****************************************************************************
// 定数バッファ
//*****************************************************************************

// マトリクスバッファ
cbuffer ConstantBuffer : register(b0)
{
    matrix WorldViewProjection;
}

// マテリアルバッファ
cbuffer MaterialBuffer : register(b1)
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    float4 Emission;
    float Shininess;
    float3 Dummy; //16bit境界用
}

cbuffer ColorBuffer : register(b2)
{
    float4 Color1;
    float4 Color2;
    float3 ColorPosition1;
    float3 ColorPosition2;
    float2 Dummy2; //16bit境界用
}

//=============================================================================
// 頂点シェーダ
//=============================================================================
void VertexShaderPolygon(in float4 inPosition : POSITION0,
						  in float4 inNormal : NORMAL0,
						  in float4 inDiffuse : COLOR0,
						  in float2 inTexCoord : TEXCOORD0,

						  out float4 outPosition : SV_POSITION,
						  out float4 outNormal : NORMAL0,
						  out float2 outTexCoord : TEXCOORD0,
						  out float4 outDiffuse : COLOR0)
{
    outPosition = mul(inPosition, WorldViewProjection);

    outNormal = inNormal;
    outTexCoord = inTexCoord;

    outDiffuse = inDiffuse;// * Diffuse;
    
    
}



//*****************************************************************************
// グローバル変数
//*****************************************************************************
Texture2D g_Texture : register(t0);
SamplerState g_SamplerState : register(s0);

//=============================================================================
// ピクセルシェーダ
//=============================================================================
void PixelShaderPolygon(in float4 inPosition : POSITION0,
						 in float4 inNormal : NORMAL0,
						 in float2 inTexCoord : TEXCOORD0,
						 in float4 inDiffuse : COLOR0,

						 out float4 outDiffuse : SV_Target)
{
    outDiffuse = g_Texture.Sample(g_SamplerState, inTexCoord);
    outDiffuse *= inDiffuse;
    
}

//=============================================================================
// グラデーションピクセルシェーダ
//=============================================================================
void PixelShaderGradation(in float4 inPosition : POSITION0,
						 in float4 inNormal : NORMAL0,
						 in float2 inTexCoord : TEXCOORD0,
						 in float4 inDiffuse : COLOR0,

						 out float4 outDiffuse : SV_Target)
{
    outDiffuse = g_Texture.Sample(g_SamplerState, inTexCoord);
    //outDiffuse *= inDiffuse;
    //float3 v0 = ColorPosition2 - ColorPosition1;
    //float3 v1 = inPosition.xyz - ColorPosition1;
    //float t = clamp(dot(v0, v1) / dot(v0, v0), 0.0f, 1.0f);
    //outDiffuse *= lerp(Color1, Color2, t);
}
