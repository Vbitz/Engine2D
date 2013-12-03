#include "main.hpp"

#include "Application.hpp"

int main(int argc, char const *argv[])
{
	return Engine::GetAppSingilton()->Start(argc, argv);
}