#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out vec3 WorldPos;
out vec3 Normal;
out vec2 TexCoords;
out vec3 Tangent;
out vec3 Bitangent;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{	
	Normal = normalize(vec3(model * vec4(aNormal, 0.0f)));	
	Tangent = normalize(vec3(model * vec4(aTangent, 0.0f)));	
	Bitangent = normalize(vec3(model * vec4(aBitangent, 0.0f)));

    TexCoords = aTexCoords;    
	WorldPos = vec3(model * vec4(aPos, 1.0));
    gl_Position =  projection * view * vec4(WorldPos, 1.0);
}