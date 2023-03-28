#pragma once

#define GLFW_INCLUDE_NONE

#include "GLFW/glfw3.h"
#include "glad/glad.h"
#include "glm/glm.hpp"

#include "OpenGLTexture.h"
#include "Camera.h"

#include "ParticleSystem.h"
#include "ShaderProgram.h"

#include <vector>

struct DFCoordinate2D
{
	DFCoordinate2D operator-(const DFCoordinate2D& rhs)
	{
		DFCoordinate2D result;
		
		result.x = this->x - rhs.x;
		result.y = this->y - rhs.y;

		return result;
	}
	
	double x;
	double y;
};

class ParticleScene
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

	Camera m_MainCamera;

	GLFWwindow* m_MainWindow;

	GLuint m_GridVB;
	GLuint m_GridVAO;

	int m_nGridLines;

	ShaderProgram m_GridShader;
	

	ParticleSystem* m_ParticleSystem;

	DFCoordinate2D m_FrameMousePosition;
	bool m_MouseGrab;

	float m_PreviousFrameTimestamp;

};