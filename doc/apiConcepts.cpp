// This is some more far off API concepts, in the future this code could be loaded the same way as JS is loaded right now
// Oh and of course all this is subject to change before being implamted
// The code show's off some texture and Drawing Concepts

#include "Engine2D.hpp" // Just the public header interfaces

#include <stdmath.h>

void GetPiO(Engine::Package::MethodArgsPtr args) {
	// Firstly the MethodArgs object is used to meet feature parity with scripting languages
	// the Ptr is used to ensure memory safety, v8 and other C++ applications do the same thing
	args->Assert(args[0]->IsDouble());
	args->Return(M_PI / args[0]->DoubleValue());
}

// Different types of Renderers
// CustomRenderer: Gives a raw framebuffer and expects (but does not require) the buffer to be flushed afrer drawing
// CustomMeshRenderer: 99% of the time this is what you want, the engine is free to optimse these into the best draw call layout
class TestingRenderer : public Engine::CustomMeshRenderer {
public:
	void Draw(Engine::ModelBufferPtr buff) {
		// Draw2D is now just a class operating on a ModelBuffer, it's the same thing as a DrawingContext in the HTML5 world
		Engine::Draw2D draw(buff);

		// Effects are saved before calling a renderer so this one derives a new one without a projection matrix
		buff->GetEffect()->SetProjectionMatrix(Engine::Matrix::Identify);

		draw.Color = Engine::Color(1.0f, 1.0f, 0.0f);
		// Texture handling is not the problem for client applications, they just refer with a URL.
		draw.Texture = "res://texture/testing2.png";
		// All the primatives come with texcoards preloaded around a bounding box
		draw.Rect(0, 0, 1, 1);
	}
};

// Now would be a good time to talk about how I see low level drawing evolving.
// The dream at this point would be a inteligent caching system with programmer driven hinting to override caching behavior
// This would also allow complex high level model rendering to take place in a userspace job manager
// The drawing is then orginised into a series of commands which are interpreted by ModelBuffer as it goes to build the low level
// VBO, this depends on how the shader is configured of course. This strcture starts to aprouch more a command buffer friendly point of
// view.

DLL_EXPORT void Init(Engine::ApplicationPtr root, Engine::Package& exports) {
	// exports is used often here to assoiate a resource to a Package so if a Package is unloaded the garbage collector can free the
	// asset
	exports->CacheAsset("res://texture/testing2.png"); // cache some assets

	Engine::EntityTemplatePtr test = root->CreateEntityTemplate(); // Remember this takes place in a dynamic link libary so get
																	// the server to give us memory

	test.SetRenderer(new Engine2D::Renderer<TestingRenderer>()); // Engine::CustomMeshRenderer is not a Engine::Renderer, it's just
																// a high level template. This makes a factory easier to implament

	exports->RegisterEntity(test); // test is just a pointer to a EntityTemplate which is overlayed over a Engine::Entity

	exports->RegisterMethod("GetPiO", GetPiO); // This is exactly the same as the API I use right now besides a new method
												// signature
}

REGISTER_PACKAGE("TestingPackage", "Vbitz", Init, NULL); // Could also be specifyed in a .json file for cross platform like
/*
{
	"name": "TestingPackage",
	"author": "Vbitz",
	"version": "0.0.1",
	"entrypoints": {
		"startup": "Init"
	},
	"binarys": {
		"win32": "Testing_x86.dll",
		"win64": "Testing_x64.dll",
		"darwin": "Testing.dylib",
		"linux32": "Testing_x86.so",
		"linux64": "Testing_x64.so"
	}
}
 */

// Quite a bit of this seems futher off down the timeline but it gives a nice API target to focus at.