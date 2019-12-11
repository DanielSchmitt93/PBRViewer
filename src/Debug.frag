#version 330 core

layout (location = 0) out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;  
in vec3 WorldPos;
in vec3 Tangent;
in vec3 Bitangent;

// ---------------------------------------------
//        --- Material parameters ---
// ---------------------------------------------
uniform sampler2D textureNormal[1];
uniform bool textureNormalAvailable;

uniform sampler2D textureRoughness[1];
uniform bool textureRoughnessAvailable;

// ---------------------------------------------
//                --- Camera ---
// ---------------------------------------------
uniform vec3 camPos; 

// ---------------------------------------------
//                --- Lights ---
// ---------------------------------------------
uniform vec3 lightPositions[4];
uniform bool isLightActive[4];
uniform vec3 lightColors[4];

// ---------------------------------------------
//             --- User settings ---
// ---------------------------------------------
uniform int debugOutput;

// ---------------------------------------------
//              --- Constants ---
// ---------------------------------------------
const float Pi = 3.14159265359;
const float PreventDivideByZeroTerm = 0.001;

// ---------------------------------------------
//       --- Common shader functions ---
// ---------------------------------------------

vec3 GetNormalFromMap(const vec3 n, const vec3 t, const vec3 b);

// ---------------------------------------------
//       --- Weak white furnance test ---
// ---------------------------------------------

float SmithG1(const float vDotH, const float roughness)
{
	// For the Weak White Furnance Test we care only about the masking function and not about the shadowing function.
	// dot(v, h) can never exceed 90 degrees, so we do not need to check for negative dot product.

	// From Disney: Squared roughness		
	float alpha = roughness * roughness;
	float a_v = vDotH / (alpha * sqrt(1 - pow(vDotH, 2)));	

	// The lambda value differs for each NDF. The procedure to derive lambda for a given NDF is described in publications like 
	// "Understanding the Masking-Shadowing Function in Microfacet-Based BRDFs" by Eric Heitz.	
	// The value here is from "Real Time Rendering 4th ed. p. 341". 
	// The same value is used in the MATLAB code from Eric Heitz (Appendix C) in the beforementioned paper.
	float lambda_GGX_v = (-1.0f + sqrt(1 + 1 / pow(a_v, 2))) / 2;
	return 1.0f / (1.0f + lambda_GGX_v);
}

float TrowbridgeReitzGGX(const vec3 n, const vec3 h, const float roughness)
{
	float nDotH = dot(n, h);
	if(nDotH <= 0.0f)
	{
		return 0.0f;
	}

    float nDotH2 = nDotH * nDotH;

	// From Disney: Squared roughness (most applications that use the GGX distribution adopted this approach).
    float a = roughness * roughness;
    float a2 = a * a;	

    float nom = a2;
    float denom = (1.0f + nDotH2 * (a2 - 1.0f));
    denom = Pi * denom * denom;

    return nom / denom;
}

// This implementation is based on the MATLAB code from Eric Heitz.
float PerformWeakWhiteFurnanceTestGGX(const vec3 n, const vec3 v)
{
	float roughness = 0.0f;	
	if(textureRoughnessAvailable)
	{
		roughness = texture(textureRoughness[0], TexCoords).g;						
	}		

	float dtheta = 0.05f;
	float dphi = 0.05f;

	float integral = 0.0f;
	for(float theta = 0; theta < Pi; theta += dtheta)
	{
		for(float phi = 0; phi < 2 * Pi; phi += dphi)
		{
			vec3 l = vec3(cos(phi) * sin(theta), sin(phi) * sin(theta), cos(theta));
			vec3 h = normalize(l + v);
			float vDotH = dot(v, h);				

			integral += sin(theta) * TrowbridgeReitzGGX(n, h, roughness) * SmithG1(vDotH, roughness) / abs(4 * vDotH);			
		}
	}

	return integral * dphi * dtheta;
}

// ---------------------------------------------
//            --- Main workflow ---
// ---------------------------------------------

void main()
{
	vec3 n = Normal;
	if(textureNormalAvailable)
	{
		n = GetNormalFromMap(Normal, Tangent, Bitangent);
	}

	vec3 v = normalize(camPos - WorldPos);	

	// Gram-Schmidt process to re-orthogonalize the vectors	(Normal-Mapping could have transformed the vectors so that they are not perpendicular anymore).		
	vec3 gramSchmidtTangent = normalize(Tangent - dot(Tangent, n) * n);	
	vec3 gramSchmidtBitangent = cross(n, gramSchmidtTangent);

	// Tangent, Bitangent and normal vector output is not dependent on light vector, so we can return early.
	// For case numbers, see PBRViewerEnumerations::DebugOutput
	switch(debugOutput)
	{
		case 1:
			float nDotV = dot(n, v);
			if(nDotV < 0.0f)
			{
				FragColor = vec4(vec3(1.0f, 0.0f, 0.0f), 1.0f);	
				return;
			}
			FragColor = vec4(0.0f);			
			return;
		case 2:
			FragColor = vec4(n, 1.0f);	
			return;
		case 3:
			FragColor = vec4(gramSchmidtTangent, 1.0f);
			return;
		case 4:
			FragColor = vec4(gramSchmidtBitangent, 1.0f);
			return;
		case 5:
			FragColor = vec4(vec3(PerformWeakWhiteFurnanceTestGGX(n, v)), 1.0f);	
			return;
	}	

	// Handle cases depending on the light vectors (if any).
	// The output color equals the resulting operation performed with the first light switched on.
	// Multiple lights are not supported currently.
	for(int i = 0; i < lightPositions.length(); ++i)
	{
		if(false == isLightActive[i])
		{
			continue;
		}

		vec3 l = normalize(lightPositions[i] - WorldPos);			
		
		// For case numbers, see PBRViewerEnumerations::DebugOutput
		switch(debugOutput)
		{			
			case 0:			
				float nDotL = dot(n, l);
				if(nDotL < 0.0f)
				{
					FragColor = vec4(vec3(1.0f, 0.0f, 0.0f), 1.0f);	
					return;
				}
				FragColor = vec4(0.0f);	
				return;			
		}
	}	
}