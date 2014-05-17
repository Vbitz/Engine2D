/*
   Filename: JSUnsafe.cpp
   Purpose:  Native memory access and management for Javascript

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

#include "JSUnsafe.hpp"

#include "Util.hpp"

#include <stdlib.h>

#include <sys/mman.h>

namespace Engine {
    
	namespace JsUnsafe {

        ENGINE_JS_METHOD(GetNumberAddress) {
            ENGINE_JS_SCOPE_OPEN;
            
            int* numAddr = (int*) malloc(sizeof(int));
            
            *numAddr = ENGINE_GET_ARG_INT32_VALUE(0);
            
            // MEMORY LEAK: Kind of the point of this
            ENGINE_JS_SCOPE_CLOSE(v8::Number::New(args.GetIsolate(), (long) numAddr));
        }

        ENGINE_JS_METHOD(GetNative) {
            ENGINE_JS_SCOPE_OPEN;
            
            long offset = (long) ENGINE_GET_ARG_NUMBER_VALUE(0);
            int length = ENGINE_GET_ARG_INT32_VALUE(1);

            v8::Handle<v8::Object> array = v8::Object::New(args.GetIsolate());

            void* rawPointer = (void*) offset;

            array->SetIndexedPropertiesToExternalArrayData(rawPointer, v8::kExternalUnsignedByteArray, length * sizeof(unsigned char));

            ENGINE_JS_SCOPE_CLOSE(array);
        }

        typedef void* (*FARPROC)(void);

        ENGINE_JS_METHOD(Call) {
            ENGINE_JS_SCOPE_OPEN;

            long address = (long) ENGINE_GET_ARG_NUMBER_VALUE(0);

            FARPROC func = (FARPROC) address;
            
            try {
                func();
                ENGINE_JS_SCOPE_CLOSE(v8::Boolean::New(args.GetIsolate(), true));
            } catch (...) {
                ENGINE_JS_SCOPE_CLOSE(v8::Boolean::New(args.GetIsolate(), false));
            }
        }

        ENGINE_JS_METHOD(Malloc) {
            ENGINE_JS_SCOPE_OPEN;

            int arrayLength = ENGINE_GET_ARG_INT32_VALUE(0);

            v8::Handle<v8::Object> array = v8::Object::New(args.GetIsolate());

            char* rawArray = (char*) malloc(arrayLength);

            array->SetIndexedPropertiesToExternalArrayData(rawArray, v8::kExternalByteArray, arrayLength * sizeof(char));

            ENGINE_JS_SCOPE_CLOSE(array);
        }

        ENGINE_JS_METHOD(Free) {
            ENGINE_JS_SCOPE_OPEN;

            v8::Handle<v8::Array> arr = v8::Handle<v8::Array>(ENGINE_GET_ARG_ARRAY(0));

            void* rawAddr = arr->GetIndexedPropertiesExternalArrayData();

            free(rawAddr);

            ENGINE_JS_SCOPE_CLOSE_UNDEFINED;
        }

        ENGINE_JS_METHOD(AddressOf) {
            ENGINE_JS_SCOPE_OPEN;
            
            v8::Handle<v8::Array> arr = v8::Handle<v8::Array>(ENGINE_GET_ARG_ARRAY(0));

            void* rawAddr = arr->GetIndexedPropertiesExternalArrayData();

            long address = (long) rawAddr;

            ENGINE_JS_SCOPE_CLOSE(v8::Number::New(args.GetIsolate(), (double) address));
        }
        
        ENGINE_JS_METHOD(AddressOfExternal) {
            ENGINE_JS_SCOPE_OPEN;
            
            ENGINE_CHECK_ARGS_LENGTH(1);
            
            ENGINE_CHECK_ARG_EXTERNAL(0, "Arg0 is the value to return the indexof");
            
            long exAddr = (long) ENGINE_GET_ARG_EXTERNAL_VALUE(0);
            
            ENGINE_JS_SCOPE_CLOSE(v8::Number::New(args.GetIsolate(), (double) exAddr));
        }

        ENGINE_JS_METHOD(MProtect) {
            ENGINE_JS_SCOPE_OPEN;
            
            long address = (long) ENGINE_GET_ARG_NUMBER_VALUE(0);

            int length = ENGINE_GET_ARG_INT32_VALUE(1);
            bool enable = ENGINE_GET_ARG_BOOLEAN_VALUE(2);

            int res = -1;

            if (enable) {
                res = mprotect((void*) address, length, PROT_READ | PROT_EXEC);
            } else {
                res = mprotect((void*) address, length, PROT_READ | PROT_WRITE);
            }

            ENGINE_JS_SCOPE_CLOSE(v8::Boolean::New(args.GetIsolate(), res > -1));
        }

        ENGINE_JS_METHOD(GetPageSize) {
            ENGINE_JS_SCOPE_OPEN;
            ENGINE_JS_SCOPE_CLOSE(v8::Number::New(args.GetIsolate(), getpagesize()));
        }
        
#define addItem(table, js_name, funct) table->Set(isolate, js_name, v8::FunctionTemplate::New(isolate, funct))
        
        void InitUnsafe(v8::Handle<v8::ObjectTemplate> unsafeTable) {
            v8::Isolate* isolate = v8::Isolate::GetCurrent();
            
            addItem(unsafeTable, "getNumberAddress", GetNumberAddress);
            addItem(unsafeTable, "getNative", GetNative);
            addItem(unsafeTable, "call", Call);
            addItem(unsafeTable, "malloc", Malloc);
            addItem(unsafeTable, "free", Free);
            addItem(unsafeTable, "addressOf", AddressOf);
            addItem(unsafeTable, "addressOfExternal", AddressOfExternal);
            addItem(unsafeTable, "mprotect", MProtect);
            addItem(unsafeTable, "getPageSize", GetPageSize);
            
            unsafeTable->Set(isolate, "pageSize", v8::Number::New(isolate, getpagesize()));
        }
        
#undef addItem

    }
}