#include <SDL.h>
#include <SDL_render.h>
#include <SDL_image.h>
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer.h"
#include "SpriteEditorWindow.h"
#include "EditorWindow.h"
#include <filesystem>

namespace PhantomEditor
{
    bool isFirstFrameCreator = true;
    bool isFirstFrameEditor = true;
    bool spriteCreatorOpen = true;
    bool spriteEditorOpen = true;
    std::filesystem::path _path;

    void SpriteEditorWindow::SetPath(std::filesystem::path path)
    {
        _path = path;
    }

    void SpriteEditorWindow::SpriteEditorSetup(SDL_Window* win)
    {
        window = win;
    }

    void SpriteEditorWindow::DrawSpriteEditor()
    {
        if (window == nullptr || !spriteEditorOpen) return;
        {
            isFirstFrameEditor = false;
            int width, height;
            SDL_GetWindowSize(window, &width, &height);
            ImGui::SetNextWindowSize(ImVec2(900, 800));
            ImGui::SetNextWindowPos(ImVec2((width - 900) / 2.0f, (height - 800) / 2.0f));
        }

        ImGui::Begin("Sprite Editor", &spriteEditorOpen);

        // Load a sprite
        static ImVec2 sprite_size;
        static ImTextureID sprite_texture;

        // Display the sprite
        if (sprite_texture)
        {
            ImGui::Image(sprite_texture, sprite_size);
        }

        // Add image editing functionality
        if (sprite_texture)
        {
        }
        ImGui::End();
    }

    void SpriteEditorWindow::DrawSpriteEditorCreation()
    {
        if (window == nullptr || !spriteCreatorOpen) return;
        if (isFirstFrameCreator)
        {
            isFirstFrameCreator = false;
            int width, height;
            SDL_GetWindowSize(window, &width, &height);
            ImGui::SetNextWindowSize(ImVec2(220, 130));
            ImGui::SetNextWindowPos(ImVec2((width - 220) / 2.0f, (height - 130) / 2.0f));
        }
        ImGui::Begin("Sprite Creator", &spriteCreatorOpen, ImGuiWindowFlags_NoResize);

        // Name input
        ImGui::Text("Name");
        ImGui::SameLine();
        static char spriteName[128] = "";
        ImGui::InputTextWithHint("##Name", "Sprite Name...", spriteName, IM_ARRAYSIZE(spriteName), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll);

        // Size inputs
        ImGui::Text("X   ");
        ImGui::SameLine();
        static char sizeX[128];
        static char sizeY[128];
        ImGui::InputTextWithHint("##SizeX", "X in pixels", sizeX, 8);
        ImGui::Text("Y   ");
        ImGui::SameLine();
        ImGui::InputTextWithHint("##SizeY", "Y in pixels", sizeY, 8);

        ImGui::Text("    ");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(150);
        if (ImGui::Button("Create Sprite", ImVec2(140, 25)))
        {
            std::string name = spriteName;
            name.erase(std::remove(name.begin(), name.end(), ' '), name.end()); // Remove spaces
            name.erase(std::remove_if(name.begin(), name.end(), [](char c) { // Remove special characters
                return !std::isalnum(c);
                }), name.end());
            if (name == "") return;

            SDL_Surface* surface = SDL_CreateRGBSurface(0, 100, 100, 32, 0, 0, 0, 0);
            SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 255, 255, 255));
            //SDL_SavePNG(surface, _path.generic_string() + "/image.png");

            spriteCreatorOpen = false;
            spriteEditorOpen = true;
        }

        ImGui::End();
    }

}