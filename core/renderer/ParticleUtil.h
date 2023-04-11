#pragma once

#include "glad/glad.h"
#include "glm/glm.hpp"

#include "RNG.h"

namespace ParticleUtil
{
	static inline GLuint GenRandomTex1D(int size)
	{
		RNG random;

		std::vector<GLfloat> randData(size);

		for(float& randomFloat : randData)
		{
			randomFloat = random.NextFloat();
		}

		GLuint texID;
		glGenTextures(1, &texID);

		glBindTexture(GL_TEXTURE_1D, texID);
		glTexStorage1D(GL_TEXTURE_1D, 1, GL_R32F, size);
		glTexSubImage1D(GL_TEXTURE_1D, 0, 0, size, GL_RED, GL_FLOAT, randData.data());

		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		glBindTexture(GL_TEXTURE_1D, 0);

		return texID;

	}

	// TODO: Figure out the math on this cookbook example
	static inline glm::mat3 MakeArbitraryBasis(const glm::vec3& direction)
	{
		glm::mat3 basis;
		glm::vec3 u, v, n;

		v = direction;
		n = glm::cross(glm::vec3(1.0f, 0.0f, 0.0f), v);

		if(glm::length(n) < 0.00001f)
		{
			n = glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), v);
		}
	
		u = glm::cross(v, n);

		basis[0] = glm::normalize(u);
		basis[1] = glm::normalize(v);
		basis[2] = glm::normalize(n);

		return basis;

	}

}