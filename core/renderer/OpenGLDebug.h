#pragma once

#include "glad/glad.h"

namespace OpenGLDebug
{
	void debugCallback(
		GLenum source, 
		GLenum type, 
		GLuint id,
		GLenum severity, 
		GLsizei length, 
		const GLchar* message, 
		const void* param);
}