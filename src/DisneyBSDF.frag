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

// The same term is used in the Frostbite engine (see SIGGRAPH 2014 - Moving Frostbite to PBR)
const float PreventDivideByZeroTerm = 0.00001f;

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

float SeparableSmith_GGX_Anisotropic(const vec3 v, const vec3 l, const vec3 n, const vec3 t, const vec3 b, const float roughness);
float TrowbridgeReitzGGX_Anisotropic(	const vec3 n, 
										const vec3 h, 
										const vec3 t,
										const vec3 b,
										const float anisotropic, 
										const float roughness);

// ---------------------------------------------
//                 --- Sheen ---
// ---------------------------------------------

vec3 CalculateTint(vec3 baseColor)
{
    // -- The color tint is never mentioned in the SIGGRAPH presentations as far as I recall but it was done in
    // --  the BRDF Explorer so I'll replicate that here.
	float luminance = 0.3f * baseColor.x + 0.6f * baseColor.y + 0.1f * baseColor.z;
	if(luminance < 0.0f)
	{
		return vec3(1.0f);
	}

    return baseColor * (1.0f / luminance);
}

vec3 EvaluateSheen(const float xDotH, const vec3 albedo)
{
    if(sheen <= 0.0f) 
	{
        return vec3(0.0f);
    }
   
    vec3 tint = CalculateTint(albedo);
    return sheen * ((1 - sheenTint) + sheenTint * tint) * (1 - pow(xDotH, 5));
}

// ---------------------------------------------
//               --- Clearcoat ---
// ---------------------------------------------

float Distribution_Clearcoat(float xDotH, float roughness)
{
    if(roughness >= 1.0f) 
	{
        return 1.0f / Pi;
    }

    float roughness2 = roughness * roughness;
    return (roughness2 - 1.0f) / (Pi * log2(roughness2) * (1.0f + (roughness2 - 1.0f) * pow(xDotH, 2)));
}

vec3 EvaluateDisneyClearcoat(const vec3 v, const vec3 l, const vec3 n, const vec3 t, const vec3 b, const vec3 h, const float roughness)
{
    if(clearcoat <= 0.0f) 
	{
        return vec3(0.0f);
    }
	
	const float staticRoughness = 0.25;	

	float xDotH = dot(v, h);

    float D = Distribution_Clearcoat(xDotH, mix(0.1f, 0.001f, roughness));
    vec3 F = fresnelSchlick(xDotH, vec3(DefaultFZeroDielectric));
    float G = SeparableSmith_GGX_Anisotropic(v, l, n, t, b, staticRoughness); // static roughness of 0.25f used for the clearcoat calculation  

    return staticRoughness * clearcoat * D * F * G;
}

float ThinTransmissionRoughness(float ior, float roughness)
{
    // -- Disney scales by (.65 * eta - .35) based on figure 15 of the 2015 PBR course notes. Based on their figure
    // -- the results match a geometrically thin solid fairly well.
    return max((0.65f * ior - 0.35f) * roughness, 0.0f);
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

	 // Calculate reflectance at normal incidence; if dielectric (like plastic) use F0 
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow).	
    vec3 F0 = vec3(DefaultFZeroDielectric); 
    F0 = mix(F0, albedo, metallic);

	// Gram-Schmidt process to re-orthogonalize the vectors	(Normal-Mapping could have transformed the vectors so that they are not perpendicular anymore).		
	vec3 gramSchmidtTangent = normalize(Tangent - dot(Tangent, n) * n);	
	vec3 gramSchmidtBitangent = cross(n, gramSchmidtTangent);

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
        
		// Calculate per-light radiance based on distance, light strength and shadow calculation.
		float lightDistanceToFragment = length(lightPositions[i] - WorldPos);
        float attenuation = 1.0f / (lightDistanceToFragment * lightDistanceToFragment);				
       
	   	float shadowCoverage = 0.0f;		
		if(textureShadowsAvailable)
		{
			shadowCoverage = CalculateShadowCoverage(lightSpaceMatrices[i] * vec4(WorldPos, 1), lightPositions[i], n, i);
		}
		
        vec3 radiance = (1.0f - shadowCoverage) * lightColors[i] * attenuation;

		// BRDF
		// Diffuse
		vec3 lambertDiffuse = albedo / Pi;
		float F_L = pow(1 - nDotL, 5);
		float F_V = pow(1 - clampedNDotV, 5);
		float RR = 2 * roughness * pow(lDotH, 2);		
		vec3 retroReflection = lambertDiffuse * RR * (F_L + F_V + F_L * F_V * (RR - 1.0f));
		vec3 diffuseTerm = lambertDiffuse * (1.0f - 0.5 * F_L) * (1.0f - 0.5 * F_V) + retroReflection;
		
		// Specular
		float D = TrowbridgeReitzGGX_Anisotropic(n, h, gramSchmidtTangent, gramSchmidtBitangent, anisotropic, roughness);   		
		vec3 F = fresnelSchlick(lDotH, F0);		        
        float G = SeparableSmith_GGX_Anisotropic(v, l, n, gramSchmidtTangent, gramSchmidtBitangent, roughness);        		
        vec3 nominator = D * G * F;
	
        float denominator = 4 * nDotL * clampedNDotV + PreventDivideByZeroTerm;
		vec3 specular  = nominator / denominator;		
		
		vec3 BRDF = diffuseTerm + specular;		    

		// BTDF
		// The Disney BSDF paper is not sufficent on its own to implement the complete BSDF.
		// TODO: Find reference implementation
		vec3 BTDF = vec3(0.0f);	
			
		Lo += (BRDF + BTDF) * radiance * nDotL;  		
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

