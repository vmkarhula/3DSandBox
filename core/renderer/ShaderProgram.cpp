#include "ShaderProgram.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include "glm/gtc/type_ptr.hpp"

ShaderProgram::ShaderProgram(const std::string& programName) :
	m_ProgramId(0),
	m_VertexShaderId(0),
	m_FragmentShaderId(0),
	m_TessellationCSId(0),
	m_GeometryShaderId(0),
	m_ComputeShaderId(0),
	m_ProgramName(programName)
{
	// To avoid calling this before OpenGL is loaded, the program is only created once we start compiling first shader
	//m_ProgramId = glCreateProgram();

}

ShaderProgram::~ShaderProgram()
{
	DeleteShaders();

	if (m_ProgramId)
	{
		glDeleteProgram(m_ProgramId);
	}
}

bool ShaderProgram::CompileShader(ShaderType type, const std::filesystem::path& filepath)
{
	if (!m_ProgramId)
	{
		m_ProgramId = glCreateProgram();
	}

	if (!std::filesystem::exists(filepath))
	{
		std::cerr << "Error! Shader file " << filepath << " does not exist. No compilation done." << std::endl;
		return false;
	}

	GLuint* targetShader;
	GLuint OpenGLShaderType = 0;

	switch (type)
	{

	case ShaderProgram::ShaderType::VERTEX:
	{
		targetShader = &m_VertexShaderId;
		OpenGLShaderType = GL_VERTEX_SHADER;
	} break;

	case ShaderProgram::ShaderType::FRAGMENT:
	{
		targetShader = &m_FragmentShaderId;
		OpenGLShaderType = GL_FRAGMENT_SHADER;
	} break;

	case ShaderProgram::ShaderType::COMPUTE:
	{
		targetShader = &m_ComputeShaderId;
		OpenGLShaderType = GL_COMPUTE_SHADER;
	} break;

	case ShaderProgram::ShaderType::GEOMETRY:
	{
		targetShader = &m_GeometryShaderId;
		OpenGLShaderType = GL_GEOMETRY_SHADER;
	} break;

	case ShaderProgram::ShaderType::TESSELLATION:
	{
		targetShader = &m_TessellationCSId;
		OpenGLShaderType = GL_TESS_CONTROL_SHADER;
	} break;

	default:
	{
		std::cerr << "Error, unimplemented shadertype when compiling: " << filepath << std::endl;
		return false;

	} break;
	}

	if (*targetShader != 0)
	{
		std::cerr << "Error, shadertype already exists in the program: " << filepath << std::endl;
	}

	std::ifstream shaderFilestream;

	shaderFilestream.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	std::string shaderCode;

	try
	{
		shaderFilestream.open(filepath);
		std::stringstream shaderfileStrStream;

		shaderfileStrStream << shaderFilestream.rdbuf();
		shaderFilestream.close();

		shaderCode = shaderfileStrStream.str();
	}

	catch (std::ifstream::failure& e)
	{
		std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: " << e.what() << std::endl;
		return false;
	}

	*targetShader = glCreateShader(OpenGLShaderType);

	if (*targetShader == 0)
	{
		std::cerr << "Error, could not create a new shader Id: " << filepath << std::endl;
	}

	// Compilation phase
	int	success;
	char	infoLog[512];

	const char* shaderCodeCStr = shaderCode.c_str();

	glShaderSource(*targetShader, 1, &shaderCodeCStr, nullptr);
	glCompileShader(*targetShader);

	if (!CheckShaderCompileErrors(*targetShader, type))
	{
		std::cerr << "Shader compilation failed at file: " << filepath << std::endl;
	}

	glAttachShader(m_ProgramId, *targetShader);

	return true;
}

bool ShaderProgram::AddTransformFeedback(const std::vector<std::string>& variableNames, TransformFeedbackType feedbacktype)
{
	//glUseProgram(m_ProgramId);
	
	std::vector<const char*> cStrings;

	GLuint bufferMode;

	switch (feedbacktype)
	{
	case ShaderProgram::TransformFeedbackType::INTERLEAVED:
	{
		bufferMode = GL_INTERLEAVED_ATTRIBS;
	} break;

	case ShaderProgram::TransformFeedbackType::SEPARATE:
	{
		bufferMode = GL_SEPARATE_ATTRIBS;
	} break;

	default:
	{
		std::cerr << "Error! Unrecognized buffer mode while setting transform feedback for shader program: " << m_ProgramName << std::endl;
		return false;

	} break;
	}

	for (const std::string& varName : variableNames)
	{
		cStrings.push_back(varName.c_str());
	}

	glTransformFeedbackVaryings(m_ProgramId, cStrings.size(), cStrings.data(), bufferMode);

	return true;
}


