#version 330 core

layout (location = 0) out vec4 FragColor;

in vec2 TexCoords;
in vec3 WorldPos;
in vec3 Normal;
in vec3 Tangent;
in vec3 Bitangent;

// ---------------------------------------------
//        --- Material parameters ---
// ---------------------------------------------
uniform sampler2D textureDiffuse[1];
uniform bool textureDiffuseAvailable;

uniform sampler2D textureNormal[1];
uniform bool textureNormalAvailable;

uniform sampler2D textureRoughness[1];
uniform bool textureRoughnessAvailable;

uniform sampler2D textureEmissive[1];
uniform bool textureEmissiveAvailable;

uniform samplerCube textureIrradiance[1];
uniform bool textureIrradianceAvailable;

uniform samplerCube texturePreFilterEnvironment[1];
uniform bool texturePrefilteredEnvironmentAvailable;

uniform sampler2D textureBRDFLookup[1];
uniform bool textureBRDFLookupAvailable;

// ---------------------------------------------
//                --- Lights ---
// ---------------------------------------------
uniform vec3 lightPositions[4];
uniform bool isLightActive[4];
uniform vec3 lightColors[4];

// ---------------------------------------------
//                --- Shadows ---
// ---------------------------------------------
uniform bool shadowsEnabled;
uniform sampler2D textureShadows[4];
uniform bool textureShadowsAvailable;
uniform mat4 lightSpaceMatrices[4];

// ---------------------------------------------
//                --- Camera ---
// ---------------------------------------------
uniform vec3 camPos;

// ---------------------------------------------
//             --- User settings ---
// ---------------------------------------------
uniform float subsurface;
uniform float metallic;
uniform float specular;
uniform float specularTint;
uniform float roughness;
uniform float anisotropic;
uniform float sheen;
uniform float sheenTint;
uniform float clearcoat;
uniform float clearcoatGloss;

uniform int renderOutput;
uniform float gamma;
uniform float exposure;

// ---------------------------------------------
//              --- Constants ---
// ---------------------------------------------
const float Pi = 3.1415926f;
const float HalfPi = 1.5707963f;

// The number 0.04 is the average reflective value of most dielectrics. It produces plausible results without the additional effort 
// to maintain a database for all materials (and determine which fragment is made of which material).	
const float DefaultFZeroDielectric = 0.04f;

// ---------------------------------------------
//       --- Common shader functions ---
// ---------------------------------------------

vec3 GetNormalFromMap(const vec3 n, const vec3 t, const vec3 b);
float CalculateShadowCoverage(const vec4 fragPosLightSpace, const vec3 lightPos, const vec3 n, const int numberOfShadowMap);
vec4 ChooseRenderOutput(const vec3 albedo, const float ao, const vec2 brdfLookup, const vec3 color, const vec3 emissive, const float metallic, const float roughness);
vec3 fresnelSchlickRoughness(const float cosTheta, const vec3 F0, const float roughness);
vec3 fresnelSchlick(const float cosTheta, const vec3 F0);
void GetAnisotropicParameters(out float a_x, out float a_y, const float anisotropic, const float roughness);

float SeparableSmith_GGX_Anisotropic(const vec3 v, const vec3 l, const vec3 n, const vec3 t, const vec3 b, const float anisotropic, const float roughness);
float TrowbridgeReitzGGX_Anisotropic(	const vec3 n, 
										const vec3 h, 
										const vec3 t,
										const vec3 b,
										const float anisotropic, 
										const float roughness);

float GTR1_Clearcoat(const float NdotH, const float a)
{
    if (a >= 1) 
	{
		return 1 / Pi;
	}	

    float a2 = a * a;
    float t = 1 + (a2 - 1) * pow(NdotH, 2);
    return (a2 - 1) / (Pi * log(a2) * t);
}

float SmithG1_GGX_Clearcoat(const float NdotV, const float roughness)
{    
    float nDotV2 = NdotV * NdotV;
    return 1 / (NdotV + sqrt(roughness + nDotV2 - roughness * nDotV2));
}

