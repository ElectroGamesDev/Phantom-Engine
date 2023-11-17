#pragma once
#ifndef SPRITE_EDITOR_WINDOW_H
#define SPRITE_EDITOR_WINDOW_H

#include <SDL.h>
#include <filesystem>

namespace PhantomEditor
{
    class SpriteEditorWindow
    {
    public:
        void SpriteEditorSetup(SDL_Window* win);
        void DrawSpriteEditor();
        void DrawSpriteEditorCreation();
        void SetPath(std::filesystem::path path);
        bool spriteCreatorOpen;
    private:
        bool spriteEditorOpen;
    };
}

#endif // SPRITE_EDITOR_WINDOW_H
