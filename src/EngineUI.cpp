/*
   Filename: EngineUI.cpp
   Purpose:  Draws the EngineUI and console on top of rendering

   Part of Engine2D

   Copyright (C) 2014 Vbitz

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#include "EngineUI.hpp"

#include "FramePerfMonitor.hpp"
#include "Config.hpp"
#include "Profiler.hpp"

namespace Engine {
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
    
    EngineUI::EngineUI(Application* app) : _app(app) {
        this->_draw = new Draw2D(app->GetRender());
    }
    
    void EngineUI::Draw() {
        if (!Config::GetBoolean("core.debug.engineUI")) {
            return;
        }
        
        RenderDriver* renderGL = this->_draw->GetRender();
        
        renderGL->ResetMatrix();
        
        if (!_showConsole) {
            renderGL->SetColor(25 / 255.0f, 25 / 255.0f, 25 / 255.0f);
        } else {
            renderGL->SetColor(40 / 255.0f, 40 / 255.0f, 40 / 255.0f, 0.95f);
        }
        
        this->_draw->Rect(0.0f, 0.0f, this->_app->GetScreenWidth(), _showConsole ? this->_app->GetScreenHeight() : 14);
        
        renderGL->SetFont("basic", 10);
        if (!this->_showConsole) {
            renderGL->SetColor(220 / 255.0f, 220 / 255.0f, 220 / 255.0f);
        } else {
            renderGL->SetColor(1.0f, 1.0f, 1.0f);
        }
        
        if (Config::GetBoolean("core.debug.profiler")) {
            double drawTime = Profiler::GetTime("Draw");
            this->_lastDrawTimes[this->_currentLastDrawTimePos++] = drawTime;
            
            if (this->_currentLastDrawTimePos > 100) {
                this->_currentLastDrawTimePos = 0;
            }
            
            this->_ss.str("");
            this->_ss.precision(4);
            this->_ss << "FPS: " << FramePerfMonitor::GetFPS();
            this->_ss << " | DrawTime: " << drawTime;
            renderGL->Print(this->_app->GetScreenWidth() - 220, 4, this->_ss.str().c_str());
            
            renderGL->DisableSmooth();
            
            this->_draw->LineGraph(this->_app->GetScreenWidth() - 430, 14, 2, 200, this->_lastDrawTimes, 100);
            
            renderGL->EnableSmooth();
        }
        
        renderGL->Print(10, 4, "-- Engine2D --");
        
        if (!_showConsole) {
            return;
        }
        
        std::vector<Logger::LogEvent>* logEvents = Logger::GetEvents();
        
        bool showVerbose = Config::GetBoolean("core.debug.engineUI.showVerboseLog"); // pretty cheap
        
        int i = this->_app->GetScreenHeight() - 40;
        
        for (auto iterator = logEvents->rbegin(); iterator < logEvents->rend(); iterator++) {
            if (iterator->Hidden) {
                continue; // don't show it if it's hidden
            }
            
            if (iterator->Type == Logger::LogType_Text) {
                if (iterator->Level == Logger::LogLevel_Verbose && !showVerbose) {
                    i -= 6; // add some padding to show that a message is there, just hidden
                    renderGL->SetColor(80 / 255.0f, 80 / 255.0f, 80 / 255.0f);
                    this->_draw->Rect(0, i + 2, this->_app->GetScreenWidth(), 2);
                } else {
                    i -= 22;
                    if (iterator->Level == Logger::LogLevel_Highlight) {
                        renderGL->SetColor(200 / 255.0f, 200 / 255.0f, 200 / 255.0f, 0.9f);
                        this->_draw->Rect(0, i + 1, this->_app->GetScreenWidth(), 20);
                    } else {
                        renderGL->SetColor(30 / 255.0f, 30 / 255.0f, 30 / 255.0f, 0.9f);
                        this->_draw->Rect(60, i + 1, this->_app->GetScreenWidth() - 60, 20);
                    }
                }
            }
            
            switch (iterator->Level) {
                case Logger::LogLevel_Verbose:
                    renderGL->SetColor(205 / 255.0f, 205 / 255.0f, 205 / 255.0f);
                    break;
                case Logger::LogLevel_User:
                    renderGL->SetColor(255 / 255.0f, 0 / 255.0f, 255 / 255.0f);
                    break;
                case Logger::LogLevel_ConsoleInput:
                    renderGL->SetColor(0 / 255.0f, 191 / 255.0f, 255 / 255.0f);
                    break;
                case Logger::LogLevel_Log:
                    renderGL->SetColor(250 / 255.0f, 250 / 255.0f, 250 / 255.0f);
                    break;
                case Logger::LogLevel_Warning:
                    renderGL->SetColor(255 / 255.0f, 165 / 255.0f, 0 / 255.0f);
                    break;
                case Logger::LogLevel_Error:
                    renderGL->SetColor(178 / 255.0f, 34 / 255.0f, 34 / 255.0f);
                    break;
                case Logger::LogLevel_Highlight:
                    renderGL->SetColor(0.1f, 0.1f, 0.1f);
                    break;
                case Logger::LogLevel_TestLog:
                    renderGL->SetColor(250 / 255.0f, 250 / 255.0f, 250 / 255.0f);
                    break;
                case Logger::LogLevel_TestError:
                    renderGL->SetColor(178 / 255.0f, 34 / 255.0f, 34 / 255.0f);
                    break;
            }
            
            std::string time = std::to_string(iterator->time);
            time.resize(6, '0');
            
            if (iterator->Level != Logger::LogLevel_Verbose || showVerbose) {
                renderGL->Print(5, i + 7, (time + "s").c_str());
            }
            
            if (iterator->Level != Logger::LogLevel_Verbose || showVerbose) {
                if (iterator->Type == Logger::LogType_Text) {
                    renderGL->Print(65, i + 7, iterator->Event.c_str());
                } else if (iterator->Type == Logger::LogType_Graphical) {
                    i -= (*iterator->GraphEvent)(65, i);
                }
            }
        
            if (i < 35) {
                break;
            }
        }
        
        renderGL->SetColor(0.0f, 0.0f, 0.0f, 0.85f);
        this->_draw->Rect(5, this->_app->GetScreenHeight() - 30, this->_app->GetScreenWidth() - 10, 25);
        
        renderGL->SetColor(1.0f, 1.0f, 1.0f);
        renderGL->SetFont("basic", 12);
        renderGL->Print(10, this->_app->GetScreenHeight() - 22, (this->_currentConsoleInput.str() + "_").c_str());
    }
    
#ifndef _PLATFORM_WIN32
#define KEY_CONSOLE 161
#else
#define KEY_CONSOLE 96
#endif

    void EngineUI::OnKeyPress(int key, int press, bool shift) {
        if (!Config::GetBoolean("core.debug.engineUI")) {
            return;
        }
        
        if (key == KEY_CONSOLE && press == GLFW_PRESS) { // `
            this->ToggleConsole();
        } else if (key == GLFW_KEY_F10 && press == GLFW_PRESS) { // f10
            Events::Emit("dumpProfile");
        }

        if (!_showConsole) {
            return;
        }

        if (key < 256 && (press == GLFW_PRESS || press == GLFW_REPEAT) && key != KEY_CONSOLE && key != GLFW_KEY_ENTER) {
#ifndef _PLATFORM_WIN32
            this->_currentConsoleInput << (char) (shift ? getSpecialChars(key) : (char) std::tolower(key));
#else
            currentConsoleInput << (char) (shift ? getSpecialChars(key) : tolower(key));
#endif
        } else if (key == GLFW_KEY_BACKSPACE && (press == GLFW_PRESS || press == GLFW_REPEAT)) {
            std::string str = this->_currentConsoleInput.str();
            if (str.length() > 0) {
                str.resize(str.length() - 1);
                this->_currentConsoleInput.str(str);
                if (str.length() != 0) {
                    this->_currentConsoleInput.seekp(str.length());
                }
            }
        } else if (key == GLFW_KEY_UP && press == GLFW_PRESS) {
            if (this->_currentHistoryLine > 0) {
                this->_currentHistoryLine--;
                this->_currentConsoleInput.str(this->_commandHistory[this->_currentHistoryLine]);
                if (this->_currentConsoleInput.str().length() != 0) {
                    this->_currentConsoleInput.seekp(this->_currentConsoleInput.str().length());
                }
            }
        } else if (key == GLFW_KEY_DOWN && press == GLFW_PRESS) {
            if (this->_commandHistory.size() > 0 && this->_currentHistoryLine < this->_commandHistory.size() - 1) {
                this->_currentHistoryLine++;
                this->_currentConsoleInput.str(this->_commandHistory[this->_currentHistoryLine]);
                if (this->_currentConsoleInput.str().length() != 0) {
                    this->_currentConsoleInput.seekp(this->_currentConsoleInput.str().length());
                }
            }
        } else if (key == GLFW_KEY_ENTER && press == GLFW_PRESS) {
            std::string command = this->_currentConsoleInput.str();
            this->_app->RunCommand(command);
            if (this->_commandHistory.size() == 0 || this->_commandHistory[this->_commandHistory.size() - 1] != command) {
                this->_commandHistory.push_back(command);
            }
            this->_currentHistoryLine = this->_commandHistory.size();
            this->_currentConsoleInput.str("");
        }
    }
    
    void EngineUI::SetActive(bool active) {
        this->_active = active;
    }
    
    void EngineUI::ToggleConsole() {
        this->_showConsole = !this->_showConsole;
    }
    
    void EngineUI::ClearConsole() {
        Logger::HideAllEvents();
    }
    
    bool EngineUI::ConsoleActive() {
        return this->_showConsole;
    }
}