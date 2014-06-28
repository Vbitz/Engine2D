/*
 Filename: RenderDriver.cpp
 Purpose:  Generic Rendering Interface Spec
 
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

#include "RenderDriver.hpp"

#include "Application.hpp"

namespace Engine {
    Drawable::~Drawable() {
        this->_cleanup();
        this->_render->_cleanupDrawable(this);
    }
    
    void RenderDriver::AddVert(float x, float y, float z) {
        this->AddVert(x, y, z, _currentColor, 0.0, 0.0);
    }
    
    void RenderDriver::AddVert(float x, float y, float z, Color4f col) {
        this->AddVert(x, y, z, col, 0.0, 0.0);
    }
    
    void RenderDriver::AddVert(float x, float y, float z, float s, float t) {
        this->AddVert(x, y, z, _currentColor, s, t);
    }
    
    void RenderDriver::AddVert(float x, float y, float z, Color4f col, float s, float t) {
        this->_addVert(x, y, z, col, s, t);
    }
    
    void RenderDriver::Print(float x, float y, const char* string) {
        RenderDriver::DrawProfiler p = this->Profile(__PRETTY_FUNCTION__);
        
        this->_getSheet(this->_currentFontName)->DrawText(this, x, y, this->_currentFontSize, string);
    }
    
    float RenderDriver::CalcStringWidth(std::string str) {
        return this->_getSheet(this->_currentFontName)->MeasureText(this->_currentFontSize, str);
    }
    
    void RenderDriver::SetFont(std::string name, int size) {
        this->_currentFontName = name;
        this->_currentFontSize = size;
    }
    
    void RenderDriver::LoadFont(std::string prettyName, std::string filename) {
        this->_sheets[prettyName] = FontSheetReader::LoadFont(filename);
        this->_currentFontName = prettyName;
    }
    
    bool RenderDriver::IsFontLoaded(std::string name) {
        return this->_sheets.count(name) > 0;
    }
    
    void RenderDriver::ClearColor(Color4f col) {
        this->_clearColor(col);
    }
    
    void RenderDriver::ClearColor(int col) {
        ClearColor(Color4f(col));
    }
    
    void RenderDriver::ClearColor(std::string colorName) {
        ClearColor(Color4f(colorName));
    }
    
    void RenderDriver::ClearColor(float r, float g, float b) {
        ClearColor(Color4f(r, g, b, 1.0f));
    }
    
    Color4f RenderDriver::GetColor() {
        return this->_currentColor;
    }
    
    void RenderDriver::SetColor(Color4f col) {
        this->_currentColor = col;
    }
    
    void RenderDriver::SetColor(int col) {
        SetColor(Color4f(col));
    }
    
    void RenderDriver::SetColor(std::string colorName) {
        if (colorName[0] == '#') {
            unsigned int tempCol;
            std::stringstream ss;
            ss << std::hex << colorName.substr(1);
            ss >> tempCol;
            SetColor(tempCol);
        } else {
            SetColor(Color4f(colorName));
        }
    }
    
    void RenderDriver::SetColor(float r, float g, float b) {
        SetColor(r, g, b, 1.0f);
    }
    
    void RenderDriver::SetColor(float r, float g, float b, float a) {
        this->_currentColor = Color4f(r, g, b, a);
    }
    
    void RenderDriver::BeginProfiling() {
        this->_profiling = true;
        this->_profileResults.clear();
    }
    
    void RenderDriver::EndProfilingFrame() {
        if (!this->_profiling) return;
        this->_profiling = false;
        Json::Value resultsObj(Json::objectValue);
        Json::Value results(Json::objectValue);
        
        for (auto iter = this->_profileResults.begin(); iter != this->_profileResults.end(); iter++) {
            Json::Value resultPoint(Json::objectValue);
            resultPoint["count"] = iter->second.callCount;
            resultPoint["avg"] = iter->second.avg;
            resultPoint["min"] = iter->second.min;
            resultPoint["max"] = iter->second.max;
            results[iter->first] = resultPoint;
        }
        
        resultsObj["results"] = results;
        
        Events::GetEvent("onDrawProfileEnd")->Emit(resultsObj);
    }
    
    FontSheetPtr RenderDriver::_getSheet(std::string fontName) {
        if (!this->IsFontLoaded("basic")) {
            Logger::begin("RenderDriver", Logger::LogLevel_Verbose) << "Loading NeoFont: " << Config::GetString("core.content.fontPath") << Logger::end();
            this->_sheets["basic"] = FontSheetReader::LoadFont(Config::GetString("core.content.fontPath"));
        }
        return this->_sheets[fontName];
    }
    
    void RenderDriver::_cleanupDrawable(DrawablePtr drawable) {
        
    }
    
    void RenderDriver::_submitProfile(const char zone[], double time) {
        if (!this->_profiling) return;
        ProfileDataPointRef pnt = this->_profileResults[zone];
        pnt.callCount++;
        if (pnt.avg == -1) {
            pnt.avg = time;
        } else {
            pnt.avg += (time - pnt.avg) / pnt.callCount;
        }
        if (time > pnt.max) {
            pnt.max = time;
        }
        if (time < pnt.min) {
            pnt.min = time;
        }
    }
}