#version 330 core

layout (location = 0) out vec4 FragColor;

in vec3 localPos;

uniform sampler2D textureEquirectangular;

// 0.1591f = 1 / (Pi * Pi)
// 0.3183f = 1 / Pi
const vec2 invAtan = vec2(0.1591f, 0.3183f);

vec2 SampleSphericalMap(const vec3 v)
{
	// See http://paulbourke.net/miscellaneous/cubemaps/ 
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;

	// Values are now in the range [-0.5, 0.5] - UV coordinates need to be in [0, 1] so we add 0.5
    uv += 0.5f;
    return uv;
}

void main()
{		
    vec2 uv = SampleSphericalMap(normalize(localPos));
    vec3 color = texture(textureEquirectangular, uv).rgb;
    
    FragColor = vec4(color, 1.0f);
}