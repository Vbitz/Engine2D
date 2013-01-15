#pragma once

#include "common.hpp"

#include "Filesystem.hpp"

#include "JSSys.hpp"
#include "JSDraw.hpp"

namespace Engine {

	GLFT_Font* 					getFont();

	int 						getScreenWidth();
	int 						getScreenHeight();

	void 						addTexture(int tex);

	void 						setDrawFunction(v8::Persistent<v8::Function> func);

}