#pragma once

#include <memory>

#include "glm/glm.hpp"
#include "glad/glad.h"

#include "ShaderProgram.h"
#include "RNG.h"
#include "OpenGLTexture.h"
#include "Camera.h"

class ParticleSystem
{
public:

	enum class ParticleEffectType
	{
		Smoke
	};

	ParticleSystem();

	void Update(float dt);
	void Render(const Camera& camera);

	void AddParticleEffect( ParticleEffectType type, glm::vec3 position,
		glm::vec3 direction, int nParticles,
		float particleLifetime);
	

private:
	
	std::unique_ptr<RNG> m_RNG;

	OpenGLTexture m_SmokeTexture;

	glm::mat4 m_Projection;

	//Shader m_ParticleShader;

	ShaderProgram m_ParticleShader;

	glm::vec3 m_EmitterPosition;

	GLuint m_RNGTexture1D;

	GLuint m_AgeBuffers[2];
	GLuint m_PositionBuffers[2];
	GLuint m_VelocityBuffers[2];
	
	GLuint m_VAOs[2];
	GLuint m_TransformFeedbacks[2];

	int m_nParticles;

	int m_DrawBufferIndex; 

	float m_ParticleLifetime;
};