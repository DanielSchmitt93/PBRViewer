#version 330 core

layout (location = 0) out vec4 FragColor;

in vec3 WorldPos;

uniform samplerCube textureEnvironment;

// ---------------------------------------------
//              --- Constants ---
// ---------------------------------------------
const float Pi = 3.1415926f;
const float HalfPi = 1.5707963f;
const float DoublePi = 6.2831853f;

void main()
{		
	// The normal vector of the hemisphere of a fragment is the position of the fragment itself.
    vec3 N = normalize(WorldPos);

    vec3 irradiance = vec3(0.0f);   
    
    // tangent space calculation from origin point
    vec3 up = vec3(0.0f, 1.0f, 0.0f);
    vec3 right = cross(up, N);
    up  = cross(N, right);
       
    float sampleDelta = 0.025f;
    float nrSamples = 0.0f;
    for(float phi = 0.0f; phi < DoublePi; phi += sampleDelta)
    {
		float sinPhi = sin(phi);
		float cosPhi = cos(phi);

        for(float theta = 0.0f; theta < HalfPi; theta += sampleDelta)
        {
			float sinTheta = sin(theta);
			float cosTheta = cos(theta);

            // Spherical to cartesian (in tangent space)
            vec3 tangentSample = vec3(sinTheta * cosPhi,  sinTheta * sinPhi, cosTheta);
           
			// Tangent space to world
            vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * N; 

            irradiance += texture(textureEnvironment, sampleVec).rgb * cosTheta * sinTheta;
            nrSamples++;
        }
    }
    
	irradiance = Pi * irradiance / nrSamples;    
    FragColor = vec4(irradiance, 1.0f);
}
