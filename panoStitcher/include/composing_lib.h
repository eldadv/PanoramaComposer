#pragma once
#ifndef COMPOSINGLIB
#define COMPOSINGLIB

#ifdef PANOCOMPOSER_EXPORTS
#define PANOCOMPOSER_EXP __declspec(dllexport)
#else
#define PANOCOMPOSER_EXP __declspec(dllimport)
#endif

// From TARGETVER.h
#include <SDKDDKVer.h>
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
// end from target ver and stdafx

enum {UINT8_GRAY = 1, UINT8_RGB, UINT8_BGR, UINT16_GRAY, UINT16_RGB, UINT16_BGR, FLOAT_GRAY, FLOAT_RGB, FLOAT_BGR};
enum { SUCCESS = 0, NOT_ENOUGH_IMAGES, };

PANOCOMPOSER_EXP int ComposePanorama(float* Rmatrices, float* Kmatrices, void* inputImages, int numOfImages, int byteType, int imageHeight, int imageWidth, int numChannels);

#endif // !COMPSINGLIB

