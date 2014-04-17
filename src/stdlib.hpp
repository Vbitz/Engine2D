#pragma once

#include <stddef.h>
#include <initializer_list>

#include <ostream>

typedef char string_char_t;

namespace Engine {
    
    class String;
    
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
    
    class StringBuilder {
    public:
        String toString();
        
        StringBuilder& operator<<(const String value);
        
        operator String();
    private:
        Array<String> _str;
    };
    
    class CString {
        
    };
    
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
}