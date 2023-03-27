#include "RNG.h"


float RNG::NextFloat()
{
   return m_RandomDistribution(m_MersenneTwister) ;
}

void RNG::shuffle(std::vector<GLfloat>& toShuffle)
{
   std::default_random_engine randomEngine = std::default_random_engine{};
   std::shuffle(toShuffle.begin(), toShuffle.end(), randomEngine);
}

RNG::RNG() :
   m_RandomDistribution(0.0f, 1.0f)
{
   std::random_device rd;
   m_MersenneTwister.seed(rd());
}
