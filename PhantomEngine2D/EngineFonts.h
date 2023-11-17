#pragma once

#include <imgui.h>

#ifndef ENGINE_FONTS_H
#define ENGINE_FONTS_H

enum EngineFonts
{
    LiberationSansBoldFont,
    LiberationSansRegularFont,
    ArialBoldFont,
    ArialRegularFont
};

ImFont* GetEngineFont(EngineFonts font, float size = 16);

#endif
