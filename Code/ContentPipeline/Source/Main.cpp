#include "ContentPipelineGame.h"
#include <Bruno/EntryPoint.h>

Bruno::Game* Bruno::CreateGame(int argc, char** argv)
{
	GameParameters parameters;
	parameters.Name = "Content Pipeline Builder";
	parameters.WindowHeight = 768;
	parameters.WindowWidth = 1024;

	return new ContentPipelineGame(parameters);
}