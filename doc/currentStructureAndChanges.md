Engine::Main
==========================
Now
------------
* Script Loading
* Script Running
* sys.drawFunc(f) and sys.keyboardFunc(f)
* OpenGL Specfic Windowing
* Module Loading
* Entry Point
* Update Loop
* Script Update Detection
* Keycode to string
* sys.setWindowInitParams(o)
* LoadBasicConfigs()

After Update
-------------
* Entry Point
* Update Loop
* Windowing moved to new class Engine::Platform::Window
* Scripting moved to new class Engine::Scripting::V8ScriptManager
* Script Update Detection moved to Engine::ResourceManager::ScriptResource
* LoadBasicConfigs() moved to struct in new header config.hpp
* Module Loading moved to new class Engine::ModuleLoader
* sys.drawFunc and sys.keyboardFunc moved to sys.on("draw", f) and sys.on("keyboard", f)
* Keycode to string moved to Engine::Platform::Input

Engine::Database
==========================
Now
----------
* SQLite Abstraction

Engine::Config
==========================
Now
----------
* Config Management

After Update
-----------
* Config Management
* LoadBasicConfigs(config_item[])
