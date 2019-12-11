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

uniform int renderOutput;
uniform float gamma;
uniform float exposure;
uniform bool customMaterialValuesEnabled;
uniform float customMetalness;
uniform float customRoughness;

// These variables control the shape of the highlight along the tangent and bitangent vectors.
uniform int n_u;
uniform int n_v;

// ---------------------------------------------
//              --- Constants ---
// ---------------------------------------------
const float Pi = 3.1415926f;
const float HalfPi = 1.5707963f;

// The number 0.04 is the average reflective value of most dielectrics. It produces plausible results without the additional effort 
// to maintain a database for all materials (and determine which fragment is made of which material).	
const float DefaultFZeroDielectric = 0.04;

// ---------------------------------------------
//       --- Common shader functions ---
// ---------------------------------------------

vec3 GetNormalFromMap(const vec3 n, const vec3 t, const vec3 b);
float CalculateShadowCoverage(const vec4 fragPosLightSpace, const vec3 lightPos, const vec3 n, const int numberOfShadowMap);
vec4 ChooseRenderOutput(const vec3 albedo, const float ao, const vec2 brdfLookup, const vec3 color, const vec3 emissive, const float metallic, const float roughness);
vec3 fresnelSchlick(const float cosTheta, const vec3 F0);
vec3 fresnelSchlickRoughness(const float cosTheta, const vec3 F0, const float roughness);

// ---------------------------------------------
//            --- Main workflow ---
// ---------------------------------------------

void main()
{	
	// There is only one texture for AO, roughness and metallic parameters	
	float ao = 0.0f;	
	if(textureRoughnessAvailable)
	{
		ao = texture(textureRoughness[0], TexCoords).r;
	}	

	float roughness = 0.0f;
	float metallic = 0.0f;

	if(customMaterialValuesEnabled)
	{
		metallic = customMetalness;
		roughness = customRoughness;		
	}
	else
	{
		if(textureRoughnessAvailable)
		{
			roughness = texture(textureRoughness[0], TexCoords).g;
			metallic  = texture(textureRoughness[0], TexCoords).b; 	
		}		
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
	
	float nDotV = dot(n, v);	
	float clampedNdotV = max(nDotV, 0.0f);

	// Multiply n_u und n_v by 10 (lower values provide better handling in UI)
	int scaledN_u = n_u * 10;
	int scaledN_v = n_v * 10;

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

		float lDotV = dot(l, v);
		float clampedLDotV = max(lDotV, 0.0f);

		vec3 h = normalize(v + l);	
		float vdotH = dot(v, h);		

		float nDotH = dot(n, h);
		float clampedNDotH = max(nDotH, 0.0f);

		float lightDistanceToFragment = length(lightPositions[i] - WorldPos);
        float attenuation = 1.0f / (lightDistanceToFragment * lightDistanceToFragment);				
       
	   	float shadowCoverage = 0.0f;		
		if(textureShadowsAvailable)
		{
			shadowCoverage = CalculateShadowCoverage(lightSpaceMatrices[i] * vec4(WorldPos, 1), lightPositions[i], n, i);
		}
		
        vec3 radiance = (1.0f - shadowCoverage) * lightColors[i] * attenuation;

		// Calculate reflectance at normal incidence; if dielectric (like plastic) use F0 
		// of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow).	
		vec3 F0 = vec3(DefaultFZeroDielectric); 
		F0 = mix(F0, albedo, metallic);

		vec3 F = fresnelSchlick(vdotH, F0);				
        
        // kS is equal to Fresnel        
		vec3 kS = F;

        // For energy conservation, the diffuse and specular light can't be above 1.0 (unless the surface emits light); 
		// to preserve this relationship the diffuse component (kD) should equal 1.0 - kS.
        vec3 kD = vec3(1.0f) - kS;
        
		// Multiply kD by the inverse metalness such that only non-metals have diffuse lighting, or a linear blend if partly metal (pure metals have no diffuse light).
        kD *= 1.0 - metallic;	        

		vec3 diffuseTerm = ((28.0f * albedo) / (23.0f * Pi)) * kD * (1.0f - pow(1.0f - nDotL * 0.5f, 5)) * (1.0f - pow(1.0f - nDotV * 0.5f, 5));		
		
		float specularTermFirstPart = sqrt((scaledN_u + 1) * (scaledN_v + 1)) / (8 * Pi);				

		// Gram-Schmidt process to re-orthogonalize the vectors	(Normal-Mapping could have transformed the vectors so that they are not perpendicular anymore).		
		vec3 gramSchmidtTangent = normalize(Tangent - dot(Tangent, n) * n);	
		vec3 gramSchmidtBitangent = cross(n, gramSchmidtTangent);

		float hDotTangent = dot(h, gramSchmidtTangent);
		float hDotBitangent = dot(h, gramSchmidtBitangent);	

		float specularTermSecondPartNominator = pow(clampedNDotH, (scaledN_u * pow(hDotBitangent, 2) + scaledN_v * pow(hDotTangent, 2)) / (1 - pow(nDotH, 2)));	
		float specularTermSecondPartDenominator = vdotH * max(nDotL, vdotH);	

		vec3 specularTerm = specularTermFirstPart * (specularTermSecondPartNominator / specularTermSecondPartDenominator) * F;

		Lo += (diffuseTerm + specularTerm) * radiance * nDotL;		
	}

	// Ambient lighting
	// If an irradiance map (skybox) is available, use this texture to calculate ambient light strength.
	// If not, use an arbitrary number as ambient strength.			
	vec2 brdfLookup  = vec2(0.0f);
	if(textureBRDFLookupAvailable)
	{
		brdfLookup = texture(textureBRDFLookup[0], vec2(max(nDotV, 0.0), roughness)).rg;
	}

	vec3 ambient = vec3(0.0f);	
	if(textureIrradianceAvailable)
	{
		vec3 r = reflect(-1 * v, n); 
		
		// Calculate reflectance at normal incidence; if dielectric (like plastic) use F0 
		// of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow).	
		vec3 F0 = vec3(DefaultFZeroDielectric); 
		F0 = mix(F0, albedo, metallic);

		vec3 kSAmbient = fresnelSchlickRoughness(max(nDotV, 0.0f), F0, roughness);
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