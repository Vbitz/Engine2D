#include "EngineUI.hpp"

namespace Engine {
    namespace EngineUI {
        bool _active = true;
        bool _showConsole = false;
        
        void Draw() {
            if (!_active) {
                return;
            }
            Draw2D::SetColor(0.2f, 0.2f, 0.2f);
            Draw2D::Rect(0.0f, 0.0f, getScreenWidth(), _showConsole ? getScreenHeight() : 14);
            Draw2D::SetFont("monospace8px");
            Draw2D::SetColor(1.0f, 1.0f, 1.0f);
            Draw2D::Print(10, 4, "-- Engine2D --");
            if (!_showConsole) {
                return;
            }
            std::vector<Logger::LogEvent>* logEvents = Logger::GetEvents();
            typedef std::vector<Logger::LogEvent>::iterator iter;
            int i = 0;
            for (iter iterator = logEvents->begin(); iterator < logEvents->end(); iterator++) {
                double timeSince = Logger::GetTime() - iterator->time;
                timeSince = std::log(timeSince);
                timeSince = std::pow(timeSince, 2);
                if (timeSince < 3.0) {
                    switch (iterator->Level) {
                        case Logger::LogLevel_Verbose:
                            Draw2D::SetColor(105 / 255.0f, 105 / 255.0f, 105 / 255.0f);
                            break;
                        case Logger::LogLevel_User:
                            Draw2D::SetColor(34 / 255.0f, 139 / 255.0f, 34 / 255.0f);
                            break;
                        case Logger::LogLevel_Log:
                            Draw2D::SetColor(192 / 255.0f, 192 / 255.0f, 192 / 255.0f);
                            break;
                        case Logger::LogLevel_Warning:
                            Draw2D::SetColor(255 / 255.0f, 165 / 255.0f, 0 / 255.0f);
                            break;
                        case Logger::LogLevel_Error:
                            Draw2D::SetColor(178 / 255.0f, 34 / 255.0f, 34 / 255.0f);
                            break;
                    }
                    Draw2D::Print(5, i++ * 10 + 16, iterator->Event.c_str());
                }
            }
        }
        
        void OnKeyPress(int key, bool press) {
            if (key == 161 && press) {
                _showConsole = !_showConsole;
            }
            if (!_active) {
                return;
            }
        }
        
        void SetActive(bool active) {
            _active = active;
        }
    }
}