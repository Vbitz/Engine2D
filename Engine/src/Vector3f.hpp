#pragma once

namespace Engine {
	// it's such a small class there's no need for a .cpp file
	class Vector3f {
		public:
			Vector3f(float x, float y, float z) {
				this->X = x;
				this->Y = y;
				this->Z = z;
			}

			float X, Y, Z;
		private:

	};
}