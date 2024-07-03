#pragma once

#if defined _WIN32 || defined _WIN64
#define SSTVLIB_IMPORT __declspec(dllimport)
#elif defined __linux__
#define SSTVLIB_IMPORT __attribute__((visibility("default")))
#else
#define SSTVLIB_IMPORT
#endif

extern "C" SSTVLIB_IMPORT int sum(int x, int y);

extern "C" SSTVLIB_IMPORT int image_sound(const char* ipath, const char* opath);