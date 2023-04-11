#include "ParticleScene.h"

#include <iostream>

#include "glad/glad.h"

#include "glm/gtc/matrix_transform.hpp"

#include "renderer/OpenGLDebug.h"


ParticleScene::ParticleScene():
	m_GridShader("GridShader"),
	m_ParticleSystem(nullptr)
{
}

ParticleScene::~ParticleScene()
{
	if(m_ParticleSystem)
	{
		delete(m_ParticleSystem);
	}
}

void ParticleScene::Init()
{
	std::cout << "ParticleScene initializing!" << std::endl;

	if (!glfwInit())
	{
		std::cout << "Failed loading glfw!" << std::endl;
	}
	
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);

	m_MainWindow = glfwCreateWindow(800, 600, "Particle Test", nullptr, nullptr);
	glfwMakeContextCurrent(m_MainWindow);
	
	if (!gladLoadGL())
	{
		std::cout << "Glad failed loading modern OpenGL" << std::endl;
	}

	// Debug setup
	glEnable(GL_DEBUG_OUTPUT);

	glDebugMessageCallback(OpenGLDebug::debugCallback, nullptr);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);


	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Despite grid being a constant size array, we'll use heap allocated vector to be sure. 
	std::vector<glm::vec3> gridData = createGrid(5, 5, 3.0f);
	m_nGridLines = gridData.size() / 2;

	glCreateVertexArrays(1, &m_GridVAO);
	glBindVertexArray(m_GridVAO);

	glCreateBuffers(1, &m_GridVB);
	glBindBuffer(GL_ARRAY_BUFFER, m_GridVB);

	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * gridData.size(), gridData.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	m_GridShader.CompileShader(ShaderProgram::ShaderType::VERTEX, "./shader/grid.vs.glsl");
	m_GridShader.CompileShader(ShaderProgram::ShaderType::FRAGMENT, "./shader/grid.fs.glsl");
	m_GridShader.LinkProgram();
		

	m_MainCamera.SetPolarFloat(glm::vec3(0.0f), 5.0f, glm::half_pi<float>(), -glm::half_pi<float>() / 2.0f);
	
	glfwSetWindowUserPointer(m_MainWindow, (void*)this);

	auto MouseButtonCallback = [](GLFWwindow* window, int mbutton, int action, int mods) 
	{
		((ParticleScene*)(glfwGetWindowUserPointer(window)))->I_MouseClickCallback(window, mbutton, action, mods);
	};

	glfwSetMouseButtonCallback(m_MainWindow, MouseButtonCallback);

}

