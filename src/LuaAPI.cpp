#include "Util.hpp"

#include <Math.hpp>

#ifdef WIN32
#define LUA_API __declspec(dllexport)
#else
#define LUA_API
#endif

extern "C" {
	LUA_API float LUA_API_ridgedNoise(float x, float y, int octaves, float frequency, float persistence) {
        return Util::ridgedNoise(vec2(x, y), octaves, frequency, persistence);
    }
}
