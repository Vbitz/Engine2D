// A second version of apiConcepts.cpp looking more into how I want the package system to work.

// Module is linked with the main Engine2D executable as a dynamic libary

#include "Engine2D.hpp"

// ScriptMethods are a thin wrapper over "native" scripting like Lua and V8
ScriptMethod(HelloWorldMethod) { // void HelloWorldMethod(Engine::ScriptArgsRef args)
	// Engine String class should implicity cast to the scriptable string object
	args.SetReturnValue(Engine::String::StrF("Hello, %s", args[0])); // Basic PrintF style string formatting
}

// EventMethods are hidden behind a macro to make them more receptive to future changes
EventMethod(ModulePostLoad) { // void ModulePostLoad(Engine::EventArgsRef args)
	// Less of a requirement more of a 
	args.GetPackage()->GetScriptTable()["helloWorld"] = HelloWorldMethod;
}

// The Single PackageInitMethod is called while the package is loading
PackageInitMethod("TestingModule") { // void __INIT_MODULE(Engine::PackageRef pkg)
	// A example of how events could work in the future. For accountings sake evens could be tied to the source they came from, this
	// makes safety though threading simpler
	pkg.GetEvents()["postLoad"]->AddListener(ModulePostLoad, "ModulePostLoad");
}