cbuffer ModelViewProjection : register(b0)
{
    float3 ViewPos;
};
struct Light
{
    float4 LightPos;
    float4 LightColor;
};
cbuffer Lights : register(b1)
{
    Light lights[5];
}
struct PixelShaderInput
{
    float3 Normal : NORMAL;
    float2 TexCoord : TEXCOORD;
    float4 WorldPos : POSITION;
};

Texture2D t1 : register(t0);
SamplerState s1 : register(s0);

float4 main(PixelShaderInput IN) : SV_TARGET
{
    float ambientStr = 0.1f;
    float attenuation = 1.0 / length(lights[0].LightPos - IN.WorldPos);

    float3 lightDir = normalize(lights[0].LightPos.xyz - IN.WorldPos.xyz);
    float3 viewDir = normalize(ViewPos.xyz - IN.WorldPos.xyz);
    float3 halfwayDir = normalize(lightDir + viewDir);
    IN.Normal = normalize(IN.Normal);
    float diff = max(dot(IN.Normal, lightDir.xyz), 0.0);
   
    float spec = pow(max(dot(IN.Normal, halfwayDir.xyz), 0.0), 16);
    float4 specular = lights[0].LightColor * spec;
    specular *= attenuation;
	
    float4 diffuse = diff * lights[0].LightColor;
    float4 ambient = ambientStr * lights[0].LightColor;
	
    float3 rslt = (ambient + diffuse + specular).xyz * float3(0.1, 0.3, 0.5) * float3(1, 0, 0);

	float gamma = 1.0 / 2.2f;
    rslt = pow(rslt, float3(gamma, gamma, gamma));
    rslt += t1.Sample(s1, IN.TexCoord);
	
    return float4(rslt, 1);
}