void main()
{
	// There is only one texture for AO, roughness and metallic parameters	
	float ao = 0.0f;	
	if(textureRoughnessAvailable)
	{
		ao = texture(textureRoughness[0], TexCoords).r;
	}		
	
	vec3 albedo = vec3(0.0f);
	if(textureDiffuseAvailable)
	{
		// Convert texture in sRGB format to linear space
		albedo = pow(texture(textureDiffuse[0], TexCoords).rgb, vec3(gamma));			
	}	
	
	// Not every model has specified an emissive texture
	vec3 emissive = vec3(0.0f);	
	if(textureEmissiveAvailable)
	{
		emissive = texture(textureEmissive[0], TexCoords).rgb;
	}	

    vec3 n = Normal;
	if(textureNormalAvailable)
	{
		n = GetNormalFromMap(Normal, Tangent, Bitangent);
	}

    vec3 v = normalize(camPos - WorldPos);	
	vec3 r = reflect(-1.0f * v, n); 
	
	float nDotV = dot(n, v);	
	float clampedNDotV = max(nDotV, 0.0f);

	float roughness2 = pow(roughness, 2);

	 // Calculate reflectance at normal incidence; if dielectric (like plastic) use F0 
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow).	
    vec3 F0 = vec3(DefaultFZeroDielectric); 
    F0 = mix(F0, albedo, metallic);

	// Gram-Schmidt process to re-orthogonalize the vectors	(Normal-Mapping could have transformed the vectors so that they are not perpendicular anymore).		
	vec3 gramSchmidtTangent = normalize(Tangent - dot(Tangent, n) * n);	
	vec3 gramSchmidtBitangent = cross(n, gramSchmidtTangent);

	// The implementation of the BRDF is based on the paper "Physically-Based Shading at Disney" from Brent Burley and 
	// the source code of the Disney BRDF Explorer: https://github.com/wdas/brdf

	// Rendering equation
    vec3 Lo = vec3(0.0f);    
	for(int i = 0; i < lightPositions.length(); ++i) 
    {
		if(false == isLightActive[i])
		{
			continue;
		}
        
        vec3 l = normalize(lightPositions[i] - WorldPos);
		float nDotL = dot(n, l);		

		// The BRDF is only defined for light positions above the surface.
		// See RealTimeRendering 4th edition - p. 312
		if(nDotL <= 0.0f)
		{
			continue;
		}

        vec3 h = normalize(v + l);		
		float lDotH = dot(l, h);	

		float nDotH = dot(n, h);
		float clampedNDotH = max(nDotH, 0.0f);		
        
		// Calculate per-light radiance based on distance, light strength and shadow calculation.
		float lightDistanceToFragment = length(lightPositions[i] - WorldPos);
        float attenuation = 1.0f / (lightDistanceToFragment * lightDistanceToFragment);				
       
	   	float shadowCoverage = 0.0f;		
		if(textureShadowsAvailable)
		{
			shadowCoverage = CalculateShadowCoverage(lightSpaceMatrices[i] * vec4(WorldPos, 1.0f), lightPositions[i], n, i);
		}
		
        vec3 radiance = (1.0f - shadowCoverage) * lightColors[i] * attenuation;

		// BRDF

		// Luminance approximation - see "Digital Image Processing 2nd edition - pp. 304-305"
		float Cdlum = 0.3f * albedo.r + 0.6f * albedo.g + 0.1f * albedo.b; 

		// Normalize luminance to isolate hue + saturation
		// Cdlum contains the luminance; Ctint the hue
		vec3 Ctint = Cdlum > 0.0f ? albedo / Cdlum : vec3(1.0f); 

		vec3 Cspec0 = mix(specular * 0.08f * mix(vec3(1.0f), Ctint, specularTint), albedo, metallic);
		vec3 Csheen = mix(vec3(1.0f), Ctint, sheenTint);

		float D = TrowbridgeReitzGGX_Anisotropic(n, h, gramSchmidtTangent, gramSchmidtBitangent, anisotropic, roughness);   		
		float F_lh = pow(1.0f - lDotH, 5);
		vec3 Fs = mix(Cspec0, vec3(1.0f), F_lh);        
        float G = SeparableSmith_GGX_Anisotropic(v, l, n, gramSchmidtTangent, gramSchmidtBitangent, anisotropic, roughness);       	

		// Diffuse fresnel - go from 1 at normal incidence to 0.5 at grazing angles based on roughness		
		float Fd90 = 0.5f + 2 * pow(lDotH, 2) * roughness2;
		float F_ln = pow(1.0f - nDotL, 5);
		float F_vn = pow(1.0f - clampedNDotV, 5);		
		vec3 Fd = (albedo / Pi) * (1 + (Fd90 - 1) * F_ln) * (1 + (Fd90 - 1) * F_vn);

		// Based on Hanrahan-Krueger brdf approximation of isotropic bssrdf
		// 1.25 scale is used to (roughly) preserve albedo
		// Fss90 used to "flatten" retroreflection based on roughness
		float Fss90 = pow(lDotH, 2) * roughness2;
		vec3 Fss = mix(vec3(1.0f), vec3(Fss90), F_ln) * mix(vec3(1.0f), vec3(Fss90), F_vn);
		vec3 ss = 1.25f * (Fss * (1 / (nDotL + clampedNDotV) - 0.5f) + 0.5f);
		
		float a_x = 0.0f;
		float a_y = 0.0f;
		GetAnisotropicParameters(a_x, a_y, anisotropic, roughness);	         		

		// Clearcoat uses a second specular lobe on top of the base material which is always isotropic and dieletric.		
		float D_Clearcoat = GTR1_Clearcoat(clampedNDotH, mix(0.1f, 0.001f, clearcoatGloss));
		float F_Clearcoat = mix(DefaultFZeroDielectric, 1.0f, F_lh);

		// Clearcoat roughness never changes (0.25 * 0.25)
		const float staticRoughnessForClearcoat = 0.0625f;
		float G_Clearcoat = SmithG1_GGX_Clearcoat(nDotL, staticRoughnessForClearcoat) * SmithG1_GGX_Clearcoat(clampedNDotV, staticRoughnessForClearcoat);

		vec3 Fsheen = F_lh * sheen * Csheen;
		vec3 diffuse = (mix(Fd, ss, subsurface) * albedo + Fsheen) * (1.0f - metallic);
		vec3 specular = D * Fs * G;
		float clearcoat = staticRoughnessForClearcoat * clearcoat * D_Clearcoat * F_Clearcoat * G_Clearcoat;
		Lo += (diffuse + specular + clearcoat) * radiance * nDotL;
	}

	// Ambient lighting
	// If an irradiance map (skybox) is available, use this texture to calculate ambient light strength.
	// If not, use an arbitrary number as ambient strength.			
	vec2 brdfLookup  = vec2(0.0f);
	if(textureBRDFLookupAvailable)
	{
		brdfLookup = texture(textureBRDFLookup[0], vec2(max(dot(n, v), 0.0f), roughness)).rg;
	}

	vec3 ambient = vec3(0.0f);	
	if(textureIrradianceAvailable)
	{
		vec3 kSAmbient = fresnelSchlickRoughness(max(dot(n, v), 0.0f), F0, roughness);
		vec3 kDAmbient = 1.0f - kSAmbient;
		kDAmbient *= 1.0f - metallic;		
		
		const float maximumLevelOfDetail = 4.0f;
		vec3 prefilteredColor = textureLod(texturePreFilterEnvironment[0], r,  roughness * maximumLevelOfDetail).rgb;    		
		vec3 specular = prefilteredColor * (kSAmbient * brdfLookup.x + brdfLookup.y);

		vec3 irradiance = texture(textureIrradiance[0], n).rgb;
		ambient = (kDAmbient * irradiance * albedo + specular) * ao;		
	}
	else
	{
		ambient = (vec3(0.01f) * albedo) * ao; 
	}
	
    vec3 color = emissive + ambient + Lo;
	
	// Gamma correction
    color = pow(color, vec3(1.0f / gamma));

    // Exposure tone mapping
    color = vec3(1.0f) - exp(-color * exposure);   	 	

    FragColor = ChooseRenderOutput(albedo, ao, brdfLookup, color, emissive, metallic, roughness );
}