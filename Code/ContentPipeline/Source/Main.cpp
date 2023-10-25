#include "ContentPipelineApplication.h"
#include <Bruno/EntryPoint.h>

Bruno::Application* Bruno::CreateApplication(int argc, char** argv)
{
	ApplicationParameters parameters;
	parameters.Name = "Content Pipeline Builder";
	parameters.WindowHeight = 768;
	parameters.WindowWidth = 1024;

	return new ContentPipelineApplication(parameters);
}