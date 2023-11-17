#include <SDL.h>
#include <SDL_render.h>
#include <SDL_image.h>
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer.h"
#include "EventSheetEditor.h"
#include "../EditorWindow.h"
#include <filesystem>
#include "../EngineFonts.h"
#include "../ConsoleLogger.h"

namespace PhantomEditor
{
    bool eventSheetEditorOpen = true;
    bool eventSheetCreatorOpen = true;
    int selectedScriptingOption = 0;
    bool changeSheet = false;
    bool eventSheetEventsWindow = false;
    bool eventSheetActionsWindow = false;
    std::string selectedEventGroup = "";
    ImFont* eventListEventTitleFont;

    void EventSheetEditor::SetPath(std::filesystem::path path)
    {
        _path = path;
    }

    void EventSheetEditor::EventSheetEditorSetup(SDL_Window* win)
    {
        window = win;
        eventListEventTitleFont = GetEngineFont(LiberationSansBoldFont, 34);
    }

    void EventSheetEditor::OpenEventSheet(std::filesystem::path path)
    {
        changeSheet = true;
        if (path.generic_string() != _path.generic_string())
        {
            _newPath = path;
        }
        if (!eventSheetEditorOpen) eventSheetEditorOpen = true;
    }

    void EventSheetEditor::SaveEventSheet()
    {
        std::ofstream file(_path);
        file << _data.dump(4);
        file.close();
    }

    void EventSheetEditor::LoadEventSheet()
    {
        std::ifstream file(_path);
        _data = json::parse(file);
        file.close();
    }

