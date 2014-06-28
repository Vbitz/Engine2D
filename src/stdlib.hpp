/*
   Filename: stdlib.hpp
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

#pragma once

#include <stddef.h>
#include <initializer_list>

#include <ostream>
#include <unordered_map>
#include <random>

typedef char string_char_t;

#define ENGINE_CLASS(name) \
    class name; \
    typedef name& name ## Ref; \
    typedef name* name ## Ptr;

namespace Engine {
    
    class String;
    
    ENGINE_CLASS(Color4f);
    
    class Color4f {
    public:
        Color4f();
        Color4f(std::string colorName);
        Color4f(int col);
        Color4f(float r, float g, float b, float a);
        
        static void SetDefinedColor(std::string name, int col);
        static Color4f FromHSV(float h, float s, float v);
        
        float r, g, b, a;
        
    private:
		static void _initPredefinedColors();
    };
    
    template<class A, class B>
    class Map {
        
    };
    
    template<class A, class B>
    class HashMap {
        
    };
    
    template<class T>
    class Array {
        
    };
    
    struct MatchResult {
        
    };
    
    class Regexp {
        
    };
    
    ENGINE_CLASS(StringBuilder);
    
    class StringBuilder {
    public:
        String toString();
        
        StringBuilderRef operator<<(const String value);
        
        operator String();
    private:
        Array<String> _str;
    };
    
    class CString {
        
    };
    
    ENGINE_CLASS(String);
    
    // The string class structure is a pretty blatent clone of JS's string prototype. Over time I will be looking carefuly at usage in order to extend the API
    class String {
    public:
        String(const char* cString);
        String(const char* cString, size_t len);
        
        ~String();
        
        char charAt(size_t index) const;
        
        String concat(Array<String> items) const;
        String concat(std::initializer_list<String> args) const;
        
        bool contains(String search) const;
        bool contains(String search, size_t startIndex) const;
        
        bool startsWith(String search) const;
        bool startsWith(String search, size_t startIndex) const;
        
        bool endsWith(String search) const;
        bool endsWith(String search, size_t startIndex) const;
        
        size_t indexOf(String search) const;
        size_t indexOf(String search, size_t fromIndex) const;
        
        size_t lastIndexOf(String search) const;
        size_t lastIndexOf(String search, size_t fromIndex) const;
        
        Array<MatchResult> match(Regexp regexp) const;
        
        String repeat(size_t count) const;
        
        String replace(String subStr, String newSubStr) const;
        // String replace(String subStr, Function replaceFunc) const;
        String replace(Regexp regexp, String newSubStr) const;
        // String replace(Regexp regexp, Function replaceFunc) const;
        
        size_t search(Regexp regexp) const;
        
        String slice(size_t beginSlice) const;
        String slice(size_t beginSlice, size_t endSlice) const;
        
        Array<String> split() const;
        Array<String> split(String separator) const;
        Array<String> split(String separator, size_t limit) const;
        Array<String> split(Regexp separator) const;
        Array<String> split(Regexp separator, size_t limit) const;
        
        String substr(size_t start) const;
        String substr(size_t start, size_t length) const;
        
        String substring(size_t indexA) const;
        String substring(size_t indexA, size_t indexB) const;
        
        String toLowerCase() const;
        String toUpperCase() const;
        
        String trim() const;
        
        size_t length() const;
        
        char operator[](size_t index) const;
        operator StringBuilder() const;
        
        friend std::ostream& operator<< (std::ostream& stream, const String& str);
        
        CString toCString() const;
        
        const size_t MAX_LENGTH = ((size_t) - 1);
        
    private:
        string_char_t* _str;
        size_t _len;
    };
    
    ENGINE_CLASS(BasicRandom);
    
    class BasicRandom {
    public:
        BasicRandom();
        BasicRandom(long seed);
        
        ~BasicRandom();
        
        int Next();
        int Next(int max);
        int Next(int min, int max);
        
        double NextDouble();
        
        double NextNormal(double mean, double sd);
        
    private:
        std::mt19937 _gen;
    };
}