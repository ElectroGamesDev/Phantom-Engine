#pragma once

#include <SDL.h>
#include <SDL_render.h>
#include <filesystem>
#include "EngineFonts.h"
#include "ConsoleLogger.h"
#include <vector>

extern SDL_Window* window;
extern SDL_Renderer* renderer;
extern EngineFonts defaultFont;
extern ImFont* defaultFont28;
extern ImFont* defaultFont34;

namespace PhantomEditor
{
    class EditorWindow
    {
    public:
        void Init(std::filesystem::path path);
        void InitFonts();
        void InitImages();
        void InitSDL();
        void CleanupSDL();
        void ProcessEvents();
        void ProcessGameEvents();
        void Render();
        void ExecuteMonoFunction(std::string className, std::string methodName);
        static bool IsPlaying();
        static SDL_Renderer* GameRenderer();
        static SDL_Renderer* GetRenderer();
        static std::vector<ConsoleLogger::ConsoleLog> consoleLogs;
        static EditorWindow* editorWindow;

    private:
        SDL_Window* window_;
        SDL_Renderer* renderer_;
        EngineFonts defaultFont_;
        ImFont* defaultFont28_;
        ImFont* defaultFont34_;
    };
}