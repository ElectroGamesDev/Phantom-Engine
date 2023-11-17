#pragma once
#ifndef EVENT_SHEET_EDITOR_WINDOW_H
#define EVENT_SHEET_EDITOR_WINDOW_H

#include <SDL.h>
#include <filesystem>
#include <json.hpp>
#include <iostream>
#include <fstream>
#include <string>

using json = nlohmann::json;

namespace PhantomEditor
{
    class EventSheetEditor
    {
    public:
        void EventSheetEditorSetup(SDL_Window* win);
        void DrawEventSheetEditor();
        void DrawEventSheetCreator();
        void OpenEventSheet(std::filesystem::path path);
        void SetPath(std::filesystem::path path);
        void DrawEventSheetEventList();
        bool eventSheetEditorOpen;
        bool eventSheetCreatorOpen;
        void SaveEventSheet();
        void LoadEventSheet();

    private:
        static inline bool isFirstFrameEditor;
        static inline std::filesystem::path _path;
        static inline std::filesystem::path _newPath;
        static inline json _data;
        static inline ImFont* eventListEventTitleFont;
    };
}

#endif // EVENT_SHEET_EDITOR_WINDOW_H
