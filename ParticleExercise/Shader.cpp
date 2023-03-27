#include "Shader.h"

#include <sstream>
#include <fstream>
#include <iostream>
#include <filesystem>

#include "glm/gtc/type_ptr.hpp"

using std::filesystem::path;

Shader::Shader()
{
}

Shader::Shader(const std::string& vertexpath, const std::string& fragmentpath, const std::string& shaderName) :
	m_ID(0),
	m_ShaderName(shaderName)
{
	path vertexFilepath(vertexpath);
	path fragmentFilepath(fragmentpath);
	
	if(!std::filesystem::exists(vertexFilepath))
	{
		std::cerr << "Error! Vertex shader file " << vertexFilepath << " does not exists" << std::endl;
	}

	if (!std::filesystem::exists(fragmentFilepath))
	{
		std::cerr << "Error! Vertex shader file " << fragmentFilepath << " does not exists" << std::endl;
	}

	std::string vertexCodeString;
	std::string fragmentCodeString;

	GLuint vertexID = 0;
	GLuint fragmentID = 0;

	vertexID = glCreateShader(GL_VERTEX_SHADER);
	
	if(!vertexID)
	{
		std::cerr << "Error! Could not create vertex shader ID" << std::endl;
		return;
	}

	fragmentID = glCreateShader(GL_FRAGMENT_SHADER);

	if(!fragmentID)
	{
		std::cerr << "Error! Could not create fragment shadar ID" << std::endl;
		return;
	}
	
	std::ifstream vertexShaderFile;
	std::ifstream fragmentShaderFile;

	vertexShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fragmentShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);



	try
	{
		vertexShaderFile.open(vertexFilepath);
		fragmentShaderFile.open(fragmentFilepath);
		
		std::stringstream vertexShaderStrStream;
		std::stringstream fragmentShaderStrStream;

		vertexShaderStrStream << vertexShaderFile.rdbuf();
		fragmentShaderStrStream << fragmentShaderFile.rdbuf();

		vertexShaderFile.close();
		fragmentShaderFile.close();

		vertexCodeString = vertexShaderStrStream.str();
		fragmentCodeString = fragmentShaderStrStream.str();

	}

	catch(std::ifstream::failure& e)
	{
		std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: " << e.what() << std::endl;
	}

	const char* vShaderCode = vertexCodeString.c_str();
	const char* fShaderCode = fragmentCodeString.c_str();

	// Compilation phase
	int success;
	char infoLog[512];

	vertexID = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexID, 1, &vShaderCode, nullptr);
	glCompileShader(vertexID);
	checkShaderCompileErrors(vertexID, "VERTEX");

	fragmentID = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentID, 1, &fShaderCode, nullptr);
	glCompileShader(fragmentID);
	checkShaderCompileErrors(fragmentID, "FRAGMENT");

	m_ID = glCreateProgram();
	glAttachShader(m_ID, vertexID);
	glAttachShader(m_ID, fragmentID);
	glLinkProgram(m_ID);

	checkShaderCompileErrors(m_ID, "PROGRAM");

	glDeleteShader(vertexID);
	glDeleteShader(fragmentID);

}

void Shader::Use()
{
	glUseProgram(m_ID);
}

void Shader::setInt(const std::string& paramName, int value)
{
	glUniform1i(findUniformLocation(paramName), (int)value);
}

void Shader::setUint(const std::string& paramName, unsigned int value)
{
	glUniform1ui(findUniformLocation(paramName), value);
}

void Shader::setFloat(const std::string& paramName, float value)
{
	glUniform1f(findUniformLocation(paramName), (float) value);
}

void Shader::setVector3f(const std::string& paramName, const glm::vec3& value)
{
	glUniform3f(findUniformLocation(paramName), value.x, value.y, value.z);
}

void Shader::setMatrix4f(const std::string name, const glm::mat4& value)
{
	glUniformMatrix4fv(findUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::checkShaderCompileErrors(unsigned int shader, std::string type)
{
	int success;

	char infoLog[1024];

	if(type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

		if(!success)
		{
			glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
		
			std::cout << "ERROR::SHADER_COMPILATION_ERROR of type " << type << "\n" << infoLog << "\n -- ------------------------------- -- " << std::endl;

		}
		
		else
		{
			glGetProgramiv(shader, GL_LINK_STATUS, &success);
			if(!success)
			{
				glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
				std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- ------------------------------- -- " << std::endl;
			}
		}
	}
}

GLint Shader::findUniformLocation(const std::string& uniformName)
{
	
	// First, check the cache
	auto cacheSearch = m_UniformCache.find(uniformName);

	// If the location is already cached, all good. Return the location
	if(cacheSearch != m_UniformCache.end())
	{
		return cacheSearch->second;
	}
	
	// Otherwise query OpenGL for location
	else
	{
		GLint uniformLoc = glGetUniformLocation(m_ID, uniformName.c_str());

		// If found, save to cache and return the location
		if (uniformLoc != -1)
		{
			m_UniformCache[uniformName] = uniformLoc;
			return uniformLoc;
		}

		// If not found, warn the user
		else
		{
			std::cerr << "ERROR::SHADER_ERROR: " << m_ShaderName << " Uniform not found: " << uniformName << std::endl;
			return uniformLoc;
		}
	}
}
