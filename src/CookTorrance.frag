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
uniform int diffuseTerm;
uniform int fresnelTerm;
uniform int normalDistributionTerm;
uniform int geometryTerm;

uniform int renderOutput;
uniform float gamma;
uniform float exposure;
uniform bool customMaterialValuesEnabled;
uniform float customMetalness;
uniform float customRoughness;

// ---------------------------------------------
//              --- Constants ---
// ---------------------------------------------
const float Pi = 3.1415926f;

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
vec3 ToPolar(const vec3 vectorToConvert);

// Common fresnel functions
vec3 fresnelSchlick(const float cosTheta, const vec3 F0);
vec3 fresnelSchlickRoughness(const float cosTheta, const vec3 F0, const float roughness);

// Common geometry functions
float SeparableSchlick_GGX(const float nDotL, const float nDotV, const float roughness);
float SeparableSmith_GGX(const float vDotN, const float lDotN, const float roughness);
float SeparableSmith_Beckmann(const float vDotN, const float lDotN, const float roughness);
float SmithHeightCorrelatedG2_GGX(const float xDotH, const float nDotV, const float nDotL, const float roughness);
float HeitzSmithHeightDirectionCorrelatedG2_GGX(const vec3 v, const vec3 l, const float nDotV, const float nDotL, const float roughness);

// Common normal distribution functions
float TrowbridgeReitzGGX(const vec3 n, const vec3 h, const float roughness);
float BeckmannNDF(const vec3 n, const vec3 h, const float roughness);
float BlinnPhongNDF(const vec3 n, const vec3 h, const float roughness);

// ---------------------------------------------
//         --- Fresnel term for BRDF ---
// ---------------------------------------------

// Get the fresnel result based on the term selected by the user.
vec3 calculateFresnel(const vec3 h, const vec3 v, const vec3 F0)
{
	// For case numbers, see PBRViewerEnumerations::FresnelTerm
	switch(fresnelTerm)
	{
		case 0:		
			// Returns F0 in order to eliminate the fresnel effect.
			return fresnelSchlick(1.0f, F0);
		case 1:
			return fresnelSchlick(dot(h, v), F0);
		default:
			return vec3(0.0f);
	}	
}

// ---------------------------------------------
//      --- Geometry function for BRDF ---
// ---------------------------------------------

// The parameter xDotH can be either dot(l, h) or dot(v, h). Both are equal.
float calculateGeometry(const vec3 v, const vec3 l, const float nDotL, const float nDotV, const float xDotH, const float roughness)
{
	// For case numbers, see PBRViewerEnumerations::GeometryTerm
	switch(geometryTerm)
	{
		case 0:
			// Arbitrary value as constant term.
			return 0.75f;
		case 1:
			// No G-model as described by Disney [2012]	
			return nDotL * nDotV; 		
		case 2:
			return SeparableSchlick_GGX(nDotL, nDotV, roughness);
		case 3:
			return SeparableSmith_GGX(nDotV, nDotL, roughness);			
		case 4:
			return SeparableSmith_Beckmann(nDotV, nDotL, roughness);
		case 5:
			return SmithHeightCorrelatedG2_GGX(xDotH, nDotV, nDotL, roughness);
		case 6:
			return HeitzSmithHeightDirectionCorrelatedG2_GGX(v, l, nDotV, nDotL, roughness);
		default:
			return 0.0f;
	}	
}

// ---------------------------------------------
// --- Normal distribution function for BRDF ---
// ---------------------------------------------

float calculateNormalDistribution(const vec3 n, const vec3 h, const float roughness)
{
	// For case numbers, see PBRViewerEnumerations::NormalDistributionTerm
	switch(normalDistributionTerm)
	{
		case 0:
			return 1.0f;
		case 1:
			return TrowbridgeReitzGGX(n, h, roughness);		
		case 2:
			return BeckmannNDF(n, h, roughness);
		case 3:
			return BlinnPhongNDF(n, h, roughness);		
		default:
			return 0.0f;
	}
}

