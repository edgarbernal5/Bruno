#include "EditorGame.h"
#include <Bruno/EntryPoint.h>

Bruno::Application* Bruno::CreateApplication(int argc, char** argv)
{
	ApplicationParameters parameters;
	parameters.Name = "Bruno Editor";
	parameters.WindowHeight = 600;
	parameters.WindowWidth = 1000;

	auto currentPath = std::filesystem::current_path();
	parameters.WorkingDirectory = currentPath.parent_path().parent_path() / "Content";

	auto projectPath = parameters.WorkingDirectory;
	return new EditorGame(parameters, projectPath);
}