#include "../Events.hpp"
#include "../Logger.hpp"

using namespace Engine;

EventMagic testing_testingEvent(Json::Value args) {
	Logger::begin("", Logger::LogLevel_Log) << "Hello World" << Logger::end();
	return EM_OK;
}

__attribute__((constructor))
void startup() {
	std::cout << "Running Startup" << std::endl;
	GetEventsSingilton()->GetEvent("testingEvent")->AddListener("testing_testingEvent", EventEmitter::MakeTarget(testing_testingEvent));
}