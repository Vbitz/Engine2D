// Still just looking into possible choices for syntax but so far I like this one a lot. It's a bit more explicit then other solutions and
// offers more control then macros.

#include "Events.hpp"
#include "ScriptingManager.hpp"

namespace Testing {
	// It's not a requirement that the items exist inside a class but it makes origination and compartmentalization easier
	class TestingEventStuff {
	private:
		// The event is added to the events system as soon as the constructor is called
		static Engine::Event testingEvent_method("testingEvent", "testingEvent_method", [] (Engine::ArgumentsRef args) {
			Engine::Logger::Log("Testing", "%s", args["Hello"]); // A simplified syntax combined with printf style formatting
		});

		// The script method is not automatically added rather accepted as a script value which translates into a function using a
		// thin wrapper
		static Engine::ScriptMethod testingScript_method([] (Engine::ArgumentsRef args) {
			args.Assert(args[1]->IsString());
			// Event methods can be directly called as if they were methods thanks to the class identifying as a functor.
			testingEvent_method({
				{"Hello", "World"}
			});
		});
	};
}