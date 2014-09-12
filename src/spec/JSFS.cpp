/*
   Filename: JSFS.cpp
   Purpose:  Javascript bindings for Engine::Filesystem

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

#include "../JSFS.hpp"

#include "../ScriptingManager.hpp"
#include "../Filesystem.hpp"

namespace Engine {
    namespace JsFS {
        void ReadFile(const v8::FunctionCallbackInfo<v8::Value>& _args) {
            ScriptingManager::Arguments args(_args);
            
            if (args.Assert(args[0]->IsString(), "Arg0 is the path to the file to read")) return;
            
            std::string path = args.StringValue(0);
            
            if (!Filesystem::FileExists(path)) {
                args.ThrowArgError("File does not exist");
                return;
            }
            
            if (args.Length() == 2) {
                if (args.Assert(args[0]->IsBoolean(), "Set Arg1 if you need a raw byte array")) return;
                long fileLength = 0;
                char* file = Filesystem::GetFileContent(path, fileLength);
                v8::Handle<v8::Array> arr = args.NewArray();
                for (int i = 0; i < fileLength; i++) {
                    arr->Set(i, args.NewNumber((unsigned char) file[i]));
                }
                args.SetReturnValue(arr);
            } else {
                args.SetReturnValue(args.NewString(Filesystem::GetFileContent(path)));
            }
        }
        
        void WriteFile(const v8::FunctionCallbackInfo<v8::Value>& _args) {
            ScriptingManager::Arguments args(_args);
            
            if (args.AssertCount(2)) return;
            
            if (args.Assert(args[0]->IsString(), "Arg0 is the path to the file to write") ||
                args.Assert(args[1]->IsString(), "Arg1 is the content to write to the file")) return;
            
            Filesystem::WriteFile(args.StringValue(0), *v8::String::Utf8Value(args[1]), args[1]->ToString()->Length());
        }
        
        void FileExists(const v8::FunctionCallbackInfo<v8::Value>& _args) {
            ScriptingManager::Arguments args(_args);
            
            if (args.AssertCount(1)) return;
            
            if (args.Assert(args[0]->IsString(), "Arg0 is the path to the file")) return;
            
            args.SetReturnValue(args.NewBoolean(Filesystem::FileExists(args.StringValue(0))));
        }
        
        void FileSize(const v8::FunctionCallbackInfo<v8::Value>& _args) {
            ScriptingManager::Arguments args(_args);
            
            if (args.AssertCount(1)) return;
            
            if (args.Assert(args[0]->IsString(), "Arg0 is the path to the file")) return;
            
            args.SetReturnValue(args.NewInt32((int) Filesystem::FileSize(args.StringValue(0))));
        }
        
        void MountFile(const v8::FunctionCallbackInfo<v8::Value>& _args) {
            ScriptingManager::Arguments args(_args);
            
            if (args.AssertCount(2)) return;
            
            if (args.Assert(args[0]->IsString(), "Arg0 is the path to the archivefile") ||
                args.Assert(args[1]->IsString(), "Arg1 is the path to mount the archive to")) return;
            
            std::string path = args.StringValue(0);
            
            if (!Filesystem::FileExists(path)) {
                bool result = Filesystem::Mount(Filesystem::GetRealPath(path), args.StringValue(1));
                
                args.SetReturnValue(args.NewBoolean(result));
            } else {
                args.ThrowArgError("File does not exist");
            }
        }
        
        void ConfigDir(const v8::FunctionCallbackInfo<v8::Value>& _args) {
            ScriptingManager::Arguments args(_args);
            
            if (args.AssertCount(1)) return;
            
            if (args.Assert(args[0]->IsString(), "Arg0 is the application name")) return;
            
            Filesystem::SetupUserDir(args.StringValue(0));
        }
        
        void HasSetConfigDir(v8::Local<v8::String> name, const v8::PropertyCallbackInfo<v8::Value>& args) {
            args.GetReturnValue().Set(Filesystem::HasSetUserDir());
        }
        
        void Mkdir(const v8::FunctionCallbackInfo<v8::Value>& _args) {
            ScriptingManager::Arguments args(_args);
            
            if (args.AssertCount(1)) return;
            
            if (args.Assert(args[0]->IsString(), "Arg0 is the path to the directory to create")) return;
            
            Filesystem::Mkdir(args.StringValue(0));
        }
        
        void Lsdir(const v8::FunctionCallbackInfo<v8::Value>& _args) {
            ScriptingManager::Arguments args(_args);
            
            if (args.AssertCount(1)) return;
            
            if (args.Assert(args[0]->IsString(), "Arg0 is the path to the directory to list")) return;
            
            v8::Handle<v8::Array> ret = args.NewArray();
            
            std::vector<std::string> files = Filesystem::GetDirectoryContent(args.StringValue(0));
            
            for (size_t i = 0; i < files.size(); i++) {
                ret->Set(i, args.NewString(files[i]));
            }
            
            args.SetReturnValue(ret);
        }
        
        void HashFile(const v8::FunctionCallbackInfo<v8::Value>& _args) {
            ScriptingManager::Arguments args(_args);
            
            if (args.AssertCount(1)) return;
            
            if (args.Assert(args[0]->IsString(), "Arg0 is the path to the file")) return;
            
            args.SetReturnValue(args.NewString(Filesystem::GetFileHexDigest(Hash::DigestType::SHA512, args.StringValue(0))));
        }
        
        void InitFS(v8::Handle<v8::ObjectTemplate> fsTable) {
            ScriptingManager::Factory f(v8::Isolate::GetCurrent());
            
            f.FillTemplate(fsTable, {
                {FTT_Static, "readFile", f.NewFunctionTemplate(ReadFile)},
                {FTT_Static, "writeFile", f.NewFunctionTemplate(WriteFile)},
                {FTT_Static, "fileExists", f.NewFunctionTemplate(FileExists)},
                {FTT_Static, "fileSize", f.NewFunctionTemplate(FileSize)},
                {FTT_Static, "mountFile", f.NewFunctionTemplate(MountFile)},
                {FTT_Static, "configDir", f.NewFunctionTemplate(ConfigDir)},
                {FTT_Static, "mkdir", f.NewFunctionTemplate(Mkdir)},
                {FTT_Static, "lsdir", f.NewFunctionTemplate(Lsdir)},
                {FTT_Static, "hashFile", f.NewFunctionTemplate(HashFile)}
            });
            
            fsTable->SetAccessor(f.NewString("hasSetConfigDir"), HasSetConfigDir);
        }
    }
}