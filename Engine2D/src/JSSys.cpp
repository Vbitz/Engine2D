#include "JSSys.hpp"

namespace Engine {

	namespace JsSys {

		v8::Handle<v8::Value> Println(const v8::Arguments& args) {
			bool first = true;
			for (int i = 0; i < args.Length(); i++) {
				v8::HandleScope handle_scope;
				if (first) {
					first = false;
				} else {
					printf(" ");
				}
				v8::String::Utf8Value str(args[i]);
				const char* cstr = *str;
				printf("%s", cstr);
			}
			printf("\n");
	
			fflush(stdout);
			return v8::Undefined();
		}

	}

}