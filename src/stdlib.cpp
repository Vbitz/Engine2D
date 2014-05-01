/*
   Filename: stdlib.cpp
   Purpose:  Implamentation of Engine basic types

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

#include "stdlib.hpp"

#include <stdlib.h>
#include <string.h>

#include <iostream>

#include <assert.h>

#define STDLIB_ASSERT(val) assert(val)

namespace Engine {
    
    std::unordered_map<std::string, Color4f> _predefinedColors;
    
    Color4f::Color4f() : r(1.0f), g(1.0f), b(1.0f), a(1.0f) {
        
    }
    
    Color4f::Color4f(std::string colorName) {
        _initPredefinedColors();
        Color4f predef = _predefinedColors[colorName];
        this->r = predef.r;
        this->g = predef.g;
        this->b = predef.b;
        this->a = predef.a;
    }
    
    Color4f::Color4f(int col) {
        this->r = (float)((col & 0xff0000) >> 16) / 255;
        this->g = (float)((col & 0x00ff00) >> 8) / 255;
        this->b = (float)( col & 0x0000ff) / 255;
        this->a = 1.0f;
    }
    
    Color4f::Color4f(float r, float g, float b, float a)
        : r(r), g(g), b(b), a(a) {
    }
    
    void Color4f::SetDefinedColor(std::string name, int col) {
        _predefinedColors[name] = col;
    }
    
    void Color4f::_initPredefinedColors() {
        static bool predefFilled = false;
        if (!predefFilled) {
            predefFilled = true;
        } else {
            return;
        }
        _predefinedColors["pink"] = Color4f(0xFFC0CB);
        _predefinedColors["lightPink"] = Color4f(0xFFB6C1);
        _predefinedColors["hotPink"] = Color4f(0xFF69B4);
        _predefinedColors["deepPink"] = Color4f(0xFF1493);
        _predefinedColors["paleVioletRed"] = Color4f(0xDB7093);
        _predefinedColors["mediumVioletRed"] = Color4f(0xC71585);
        _predefinedColors["lightSalmon"] = Color4f(0xFFA07A);
        _predefinedColors["salmon"] = Color4f(0xFA8072);
        _predefinedColors["darkSalmon"] = Color4f(0xE9967A);
        _predefinedColors["lightCoral"] = Color4f(0xF08080);
        _predefinedColors["indianRed"] = Color4f(0xCD5C5C);
        _predefinedColors["crimson"] = Color4f(0xDC143C);
        _predefinedColors["fireBrick"] = Color4f(0xB22222);
        _predefinedColors["darkRed"] = Color4f(0x8B0000);
        _predefinedColors["red"] = Color4f(0xFF0000);
        _predefinedColors["orangeRed"] = Color4f(0xFF4500);
        _predefinedColors["tomato"] = Color4f(0xFF6347);
        _predefinedColors["coral"] = Color4f(0xFF7F50);
        _predefinedColors["darkOrange"] = Color4f(0xFF8C00);
        _predefinedColors["orange"] = Color4f(0xFFA500);
        _predefinedColors["gold"] = Color4f(0xFFD700);
        _predefinedColors["yellow"] = Color4f(0xFFFF00);
        _predefinedColors["lightYellow"] = Color4f(0xFFFFE0);
        _predefinedColors["lemonChiffon"] = Color4f(0xFFFACD);
        _predefinedColors["lightGoldenrodYellow"] = Color4f(0xFAFAD2);
        _predefinedColors["papayaWhip"] = Color4f(0xFFEFD5);
        _predefinedColors["moccasin"] = Color4f(0xFFE4B5);
        _predefinedColors["peachPuff"] = Color4f(0xFFDAB9);
        _predefinedColors["paleGoldenrod"] = Color4f(0xEEE8AA);
        _predefinedColors["khaki"] = Color4f(0xF0E68C);
        _predefinedColors["darkKhaki"] = Color4f(0xBDB76B);
        _predefinedColors["cornsilk"] = Color4f(0xFFF8DC);
        _predefinedColors["blanchedAlmond"] = Color4f(0xFFEBCD);
        _predefinedColors["bisque"] = Color4f(0xFFE4C4);
        _predefinedColors["navajoWhite"] = Color4f(0xFFDEAD);
        _predefinedColors["wheat"] = Color4f(0xF5DEB3);
        _predefinedColors["burlyWood"] = Color4f(0xDEB887);
        _predefinedColors["tan"] = Color4f(0xD2B48C);
        _predefinedColors["rosyBrown"] = Color4f(0xBC8F8F);
        _predefinedColors["sandyBrown"] = Color4f(0xF4A460);
        _predefinedColors["goldenrod"] = Color4f(0xDAA520);
        _predefinedColors["darkGoldenrod"] = Color4f(0xB8860B);
        _predefinedColors["peru"] = Color4f(0xCD853F);
        _predefinedColors["chocolate"] = Color4f(0xD2691E);
        _predefinedColors["saddleBrown"] = Color4f(0x8B4513);
        _predefinedColors["sienna"] = Color4f(0xA0522D);
        _predefinedColors["brown"] = Color4f(0xA52A2A);
        _predefinedColors["maroon"] = Color4f(0x800000);
        _predefinedColors["darkOliveGreen"] = Color4f(0x556B2F);
        _predefinedColors["olive"] = Color4f(0x808000);
        _predefinedColors["oliveDrab"] = Color4f(0x6B8E23);
        _predefinedColors["yellowGreen"] = Color4f(0x9ACD32);
        _predefinedColors["limeGreen"] = Color4f(0x32CD32);
        _predefinedColors["lime"] = Color4f(0x00FF00);
        _predefinedColors["lawnGreen"] = Color4f(0x7CFC00);
        _predefinedColors["chartreuse"] = Color4f(0x7FFF00);
        _predefinedColors["greenYellow"] = Color4f(0xADFF2F);
        _predefinedColors["springGreen"] = Color4f(0x00FF7F);
        _predefinedColors["nediumSpringGreen"] = Color4f(0x00FA9A);
        _predefinedColors["lightGreen"] = Color4f(0x90EE90);
        _predefinedColors["paleGreen"] = Color4f(0x98FB98);
        _predefinedColors["darkSeaGreen"] = Color4f(0x8FBC8F);
        _predefinedColors["mediumSeaGreen"] = Color4f(0x3CB371);
        _predefinedColors["seaGreen"] = Color4f(0x2E8B57);
        _predefinedColors["forestGreen"] = Color4f(0x228B22);
        _predefinedColors["green"] = Color4f(0x008000);
        _predefinedColors["darkGreen"] = Color4f(0x006400);
        _predefinedColors["mediumAquamarine"] = Color4f(0x66CDAA);
        _predefinedColors["aqua"] = Color4f(0x00FFFF);
        _predefinedColors["cyan"] = Color4f(0x00FFFF);
        _predefinedColors["lightCyan"] = Color4f(0xE0FFFF);
        _predefinedColors["paleTurquoise"] = Color4f(0xAFEEEE);
        _predefinedColors["aquamarine"] = Color4f(0x7FFFD4);
        _predefinedColors["turquoise"] = Color4f(0x40E0D0);
        _predefinedColors["mediumTurquoise"] = Color4f(0x48D1CC);
        _predefinedColors["darkTurquoise"] = Color4f(0x00CED1);
        _predefinedColors["lightSeaGreen"] = Color4f(0x20B2AA);
        _predefinedColors["cadetBlue"] = Color4f(0x5F9EA0);
        _predefinedColors["darkCyan"] = Color4f(0x008B8B);
        _predefinedColors["teal"] = Color4f(0x008080);
        _predefinedColors["lightSteelBlue"] = Color4f(0xB0C4DE);
        _predefinedColors["powderBlue"] = Color4f(0xB0E0E6);
        _predefinedColors["lightBlue"] = Color4f(0xADD8E6);
        _predefinedColors["skyBlue"] = Color4f(0x87CEEB);
        _predefinedColors["lightSkyBlue"] = Color4f(0x87CEFA);
        _predefinedColors["deepSkyBlue"] = Color4f(0x00BFFF);
        _predefinedColors["dodgerBlue"] = Color4f(0x1E90FF);
        _predefinedColors["dornflowerBlue"] = Color4f(0x6495ED);
        _predefinedColors["steelBlue"] = Color4f(0x4682B4);
        _predefinedColors["royalBlue"] = Color4f(0x4169E1);
        _predefinedColors["blue"] = Color4f(0x0000FF);
        _predefinedColors["mediumBlue"] = Color4f(0x0000CD);
        _predefinedColors["darkBlue"] = Color4f(0x00008B);
        _predefinedColors["navy"] = Color4f(0x000080);
        _predefinedColors["midnightBlue"] = Color4f(0x191970);
        _predefinedColors["lavender"] = Color4f(0xE6E6FA);
        _predefinedColors["thistle"] = Color4f(0xD8BFD8);
        _predefinedColors["plum"] = Color4f(0xDDA0DD);
        _predefinedColors["violet"] = Color4f(0xEE82EE);
        _predefinedColors["orchid"] = Color4f(0xDA70D6);
        _predefinedColors["fuchsia"] = Color4f(0xFF00FF);
        _predefinedColors["magenta"] = Color4f(0xFF00FF);
        _predefinedColors["mediumOrchid"] = Color4f(0xBA55D3);
        _predefinedColors["mediumPurple"] = Color4f(0x9370DB);
        _predefinedColors["blueViolet"] = Color4f(0x8A2BE2);
        _predefinedColors["darkViolet"] = Color4f(0x9400D3);
        _predefinedColors["darkOrchid"] = Color4f(0x9932CC);
        _predefinedColors["darkMagenta"] = Color4f(0x8B008B);
        _predefinedColors["purple"] = Color4f(0x800080);
        _predefinedColors["indigo"] = Color4f(0x4B0082);
        _predefinedColors["darkSlateBlue"] = Color4f(0x483D8B);
        _predefinedColors["slateBlue"] = Color4f(0x6A5ACD);
        _predefinedColors["mediumSlateBlue"] = Color4f(0x7B68EE);
        _predefinedColors["white"] = Color4f(0xFFFFFF);
        _predefinedColors["snow"] = Color4f(0xFFFAFA);
        _predefinedColors["honeydew"] = Color4f(0xF0FFF0);
        _predefinedColors["mintCream"] = Color4f(0xF5FFFA);
        _predefinedColors["azure"] = Color4f(0xF0FFFF);
        _predefinedColors["aliceBlue"] = Color4f(0xF0F8FF);
        _predefinedColors["ghostWhite"] = Color4f(0xF8F8FF);
        _predefinedColors["whiteSmoke"] = Color4f(0xF5F5F5);
        _predefinedColors["seashell"] = Color4f(0xFFF5EE);
        _predefinedColors["beige"] = Color4f(0xF5F5DC);
        _predefinedColors["oldLace"] = Color4f(0xFDF5E6);
        _predefinedColors["floralWhite"] = Color4f(0xFFFAF0);
        _predefinedColors["ivory"] = Color4f(0xFFFFF0);
        _predefinedColors["antiqueWhite"] = Color4f(0xFAEBD7);
        _predefinedColors["linen"] = Color4f(0xFAF0E6);
        _predefinedColors["lavenderBlush"] = Color4f(0xFFF0F5);
        _predefinedColors["mistyRose"] = Color4f(0xFFE4E1);
        _predefinedColors["gainsboro"] = Color4f(0xDCDCDC);
        _predefinedColors["lightGrey"] = Color4f(0xD3D3D3);
        _predefinedColors["silver"] = Color4f(0xC0C0C0);
        _predefinedColors["darkGrey"] = Color4f(0xA9A9A9);
        _predefinedColors["grey"] = Color4f(0x808080);
        _predefinedColors["dimGrey"] = Color4f(0x696969);
        _predefinedColors["lightSlateGrey"] = Color4f(0x778899);
        _predefinedColors["slateGrey"] = Color4f(0x708090);
        _predefinedColors["darkSlateGrey"] = Color4f(0x2F4F4F);
        _predefinedColors["black"] = Color4f(0x000000);
    }
    
    String StringBuilder::toString() {
        return "";
    }
    
    StringBuilder& StringBuilder::operator<<(const Engine::String value) {
        
    }
    
    StringBuilder::operator String() {
        return this->toString();
    }
    
    String::String(const char* cString) : String(cString, strlen(cString)) { }
    
    String::String(const char* cString, size_t len) {
        this->_len = len;
        this->_str = new string_char_t[len];
        
        for (size_t i = 0; i < len; i++) {
            this->_str[i] = cString[i];
        }
    }
    
    String::~String() {
        STDLIB_ASSERT(this != NULL);
        delete [] this->_str;
    }
    
    char String::charAt(size_t index) const {
        STDLIB_ASSERT(this != NULL);
        if (index > this->_len) return '\0';
        return this->_str[index];
    }
    
    String String::concat(Array<String> items) const {
        return "";
    }
    
    String String::concat(std::initializer_list<String> args) const {
        StringBuilder sb = *this;
        for (auto iter = args.begin(); iter != args.end(); iter++) {
            sb << *iter;
        }
        return sb;
    }
    
    bool String::contains(String search) const {
        return this->contains(search, 0);
    }
    
    bool String::contains(String search, size_t startIndex) const {
        
    }
    
    bool String::startsWith(String search) const {
        return this->startsWith(search, 0);
    }
    
    bool String::startsWith(String search, size_t startIndex) const {
        
    }
    
    bool String::endsWith(String search) const {
        return this->endsWith(search, this->length());
    }
    
    bool String::endsWith(String search, size_t startIndex) const {
        
    }
    
    size_t String::indexOf(String search) const {
        return this->indexOf(search, 0);
    }
    
    size_t String::indexOf(String search, size_t fromIndex) const {
        return strcspn(this->_str, &(search._str[fromIndex]));
    }
    
    size_t String::lastIndexOf(String search) const {
        return this->lastIndexOf(search, this->length());
    }
    
    size_t String::lastIndexOf(String search, size_t fromIndex) const {
        
    }
    
    Array<MatchResult> String::match(Regexp regexp) const {
        return Array<MatchResult>();
    }
    
    size_t String::length() const {
        return this->_len;
    }
    
    char String::operator[](size_t index) const {
        return this->charAt(index);
    }
    
    String::operator StringBuilder() const {
        StringBuilder b;
        b << *this;
        return b;
    }
    
    std::ostream& operator<< (std::ostream& stream, const String& str) {
        stream << std::string(str._str, str._len);
    }
    
    CString String::toCString() const {
        
    }

}