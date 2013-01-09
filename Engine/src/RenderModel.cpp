#include "RenderModel.hpp"

namespace Engine {
	namespace Render {
		RenderModel::RenderModel() : _currentShader(Shader(PreDefinedShader_Basic)) {
		}
	}
}