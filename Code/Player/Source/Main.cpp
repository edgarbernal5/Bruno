#include "PlayerGame.h"
#include <Bruno/EntryPoint.h>
#include <filesystem>

Bruno::Application* Bruno::CreateApplication(int argc, char** argv)
{
	ApplicationParameters parameters;
	parameters.WindowHeight = 600;
	parameters.WindowWidth = 800;

	auto currentPath = std::filesystem::current_path();
	parameters.WorkingDirectory = currentPath.parent_path().parent_path() / "Content";

	return new PlayerGame(parameters);
}