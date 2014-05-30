// Module is linked with the main Engine2D executable as a dynamic libary

#include "Engine2D.hpp"

ScriptMethod(HelloWorldMethod) {
	args.SetReturnValue(Engine::String::StrF("Hello, %s", args[0]));
}

EventMethod(ModulePostLoad) {
	args.GetPackage()->GetScriptTable()["helloWorld"] = HelloWorldMethod;
}

ModuleInitMethod("TestingModule") {
	pkg.GetEvents()["postLoad"]->AddListener(ModulePostLoad, "ModulePostLoad");
}