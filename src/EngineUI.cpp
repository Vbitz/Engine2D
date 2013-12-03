#include "EngineUI.hpp"

#include "Application.hpp"

namespace Engine {
    namespace EngineUI {
        std::stringstream ss;
        
        std::stringstream currentConsoleInput;
        
        bool _active = true;
        bool _showConsole = false;
        
        char getSpecialChars(int key) {
            switch (key) {
                case '[':       return '{';
                case ']':       return '}';
                case '-':       return '_';
                case '=':       return '+';
                case '\\':      return '|';
                case '`':       return '~';
                case '1':       return '!';
                case '2':       return '@';
                case '3':       return '#';
                case '4':       return '$';
                case '5':       return '%';
                case '6':       return '^';
                case '7':       return '&';
                case '8':       return '*';
                case '9':       return '(';
                case '0':       return ')';
                case ',':       return '<';
                case '.':       return '>';
                case ';':       return ':';
                case '\'':      return '"';
                case '/':       return '?';
                default:        return key;
            }
        }
        
        // just temporey until the profiler has this built in
        float lastDrawTimes[100];
        int currentLastDrawTimePos = 0;
        
        float fpsTimer = 0.0f;
        float currentFPS = 0;
        
        void Draw() {
            if (!Config::GetBoolean("core.debug.engineUI")) {
                return;
            }
            
            Application* app = GetAppSingilton();
            
            if (!_showConsole) {
                Draw2D::SetColor(25 / 255.0f, 25 / 255.0f, 25 / 255.0f);
            } else {
                Draw2D::SetColor(40 / 255.0f, 40 / 255.0f, 40 / 255.0f, 0.95f);
            }
            
            Draw2D::Rect(0.0f, 0.0f, app->GetScreenWidth(), _showConsole ? app->GetScreenHeight() : 14);
            
            Draw2D::SetFont("basic", 10);
            if (!_showConsole) {
                Draw2D::SetColor(220 / 255.0f, 220 / 255.0f, 220 / 255.0f);
            } else {
                Draw2D::SetColor(1.0f, 1.0f, 1.0f);
            }
            
            double drawTime = Profiler::GetTime("Draw");
            lastDrawTimes[currentLastDrawTimePos++] = drawTime;
            if (currentLastDrawTimePos > 100) {
                currentLastDrawTimePos = 0;
            }
            
            float fps = 1000 / (Profiler::GetTime("Frame") * 1000);
            
            fpsTimer += Profiler::GetTime("Frame");
            
            if (fpsTimer > 0.1f) {
                currentFPS = glm::floor(fps);
                fpsTimer = 0;
            }
            
            Draw2D::Print(10, 4, "-- Engine2D --");
            ss.str("");
            ss.precision(4);
            ss << "FPS: " << currentFPS;
            ss << " | DrawTime: " << drawTime;
            Draw2D::Print(app->GetScreenWidth() - 220, 4, ss.str().c_str());
            
            Draw2D::DisableSmooth();
            
            Draw2D::LineGraph(app->GetScreenWidth() - 430, 14, 2, 200, lastDrawTimes, 100);
            
            Draw2D::EnableSmooth();
            
            if (!_showConsole) {
                return;
            }
            
            std::vector<Logger::LogEvent>* logEvents = Logger::GetEvents();
            
            bool showVerbose = Config::GetBoolean("core.debug.engineUI.showVerboseLog"); // pretty cheap
            
            int i = app->GetScreenHeight() - 40;
            
            for (auto iterator = logEvents->rbegin(); iterator < logEvents->rend(); iterator++) {
                if (iterator->Hidden) {
                    continue; // don't show it if it's hidden
                }
                
                if (iterator->Type == Logger::LogType_Text) {
                    if (iterator->Level == Logger::LogLevel_Verbose && !showVerbose) {
                        i -= 6; // add some padding to show that a message is there, just hidden
                        Draw2D::SetColor(80 / 255.0f, 80 / 255.0f, 80 / 255.0f);
                        Draw2D::Rect(0, i + 2, app->GetScreenWidth(), 2);
                    } else {
                        i -= 22;
                        Draw2D::SetColor(30 / 255.0f, 30 / 255.0f, 30 / 255.0f, 0.9f);
                        Draw2D::Rect(60, i + 1, app->GetScreenWidth() - 60, 20);
                    }
                }
                
                switch (iterator->Level) {
                    case Logger::LogLevel_Verbose:
                        Draw2D::SetColor(205 / 255.0f, 205 / 255.0f, 205 / 255.0f);
                        break;
                    case Logger::LogLevel_User:
                        Draw2D::SetColor(255 / 255.0f, 0 / 255.0f, 255 / 255.0f);
                        break;
                    case Logger::LogLevel_ConsoleInput:
                        Draw2D::SetColor(0 / 255.0f, 191 / 255.0f, 255 / 255.0f);
                        break;
                    case Logger::LogLevel_Log:
                        Draw2D::SetColor(250 / 255.0f, 250 / 255.0f, 250 / 255.0f);
                        break;
                    case Logger::LogLevel_Warning:
                        Draw2D::SetColor(255 / 255.0f, 165 / 255.0f, 0 / 255.0f);
                        break;
                    case Logger::LogLevel_Error:
                        Draw2D::SetColor(178 / 255.0f, 34 / 255.0f, 34 / 255.0f);
                        break;
                    case Logger::LogLevel_TestLog:
                        Draw2D::SetColor(250 / 255.0f, 250 / 255.0f, 250 / 255.0f);
                        break;
                    case Logger::LogLevel_TestError:
                        Draw2D::SetColor(178 / 255.0f, 34 / 255.0f, 34 / 255.0f);
                        break;
                }
                
                std::string time = std::to_string(iterator->time);
                time.resize(6, '0');
                
                if (iterator->Level != Logger::LogLevel_Verbose || showVerbose) {
                    Draw2D::Print(5, i + 7, (time + "s").c_str());
                }
                
                if (iterator->Level != Logger::LogLevel_Verbose || showVerbose) {
                    if (iterator->Type == Logger::LogType_Text) {
                        Draw2D::Print(65, i + 7, iterator->Event.c_str());
                    } else if (iterator->Type == Logger::LogType_Graphical) {
                        i -= (*iterator->GraphEvent)(65, i);
                    }
                }
            
                if (i < 35) {
                    break;
                }
            }
            
            Draw2D::SetColor(0.0f, 0.0f, 0.0f, 0.85f);
            Draw2D::Rect(5, app->GetScreenHeight() - 30, app->GetScreenWidth() - 10, 25);
            
            Draw2D::SetColor(1.0f, 1.0f, 1.0f);
            Draw2D::SetFont("basic", 12);
            Draw2D::Print(10, app->GetScreenHeight() - 22, (currentConsoleInput.str() + "_").c_str());
        }
        
#ifndef _PLATFORM_WIN32
#define KEY_CONSOLE 161
#else
#define KEY_CONSOLE 96
#endif

