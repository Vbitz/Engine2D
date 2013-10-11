#include <Engine2D.hpp>

#include <someRandomCoolLibary.h>

ENGINE_JS_METHOD(Testing) {
	ENGINE_JS_SCOPE_OPEN;

	ENGINE_CHECK_ARG_COUNT(1);

	ENGINE_CHECK_ARG_INT32(0, "Arg0 is the number to get somethingIfyed");

	ENGINE_JS_SCOPE_CLOSE(v8::Number::New(
		someRandomCoolLibary_exec(ENGINE_GET_ARG_INT32_VALUE(0))
		));
}

ENGINE_EXTENTION_INIT_METHOD(DraftExtention) {
	ENGINE_CREATE_EXTENTION("DraftExtention", "de");
	someRandomCoolLibary_init();

	ENGINE_ADD_METHOD_EX("testing", "Testing");

	ENGINE_REGISTER_EXTENTION;
}

ENGINE_EXTENTION_DESTROY_METHOD() {
	someRandomCoolLibary_destroy();
}