#include "Logger.hpp"

namespace Engine {
	namespace Logger {
		void WriteError(std::string err) {
			std::cout << "Error: " << err << std::endl;
		}

		void WriteVerbose(std::string data) {
			std::cout << "Verbose: " << data << std::endl;
		}

		void WriteWarning(std::string warn) {
			std::cout << "Warning: " << warn << std::endl;
		}
	}
}