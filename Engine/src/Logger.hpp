#pragma once

#include <iostream>

namespace Engine {
	namespace Logger {
		void WriteError(std::string err);
		void WriteVerbose(std::string text);
		void WriteWarning(std::string warn);
	}
}