#include "../Engine2D.hpp"
#include "../Events.hpp"
#include "../Logger.hpp"

using namespace Engine;

EventMagic v8_postGC_test(Json::Value val, void* userPointer) {
	Logger::begin("gcDebug", Logger::LogLevel_Verbose) << "GC: " << val["type"].asString() << Logger::end();
}

void startup() {
	GetEventsSingilton()->GetEvent("v8_postGC")->AddListener("v8_postGC_test", EventEmitter::MakeTarget(v8_postGC_test));
}

ENGINE_ADDON("gc_debug", "Engine2D", startup, NULL, LoadOrder::PreScript);