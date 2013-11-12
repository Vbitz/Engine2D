#pragma once

#include <string>
#include <map>
#include <functional>
#include <stdarg.h>

#include "Scripting.hpp"

namespace Engine {
    namespace Events {
        
        class EventArgs {
        public:
            EventArgs() : _readOnly(false) {}
            EventArgs(std::map<std::string, std::string> map);
            EventArgs(v8::Handle<v8::Object> obj);
            
            std::string get(std::string key) const;
            
            bool equals(const EventArgs& rhs);
            
            bool operator==(const EventArgs& rhs) {
                return this->equals(rhs);
            }
            
            std::string operator[](const std::string key) const {
                return this->get(key);
            }
            
            std::string operator[](std::string key) {
                return this->get(key);
            }
            
            void push(std::string key, std::string value);
            
            v8::Handle<v8::Object> getObject();
        
            bool equalsValue(std::string key, std::string value, bool required);
            
        private:
            bool _readOnly;
            std::map<std::string, std::string> _map;
            v8::Persistent<v8::Object> _obj;
        };
        
        typedef void (*EventTargetFunc)(EventArgs e);
        
        void Emit(std::string evnt, std::function<bool(EventArgs)> filter, EventArgs args);
        void Emit(std::string evnt, EventArgs args);
        void Emit(std::string evnt);
        int On(std::string evnt, EventArgs e, EventTargetFunc target);
        int On(std::string evnt, EventArgs e, v8::Persistent<v8::Function> target);
        int On(std::string evnt, EventTargetFunc target);
        int On(std::string evnt, v8::Persistent<v8::Function> target);
        void Clear(int eventID);
    }
}