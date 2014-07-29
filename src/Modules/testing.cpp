#include "../Events.hpp"
#include "../Logger.hpp"
#include "../Draw2D.hpp"
#include "../Application.hpp"

using namespace Engine;

Draw2DPtr draw;

EventMagic testing_draw(Json::Value args) {
	draw->Rect(100, 100, 50, 50);
	return EM_OK;
}

__attribute__((constructor))
void startup() {
	Logger::begin("testingAddon", Logger::LogLevel_Log) << "Loading Testing Addon" << Logger::end();
	draw = new Draw2D(GetAppSingilton()->GetRender());
	GetEventsSingilton()->GetEvent("draw")->AddListener("testing_draw", EventEmitter::MakeTarget(testing_draw));
}