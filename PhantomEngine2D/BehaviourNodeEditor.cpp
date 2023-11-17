#include <SDL.h>
#include <SDL_render.h>
#include <SDL_image.h>
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer.h"
#include "BehaviourNodeEditor.h"
#include "EditorWindow.h"
#include <filesystem>
#include <imgui_internal.h>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <utility>

namespace PhantomEditor
{
    bool isFirstFrameBehaviourNodeEditor = true;
    bool behaviourNodeEditorOpen = false;
    std::filesystem::path _BNEPath;

    //namespace ed = ax::NodeEditor;

    //static ed::EditorContext* m_Editor = nullptr;

    //extern "C" __declspec(dllimport) short __stdcall GetAsyncKeyState(int vkey);
    //extern "C" bool Debug_KeyPress(int vkey)
    //{
    //    static std::map<int, bool> state;
    //    auto lastState = state[vkey];
    //    state[vkey] = (GetAsyncKeyState(vkey) & 0x8000) != 0;
    //    if (state[vkey] && !lastState)
    //        return true;
    //    else
    //        return false;
    //}

    static inline ImRect ImGui_GetItemRect()
    {
        return ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
    }

    static inline ImRect ImRect_Expanded(const ImRect& rect, float x, float y)
    {
        auto result = rect;
        result.Min.x -= x;
        result.Min.y -= y;
        result.Max.x += x;
        result.Max.y += y;
        return result;
    }


    enum class PinType
    {
        Flow,
        Bool,
        Int,
        Float,
        String,
        Object,
        Function,
        Delegate,
    };

    enum class PinKind
    {
        Output,
        Input
    };

    enum class NodeType
    {
        Blueprint,
        Simple,
        Tree,
        Comment,
        Houdini
    };

    struct Node;

    //struct Pin
    //{
    //    ed::PinId   ID;
    //    Node* Node;
    //    std::string Name;
    //    PinType     Type;
    //    PinKind     Kind;

    //    Pin(int id, const char* name, PinType type) :
    //        ID(id), Node(nullptr), Name(name), Type(type), Kind(PinKind::Input)
    //    {
    //    }
    //};

    //struct Node
    //{
    //    ed::NodeId ID;
    //    std::string Name;
    //    std::vector<Pin> Inputs;
    //    std::vector<Pin> Outputs;
    //    ImColor Color;
    //    NodeType Type;
    //    ImVec2 Size;

    //    std::string State;
    //    std::string SavedState;

    //    Node(int id, const char* name, ImColor color = ImColor(255, 255, 255)) :
    //        ID(id), Name(name), Color(color), Type(NodeType::Blueprint), Size(0, 0)
    //    {
    //    }
    //};

    //struct Link
    //{
    //    ed::LinkId ID;

    //    ed::PinId StartPinID;
    //    ed::PinId EndPinID;

    //    ImColor Color;

    //    Link(ed::LinkId id, ed::PinId startPinId, ed::PinId endPinId) :
    //        ID(id), StartPinID(startPinId), EndPinID(endPinId), Color(255, 255, 255)
    //    {
    //    }
    //};

    //struct NodeIdLess
    //{
    //    bool operator()(const ed::NodeId& lhs, const ed::NodeId& rhs) const
    //    {
    //        return lhs.AsPointer() < rhs.AsPointer();
    //    }
    //};

    static bool Splitter(bool split_vertically, float thickness, float* size1, float* size2, float min_size1, float min_size2, float splitter_long_axis_size = -1.0f)
    {
        using namespace ImGui;
        ImGuiContext& g = *GImGui;
        ImGuiWindow* window = g.CurrentWindow;
        ImGuiID id = window->GetID("##Splitter");
        ImRect bb;
        ImVec2 minSplit = (split_vertically ? ImVec2(*size1, 0.0f) : ImVec2(0.0f, *size1));
        ImVec2 maxSplit = CalcItemSize(split_vertically ? ImVec2(thickness, splitter_long_axis_size) : ImVec2(splitter_long_axis_size, thickness), 0.0f, 0.0f);
        bb.Min = ImVec2(window->DC.CursorPos.x + minSplit.x, window->DC.CursorPos.y + minSplit.y);
        bb.Max = ImVec2(bb.Min.x + maxSplit.x, bb.Min.y + maxSplit.y);
        return SplitterBehavior(bb, id, split_vertically ? ImGuiAxis_X : ImGuiAxis_Y, size1, size2, min_size1, min_size2, 0.0f);
    }

    void DrawNodesList1()
    {
        // Display a list of nodes that can be dragged into the editor
        ImGui::Text("Nodes List:");
        ImGui::Separator();
        if (ImGui::Button("Node 1"))
        {
            // TODO: Create a new node of type 1
        }
        if (ImGui::Button("Node 2"))
        {
            // TODO: Create a new node of type 2
        }
        // Add more buttons for additional nodes as needed
    }

    void DrawNodesList()
    {
        // Here you can create a list of available nodes that the user can drag into the editor
        // For example:
        ImGui::BeginChild("Nodes List", ImVec2(100, 0), true);
        ImGui::Text("Nodes List");
        ImGui::Separator();
        if (ImGui::Button("Add Node"))
        {
            // Add your node creation logic here
        }
        ImGui::EndChild();
    }

