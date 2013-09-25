#include "JSMod.hpp"

#include <dlfcn.h>

namespace Engine {
    
	namespace JSMod {
        
        typedef void* (*FARPROC)(void);
        
        int _lastOpenModule = 0;
        std::map<int, void*> _openModules;

        ENGINE_JS_METHOD(OpenModule) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARGS_LENGTH(1);
            
            ENGINE_CHECK_ARG_STRING(0, "Arg0 is the path of the module to load");
            
            std::string moduleName = ENGINE_GET_ARG_CPPSTRING_VALUE(0) + _PLATFORM_DYLINK;
            
            if (!Filesystem::FileExists(moduleName)) {
                ENGINE_THROW_ARGERROR("module does not exist for this platform");
            }
            
            void* module = dlopen(Filesystem::GetRealPath(moduleName).c_str(), RTLD_LAZY);
            
            if (module == NULL) {
                ENGINE_THROW_ARGERROR("module could not be loaded");
            }
            
            int modID = _lastOpenModule;
            _openModules[_lastOpenModule++] = module;

            ENGINE_JS_SCOPE_CLOSE(v8::Number::New(modID));
        }
        
        ENGINE_JS_METHOD(CallMethod) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARGS_LENGTH(2);
            
            ENGINE_CHECK_ARG_INT32(0, "Arg0 is the handle to the module returned from mod.open");
            ENGINE_CHECK_ARG_STRING(1, "Arg1 is the name of the method to call");
            
            FARPROC proc = (FARPROC) dlsym(_openModules[ENGINE_GET_ARG_INT32_VALUE(0)], *ENGINE_GET_ARG_CSTRING_VALUE(1));
            
            proc();
            
            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }
        
#define addItem(table, js_name, funct) table->Set(js_name, v8::FunctionTemplate::New(funct))
        
        void InitMod(v8::Handle<v8::ObjectTemplate> modTable) {
            addItem(modTable, "open", OpenModule);
            addItem(modTable, "call", CallMethod);
        }
        
#undef addItem
        
        void CloseAllOpenModules() {
            for (auto iter = _openModules.begin(); iter != _openModules.end(); iter++) {
                if (iter->second != NULL) {
                    dlclose(iter->second);
                }
            }
        }

    }
}