void ParticleScene::Run()
{
	std::cout << "ParticleScene running!" << std::endl;
	
	glm::mat4 mvp = glm::mat4(1.0f);

	m_GridShader.Use();
	
	glm::mat4 view = m_MainCamera.GetViewMatrix();
	glm::mat4 proj = glm::perspective(glm::radians(90.0f), 16.0f / 9.0f, 0.3f, 100.0f);
	glm::mat4 world = glm::mat4(1.0f);
	
	mvp = proj * view * world;
	

	m_ParticleSystem = new ParticleSystem();
	m_ParticleSystem->AddParticleEffect(ParticleSystem::ParticleEffectType::Smoke, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 1500, 5.0f);

	m_GridShader.Use();
	m_GridShader.SetMatrix4f("mvp", mvp);


	m_PreviousFrameTimestamp = glfwGetTime();

	while(!glfwWindowShouldClose(m_MainWindow))
	{
		float newFrameTimestamp = glfwGetTime();
		float dt = newFrameTimestamp - m_PreviousFrameTimestamp;
		m_PreviousFrameTimestamp = newFrameTimestamp;
		
		glfwPollEvents();

		if (glfwGetKey(m_MainWindow, GLFW_KEY_UP) == GLFW_PRESS)
		{
			m_MainCamera.AdjustZoom(-0.1f);
		}

		if(glfwGetKey(m_MainWindow, GLFW_KEY_DOWN) == GLFW_PRESS)
		{
			m_MainCamera.AdjustZoom(0.1f);
		}

		if(glfwGetKey(m_MainWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(m_MainWindow, GLFW_TRUE);
		}

		if(glfwGetKey(m_MainWindow, GLFW_KEY_LEFT) == GLFW_PRESS)
		{
			m_MainCamera.PanLeft(0.1f);
		}

		if(glfwGetKey(m_MainWindow, GLFW_KEY_RIGHT) == GLFW_PRESS)
		{
			m_MainCamera.PanRight(0.1f);
		}

		ProcessMouseInput();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glm::mat4 view = m_MainCamera.GetViewMatrix();
		mvp = proj * view * world;

		glBindVertexArray(m_GridVAO);

		m_GridShader.Use();
		m_GridShader.SetMatrix4f("mvp", mvp);
		
		glDrawArrays(GL_LINES, 0, m_nGridLines * 2);
		
		if(m_ParticleSystem)
		{
			m_ParticleSystem->Update(dt);
			m_ParticleSystem->Render(m_MainCamera);
		}

		glfwSwapBuffers(m_MainWindow);
	}
	
	glfwTerminate();

}

std::vector<glm::vec3> ParticleScene::createGrid(int nRows, int nColumns, float squareSize)
{
	
	float width = nColumns * squareSize;
	float height = nRows * squareSize;

	// For origo centered grid we'll use the span [-halfWidth, halfWidth] and so on
	float halfWidth = width / 2.0f;
	float halfHeight = height / 2.0f;

	// Avoid constant resizing with predetermined size for vector
	std::vector<glm::vec3> grid(2 * (nRows + 1) + 2 * (nColumns + 1));

	int initialSize = grid.size();

	// We'll keep track of the grid vector index's position 
	int vertexIndex = 0;

	// Row lines

	// We'll need nColumns + 1 and nRows + 1 to form sufficient number of squares ( think of n = 1 case )
	for(int row = 0; row < nRows + 1; row++)
	{
		grid[vertexIndex] = glm::vec3(-halfWidth, 0.0f, -halfHeight + (row * squareSize));
		vertexIndex++;

		grid[vertexIndex] = glm::vec3(halfWidth, 0.0f, -halfHeight + (row * squareSize));
		vertexIndex++;
	}

	// Column lines

	for(int column = 0; column < nColumns + 1; column++)
	{
		grid[vertexIndex] = glm::vec3(-halfWidth + (column * squareSize), 0.0f, -halfHeight);
		vertexIndex++;

		grid[vertexIndex] = glm::vec3(-halfWidth + (column * squareSize), 0.0f, halfHeight);
		vertexIndex++;
	}
	
	assert(vertexIndex == initialSize);
	
	return grid;
}

void ParticleScene::ProcessMouseInput()
{

	if(m_MouseGrab)
	{
		
		DFCoordinate2D current;
		DFCoordinate2D mouseChange;
		
		glfwGetCursorPos(m_MainWindow, &current.x, &current.y);

		mouseChange = current - m_FrameMousePosition;
		m_FrameMousePosition = current;

		float panX = (float)(mouseChange.x / 100.0f);
		float panY = (float)(mouseChange.y / 100.0f);

		m_MainCamera.Pan(panX, panY);
	}
}

void ParticleScene::I_MouseClickCallback(GLFWwindow* window, int mbutton, int action, int mods)
{
		if(mbutton == GLFW_MOUSE_BUTTON_LEFT)
		{
			if(action == GLFW_PRESS)
			{
				m_MouseGrab = true;
				glfwGetCursorPos(m_MainWindow, &m_FrameMousePosition.x, &m_FrameMousePosition.y);
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

			}
			
			else if (action == GLFW_RELEASE)
			{
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
				m_MouseGrab = false;
			}
			
		}
}
