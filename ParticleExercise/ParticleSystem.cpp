#include "ParticleSystem.h"
#include "ParticleUtil.h"

#include "glm/gtc/matrix_transform.hpp"

#include <vector>
#include <iostream>


ParticleSystem::ParticleSystem() :
	m_RNG(),
	m_ParticleShader("Particle Shader"),
	m_SmokeTexture("./res/img/smoke.png"),
	m_RNGTexture1D(0),
	m_nParticles(0),
	m_TransformFeedbacks{},
	m_VAOs{},
	m_PositionBuffers{},
	m_VelocityBuffers{},
	m_AgeBuffers{},
	m_EmitterPosition(glm::vec3(0.0f)),
	m_ParticleLifetime(0.0f),
	m_DrawBufferIndex(1),
	m_Projection(glm::perspective(glm::radians(60.0f), (float)(800.0f / 600.0f), 0.3f, 100.0f))
{
	m_ParticleShader.CompileShader(ShaderProgram::ShaderType::VERTEX, "./shader/particleshader.vs.glsl");
	m_ParticleShader.CompileShader(ShaderProgram::ShaderType::FRAGMENT, "./shader/particleshader.fs.glsl");
	
	std::vector<std::string> feedbackVariables{ "resultPosition", "resultVelocity", "resultAge" };
	m_ParticleShader.AddTransformFeedback(feedbackVariables, ShaderProgram::TransformFeedbackType::SEPARATE);
	
	if(!m_ParticleShader.LinkProgram())
	{
		std::cerr << "Error linking particleshader" << std::endl;
	}
	
	m_ParticleShader.Use();
	m_ParticleShader.SetFloat("ParticleLifetime", 5.0f);
	m_ParticleShader.SetMatrix3fv("EmitterBasis", ParticleUtil::MakeArbitraryBasis(glm::vec3(0.0f, 1.0f, 0.5f)));
	m_ParticleShader.SetFloat("ParticleSize", 0.05f);
}

void ParticleSystem::Update(float dt)
{
	glBindVertexArray(m_VAOs[1 - m_DrawBufferIndex]);

	m_ParticleShader.Use();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_SmokeTexture.ID());

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_1D, m_RNGTexture1D);
		
	// Set the update phase active on shader
	m_ParticleShader.SetInt("Pass", 1);
	m_ParticleShader.SetVector3f("Accel", glm::vec3(0.0f, 0.5f, 0.0f));
	m_ParticleShader.SetFloat("DeltaT", dt);
	m_ParticleShader.SetInt("RandomTex", 1);

	// Set up the transform feedback on shader pipeline
	glEnable(GL_RASTERIZER_DISCARD);
	
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, m_TransformFeedbacks[m_DrawBufferIndex]);
	glBeginTransformFeedback(GL_POINTS);
	
	glVertexAttribDivisor(0, 0);
	glVertexAttribDivisor(1, 0);
	glVertexAttribDivisor(2, 0);
	
	glDrawArrays(GL_POINTS, 0, m_nParticles);
	
	glBindTexture(GL_TEXTURE_1D, 0);
	
	glBindVertexArray(0);


	// Disable transform feedback
	glEndTransformFeedback();
	glDisable(GL_RASTERIZER_DISCARD);
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);

}

void ParticleSystem::Render(const Camera& camera)
{
	m_ParticleShader.Use();

	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 view = camera.GetViewMatrix();

	glm::mat4 modelView = view * model;
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_SmokeTexture.ID());
	
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_1D, m_RNGTexture1D);

	// Set the render phase active on shader
	m_ParticleShader.SetInt("ParticleTex", 0);
	m_ParticleShader.SetInt("RandomTex", 1);
	m_ParticleShader.SetInt("Pass", 2);
	m_ParticleShader.SetMatrix4f("Proj", glm::perspective(glm::radians(60.0f), 16.0f / 9.0f, 0.1f, 100.0f));
	m_ParticleShader.SetMatrix4f("MV", modelView);
	m_ParticleShader.SetVector3f("EmitterPosition", m_EmitterPosition);
	m_ParticleShader.SetInt("ParticleTex", 0);

	glDepthMask(GL_FALSE);
	
	glBindVertexArray(m_VAOs[m_DrawBufferIndex]);
	glVertexAttribDivisor(0, 1);
	glVertexAttribDivisor(1, 1);
	glVertexAttribDivisor(2, 1);
	glDrawArraysInstanced(GL_TRIANGLES, 0, 6, m_nParticles);
	glBindVertexArray(0);

	glDepthMask(GL_TRUE);

	m_DrawBufferIndex = 1 - m_DrawBufferIndex;
}

