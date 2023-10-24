#include "PlayerGame.h"
#include <Bruno/EntryPoint.h>

Bruno::Application* Bruno::CreateApplication(int argc, char** argv)
{
	ApplicationParameters parameters;
	parameters.WindowHeight = 600;
	parameters.WindowWidth = 800;

	return new PlayerGame(parameters);
}