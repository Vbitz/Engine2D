#pragma once

#include "../RenderDriver.hpp"
#include "../Draw2D.hpp"

namespace Engine {
    namespace Drawables {
        class CubeDrawableTest : public Drawable {
        public:
            CubeDrawableTest(RenderDriver* render) : Drawable(render) { }
            
            void Draw() override;
            
        protected:
            void _init() override;
            
        private:
            Draw2D* _draw;
        };
    }
}