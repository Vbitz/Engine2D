// Over my time developing Engine2D the core architecture has changed very little since the start of the project, over 2 years ago when
// I started the project in the current form it still had the same basic pipeline for getting from script to rendering.
// Although I like the old structure I think it's about time to rethink it and write one that's more effective.

// Core design goals
// - Fast: 		Speed is very important in Engine2D so the new system should be open to common industry level optimizations like batching and 
//				tessellation.
// - Concise:	Other languages and engines leave a lot of busy work between the programmer and graphics on the screen, rather then that
//				I'd like to encourage writing very little code to get the job done
// - Expandable:The Engine should be modular enough that programming with it is not constrained to the core feature set.

#include <Engine2D.hpp>
#include <Engine2D/Renderer.hpp>
#include <Engine2D/Scripting.hpp>
#include <Engine2D/ModelComponent.hpp>
#include <Engine2D/Font.hpp>
#include <Engine2D/Draw3D.hpp>

class TestingDrawable : public Engine::LevelEntity, public Engine::Scriptable {
private:
	std::string _text = "";

	Engine::ModelComponent _model;	// The real resources used like the VertexBuffer and such are reference counted and freed when
									// no longer needed
	Engine::Font _font;

	const int MAX_LENGTH = 16;

public:
	TestingDrawable() : Engine::LevelEntity("TestingDrawable"), Engine::Scriptable("TestingDrawable") {
		this->_model = this->AddComponent<Engine::ModelComponent>(); // Attaches the component to the drawable for tracking purposes
																	// and makes sure state is saved based on context
		this->_font = this->AddResource<Engine::Font>(Engine::Font::Builtin_SansSerif);
																	// Resources are specified by URL's but Builtin_SansSerif
																	// shortcuts the normal creation and just binds the font as a
																	// reference

		this->AddValue("text", _text).SetNoScript();				// Sets the value to save/load
	}

	// These functions are scripting language agnostic but are considered SlowCalls by the engine since they involve a lockup rather
	// then a inline method call. Functions compiled into the ScriptEngine use macros and typedefs to hide implementation details
	static void Test(const Engine::ScriptCallbackInfo &args) {
		std::string text;
		// Like the Gen3 Assert model on steroids, a series of classes deriving from CallState that return boolean depending
		// on the state of the value 
		if (args.AssertCallState({
			Engine::CallState::ArgValue<std::string>("text", text)
		})) return;

		if (text.length() > MAX_LENGTH) {
			args.ThrowArgError("[text] exceeds max length: %d", MAX_LENGTH);
		} else {
			this->_text = text;
			// All drawables use a dirty/clean system to define rendering, mitigations are done using shader uniforms
			this->SetDirty();
		}
	}

	void OnScriptInit() override {
		this->BindConstructor(); // Binds a default constructor that optionally takes a JSON value to load
		this->BindMethod("test", Test);
	}

	void OnDraw(const Engine::BatchedBuffer &buff) {
		Engine::Draw3D draw(buff);
		draw.Draw(this->_model);
		draw.PrintOverlay(this->_model.GetAttachment("_text", glm::vec2(0, 0)), this->_font, this->_text);
	}
};

void OnCreateDrawableCallback(const Engine::EventCallbackInfo &args) {
	// Rather then the current system this one is not straight Json::Value's being passed to each listener,
	// over time I've figured out that doesn't scale at all, userPointers are a nice first step but what's really needed
	// is smart read-only structures free to store whatever information they need.
	args["drawable"].As<Engine::Drawable>()->SetValue("testing", true);
}

void Addon_Start(Engine::AddonInterfacePtr addon) {
	// Require modules to start with, the require is synchronous although some aspects are lazy loaded on demand 
	Engine::RendererPtr render = addon->Require<Engine::Renderer>(Engine::CoreAddons::Renderer);
	Engine::ScriptEnginePtr script = addon->Require<Engine::ScriptEngine>(Engine::CoreAddons::Scripting);

	addon->BindScriptRoot(script, addon->GetScriptName()); // Creates a table in the script require registry to match these bindings
	addon->RegisterLevelEntity(render, TestingDrawable); // Takes a Engine::LevelEntity
	addon->RegisterScriptBinding(script, TestingDrawable); // Takes a Engine::Scriptable

	addon->BindEvent("onCreateDrawable", OnCreateDrawableCallback);
}

ENGINE_ADDON("Testing Addon", "Vbitz", Addon_Start, NULL, Engine::LoadOrder::Loaded);