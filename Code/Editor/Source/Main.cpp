#include "EditorGame.h"
#include <Bruno/EntryPoint.h>

Bruno::Game* Bruno::CreateGame(int argc, char** argv)
{
	GameParameters parameters;
	parameters.Name = "Bruno Editor";
	parameters.WindowHeight = 600;
	parameters.WindowWidth = 800;

	return new EditorGame(parameters);
}