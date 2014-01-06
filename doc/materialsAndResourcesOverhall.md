# Materials and Resources Overhaul

## Current State
Right now (06/01/2014) the engine has a annoying issue with the design of the "materials" system and resource system. The materials system is a thin abstraction over the top of normal OpenGL 3.x tutorial code. This code has worked so far but as I've started working almost exclusively in Modern OpenGL I've quickly run into painful issues with the overall resource architecture in the engine. The main issue right now is the code handles changes in context state very poorly, for example right now the engine uses native fullscreen changes (Creating a fullscreen window over a borderless window), though I won't be using that approach in the future it's a easy to point at issue with my current system.

Right now resources in Engine2D work like so, say I want to load a font…

- Application::LoadFont Calls ResourceManager::Load(filename)
- ResourceManager allocates a new FileSource which uses lazy loading to load the file data which is represented as a byte array, see the first issue.
- Application::LoadFont then inserts a new FontResource into a internal structure, this is completely the wrong approach to take.
- The new FontResource is associated with the underlying Source which is stored by ResourceManager

This concludes the font loading stage, basically just setting up data structures which manage font loading though GLFT_Font. This approach is flawed in anything but trivial use cases because it fragments loading into 3 different stages.

- Preload
- Resource Initialisation
- Resource Loading

Each of these stages takes place during different parts of the program. Preload is a free operation basically, this takes place after the graphics context is ready but it's only readying the basic structure for later stages, I could be loading the File at that point to reduce disk IO while the game is rendering frames. The next stage creates a FontResource to hold the set of GLFT_Font objects that do the real loading. Only once a font is required does the font render to a texture take place also if this is the first time it's being used the Filesystem is invoked to load the file. This process as a whole is single threaded and goes in country to modern application development concepts.

That's the underlying ResourceManager but how does rendering work?

All the GL3 rendering code is in 2 classes right now GL3Buffer and Shader, both are tied very closely to each other, GL3Buffer calls Shader to manage the real drawing the goes on. Neither of these classes makes any calls to ResourceManager, not that that would fix anything. Right now the Shader code is almost a year old and lacks design. It lacks a effective and well written method for restoring shader state on losing the graphics context, right now it just spams GL Errors until the engine crashes due to the state losing sync.

OK now how do I fix this? I feel that developing 2 approaches will stop me going though this same routine in the future when it's harder to fix.

## New Design 1
Design 1 is based off how I intended to improve the old system. Rather then approaching resources like a object orientated problem I use URL's to load the resources. 

## New Design 2