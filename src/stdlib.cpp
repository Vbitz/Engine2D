#include "stdlib.hpp"

#include <stdlib.h>
#include <string.h>

#include <iostream>

#include <assert.h>

#define STDLIB_ASSERT(val) assert(val)

namespace Engine {
    
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