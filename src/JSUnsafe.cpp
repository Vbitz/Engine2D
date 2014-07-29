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

#include "ScriptingManager.hpp"

#include <stdlib.h>
#include <unistd.h>

#include <sys/mman.h>

namespace Engine {
    
	namespace JsUnsafe {

        void GetNumberAddress(const v8::FunctionCallbackInfo<v8::Value>& _args) {
            ScriptingManager::Arguments args(_args);
            
            int* numAddr = (int*) malloc(sizeof(int));
            
            *numAddr = args.NumberValue(0);
            
            // MEMORY LEAK: Kind of the point of this
            args.SetReturnValue(args.NewNumber((long) numAddr));
        }

        void GetNative(const v8::FunctionCallbackInfo<v8::Value>& _args) {
            ScriptingManager::Arguments args(_args);
            
            long offset = (long) args.NumberValue(0);
            int length = args.Int32Value(1);

            v8::Handle<v8::Object> array = v8::Object::New(args.GetIsolate());

            void* rawPointer = (void*) offset;

            array->SetIndexedPropertiesToExternalArrayData(rawPointer, v8::kExternalUnsignedByteArray, length * sizeof(unsigned char));

            args.SetReturnValue(array);
        }

        typedef void* (*FARPROC)(void);

        void Call(const v8::FunctionCallbackInfo<v8::Value>& _args) {
            ScriptingManager::Arguments args(_args);

            long address = (long) args.NumberValue(0);

            FARPROC func = (FARPROC) address;
            
            try {
                func();
                args.SetReturnValue(args.NewBoolean(true));
            } catch (...) {
                args.SetReturnValue(args.NewBoolean(false));
            }
        }

        void Malloc(const v8::FunctionCallbackInfo<v8::Value>& _args) {
            ScriptingManager::Arguments args(_args);

            int arrayLength = args.Int32Value(0);

            v8::Handle<v8::Object> array = v8::Object::New(args.GetIsolate());

            char* rawArray = (char*) malloc(arrayLength);

            array->SetIndexedPropertiesToExternalArrayData(rawArray, v8::kExternalByteArray, arrayLength * sizeof(char));

            args.SetReturnValue(array);
        }

        void Free(const v8::FunctionCallbackInfo<v8::Value>& _args) {
            ScriptingManager::Arguments args(_args);

            v8::Handle<v8::Array> arr = v8::Handle<v8::Array>::Cast(args[0]);

            void* rawAddr = arr->GetIndexedPropertiesExternalArrayData();

            free(rawAddr);
        }

        void AddressOf(const v8::FunctionCallbackInfo<v8::Value>& _args) {
            ScriptingManager::Arguments args(_args);
            
            v8::Handle<v8::Array> arr = v8::Handle<v8::Array>::Cast(args[0]);

            void* rawAddr = arr->GetIndexedPropertiesExternalArrayData();

            long address = (long) rawAddr;

            args.SetReturnValue(args.NewNumber((double) address));
        }
        
        void AddressOfExternal(const v8::FunctionCallbackInfo<v8::Value>& _args) {
            ScriptingManager::Arguments args(_args);
            
            long exAddr = (long) args.ExternalValue(0);
            
            args.SetReturnValue(args.NewNumber((double) exAddr));
        }

        void MProtect(const v8::FunctionCallbackInfo<v8::Value>& _args) {
            ScriptingManager::Arguments args(_args);
            
            long address = (long) args.NumberValue(0);

            int length = args.Int32Value(1);
            bool enable = args.BooleanValue(2);

            int res = -1;

            if (enable) {
                res = mprotect((void*) address, length, PROT_READ | PROT_EXEC);
            } else {
                res = mprotect((void*) address, length, PROT_READ | PROT_WRITE);
            }

            args.SetReturnValue(args.NewBoolean(res > -1));
        }

        void GetPageSize(const v8::FunctionCallbackInfo<v8::Value>& _args) {
            ScriptingManager::Arguments args(_args);
            args.SetReturnValue(args.NewNumber(getpagesize()));
        }
        
        void InitUnsafe(v8::Handle<v8::ObjectTemplate> unsafeTable) {
            ScriptingManager::Factory f(v8::Isolate::GetCurrent());
            
            f.FillTemplate(unsafeTable, {
                {FTT_Static, "getNumberAddress", f.NewFunctionTemplate(GetNumberAddress)},
                {FTT_Static, "getNative", f.NewFunctionTemplate(GetNative)},
                {FTT_Static, "call", f.NewFunctionTemplate(Call)},
                {FTT_Static, "malloc", f.NewFunctionTemplate(Malloc)},
                {FTT_Static, "free", f.NewFunctionTemplate(Free)},
                {FTT_Static, "addressOf", f.NewFunctionTemplate(AddressOf)},
                {FTT_Static, "addressOfExternal", f.NewFunctionTemplate(AddressOfExternal)},
                {FTT_Static, "mprotect", f.NewFunctionTemplate(MProtect)},
                {FTT_Static, "getPageSize", f.NewFunctionTemplate(GetPageSize)},
                {FTT_Static, "pageSize", f.NewNumber(getpagesize())}
            });
        }

    }
}