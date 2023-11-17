#pragma once

#include <SDL.h>
#include <SDL_render.h>
#include "EngineFonts.h"

extern SDL_Window* window;
extern SDL_Renderer* renderer;
extern bool mainWindowOpen;
extern char projectName[256];
extern bool creatingProject;
extern EngineFonts defaultFont;
extern ImFont* defaultFont28;
extern ImFont* defaultFont34;

void InitFonts();
void InitSDL();
void CleanupSDL();
void ProcessEvents();
void Render();
