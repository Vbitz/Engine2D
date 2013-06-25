#include "EngineUI.hpp"

namespace Engine {
    namespace EngineUI {
        std::stringstream ss;
        
        std::stringstream currentConsoleInput;
        
        bool _active = true;
        bool _showVerbose = false;
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
        
        void Draw() {
            if (!_active) {
                return;
            }
            if (!_showConsole) {
                Draw2D::SetColor(25 / 255.0f, 25 / 255.0f, 25 / 255.0f);
            } else {
                Draw2D::SetColor(40 / 255.0f, 40 / 255.0f, 40 / 255.0f, 0.95f);
            }
            Draw2D::Rect(0.0f, 0.0f, getScreenWidth(), _showConsole ? getScreenHeight() : 14);
            Draw2D::SetFont("basic", 10);
            if (!_showConsole) {
                Draw2D::SetColor(220 / 255.0f, 220 / 255.0f, 220 / 255.0f);
            } else {
                Draw2D::SetColor(1.0f, 1.0f, 1.0f);
            }
            Draw2D::Print(10, 4, "-- Engine2D --");
            ss.str("");
            ss.precision(4);
            ss << "DrawTime: " << Profiler::GetTime("Draw");
            Draw2D::Print(getScreenWidth() - 150, 4, ss.str().c_str());
            if (!_showConsole) {
                return;
            }
            std::vector<Logger::LogEvent>* logEvents = Logger::GetEvents();
            int i = getScreenHeight() - 40;
            for (auto iterator = logEvents->rbegin(); iterator < logEvents->rend(); iterator++) {
                if (iterator->Level == Logger::LogLevel_Verbose && !_showVerbose) {
                    i -= 6; // add some padding to show that a message is there, just hidden
                    Draw2D::SetColor(80 / 255.0f, 80 / 255.0f, 80 / 255.0f);
                    Draw2D::Rect(0, i + 2, getScreenWidth(), 2);
                } else {
                    i -= 22;
                    Draw2D::SetColor(30 / 255.0f, 30 / 255.0f, 30 / 255.0f, 0.9f);
                    Draw2D::Rect(60, i + 1, getScreenWidth() - 60, 20);
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
                    std::string time = std::to_string(iterator->time);
                    time.resize(6, '0');
                    Draw2D::Print(5, i + 7, (time + "s").c_str());
                    Draw2D::Print(65, i + 7, iterator->Event.c_str());
                }
                if (i < 35) {
                    break;
                }
            }
            Draw2D::SetColor(0.0f, 0.0f, 0.0f, 0.85f);
            Draw2D::Rect(5, getScreenHeight() - 30, getScreenWidth() - 10, 20);
            Draw2D::SetColor(1.0f, 1.0f, 1.0f);
            Draw2D::Print(10, getScreenHeight() - 25, (currentConsoleInput.str() + "_").c_str());
        }
        
        void OnKeyPress(int key, int press, bool shift) {
            if (key == 161 && press == GLFW_PRESS) { // `
                ToggleConsole();
            } else if (key == 299 && press == GLFW_PRESS) { // f10
                dumpProfile();
            }
            if (!_showConsole) {
                return;
            }
            if (key < 256 && (press == GLFW_PRESS || press == GLFW_REPEAT) && key != '\n' && key != 161) {
                currentConsoleInput << (char) (shift ? getSpecialChars(key) : (char) std::tolower(key));
            } else if (key == GLFW_KEY_BACKSPACE && (press == GLFW_PRESS || press == GLFW_REPEAT)) {
                std::string str = currentConsoleInput.str();
                if (str.length() > 0) {
                    str.resize(str.length() - 1);
                    currentConsoleInput.str(str);
                    currentConsoleInput.seekp(str.length());
                }
            } else if (key == GLFW_KEY_ENTER && press == GLFW_PRESS) {
                runCommand(currentConsoleInput.str());
                currentConsoleInput.str("");
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
        
        bool ConsoleActive() {
            return _showConsole;
        }
    }
}