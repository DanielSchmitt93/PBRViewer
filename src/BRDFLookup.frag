#version 330 core

layout (location = 0) out vec2 FragColor;

in vec2 TexCoords;

// ---------------------------------------------
//              --- Constants ---
// ---------------------------------------------
const float Pi = 3.1415926f;

// ---------------------------------------------
//       --- Common shader functions ---
// ---------------------------------------------
vec3 ImportanceSampleGGX(const uint iteration, const uint sampleAmout, const vec3 N, const float roughness);
float SeparableSchlick_GGX(const float nDotL, const float nDotV, const float roughness);

vec2 IntegrateBRDF(const float nDotV, const float roughness)
{
    vec3 V;
    V.x = sqrt(1.0f - nDotV * nDotV);
    V.y = 0.0f;
    V.z = nDotV;

    float A = 0.0f;
    float B = 0.0f; 

    const vec3 N = vec3(0.0f, 0.0f, 1.0f);
    
    const uint sampleAmount = 1024u;
    for(uint i = 0u; i < sampleAmount; ++i)
    {       
        vec3 H = ImportanceSampleGGX(i, sampleAmount, N, roughness);
        vec3 L = normalize(2.0f * dot(V, H) * H - V);

        float nDotL = max(L.z, 0.0f);
        float nDotH = max(H.z, 0.0f);
        float vDotH = max(dot(V, H), 0.0f);

        if(nDotL > 0.0f)
        {
            float G = SeparableSchlick_GGX(nDotV, nDotL, roughness);
            float G_Vis = (G * vDotH) / (nDotH * nDotV);
            float Fc = pow(1.0f - vDotH, 5.0f);

            A += (1.0f - Fc) * G_Vis;
            B += Fc * G_Vis;
        }
    }

    A /= float(sampleAmount);
    B /= float(sampleAmount);
    return vec2(A, B);
}

void main() 
{
    vec2 integratedBRDF = IntegrateBRDF(TexCoords.x, TexCoords.y);
    FragColor = integratedBRDF;
}