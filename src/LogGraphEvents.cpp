#include "LogGraphEvents.hpp"

namespace Engine {
    namespace LogGraphEvents {
        int TestingEvent::Run(int x, int y) {
            Draw2D::SetColor("white");
            Draw2D::Rect(x + 10, y - 30, 20, 20);
            return 40;
        }
    }
}