#pragma once

#include <GLFW/glfw3.h>

#include "Camera.h"

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


class SceneBase
{
public:


	virtual ~SceneBase() = default;

	virtual void Init() = 0;
	virtual void Run() = 0;

	void I_MouseClickCallback(GLFWwindow* window, int mbutton, int action, int mods);

protected:


	GLFWwindow* m_MainWindow;
	
	GLuint m_MainFramebuffer;

	//
	Camera m_MainCamera;


	// Input 
	bool m_MouseGrab;
	
	// Framestats
	float m_PreviousFrameTimestamp;

	DFCoordinate2D m_FrameMousePosition;

};