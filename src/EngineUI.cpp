#include "EngineUI.hpp"

namespace Engine {
    namespace EngineUI {
        std::stringstream ss;
        
        bool _active = true;
        bool _showVerbose = false;
        bool _showConsole = false;
        
        void Draw() {
            if (!_active) {
                return;
            }
            Draw2D::SetColor(0.1f, 0.1f, 0.1f);
            Draw2D::Rect(0.0f, 0.0f, getScreenWidth(), _showConsole ? getScreenHeight() : 14);
            Draw2D::SetFont("monospace", 10);
            Draw2D::SetColor(1.0f, 1.0f, 1.0f);
            Draw2D::Print(10, 4, "-- Engine2D --");
            ss.str("");
            ss << "DrawTime: " << Profiler::GetTime("Draw");
            Draw2D::Print(getScreenWidth() - 240, 4, ss.str().c_str());
            if (!_showConsole) {
                return;
            }
            std::vector<Logger::LogEvent>* logEvents = Logger::GetEvents();
            int i = 2;
            for (auto iterator = logEvents->rbegin(); iterator < logEvents->rend(); iterator++) {
                switch (iterator->Level) {
                    case Logger::LogLevel_Verbose:
                        Draw2D::SetColor(205 / 255.0f, 205 / 255.0f, 205 / 255.0f);
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
                if (iterator->Level == Logger::LogLevel_Verbose && !_showVerbose) {
                    i += 4; // add some padding to show that a message is there, just hidden
                } else {
                    Draw2D::Print(5, i += 12, iterator->Event.c_str());
                }
                if (i > getScreenHeight()) {
                    break;
                }
            }
        }
        
        void OnKeyPress(int key, bool press) {
            if (key == 161 && press) {
                _showConsole = !_showConsole;
            } else if (key == 299 && press) {
                dumpProfile();
            }
            if (!_active) {
                return;
            }
        }
        
        void SetActive(bool active) {
            _active = active;
        }
        
        void SetShowVerbose(bool show) {
            _showVerbose = show;
        }
        
        void ToggleConsole() {
            _showConsole = !_showConsole;
        }
    }
}