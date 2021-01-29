cbuffer ModelViewProjection : register(b0)
{
    matrix Model;
    matrix MVP;
};


struct Vertex
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float2 TexCoord : TEXCOORD;
};

struct VertexShaderOutput
{
    float3 Normal : NORMAL;
    float2 TexCoord : TEXCOORD;
    float4 WorldPos : POSITION;
    float4 Position : SV_POSITION;

};

VertexShaderOutput main(Vertex IN)
{
    VertexShaderOutput OUT;
	
    OUT.Position = mul(MVP, float4(IN.Position, 1.0f));
    OUT.WorldPos = mul(Model, float4(IN.Position, 1.0f));
    OUT.Normal = IN.Normal;
    OUT.TexCoord = IN.TexCoord;
    return OUT;
}