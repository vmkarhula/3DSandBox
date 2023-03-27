#pragma once

#include "glad/glad.h"
#include "glm/glm.hpp"

#include <unordered_map>
#include <string>

// A simple shader class strongly inspired by learnopengl.com tutorials
class Shader
{
public:	

	enum class ShaderStatus { DUMMY, COMPILED };

	// Dummy shader placeholder for example when OpenGL isn't loaded yet
	Shader();

	Shader(const std::string& vertexPath, const std::string& fragmentPath, const std::string& shaderName = "");
	
	// Could be even public member variable as some tutorials seem to do.
	GLuint ID() { return m_ID; };
	
	// Binds the shader
	void Use();

	// Uniform Setters
	void setInt(const std::string& paramName, int value);
	void setUint(const std::string& paramName, unsigned int value);
	void setFloat(const std::string& paramName, float value);
	void setVector3f(const std::string& paramName, const glm::vec3& value);
	void setMatrix4f(const std::string name, const glm::mat4& value);


// Private member functions
private:

	void checkShaderCompileErrors(unsigned int shader, std::string type);
	GLint findUniformLocation(const std::string& uniformName);

// Private member variables 
private:

	std::string m_ShaderName;
	GLuint m_ID; 
	std::unordered_map<std::string, GLuint> m_UniformCache;
	

};