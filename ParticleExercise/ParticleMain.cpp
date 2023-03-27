#include <iostream>
#include <memory>
#include "ParticleScene.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

int main(int argc, char ** argv)
{
	std::unique_ptr<ParticleScene> scene = std::make_unique<ParticleScene>();
	scene->Init();
	scene->Run();

	return 0;
}