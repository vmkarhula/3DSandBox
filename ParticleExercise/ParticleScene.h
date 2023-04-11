#pragma once

#define GLFW_INCLUDE_NONE

#include "GLFW/glfw3.h"
#include "glad/glad.h"
#include "glm/glm.hpp"

#include "SceneBase.h"

#include "renderer/OpenGLTexture.h"
#include "renderer/ParticleSystem.h"
#include "renderer/ShaderProgram.h"

#include <vector>


class ParticleScene: public SceneBase
{

public:

	ParticleScene();
	~ParticleScene();

	void Init();
	void Run();

	void I_MouseClickCallback(GLFWwindow* window, int mbutton, int action, int mods);

private:

	std::vector<glm::vec3> createGrid(int rows, int columns, float squareSize);
	void ProcessMouseInput();
	

private:

	GLuint m_GridVB;
	GLuint m_GridVAO;
	int m_nGridLines;

	ShaderProgram m_GridShader;
	
	ParticleSystem* m_ParticleSystem;



};