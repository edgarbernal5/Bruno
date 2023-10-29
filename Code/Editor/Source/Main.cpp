#include "EditorGame.h"
#include <Bruno/EntryPoint.h>

Bruno::Application* Bruno::CreateApplication(int argc, char** argv)
{
	ApplicationParameters parameters;
	parameters.Name = "Bruno Editor";
	parameters.WindowHeight = 600;
	parameters.WindowWidth = 800;

	auto currentPath = std::filesystem::current_path();
	parameters.WorkingDirectory = currentPath.parent_path().parent_path() / "Content";

	return new EditorGame(parameters);
}