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

#include <OpenGL/gl3.h>

#include "FramePerfMonitor.hpp"
#include "Config.hpp"
#include "Profiler.hpp"

#define SEC_TO_NSEC 1000000000

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
    
    double _getHeapUsage() {
        v8::HeapStatistics stats;
        v8::Isolate::GetCurrent()->GetHeapStatistics(&stats);
        return (double) stats.used_heap_size() / (double) stats.total_heap_size();
    }
    
    EngineUI::EngineUI(ApplicationPtr app) : _app(app) {
        this->_draw = new Draw2D(app->GetRender());
        
        for (int i = 0; i < timingResolution; i++) {
            this->_lastDrawTimes[i] = 0.0f;
            this->_lastHeapUsages[i] = 0.0f;
        }
        
        GetEventsSingilton()->GetEvent("onProfileEnd")->AddListener("EngineUI::_profilerHook", EventEmitter::MakeTarget(_profilerHook));
        GetEventsSingilton()->GetEvent("captureLastDrawTimes")->AddListener("EngineUI::_captureLastDrawTimes", EventEmitter::MakeTarget(_captureLastDrawTimes));
    }
    
    void EngineUI::Draw() {
        if (!Config::GetBoolean("core.debug.engineUI")) {
            return;
        }
        
        RenderDriverPtr renderGL = this->_draw->GetRender();
        WindowPtr window = this->_app->GetWindow();
        glm::vec2 windowSize = window->GetWindowSize();
        
        renderGL->ResetMatrix();
        
        if (!_showConsole) {
            renderGL->SetColor(25 / 255.0f, 25 / 255.0f, 25 / 255.0f);
        } else {
            renderGL->SetColor(40 / 255.0f, 40 / 255.0f, 40 / 255.0f, 0.95f);
        }
        
        this->_draw->Rect(0.0f, 0.0f, windowSize.x, _showConsole ? windowSize.y : 14);
        
        renderGL->SetFont("basic", 10);
        if (!this->_showConsole) {
            renderGL->SetColor(220 / 255.0f, 220 / 255.0f, 220 / 255.0f);
        } else {
            renderGL->SetColor(1.0f, 1.0f, 1.0f);
        }
        
        renderGL->Print(10, 4, "-- Engine2D --");
        
        if (Config::GetBoolean("core.debug.profiler")) {
            double drawTime = Profiler::GetTime("Draw");
            this->_lastHeapUsages[this->_currentLastDrawTimePos] = _getHeapUsage();
            this->_lastDrawTimes[this->_currentLastDrawTimePos++] = drawTime;
            
            if (this->_currentLastDrawTimePos >= timingResolution) {
                this->_currentLastDrawTimePos = 0;
            }
            
            renderGL->DisableSmooth();
            
            glLineWidth(0.1); // HACK: Until this is exposed by RenderGL
            
            double lineGraphScale = Config::GetFloat("core.debug.engineUI.profilerScale");
            
            this->_draw->LineGraph(windowSize.x - 440, 14, 0.5, lineGraphScale, this->_lastDrawTimes, timingResolution);
            
            renderGL->EnableSmooth();
            
            this->_ss.str("");
            this->_ss << "FPS: " << FramePerfMonitor::GetFPS();
            this->_ss << " | DrawTime (ms): ";
            
            renderGL->Print(windowSize.x - 230, 4, this->_ss.str().c_str());
            
            this->_ss.precision(6);
            this->_ss.str("");
            
            double drawTimeMS = (drawTime * 1000);
            
            this->_ss << (drawTimeMS);
            
            if (drawTimeMS > (1000 / 60)) {
                renderGL->SetColor("red");
            } else {
                renderGL->SetColor("green");
            }
            
            renderGL->Print(windowSize.x - 60, 4, this->_ss.str().c_str());
        }
        
        if (!_showConsole) {
            return;
        }
        
        if (this->_currentView == CurrentView::Console) {
            std::vector<Logger::LogEvent>* logEvents = Logger::GetEvents();
            
            bool showVerbose = Config::GetBoolean("core.debug.engineUI.showVerboseLog"); // pretty cheap
            
            int i = windowSize.y - 40;
            
            for (auto iterator = logEvents->rbegin(); iterator < logEvents->rend(); iterator++) {
                if (iterator->Hidden) {
                    continue; // don't show it if it's hidden
                }
                
                if (iterator->Type == Logger::LogType_Text) {
                    if (iterator->Level == Logger::LogLevel_Verbose && !showVerbose) {
                        i -= 6; // add some padding to show that a message is there, just hidden
                        renderGL->SetColor(80 / 255.0f, 80 / 255.0f, 80 / 255.0f);
                        this->_draw->Rect(0, i + 2, windowSize.x, 2);
                    } else {
                        i -= 22;
                        if (iterator->Level == Logger::LogLevel_Highlight) {
                            renderGL->SetColor(200 / 255.0f, 200 / 255.0f, 200 / 255.0f, 0.9f);
                            this->_draw->Rect(0, i + 1, windowSize.x, 20);
                        } else {
                            renderGL->SetColor(30 / 255.0f, 30 / 255.0f, 30 / 255.0f, 0.9f);
                            this->_draw->Rect(60, i + 1, windowSize.x - 60, 20);
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
            this->_draw->Rect(5, windowSize.y - 30, windowSize.x - 10, 25);
            
            renderGL->SetColor(1.0f, 1.0f, 1.0f);
            renderGL->SetFont("basic", 12);
            renderGL->Print(10, windowSize.y - 22, (this->_currentConsoleInput.str() + "_").c_str());
        } else if (this->_currentView == CurrentView::Settings) {
            int i = 50;
            Config::UIConfigCollection configItems = Config::GetAllUI();
            
            for (auto iter = configItems.begin(); iter != configItems.end(); iter++) {
                
                auto renderConfigItem = [&](int x) {
                    // render key value
                    renderGL->SetColor(250 / 255.0f, 250 / 255.0f, 250 / 255.0f);
                    renderGL->Print(x + 37, i + 2, iter->first.c_str());
                    
                    float textOffset = (x > 0 ? windowSize.x : (windowSize.x / 2)) - 55;
                    if (iter->second.type == Config::ConfigType_Bool) {
                        // render boolean checkbox
                        if (iter->second.value == "true") {
                            renderGL->SetColor(20 / 255.0f, 160 / 255.0f, 20 / 255.0f);
                        } else {
                            renderGL->SetColor(160 / 255.0f, 20 / 255.0f, 20 / 255.0f);
                        }
                        this->_draw->Rect(textOffset - 20, i + 2, 15, 15);
                    } else {
                        float valueLength = renderGL->CalcStringWidth(iter->second.value);
                        renderGL->SetColor(160 / 255.0f, 160 / 255.0f, 160 / 255.0f);
                        renderGL->Print(textOffset - valueLength, i + 2, iter->second.value.c_str());
                    }
                };
                
                renderGL->SetColor(30 / 255.0f, 30 / 255.0f, 30 / 255.0f, 0.9f);
                this->_draw->Rect(30, i, (windowSize.x / 2) - 70, 20);
                this->_draw->Rect((windowSize.x / 2) + 30, i, (windowSize.x / 2) - 70, 20);
                
                renderConfigItem(0);
                
                iter++;
                if (iter == configItems.end()) break;
                
                renderConfigItem((windowSize.x / 2));
                
                i+= 22;
                if (i > (windowSize.y - 20)) {
                    break;
                }
            }
        } else if (this->_currentView == CurrentView::Profiler) {
            int y = (windowSize.y / 2) - 50;
            
            std::stringstream ss;
            
            // DrawTime Graph
            renderGL->SetColor(80 / 255.0f, 80 / 255.0f, 80 / 255.0f, 0.8f);
            this->_draw->Rect(0, 50, windowSize.x, 25);
            
            ss << "Current Scale : " << this->_profilerDrawTimeScale << " : Press [/] to increase and decrease";
            
            renderGL->SetColor(200 / 255.0f, 200 / 255.0f, 200 / 255.0f);
            renderGL->SetFont("basic", 16);
            renderGL->Print(20, 55, "DrawTime Graph");
            renderGL->Print(windowSize.x - 480, 55, ss.str().c_str());
            
            renderGL->SetColor("white");
            
            this->_draw->LineGraph(80, y - 70, ((windowSize.x - 120) / timingResolution), this->_profilerDrawTimeScale, this->_lastDrawTimes, timingResolution);
            
            renderGL->SetColor("red");
            
            this->_draw->LineGraph(80, y - 70, ((windowSize.x - 120) / timingResolution), y - 70 - 95, this->_lastHeapUsages, timingResolution);
            
            renderGL->SetColor(150 / 255.0f, 150 / 255.0f, 150 / 255.0f);
            
            this->_draw->Line(80, y - 70, windowSize.x - 40, y - 70);
            this->_draw->Line(80, 95, 80, y - 70);
            
            renderGL->SetFont("basic", 10);
            
            int graphHeight = y - 70 - 95;
            double gsYinc = 0;
            
            for (int gsY = y - 70; gsY > 95; gsY -= ((y - 70 - 95) / 10)) {
                ss.str("");
                ss << (gsYinc * 1000) << "ms";
                renderGL->Print(10, gsY - 5, ss.str().c_str());
                gsYinc += 0;
            }
            
            // RenderDriver Profiler
            
            int x = 0;
            
            ss.str("");
            
            renderGL->SetColor(80 / 255.0f, 80 / 255.0f, 80 / 255.0f, 0.8f);
            this->_draw->Rect(0, y - 50, windowSize.x, 25);
            
            renderGL->SetColor(200 / 255.0f, 200 / 255.0f, 200 / 255.0f);
            renderGL->SetFont("basic", 16);
            renderGL->Print(20, y - 45, "RenderDriver Profiler");
            renderGL->Print(windowSize.x - 280, y - 45, "Press R to Refresh | -/= to Scroll");
            
            renderGL->SetFont("basic", 10);
            
            this->_draw->Rect(0, y - 20, windowSize.x, 18);
            
            renderGL->SetColor(0.1f, 0.1f, 0.1f);
            
            renderGL->Print(20,  y - 15, "Name");
            renderGL->Print(680, y - 15, "Count");
            renderGL->Print(750, y - 15, "Avg (ns)");
            renderGL->Print(850, y - 15, "Min (ns)");
            renderGL->Print(950, y - 15, "Max (ns)");
            
            y = (windowSize.y / 2) - 50;
            
            this->_profilerX = 0;
            
            y = this->_renderProfileZone(renderGL, windowSize, this->_cachedProfilerDetails["children"], x, 0, y, y);
        }
        
        renderGL->SetFont("basic", 10);
        
        renderGL->SetColor(30 / 255.0f, 30 / 255.0f, 30 / 255.0f, 0.95f);
        this->_draw->Rect(0, 20, windowSize.x, 20);
        
        if (this->_currentView == CurrentView::Console)
            renderGL->SetColor(200 / 255.0f, 200 / 255.0f, 200 / 255.0f);
        else
            renderGL->SetColor(150 / 255.0f, 150 / 255.0f, 150 / 255.0f);
        renderGL->Print(10, 24, "Console (F1)");
        
        if (this->_currentView == CurrentView::Settings)
            renderGL->SetColor(200 / 255.0f, 200 / 255.0f, 200 / 255.0f);
        else
            renderGL->SetColor(150 / 255.0f, 150 / 255.0f, 150 / 255.0f);
        renderGL->Print(130, 24, "Settings (F2)");
        
        if (this->_currentView == CurrentView::Profiler)
            renderGL->SetColor(200 / 255.0f, 200 / 255.0f, 200 / 255.0f);
        else
            renderGL->SetColor(150 / 255.0f, 150 / 255.0f, 150 / 255.0f);
        renderGL->Print(250, 24, "Profiler (F3)");
    }
    
    int EngineUI::_renderProfileZone(RenderDriverPtr renderGL, glm::vec2 windowSize, Json::Value data, int x, int xIndent, int baseY, int y) {
        static std::stringstream ss;
        for (auto iter = data.begin();
             iter != data.end();
             iter++) {
            if (y - this->_currentProfilerScroll >= baseY) {
                if (this->_profilerX++ % 2) {
                    renderGL->SetColor(60 / 255.0f, 60 / 255.0f, 60 / 255.0f, 0.9f);
                } else {
                    renderGL->SetColor(30 / 255.0f, 30 / 255.0f, 30 / 255.0f, 0.9f);
                }
                this->_draw->Rect(0, y - this->_currentProfilerScroll, windowSize.x, 18);
                
                std::string key = iter.key().asString();
                
                renderGL->SetColor(200 / 255.0f, 200 / 255.0f, 200 / 255.0f);
                renderGL->Print(xIndent + 20, y - this->_currentProfilerScroll + 4, key.c_str());
                
                ss.str("");
                ss << (*iter)["count"].asDouble();
                
                renderGL->Print(680, y - this->_currentProfilerScroll + 4, ss.str().c_str());
                
                ss.str("");
                ss << (*iter)["avg"].asDouble() * SEC_TO_NSEC;
                
                renderGL->Print(750, y - this->_currentProfilerScroll + 4, ss.str().c_str());
                
                ss.str("");
                ss << (*iter)["min"].asDouble() * SEC_TO_NSEC;
                
                renderGL->Print(850, y - this->_currentProfilerScroll + 4, ss.str().c_str());
                
                ss.str("");
                ss << (*iter)["max"].asDouble() * SEC_TO_NSEC;
                
                renderGL->Print(950, y - this->_currentProfilerScroll + 4, ss.str().c_str());
            }
            
            y += 18;
            if (y - this->_currentProfilerScroll > windowSize.y) {
                break;
            }
            
            y = this->_renderProfileZone(renderGL, windowSize, (*iter)["children"], x, xIndent + 25, baseY, y);
        }
        return y;
    }

    void EngineUI::OnKeyPress(int key, int press, bool shift) {
        if (!Config::GetBoolean("core.debug.engineUI")) {
            return;
        }
        
        if (key == Key_Console && press == Key_Press) { // `
            this->ToggleConsole();
        } else if (key == Key_F10 && press == Key_Press) { // f10
            GetEventsSingilton()->GetEvent("dumpProfile")->Emit();
        }

        if (!_showConsole) {
            return;
        }
        
        if (key == Key_F1 && press == Key_Press) {
            this->_currentView = CurrentView::Console;
        } else if (key == Key_F2 && press == Key_Press) {
            this->_currentView = CurrentView::Settings;
        } else if (key == Key_F3 && press == Key_Press) {
            this->_currentView = CurrentView::Profiler;
        } else if (this->_currentView == CurrentView::Console) {
            if (key < 256 && (press == Key_Press || press == Key_Repeat) && key != Key_Console && key != Key_Enter) {
#ifndef _PLATFORM_WIN32
                this->_currentConsoleInput << (char) (shift ? getSpecialChars(key) : (char) std::tolower(key));
#else
                currentConsoleInput << (char) (shift ? getSpecialChars(key) : tolower(key));
#endif
            } else if (key == Key_Backspace && (press == Key_Press || press == Key_Repeat)) {
                std::string str = this->_currentConsoleInput.str();
                if (str.length() > 0) {
                    str.resize(str.length() - 1);
                    this->_currentConsoleInput.str(str);
                    if (str.length() != 0) {
                        this->_currentConsoleInput.seekp(str.length());
                    }
                }
            } else if (key == Key_Up && press == Key_Press) {
                if (this->_currentHistoryLine > 0) {
                    this->_currentHistoryLine--;
                    this->_currentConsoleInput.str(this->_commandHistory[this->_currentHistoryLine]);
                    if (this->_currentConsoleInput.str().length() != 0) {
                        this->_currentConsoleInput.seekp(this->_currentConsoleInput.str().length());
                    }
                }
            } else if (key == Key_Down && press == Key_Press) {
                if (this->_commandHistory.size() > 0 && this->_currentHistoryLine < this->_commandHistory.size() - 1) {
                    this->_currentHistoryLine++;
                    this->_currentConsoleInput.str(this->_commandHistory[this->_currentHistoryLine]);
                    if (this->_currentConsoleInput.str().length() != 0) {
                        this->_currentConsoleInput.seekp(this->_currentConsoleInput.str().length());
                    }
                }
            } else if (key == Key_Enter && press == Key_Press) {
                std::string command = this->_currentConsoleInput.str();
                this->_app->GetScriptingContext()->RunCommand(command);
                if (this->_commandHistory.size() == 0 || this->_commandHistory[this->_commandHistory.size() - 1] != command) {
                    this->_commandHistory.push_back(command);
                }
                this->_currentHistoryLine = this->_commandHistory.size();
                this->_currentConsoleInput.str("");
            }
        } else if (this->_currentView == CurrentView::Profiler) {
            if (key == 'R' && press == Key_Press) {
                this->_cachedProfilerDetails = this->_currentProfilerDetails;
            } else if (key == '[' && (press == Key_Press || press == Key_Repeat)) {
                if (this->_profilerDrawTimeScale > 500) {
                    this->_profilerDrawTimeScale -= 500;
                }
            } else if (key == ']' && (press == Key_Press || press == Key_Repeat)) {
                if (this->_profilerDrawTimeScale < 40000) {
                    this->_profilerDrawTimeScale += 500;
                }
            } else if (key == '-' && (press == Key_Press || press == Key_Repeat)) {
                if (this->_currentProfilerScroll > 0) {
                    this->_currentProfilerScroll -= 10;
                }
            } else if (key == '=' && (press == Key_Press || press == Key_Repeat)) {
                if (this->_currentProfilerScroll < 6000) {
                    this->_currentProfilerScroll += 10;
                }
            }
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
    
    EventMagic EngineUI::_profilerHook(Json::Value args) {
        EngineUIPtr eui = GetAppSingilton()->GetEngineUI();
        
        eui->_currentProfilerDetails = args["results"];
    }
    
    EventMagic EngineUI::_captureLastDrawTimes(Json::Value args) {
        EngineUIPtr eui = GetAppSingilton()->GetEngineUI();
        
        Json::Value eArgs(Json::objectValue);
        Json::Value eArray(Json::arrayValue);
        
        for (int i = 0; i < 400; i++) {
            eArray[0] = eui->_lastDrawTimes[i];
        }
        
        eArgs["values"] = eArray;
        
        GetEventsSingilton()->GetEvent("captureLastDrawTimes_callback")->Emit(eArgs);
    }
}