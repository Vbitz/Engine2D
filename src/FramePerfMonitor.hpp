#pragma once 

namespace Engine {
	namespace FramePerfMonitor {
        void BeginFrame();
		void EndFrame();

		double GetFrameTime();
        double GetFPS();
	}
}