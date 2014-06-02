// Still just looking into possible choices for syntax but so far I like this one a lot. It's a bit more explicit then other solutions and
// offers more control then macros.

#include "Events.hpp"
#include "ScriptingManager.hpp"

namespace Testing {
	class TestingEventStuff {
	private:
		static Engine::Event testingEvent_method("testingEvent", "testingEvent_method", [] (Engine::ArgumentsRef args) {
			Engine::Logger::LogText("Testing", Engine::Logger::LogLevel_Log, "%s", args["Hello"]);
		});

		static Engine::ScriptMethod testingScript_method([] (Engine::ArgumentsRef args) {
			args.Assert(args[1]->IsString());
			testingEvent_method({
				{"Hello", "World"}
			});
		});
	};
}