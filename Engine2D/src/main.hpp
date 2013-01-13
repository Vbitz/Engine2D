#ifndef __MAINH
#define __MAINH

#include "common.hpp"

GLFT_Font* getFont();

int getScreenWidth();
int getScreenHeight();

void addTexture(int tex);

void setDrawFunction(v8::Persistent<v8::Function> func);

#endif