#include "../Engine2D.hpp"
#include "../Events.hpp"
#include "../Package.hpp"
#include "../Filesystem.hpp"
#include "../Application.hpp"

using namespace Engine;

void startup() {
	std::cout << "Loaded buildEpkgFromJson" << std::endl;
	Filesystem::SetupUserDir("buildEpkgFromJson");
	PackagePtr pkg = Package::FromJsonSpec(std::string("testing_content.json"), "testing_output.epkg");
}

ENGINE_ADDON("buildEpkgFromJson", "Engine2D", startup, NULL, LoadOrder::Loaded);