    void EventSheetEditor::DrawEventSheetEditor()
    {
        if (window == nullptr || !eventSheetEditorOpen) return;
        {
            isFirstFrameEditor = false;
            int width, height;
            SDL_GetWindowSize(window, &width, &height);
            ImGui::SetNextWindowSize(ImVec2(900, 800));
            ImGui::SetNextWindowPos(ImVec2((width - 900) / 2.0f, (height - 800) / 2.0f));
        }

        if (changeSheet)
        {
            // TODO: Check if saved and stuff and pop up asking if they want to save, unsave and go to the over behaviour, or stay. "Save and switch, "Unsave and switch", "Stay" or "Cancel"
            SaveEventSheet();
            changeSheet = false;
            _path = _newPath;
            LoadEventSheet();
        }

        ImGui::Begin("Event Sheet Editor", &eventSheetEditorOpen, ImGuiWindowFlags_NoSavedSettings);

        //const int numRows = 10;
        //const int numCols = 10;
        //const ImVec2 cellSize = ImVec2(60, 30);

        //ImGui::Columns(numCols, nullptr, false);
        //for (int i = 0; i < numRows * numCols; i++)
        //{
        //    ImGui::BeginGroup();

        //    // Draw the cell background
        //    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
        //    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
        //    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.4f, 0.4f, 0.4f, 1.0f));
        //    ImGui::Button(("##button" + std::to_string(i)).c_str(), cellSize);
        //    ImGui::PopStyleColor(3);

        //    // Draw the cell content
        //    ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x+10,ImGui::GetCursorPos().y+5));
        //    ImGui::Text("Event %d", i + 1);

        //    ImGui::EndGroup();
        //    ImGui::NextColumn();
        //}
        //ImGui::Columns(1);

        // Calculate the position and size of the left and right regions
        const float splitterSize = 4.0f;
        const ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();
        const ImVec2 leftRegionSize(contentRegionAvailable.x * 0.5f - splitterSize * 0.5f, contentRegionAvailable.y);
        const ImVec2 rightRegionSize(contentRegionAvailable.x * 0.5f - splitterSize * 0.5f, contentRegionAvailable.y);

        // Draw the left region
        ImGui::BeginChild("Left Region", leftRegionSize, true, ImGuiWindowFlags_NoScrollbar);
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0.2f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0.4f));

        // Events
        float nextY = 0;
        if (_data.find("EventGroups") != _data.end()) {
            auto& eventGroupsArray = _data["EventGroups"];
            //std::string jsonStr = _data.dump();
            //ConsoleLogger::InfoLog(jsonStr);
            for (auto it = _data["EventGroups"].begin(); it != _data["EventGroups"].end(); ++it)
            {
                json& eventGroup = it.value();
                ImGui::SetNextWindowPos(ImVec2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y + nextY));
                ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.2f, 0.2f, 0.2f, 0.5f));
                ImGui::BeginChild(("EventChild##EventGroup" + it.key()).c_str(), ImVec2(leftRegionSize.x, (20 * (eventGroup.size()+1))), true, ImGuiWindowFlags_NoScrollbar);

                int nextEventY = 5;
                for (auto event = eventGroup.begin(); event != eventGroup.end(); ++event)
                {
                    if (event.value().find("Settings") != event.value().end())
                    {
                        std::string fullEventName = event.key();
                        //for (auto ev = event.value().begin(); ev != event.value().end(); ++ev) {
                        //    fullEventName = ev.key();
                        //}
                        std::string eventName = fullEventName.substr(0, fullEventName.find("_"));
                        ImGui::SetCursorPos(ImVec2(-7, nextEventY));
                        std::string buttonText;
                        if (event.value()["Settings"].find("Key") != event.value()["Settings"].end())
                            buttonText = eventName + std::string(" - ") + std::string(event.value()["Settings"]["Key"]) + std::string(" Key");
                        else buttonText = eventName;
                        if (event.value()["Settings"].is_null() || event.value()["Settings"]["Key"].is_null()) ImGui::Text((eventName).c_str());
                        else ImGui::Button(buttonText.c_str(), ImVec2(round(buttonText.length() * 8.33), 17));
                        nextEventY += 25;
                    }
                }

                ImGui::SetCursorPos(ImVec2(1.5f, nextEventY));
                if (ImGui::Button(("Add Event##EventGroupAddButton" + it.key()).c_str(), ImVec2(75, 17)))
                {
                    eventSheetEventsWindow = true;
                    selectedEventGroup = it.key();
                }
                ImGui::EndChild();
                ImGui::PopStyleColor();
                nextY += (23 * (eventGroup.size() + 1));
            }
        }
        // End of Events
        
        // Add Event Group
        ImGui::SameLine();
        ImGui::SetCursorPos(ImVec2(1.5f, nextY));
        if (ImGui::Button("Add Event##AddEventGroup", ImVec2(75, 17)))
        {
            if (_data.find("EventGroups") == _data.end())
            {
                int eventIndex = _data["EventGroups"].size() - 1;
                _data["EventGroups"].push_back({});
                _data["EventGroups"] = { { std::to_string(eventIndex), {} } };
                _data["EventGroups"][std::to_string(eventIndex)]["Actions"] = {};
            }
            else
            {
                int eventIndex = _data["EventGroups"].size();
                _data["EventGroups"][std::to_string(eventIndex)] = {};
                _data["EventGroups"][std::to_string(eventIndex)]["Actions"] = {};
            }

            //_data["ActionGroups"].push_back({});
            SaveEventSheet();
            DrawEventSheetEditor();
        }
        ImGui::PopStyleColor(3);
        ImGui::EndChild();
        // End of Add Event Group

        // Draw the vertical splitter
        ImGui::SameLine();
        //ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
        //ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.6f, 0.6f, 0.6f, 1.0f));
        //ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.7f, 0.7f, 0.7f, 1.0f));
        //ImGui::Button("Button##Unknown", ImVec2(splitterSize, contentRegionAvailable.y));
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        drawList->AddRectFilled(ImVec2(0,0), ImVec2(splitterSize, contentRegionAvailable.y), ImGui::GetColorU32(ImVec4(0.7f, 0.7f, 0.7f, 1.0f)));
        //ImGui::PopStyleColor(3);

        // Draw the right region
        ImGui::SameLine();
        ImGui::BeginChild("Right Region", rightRegionSize, true);
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0.2f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0.4f));
        //ImGui::Button("Add Action", ImVec2(100, 17));
        // Events
        nextY = 0;
        for (auto event = _data["EventGroups"].begin(); event != _data["EventGroups"].end(); ++event)
        {
            float nextActionY = 5;
            if (event.value().find("Actions") != event.value().end()) {
                int groupHeight = 0;
                if (event.value().size() > event.value()["Actions"].size()+1) groupHeight = 20 * (event.value().size() + 1);
                else groupHeight = 20 * (event.value()["Actions"].size() + 2);

                ImGui::SetNextWindowPos(ImVec2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y + nextY));
                ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.2f, 0.2f, 0.2f, 0.5f));
                ImGui::BeginChild(("ActionChild##ActionGroup" + event.key()).c_str(), ImVec2(leftRegionSize.x, groupHeight), true, ImGuiWindowFlags_NoScrollbar);

                for (auto action = event.value()["Actions"].begin(); action != event.value()["Actions"].end(); ++action)
                {
                    ImGui::SetCursorPos(ImVec2(1.5f, nextActionY));
                    if (ImGui::Button((action.key() + "##ActionGroupAddButton").c_str(), ImVec2(75, 17)))
                    {
                        selectedEventGroup = action.key();
                        eventSheetEventsWindow = true;
                    }

                    nextActionY += 25;
                }

                ImGui::SetCursorPos(ImVec2(1.5f, nextActionY));
                if (ImGui::Button(("Add Action##AddActionButton"), ImVec2(75, 17)))
                {
                    //selectedEventGroup = action.key();
                    eventSheetEventsWindow = true;
                }

                ImGui::EndChild();
                ImGui::PopStyleColor();
            }
            nextY += (23 * (event.value().size() + 1));
            // End of Events
        }
        // End of Events
        ImGui::PopStyleColor(3);
        ImGui::EndChild();

        ImGui::End();
    }

    void EventSheetEditor::DrawEventSheetCreator()
    {
        if (window == nullptr || !eventSheetCreatorOpen) return;
        int width, height;
        SDL_GetWindowSize(window, &width, &height);
        ImGui::SetNextWindowSize(ImVec2(180, 200));
        ImGui::SetNextWindowPos(ImVec2((width - 180) / 2.0f, (height - 200) / 2.0f));
        ImGui::Begin("Event List", &eventSheetCreatorOpen, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);

        ImGui::Text("     Behaviour Name");
        static char behaviourName[128] = "";
        ImGui::SetNextItemWidth(163);
        ImGui::InputTextWithHint("##Name", "Behaviour Name...", behaviourName, IM_ARRAYSIZE(behaviourName), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll);

        ImGui::NewLine();

        ImGui::Text("    Scripting Method");
        if (ImGui::RadioButton("Nodes##nodes_option", selectedScriptingOption == 1)) { selectedScriptingOption = 1; }
        if (ImGui::RadioButton("Event Sheet##event_sheet_option", selectedScriptingOption == 2)) { selectedScriptingOption = 2; }

        ImGui::NewLine();

        if (ImGui::Button("Create Behaviour##create_button", ImVec2(163, 20)))
        {
            if (selectedScriptingOption == 0)
            {
                ImGui::End();
                return;
            }
            std::string tempName = behaviourName;
            tempName.erase(std::remove(tempName.begin(), tempName.end(), ' '), tempName.end()); // Remove spaces
            tempName.erase(std::remove_if(tempName.begin(), tempName.end(), [](char c) { // Remove special characters
                return !std::isalnum(c);
                }), tempName.end());
            if (tempName == "") return;

            std::string name = behaviourName;
            if (selectedScriptingOption == 1)
            {
                ImGui::End();
                return;
            }
            else if (selectedScriptingOption == 2)
            {
                nlohmann::json emptyFile = {};
                std::ofstream file(_path / (name + ".behaviour"));
                file << emptyFile;
                file.close();
            }
            eventSheetCreatorOpen = false;
            eventSheetEditorOpen = true;
            SetPath(_path / (name + ".behaviour"));
            _data = { {"EventGroups"} }; // set this to New Data
        }

        ImGui::End();
    }

    enum EventTypes
    {
        Null,
        Any_Key_Pressed,
        Any_Key_Released,
        Key_Pressed,
        Key_Released,
        Game_Start,
        Game_Update
    };

    struct Event {
        bool requiresSettings = true;
        bool inputType = false;
        EventTypes eventType = Null;
        bool canBeApplied = false;
        std::string eventName = "Unknown";
    };

    void EventSheetEditor::DrawEventSheetEventList()
    {
        if (window == nullptr || !eventSheetEventsWindow) return;
        int width, height;
        SDL_GetWindowSize(window, &width, &height);
        ImGui::SetNextWindowSize(ImVec2(1300, 800));
        ImGui::SetNextWindowPos(ImVec2((width - 1300) / 2.0f, (height - 800) / 2.0f));
        ImGui::Begin("Event List", &eventSheetEventsWindow, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse);

        static Event event;

        // Calculate the position and size of the left and right regions
        const float splitterSize = 4.0f;
        const ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();
        const ImVec2 leftRegionSize(contentRegionAvailable.x * 0.5f - splitterSize * 0.5f, contentRegionAvailable.y);
        const ImVec2 rightRegionSize(contentRegionAvailable.x * 0.5f - splitterSize * 0.5f, contentRegionAvailable.y);

        // Draw the left region
        ImGui::BeginChild("Left Region", leftRegionSize, true, ImGuiWindowFlags_NoScrollbar);
        //ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        //ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0.2f));
        //ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0.4f));

        // INPUT EVENTS
        static const char* inputEventList[] = { "Any Key Pressed", "Any Key Released", "Key Pressed", "Key Released" };
        static int selectedInputEvent = -1;
        int inputEventListSize = sizeof(inputEventList) / sizeof(inputEventList[0]);
        std::string inputEventSelectedString;
        if (selectedInputEvent == -1) inputEventSelectedString = "";
        else inputEventSelectedString = std::string(" - ") + inputEventList[selectedInputEvent];

        ImGui::SetNextWindowSizeConstraints(ImVec2(ImGui::GetWindowSize().x-15, inputEventListSize*20), ImVec2(ImGui::GetWindowSize().x-15, inputEventListSize*20));
        ImGui::PushItemWidth(ImGui::GetWindowSize().x-15);
        if (ImGui::BeginCombo("##InputEventDropdown", ("Input Events" + inputEventSelectedString).c_str())) {
            for (int i = 0; i < inputEventListSize; i++) {
                bool is_selected = (selectedInputEvent == i);
                if (ImGui::Selectable(inputEventList[i], is_selected))
                {
                    if (event.eventType != Null) event = Event();
                    event.canBeApplied = false;
                    event.inputType = true;
                    selectedInputEvent = i;
                    switch (i) {
                    case 0: event.eventType = Any_Key_Pressed; break;
                    case 1: event.eventType = Any_Key_Released; break;
                    case 2: event.eventType = Key_Pressed; break;
                    case 3: event.eventType = Key_Released; break;
                    }
                }
                if (is_selected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }
        // INPUT EVENTS

        // OTHER EVENTS
        ImGui::SetCursorPosY(35);
        if (ImGui::Button("Game Start Event", ImVec2(ImGui::GetWindowSize().x - 15, 17))) { if (event.eventType != Null) event = Event(); event.eventName = "Game Start"; event.eventType = Game_Start; event.requiresSettings = false; }
        ImGui::SetCursorPosY(60);
        if (ImGui::Button("Game Update Event", ImVec2(ImGui::GetWindowSize().x - 15, 17))) { if (event.eventType != Null) event = Event(); event.eventName = "Game Update"; event.eventType = Game_Update; event.requiresSettings = false; }
        // OTHER EVENTS

        //ImGui::PopStyleColor(3);
        ImGui::EndChild();

        ImDrawList* drawList = ImGui::GetWindowDrawList();
        drawList->AddRectFilled(ImVec2(0, 0), ImVec2(splitterSize, contentRegionAvailable.y), ImGui::GetColorU32(ImVec4(0.7f, 0.7f, 0.7f, 1.0f)));

        // Draw the right region
        ImGui::SameLine();
        ImGui::BeginChild("Right Region", rightRegionSize, true);
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0.2f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0.4f));
        ImGui::PopStyleColor(3);

        //static json settingsJson = NULL;

        json settingsJson;
        if (event.eventType != Null)
        {
            // INPUT EVENT
            if (event.inputType)
            {
                if (selectedInputEvent != -1)
                {
                    ImGui::PushFont(eventListEventTitleFont);
                    ImGui::SetCursorPosX((ImGui::GetWindowWidth() - ImGui::CalcTextSize(inputEventList[selectedInputEvent]).x) / 2);
                    ImGui::Text(inputEventList[selectedInputEvent]);
                    ImGui::PopFont();

                    static std::string keyValue = "";
                    event.eventName = inputEventList[selectedInputEvent];
                    static const char* inputKeyEventList[] = { "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z" };
                    static int selectedInputKeyEvent = -1;
                    static std::string selectedInputKeyEventString;
                    if (selectedInputKeyEvent == -1) selectedInputKeyEventString = "Key";
                    else selectedInputKeyEventString = inputKeyEventList[selectedInputKeyEvent];
                    int inputKeyEventListSize = sizeof(inputKeyEventList) / sizeof(inputKeyEventList[0]);

                    ImGui::SetNextWindowSizeConstraints(ImVec2(ImGui::GetWindowSize().x - 15, inputKeyEventListSize * 17), ImVec2(ImGui::GetWindowSize().x - 15, inputKeyEventListSize * 17));
                    ImGui::PushItemWidth(ImGui::GetWindowSize().x - 15);
                    if (ImGui::BeginCombo("##InputKeyEventDropdown", selectedInputKeyEventString.c_str())) {
                        for (int i = 0; i < inputKeyEventListSize; i++) {
                            bool is_selected = (selectedInputKeyEvent == i);
                            if (ImGui::Selectable(inputKeyEventList[i], is_selected)) selectedInputKeyEvent = i;
                            if (is_selected) ImGui::SetItemDefaultFocus();
                        }
                        ImGui::EndCombo();
                    }
                    if (selectedInputKeyEvent == -1) keyValue = "";
                    else
                    {
                        keyValue = inputKeyEventList[selectedInputKeyEvent];
                        event.canBeApplied = true;
                    }
                    settingsJson["Key"] = keyValue;
                }
            }
            // INPUT EVENT

            else
            {
                ImGui::PushFont(eventListEventTitleFont);
                ImGui::SetCursorPosX((ImGui::GetWindowWidth() - ImGui::CalcTextSize(event.eventName.c_str()).x) / 2);
                ImGui::Text(event.eventName.c_str());
                ImGui::PopFont();

                settingsJson = {};
                event.canBeApplied = true;
            }
        }
        ImGui::EndChild();

        ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() - 100, ImGui::GetWindowHeight() - 30));
        if (ImGui::Button("Apply", ImVec2(75, 25)) && (event.canBeApplied || !event.requiresSettings) && event.eventType != Null)
        {
            selectedInputEvent = -1;
            eventSheetEventsWindow = false;
            //std::string jsonStr = _data.dump();
            //ConsoleLogger::InfoLog(jsonStr);

            _data["EventGroups"][selectedEventGroup][event.eventName] = {};
            json& eventData =_data["EventGroups"][selectedEventGroup][event.eventName];
            eventData["Settings"] = settingsJson;
            //eventData["Actions"];

            //jsonStr = _data.dump();
            //ConsoleLogger::InfoLog(jsonStr);
            SaveEventSheet();
        }
        ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() - 100 - 85, ImGui::GetWindowHeight() - 30));
        if (ImGui::Button("Cancel", ImVec2(75, 25)))
        {
            selectedInputEvent = -1;
            eventSheetEventsWindow = false;
        }
        ImGui::End();
    }

    void Cleanup() // Todo (Don't forget to add cleanup to blueprints and sprite editor). Clean up fonts (in PhantomEngine2D too), textures, etc.
    {

    }
}