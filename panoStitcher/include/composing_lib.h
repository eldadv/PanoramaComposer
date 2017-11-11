#pragma once
#ifndef COMPOSINGLIB
#define COMPOSINGLIB

#ifdef PANOCOMPOSER_EXPORTS
#define PANOCOMPOSER_EXP __declspec(dllexport)
#else
#define PANOCOMPOSER_EXP __declspec(dllimport)
#endif



enum {UINT8_GRAY = 1, UINT8_RGB, UINT8_BGR, UINT16_GRAY, UINT16_RGB, UINT16_BGR, FLOAT_GRAY, FLOAT_RGB, FLOAT_BGR};
enum { SUCCESS = 0, NOT_ENOUGH_IMAGES, };

#endif // !COMPSINGLIB