        void OnKeyPress(int key, int press, bool shift) {
            if (!Config::GetBoolean("core.debug.engineUI")) {
                return;
            }
            
            Application* app = GetAppSingilton();

			if (key == KEY_CONSOLE && press == GLFW_PRESS) { // `
                ToggleConsole();
			} else if (key == GLFW_KEY_F10 && press == GLFW_PRESS) { // f10
				app->DumpProfile();
            }

            if (!_showConsole) {
                return;
            }

			if (key < 256 && (press == GLFW_PRESS || press == GLFW_REPEAT) && key != KEY_CONSOLE && key != GLFW_KEY_ENTER) {
#ifndef _PLATFORM_WIN32
				currentConsoleInput << (char) (shift ? getSpecialChars(key) : (char) std::tolower(key));
#else
				currentConsoleInput << (char) (shift ? getSpecialChars(key) : tolower(key));
#endif
            } else if (key == GLFW_KEY_BACKSPACE && (press == GLFW_PRESS || press == GLFW_REPEAT)) {
                std::string str = currentConsoleInput.str();
                if (str.length() > 0) {
                    str.resize(str.length() - 1);
                    currentConsoleInput.str(str);
					if (str.length() != 0) {
						currentConsoleInput.seekp(str.length());
					}
                }
            } else if (key == GLFW_KEY_ENTER && press == GLFW_PRESS) {
                app->RunCommand(currentConsoleInput.str());
                currentConsoleInput.str("");
            }
        }
        
        void SetActive(bool active) {
            _active = active;
        }
        
        void ToggleConsole() {
            _showConsole = !_showConsole;
        }
        
        void ClearConsole() {
            Logger::HideAllEvents();
        }
        
        bool ConsoleActive() {
            return _showConsole;
        }
    }
}