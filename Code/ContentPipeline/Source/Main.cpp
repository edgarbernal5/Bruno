#include "ContentPipelineApplication.h"
#include <Bruno/EntryPoint.h>

Bruno::Application* Bruno::CreateApplication(int argc, char** argv)
{
	ApplicationParameters parameters;
	parameters.Name = "Content Pipeline Builder";
	parameters.WindowHeight = 768;
	parameters.WindowWidth = 1024;

	auto currentPath = std::filesystem::current_path();
	parameters.WorkingDirectory = currentPath.parent_path().parent_path() / "Content";

	return new ContentPipelineApplication(parameters);
}