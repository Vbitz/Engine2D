#pragma once

#include "Logger.hpp"

#include "Draw2D.hpp"

namespace Engine {
    namespace LogGraphEvents {
        class TestingEvent : public Logger::LogGraphEvent {
        public:
            int Run(int x, int y) override;
        };
    }
}