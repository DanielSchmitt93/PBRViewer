#version 330 core

layout (location = 0) out vec4 FragColor;

in vec3 WorldPos;

uniform samplerCube textureEnvironmentMap;
uniform int cubemapFaceResolution;

// ---------------------------------------------
//        --- Material parameters ---
// ---------------------------------------------
uniform float roughness;

// ---------------------------------------------
//              --- Constants ---
// ---------------------------------------------
const float Pi = 3.1415926f;

// The same term is used in the Frostbite engine (see SIGGRAPH 2014 - Moving Frostbite to PBR)
const float PreventDivideByZeroTerm = 0.00001f;

// ---------------------------------------------
//       --- Common shader functions ---
// ---------------------------------------------
float TrowbridgeReitzGGX(const vec3 n, const vec3 h, const float roughness);
vec3 ImportanceSampleGGX(const uint iteration, const uint sampleAmout, const vec3 N, const float roughness);

void main()
{		
    vec3 n = normalize(WorldPos);
    
    // Make the simplyfying assumption that v equals r equals n (adopted from "Real Shading in Unreal Engine 4").
    vec3 r = n;
    vec3 v = r;

    const uint sampleAmount = 1024u;
    vec3 prefilteredColor = vec3(0.0f);
    float totalWeight = 0.0f;
    
    for(uint i = 0u; i < sampleAmount; ++i)
    {
        // Generates a sample vector that's biased towards the preferred alignment direction (importance sampling).       
        vec3 h = ImportanceSampleGGX(i, sampleAmount, n, roughness);
        vec3 l  = normalize(2.0f * dot(v, h) * h - v);

        float nDotL = dot(n, l);
        if(nDotL > 0.0f)
        {
            // Sample from the environment's mip level based on roughness/pdf
			// For details see section "20.4 Mipmap Filtered Samples" in the book "GPU Gems 3".
            float D = TrowbridgeReitzGGX(n, h, roughness);
            float nDotH = max(dot(n, h), 0.0f);
            float hDotV = dot(h, v);

            float pdf = D * nDotH / (4.0f * hDotV) + PreventDivideByZeroTerm;           
		    float omega_s = 1.0f / (sampleAmount * pdf + PreventDivideByZeroTerm);

			// The distortion term was adopted from Chetan Jags.
			// See https://chetanjags.wordpress.com/2015/08/26/image-based-lighting/
			float distortion = 4.0f * Pi;
            float omega_p  = distortion / (6.0f * cubemapFaceResolution * cubemapFaceResolution);          

            float mipLevel = roughness == 0.0f ? 0.0f : 0.5f * log2(omega_s / omega_p); 
            
            prefilteredColor += textureLod(textureEnvironmentMap, l, mipLevel).rgb * nDotL;
            totalWeight += nDotL;
        }
    }

    prefilteredColor = prefilteredColor / totalWeight;
    FragColor = vec4(prefilteredColor, 1.0f);
}