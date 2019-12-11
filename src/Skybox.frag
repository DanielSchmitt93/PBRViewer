#version 330 core

layout (location = 0) out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube textureEnvironment;

// ---------------------------------------------
//             --- User settings ---
// ---------------------------------------------
uniform int mipMapLevel;
uniform float gamma;
uniform float exposure;

void main()
{		
    vec3 envColor = textureLod(textureEnvironment, TexCoords, mipMapLevel).rgb;
    
    // Gamma correction
	envColor = pow(envColor, vec3(1.0f / gamma)); 
	
    // Exposure tone mapping
    envColor = vec3(1.0f) - exp(-envColor * exposure);    

	FragColor = vec4(envColor, 1.0f);
}
