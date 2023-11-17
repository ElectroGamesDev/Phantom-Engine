#pragma once
#ifndef BEHAVIOUR_NODE_EDITOR_WINDOW_H
#define BEHAVIOUR_NODE_EDITOR_WINDOW_H

#include <SDL.h>
#include <filesystem>

namespace PhantomEditor
{
    class BehaviourNodeEditor
    {
    public:
        void BehaviourNodeEditorSetup(SDL_Window* win);
        void DrawBehaviourNodeEditor();
        void SetPath(std::filesystem::path path);
        bool behaviourNodeEditorOpen;
    };
}

#endif // BEHAVIOUR_NODE_EDITOR_WINDOW_H