bool ShaderProgram::LinkProgram()
{

	glLinkProgram(m_ProgramId);

	if (!CheckShaderCompileErrors(m_ProgramId, ShaderType::PROGRAM))
	{
		std::cerr << "Error linking the program: " << m_ProgramName << std::endl;
		return false;
	}

	// All linked up, no need for the individual shaders anymore. 
	DeleteShaders();

	m_LinkingState = LinkingState::LINKED;

	return true;
}

GLuint ShaderProgram::ShaderId(ShaderType type)
{
	return 0;
}

void ShaderProgram::Use()
{
	glUseProgram(m_ProgramId);
}

void ShaderProgram::SetInt(const std::string& paramName, int value)
{
	glUniform1i(FindUniformLocation(paramName), value);
}

void ShaderProgram::SetUint(const std::string& paramName, unsigned int value)
{
	glUniform1ui(FindUniformLocation(paramName), value);
}

void ShaderProgram::SetFloat(const std::string& paramName, float value)
{
	glUniform1f(FindUniformLocation(paramName), value);
}

void ShaderProgram::SetVector3f(const std::string& paramName, const glm::vec3& value)
{
	glUniform3f(FindUniformLocation(paramName), value.x, value.y, value.z);
}

void ShaderProgram::SetMatrix3fv(const std::string& paramName, const glm::mat3& value)
{
	glUniformMatrix3fv(FindUniformLocation(paramName), 1, GL_FALSE, glm::value_ptr(value));
}

void ShaderProgram::SetMatrix4f(const std::string& name, const glm::mat4& value)
{
	glUniformMatrix4fv(FindUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
}

GLint ShaderProgram::FindUniformLocation(const std::string& uniformName)
{
	// First, check the cache
	auto cacheSearch = m_UniformCache.find(uniformName);

	// If the location is already cached, all good. Return the location
	if (cacheSearch != m_UniformCache.end())
	{
		return cacheSearch->second;
	}

	// Otherwise query OpenGL for location
	else
	{
		GLint uniformLoc = glGetUniformLocation(m_ProgramId, uniformName.c_str());

		// If found, save to cache and return the location
		if (uniformLoc != -1)
		{
			m_UniformCache[uniformName] = uniformLoc;
			return uniformLoc;
		}

		// If not found, warn the user
		else
		{
			std::cerr << "ERROR::SHADER_ERROR: " << m_ProgramName << " Uniform not found: " << uniformName << std::endl;
			return uniformLoc;
		}
	}
}

bool ShaderProgram::CheckShaderCompileErrors(unsigned int shader, ShaderType type)
{
	int success;

	char infoLog[1024];

	if (type != ShaderType::PROGRAM)
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

		if (!success)
		{
			glGetShaderInfoLog(shader, 1024, nullptr, infoLog);

			std::cout << "ERROR::SHADER_COMPILATION_ERROR of type " << "\n" << infoLog << "\n -- ------------------------------- -- " << std::endl;
			return false;
		}
	}
	
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
			std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << "\n" << infoLog << "\n -- ------------------------------- -- " << std::endl;
			return false;
		}
	}


	return true;
}

void ShaderProgram::DeleteShaders()
{
	if (m_VertexShaderId)
	{
		glDeleteShader(m_VertexShaderId);
	}

	if (m_FragmentShaderId)
	{
		glDeleteShader(m_FragmentShaderId);
	}

	if (m_GeometryShaderId)
	{
		glDeleteShader(m_GeometryShaderId);
	}

	if (m_ComputeShaderId)
	{
		glDeleteShader(m_ComputeShaderId);
	}

	if (m_TessellationCSId)
	{
		glDeleteShader(m_TessellationCSId);
	}
}
