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
Design 1 is based off how I intended to improve the old system. Rather then approaching resources like an object orientated problem I would use URL's and a stateless solution to load the resources. The code could look like the following sample rather then the current method.

```
	Draw2D::SetShader("res://shader/basic.json", true);
	Draw2D::BindShader();
	Draw2D::SetTexture("res://image/testing.png", true); // Throw an error if we load the image at this point
	Draw2D::Rectangle(10, 10, 60, 60);
```

This approach has it's upsides and downsides. It's really easy to use but it lacks once you start doing more complex shader operations, for example how do you set a uniform in a shader. You could use a stateless approach like...

```
	Draw2D::SetShaderUniform("test", 1.0f);
```

This approach could very well work and ties more directly to how I design API's in Engine2D right now. As such this approach will be the easiest to implement.

## New Design 2
One of the biggest issues with how I've designed rendering in the Engine is I try to offload as much state as I can to the graphics driver, in the long term this is a mistake. If I lose the graphics context I need to ask the scripter to restore the state of the context. What if I take an approach more in sync with how the industry approaches the same issue rather then trying to reinvent the wheel (more). The industry makes a important distinction between Textures and Materials. Materials set the logic of how to render polygons, they setup everything from what Texture to use to how to use shader uniforms. They're nearly ubiquitous in the 3D Graphics industry. So how would they look in Engine2D?

```
	Material* mat = new Material(); // Materials are cheap, they just store state rather then low level objects.
	mat->SetShader("res://shader/basic.json", true);
	mat->BindUniform("test", Material::UniformType_Float);
	mat["color"] = Color(0x101010);
	mat["texture"] = Texture("res://image/testing.png", true);
	mat["test"] = 1.0f;
	Draw2D::SetMaterial(mat);
	Draw2D::Rectangle(10, 10, 60, 60);
```

This design is more complex to program from a basic front but it makes up for it in being able to restore state. One advantage of this design is it get's serialised easily, for example the above can be replicated like so

```
	{
		"shader": {
			"templatePath": "res://shader/basic.json",
			"uniforms": [
				{"name": "test", "type": "float"}			]
		},
		"parameters": {
			{"name": "color", "value": "color:#101010"},
			{"name": "texture", "value": "texture:res://image/testing.png"},
			{"name": "test", "value": 1.0}		}	}
```

What does this design mean for the Resource/Source system though? Well The concept of Sources was developed thinking that sources would be used most of the time in a one to many relationship but in practice I've found this is just not true. Resources are each specialised and though inheriting from the same base should exist to fill interfaces based on the type of Resource so A basic resource handling image loading would implement `IImageResource` which exposes a raw byte array of pixels for the image ready to be uploaded to the graphics card or a compressed DXT image ready to be uploaded slightly differently.

## Conclusion
Both Designs have their own advantages, Design 1 is easier to program and fits in well with the current API but lacks the same level of power and finesse of Design 2. Design 2 is harder to program but makes up for it with less bugs down the road, it's also easy to load from JSON. If I were to look at the Engine as a basic 2d graphics api I would choose Design 1. As I intend to move the engine towards being a modern 3D engine in the future Design 2 is the best bet for doing that without additional hassle of going though this again.