    void ProcessEditorEvents()
    {
        // Here you can process any events in the editor
        // For example:
        if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Delete)))
        {
            // Delete the selected node
        }
    }

    //void DrawNodes()
    //{
    //    // Here you can draw any nodes that are currently in the editor
    //    // For example:
    //    static int node_id = 1;
    //    if (ImGui::BeginNode(node_id))
    //    {
    //        ImGui::Text("Node Title");
    //        ImGui::InputFloat("Value", &node_value);
    //        ImGui::EndNode();
    //    }
    //}


    void BehaviourNodeEditor::SetPath(std::filesystem::path path)
    {
        _BNEPath = path;
    }

    void BehaviourNodeEditor::BehaviourNodeEditorSetup(SDL_Window* win)
    {
        window = win;

        //ed::Config config;
        //config.SettingsFile = "MyEditor.json";
        //config.LoadNodeSettings = [](ed::NodeId nodeId, void* userPointer) -> ed::NodeSettings*
        //{
        //    return nullptr;
        //};
        //config.SaveNodeSettings = [](ed::NodeId nodeId, const ed::NodeSettings* data, void* userPointer)
        //{

        //};

        //m_Editor = ed::CreateEditor();

        // Set style colors for the editor
        //ed::PushStyleColor(ed::StyleColor_NodeBg, ImColor(60, 60, 70, 200));
        //ed::PushStyleColor(ed::StyleColor_NodeBorder, ImColor(60, 60, 60, 200));
        //ed::PushStyleColor(ed::StyleColor_HoveredNodeBg, ImColor(75, 75, 85, 220));
        //ed::PushStyleColor(ed::StyleColor_HoveredNodeBorder, ImColor(75, 75, 75, 220));
        //ed::PushStyleColor(ed::StyleColor_SelectedNodeBg, ImColor(110, 110, 120, 200));
        //ed::PushStyleColor(ed::StyleColor_SelectedNodeBorder, ImColor(110, 110, 110, 200));

        //ImNodes::PushColorStyle(ImNodesCol_NodeBackground, ImColor(60, 60, 70, 200));
        //ImNodes::PushColorStyle(ImNodesCol_NodeBackgroundHovered, ImColor(75, 75, 85, 220));
        //ImNodes::PushColorStyle(ImNodesCol_NodeBackgroundSelected, ImColor(110, 110, 120, 200));
        //ImNodes::PushColorStyle(ImNodesCol_NodeOutline, ImColor(60, 60, 60, 200));
        //ImNodes::PushColorStyle(ImNodesCol_TitleBar, ImColor(60, 60, 70, 200));
        //ImNodes::PushColorStyle(ImNodesCol_TitleBarHovered, ImColor(75, 75, 85, 220));
        //ImNodes::PushColorStyle(ImNodesCol_TitleBarSelected, ImColor(110, 110, 120, 200));


        // Set the theme for the editor
        //ed::StyleVar styleVar;
        //styleVar = ed::StyleVar_NodePadding;
        //ed::PushStyleVar(styleVar, ImVec4(0.8f, 0.8f, 0.8f, 1.0f));
        //ed::PushStyleVar(ed::StyleVar_NodeRounding, 12.0f);
        //ed::PushStyleVar(ed::StyleVar_SourceDirection, ImVec2(0.0f, 1.0f));
        //ed::PushStyleVar(ed::StyleVar_TargetDirection, ImVec2(0.0f, -1.0f));
        //ed::PushStyleVar(ed::StyleVar_LinkStrength, 0.0f);
        //ed::PushStyleVar(ed::StyleVar_PinBorderWidth, 1.0f);
        //ed::PushStyleVar(ed::StyleVar_PinRadius, 4.0f);
        //ed::PushStyleColor(ed::StyleColor_NodeBg, ImVec4(0.4f, 0.4f, 0.4f, 1.0f));
        //ed::PushStyleColor(ed::StyleColor_NodeBorder, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
        //ed::PushStyleColor(ed::StyleColor_HoveredNode, ImVec4(0.6f, 0.6f, 0.6f, 1.0f));
        //ed::PushStyleColor(ed::StyleColor_HovNodeBorder, ImVec4(0.4f, 0.4f, 0.4f, 1.0f));
        //ed::PushStyleColor(ed::StyleColor_SelectedNodeBg, ImVec4(0.8f, 0.8f, 0.8f, 1.0f));
        //ed::PushStyleColor(ed::StyleColor_SelNodeBorder, ImVec4(0.6f, 0.6f, 0.6f, 1.0f));
    }

    void BehaviourNodeEditor::DrawBehaviourNodeEditor()
    {
        if (window == nullptr || !behaviourNodeEditorOpen) return;
        if (isFirstFrameBehaviourNodeEditor)
        {
            isFirstFrameBehaviourNodeEditor = false;
            int width, height;
            SDL_GetWindowSize(window, &width, &height);
            ImGui::SetNextWindowSize(ImVec2(220, 130));
            ImGui::SetNextWindowPos(ImVec2((width - 220) / 2.0f, (height - 130) / 2.0f));
        }
        ImGui::Begin("Visual Scripting", &behaviourNodeEditorOpen, ImGuiWindowFlags_NoResize);

        //DrawNodesList();

        //ed::SetCurrentEditor(m_Editor);
        //ed::Begin("My BluePrint");

        //// Process any events in the editor
        //ProcessEditorEvents();

        // Draw any nodes that are currently in the editor
        //DrawNodes();

        // End the BluePrint editor
/*        ed::End();
        ed::SetCurrentEditor(nullptr)*/;

        //ed::ProcessNodeEditorEvents(*m_Editor);


        ImGui::End();
    }

}