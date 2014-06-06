#include "CubeDrawableTest.hpp"

namespace Engine {
    namespace Drawables {
        void CubeDrawableTest::_init() {
            this->_draw = new Draw2D(this->_render);
        }
        
        void CubeDrawableTest::Draw() {
            this->_render->SetColor(1.0f, 0.45f, 1.0f);
            this->_draw->Rect(100, 100, 200, 200);
        }
    }
}