// ---------------------------------------------
//         --- Diffuse term for BRDF ---
// ---------------------------------------------

// Get the fresnel result based on the term selected by the user.
vec3 calculateDiffuse(	const vec3 albedo, 
						const vec3 kD, 
						const float nDotL, 
						const float nDotV, 
						const float lDotH, 
						const float roughness,
						const vec3 F0)
{
	// For case numbers, see PBRViewerEnumerations::DiffuseTerm
	switch(diffuseTerm)
	{
		case 0:		
			vec3 noDiffuse = vec3(0.0f);
			return noDiffuse;
		case 1:
			vec3 lambertian = albedo / Pi;
			return lambertian;
		case 2:
			vec3 lambertianEnergyConserving = kD * (albedo / Pi);
			return lambertianEnergyConserving;
		case 3:
			// We use the base diffuse model from Disney [Burley 2012] without the subsurface scattering term.
			// The subsurface scattering term requires an additional parameter k_ss which blends between the subsurface- and diffuse term (see DisneyBRDF.frag)
			// This shader should not get any more complicated so we discarded this parameter.	
			// For more details to the disney approach please refer to the original paper from [2012].
			float fd90 = 0.5f + 2 * pow(lDotH, 2) * pow(roughness, 2);
			vec3 burleyDiffuse = (kD * albedo / Pi) * (1 + (fd90 - 1) * pow((1 - nDotL), 5)) * (1 + (fd90 - 1) * pow((1 - nDotV), 5));
			return burleyDiffuse;
		case 4:		
			// This equation is only valid for surfaces where the specular reflectance is that of a perfect Fresnel mirror.
			vec3 shirleyDiffuse = (21 / (20 * Pi)) * (1 - F0) * albedo * (1 - pow((1 - nDotL), 5)) * (1 - pow((1 - nDotV), 5));
			return shirleyDiffuse;
		case 5:
			// From the paper "An Anisotropic Phong BRDF Model" by Michael Ashikhmin and Peter Shirley.
			vec3 ashikhminShirleyDiffuse = ((28 * albedo) / (23 * Pi)) * kD * (1 - pow(1 - nDotL / 2, 5)) * (1 - pow(1 - nDotV / 2, 5));
			return ashikhminShirleyDiffuse;
		default:
			return vec3(0.0f);
	}	
}

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
	vec3 r = reflect(-1.0f * v, n); 

	float nDotV = dot(n, v);
	float clampedNdotV = max(nDotV, 0.0f);	

    // Calculate reflectance at normal incidence; if dielectric (like plastic) use F0 
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow).	
    vec3 F0 = vec3(DefaultFZeroDielectric); 
    F0 = mix(F0, albedo, metallic);

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

		vec3 F = calculateFresnel(h, v, F0);			
        float NDF = calculateNormalDistribution(n, h, roughness);   
        float G = calculateGeometry(v, l, nDotL, clampedNdotV, lDotH, roughness);        
        vec3 nominator = NDF * G * F; 		
		
        float denominator = 4 * nDotL * clampedNdotV + PreventDivideByZeroTerm;
		vec3 specular  = nominator / denominator;		     
        
        // kS is equal to Fresnel        
		vec3 kS = F;

        // For energy conservation, the diffuse and specular light can't
        // be above 1.0 (unless the surface emits light); to preserve this
        // relationship the diffuse component (kD) should equal 1.0 - kS.
        vec3 kD = vec3(1.0f) - kS;
        
		// Multiply kD by the inverse metalness such that only non-metals 
        // have diffuse lighting, or a linear blend if partly metal (pure metals
        // have no diffuse light).
        kD *= 1.0f - metallic;	        		

		vec3 diffuseTerm = calculateDiffuse(albedo, kD, nDotL, clampedNdotV, lDotH, roughness, F0);		
		Lo += (diffuseTerm + specular) * radiance * nDotL;  		
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