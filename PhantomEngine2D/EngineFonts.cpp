#include "EngineFonts.h"

ImFont* GetEngineFont(EngineFonts font, float size)
{
    ImGuiIO& io = ImGui::GetIO();
    switch (font)
    {
    case LiberationSansBoldFont:
        return io.Fonts->AddFontFromFileTTF("Fonts/LiberationSans-Bold.ttf", size);
        break;
    case LiberationSansRegularFont:
        return io.Fonts->AddFontFromFileTTF("Fonts/LiberationSans-Regular.ttf", size);
        break;
    default:
        return io.Fonts->Fonts[0];
    }
}