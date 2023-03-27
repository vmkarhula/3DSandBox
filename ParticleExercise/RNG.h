#pragma once

#include <random>

#include "glad/glad.h"

class RNG
{
public:
	
	RNG();
	
	float NextFloat();

	static void shuffle(std::vector<GLfloat>& toShuffle);



private:
		
	std::mt19937 m_MersenneTwister;
	std::uniform_real_distribution<float> m_RandomDistribution;
};