#include "FramePerfMonitor.hpp"

#include "Platform.hpp"
#include <math.h>

namespace Engine {
	namespace FramePerfMonitor {
        double  _startTime = -1.0,
                _rawFrameTime = -1.0,
                _fpsTimer = 0.0,
                _currentFPS = 0.0;
        
        void BeginFrame() {
            _startTime = Platform::GetTime();
        }
        
		void EndFrame() {
            _rawFrameTime = Platform::GetTime() - _startTime;
            
            float fps = 1000 / (_rawFrameTime * 1000);
            
            _fpsTimer += _rawFrameTime;
            
            if (_fpsTimer > 0.1f) {
                _currentFPS = floor(fps);
                _fpsTimer = 0;
            }
        }

		double GetFrameTime() {
            return _rawFrameTime;
        }
        
        double GetFPS() {
            return _currentFPS;
        }
	}
}