#pragma once

typedef char string_char_t;

namespace Engine {
    
    class String;
    
    template<class T>
    class Array {
        
    };
    
    struct MatchResult {
        
    };
    
    class Regexp {
        
    };
    
    class StringStream {
        
    };
    
    class CString {
        
    };
    
    // The string class structure is a pretty blatent clone of JS's string prototype. Over time I will be looking carefuly at usage in order to extend the API
    class String {
    public:
        String(const char* cString);
        String(const char* cString, int len);
        
        char charAt(int index);
        
        String concat(...);
        
        bool contains(String search);
        bool contains(String search, int startIndex);
        
        bool endsWith(String search);
        bool endsWith(String search, int startIndex);
        
        bool startsWith(String search);
        bool startsWith(String search, int startIndex);
        
        int indexOf(String search);
        int indexOf(String search, int fromIndex);
        
        int lastIndexOf(String search);
        int lastIndexOf(String search, int fromIndex);
        
        Array<MatchResult> match(Regexp regexp);
        
        String repeat(int count);
        
        String replace(String subStr, String newSubStr);
        // String replace(String subStr, Function replaceFunc);
        String replace(Regexp regexp, String newSubStr);
        // String replace(Regexp regexp, Function replaceFunc);
        
        int search(Regexp regexp);
        
        String slice(int beginSlice);
        String slice(int beginSlice, int endSlice);
        
        Array<String> split();
        Array<String> split(String separator);
        Array<String> split(String separator, int limit);
        Array<String> split(Regexp separator);
        Array<String> split(Regexp separator, int limit);
        
        String substr(int start);
        String substr(int start, int length);
        
        String substring(int indexA);
        String substring(int indexA, int indexB);
        
        String toLowerCase();
        String toUpperCase();
        
        String trim();
        
        int length();
        
        char operator[](int index);
        
        CString toCString();
        
    private:
        string_char_t* _str;
        int _len;
    };
}