void ParticleSystem::AddParticleEffect(ParticleEffectType type, glm::vec3 position, glm::vec3 direction, int nParticles, float particleLifetime)
{
	
	m_nParticles = nParticles;
	m_ParticleLifetime = particleLifetime;
	m_EmitterPosition = position;

	m_RNGTexture1D = ParticleUtil::GenRandomTex1D(3 * nParticles);
	
	// Prepare the double buffers
	
	glGenBuffers(2, m_PositionBuffers);
	glGenBuffers(2, m_AgeBuffers);
	glGenBuffers(2, m_VelocityBuffers);

	// Set buffer sizes
	
	int bufferSize = nParticles * sizeof(glm::vec3);

	glBindBuffer(GL_ARRAY_BUFFER, m_PositionBuffers[0]);
	glBufferData(GL_ARRAY_BUFFER, bufferSize, 0, GL_DYNAMIC_COPY);
	glBindBuffer(GL_ARRAY_BUFFER, m_PositionBuffers[1]);
	glBufferData(GL_ARRAY_BUFFER, bufferSize, 0, GL_DYNAMIC_COPY);

	glBindBuffer(GL_ARRAY_BUFFER, m_VelocityBuffers[0]);
	glBufferData(GL_ARRAY_BUFFER, bufferSize, 0, GL_DYNAMIC_COPY);
	glBindBuffer(GL_ARRAY_BUFFER, m_VelocityBuffers[1]);
	glBufferData(GL_ARRAY_BUFFER, bufferSize, 0, GL_DYNAMIC_COPY);

	glBindBuffer(GL_ARRAY_BUFFER, m_AgeBuffers[0]);
	glBufferData(GL_ARRAY_BUFFER, nParticles * sizeof(float), 0, GL_DYNAMIC_COPY);
	glBindBuffer(GL_ARRAY_BUFFER, m_AgeBuffers[1]);
	glBufferData(GL_ARRAY_BUFFER, nParticles * sizeof(float), 0, GL_DYNAMIC_COPY);


	std::vector<float> initialAge(nParticles);

	float rate = particleLifetime / nParticles;

	for (int i = 0; i < m_nParticles; i++)
	{
		initialAge[i] = rate * (i - m_nParticles);
	}

	glBindBuffer(GL_ARRAY_BUFFER, m_AgeBuffers[0]);
	glBufferSubData(GL_ARRAY_BUFFER, 0,  m_nParticles * sizeof(float), initialAge.data());

	glBindBuffer(GL_ARRAY_BUFFER, 0);


	// Set up VAOs
	glGenVertexArrays(1, &m_VAOs[0]);
	glGenVertexArrays(1, &m_VAOs[1]);

	// Buffer set #1 
	glBindVertexArray(m_VAOs[0]);
	
	glBindBuffer(GL_ARRAY_BUFFER, m_PositionBuffers[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	
	glBindBuffer(GL_ARRAY_BUFFER, m_VelocityBuffers[0]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, m_AgeBuffers[0]);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);


	// Buffer set #2
	glBindVertexArray(m_VAOs[1]);

	glBindBuffer(GL_ARRAY_BUFFER, m_PositionBuffers[1]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, m_VelocityBuffers[1]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, m_AgeBuffers[1]);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);
	
	glBindVertexArray(0);

	// Transform feedback #1
	glGenTransformFeedbacks(2, m_TransformFeedbacks);
	
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, m_TransformFeedbacks[0]);

	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, m_PositionBuffers[0]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, m_VelocityBuffers[0]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 2, m_AgeBuffers[0]);

	// Transform feedback #2
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, m_TransformFeedbacks[1]);
	
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, m_PositionBuffers[1]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, m_VelocityBuffers[1]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 2, m_AgeBuffers[1]);

	// Unbind tranform feedback for now
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
}
