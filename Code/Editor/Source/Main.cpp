#include "EditorGame.h"
#include <Bruno/EntryPoint.h>

Bruno::Application* Bruno::CreateApplication(int argc, char** argv)
{
	ApplicationParameters parameters;
	parameters.Name = "Bruno Editor";
	parameters.WindowHeight = 600;
	parameters.WindowWidth = 800;

	return new EditorGame(parameters);
}