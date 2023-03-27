#version 460

const float PI = 3.14159265359;

layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 particleVelocity;
layout (location = 2) in float particleAge;

// Transform feedback results
/*
layout(location = 0) out vec3 resultPosition;
layout(location = 1) out vec3 resultVelocity;
layout(location = 2) out float resultAge;  
*/

/*layout (xfb_buffer = 0, xfb_offset = 0)*/ out vec3 resultPosition;
/*layout (xfb_buffer = 1, xfb_offset = 0)*/ out vec3 resultVelocity;
/*layout (xfb_buffer = 2, xfb_offset = 0)*/ out float resultAge;

//uniform float particleSpeedDelta;
uniform int Pass;
uniform float deltaTime;

out float Transparency;
out vec2 texCoord;

//uniform float   Time;
uniform float   DeltaT;
uniform vec3    Accel;
uniform float   ParticleLifetime;
//uniform float   MinParticleSize;
//uniform float   MaxParticleSize;
uniform mat3    EmitterBasis;
uniform vec3    EmitterPosition;
uniform float   ParticleSize;

uniform mat4 MV;
uniform mat4 Proj;

//out vec4 gl_Position;

const vec3 offsets[] = vec3[](vec3(-0.5,-0.5,0), vec3(0.5,-0.5,0), vec3(0.5,0.5,0),
                              vec3(-0.5,-0.5,0), vec3(0.5,0.5,0), vec3(-0.5,0.5,0) );

const vec2 texCoords[] = vec2[](vec2(0,0), vec2(1,0), vec2(1,1), vec2(0,0), vec2(1,1), vec2(0,1));

uniform sampler1D RandomTex;



vec3 randomInitialVelocity()
{
    float theta = mix(0.0, PI / 1.5, texelFetch(RandomTex, 3* gl_VertexID, 0).r);
    float phi = mix(0.0, 2.0 * PI, texelFetch(RandomTex, 3* gl_VertexID + 1, 0).r);
    float velocity = mix(0.1, 0.2, texelFetch(RandomTex,3 * gl_VertexID + 2, 0).r);

    vec3 v = vec3(sin(theta) * cos(phi), cos(theta), sin(theta) * sin(phi));

    return normalize(EmitterBasis * v) * velocity;
}

void update()
{    
    if(particleAge < 0 || particleAge > ParticleLifetime)
    {
        resultPosition = EmitterPosition;
        resultVelocity = randomInitialVelocity();
        
        // If not yet visible
        if(particleAge < 0)
        {
            resultAge = particleAge + DeltaT;    
        }

        // If past the 'expiration date', reset back to beginning of lifetime
        else
        {
            resultAge = (particleAge - ParticleLifetime) + DeltaT;
        }
    }
    
    else 
    {
        resultPosition = vertexPosition + particleVelocity * DeltaT;
        resultVelocity = particleVelocity + Accel * DeltaT;
        resultAge = particleAge + DeltaT;
    }
}

void render()
{
    Transparency = 0.0;
    vec3 posCam = vec3(0.0);

    if(particleAge >= 0.0)
    {
        posCam = (MV * vec4(vertexPosition, 1)).xyz + offsets[gl_VertexID] * ParticleSize;
        Transparency = clamp(1.0 - particleAge / ParticleLifetime, 0, 1);
    }

    texCoord = texCoords[gl_VertexID];
    
    gl_Position = Proj * vec4(posCam, 1);
    //gl_Position = vec4(0.5, 0.5, 0.5, 1.0);
}

void main()
{
    if(Pass == 1)
    {
        update();
    }

    else if(Pass == 2)
    {
        render();
    }
}