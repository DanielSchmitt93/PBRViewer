#version 330 core

layout (location = 0) out vec4 FragColor;

in vec2 TexCoords;
in vec3 WorldPos;
in vec3 Normal;
in vec3 Tangent;
in vec3 Bitangent;

uniform sampler2D textureDiffuse[1];
uniform bool textureDiffuseAvailable;

uniform sampler2D textureEmissive[1];
uniform bool textureEmissiveAvailable;

uniform vec3 lightColor;

void main()
{
	vec3 emissive = vec3(0.0f);
	if(textureEmissiveAvailable)
	{
		emissive = texture(textureEmissive[0], TexCoords).rgb;		
	}	

	vec3 albedo = vec3(0.0f);
	if(textureDiffuseAvailable)
	{		
		albedo = texture(textureDiffuse[0], TexCoords).rgb;			
	}		

	// The light color should only alter the light bulb and not the complete mesh.
	// If we have an emissive value != 0 we know that this fragment is within the light bulb.
	// But the albedo value within the light bulb is also 1.
	// In this case the emissive value will be subtracted so that the albedo has no influence anymore.
	// The second part of the term will take over and set the light color accordingly.
	// Additionally, we need to divide by 5 because the light strength is set to 5 globally (see PBRViewerModel.cpp).
	FragColor = vec4(albedo - emissive + emissive * lightColor * 0.2f , 1.0f);    
}