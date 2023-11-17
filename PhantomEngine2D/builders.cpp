#include "builders.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

namespace ed = ax::NodeEditor;
namespace util = ax::NodeEditor::Utilities;

util::BlueprintNodeBuilder::BlueprintNodeBuilder(ImTextureID texture, int textureWidth, int textureHeight) :
    HeaderTextureId(texture),
    HeaderTextureWidth(textureWidth),
    HeaderTextureHeight(textureHeight),
    CurrentNodeId(0),
    CurrentStage(Stage::Invalid),
    HasHeader(false)
{
}

void util::BlueprintNodeBuilder::Begin(ed::NodeId id)
{
    HasHeader = false;
    HeaderMin = HeaderMax = ImVec2();

    ed::PushStyleVar(StyleVar_NodePadding, ImVec4(8, 4, 8, 8));

    ed::BeginNode(id);

    ImGui::PushID(id.AsPointer());
    CurrentNodeId = id;

    SetStage(Stage::Begin);
}

void util::BlueprintNodeBuilder::End()
{
    SetStage(Stage::End);

    ed::EndNode();

    if (ImGui::IsItemVisible())
    {
        auto alpha = static_cast<int>(255 * ImGui::GetStyle().Alpha);

        auto drawList = ed::GetNodeBackgroundDrawList(CurrentNodeId);

        const auto halfBorderWidth = ed::GetStyle().NodeBorderWidth * 0.5f;

        auto headerColor = IM_COL32(0, 0, 0, alpha) | (HeaderColor & IM_COL32(255, 255, 255, 0));
        if ((HeaderMax.x > HeaderMin.x) && (HeaderMax.y > HeaderMin.y) && HeaderTextureId)
        {
            const auto uv = ImVec2(
                (HeaderMax.x - HeaderMin.x) / (float)(4.0f * HeaderTextureWidth),
                (HeaderMax.y - HeaderMin.y) / (float)(4.0f * HeaderTextureHeight));

            drawList->AddImageRounded(HeaderTextureId,
                ImVec2(HeaderMin.x - ImVec2(8 - halfBorderWidth, 4 - halfBorderWidth).x, HeaderMin.y - ImVec2(8 - halfBorderWidth, 4 - halfBorderWidth).y),
                ImVec2(HeaderMax.x + ImVec2(8 - halfBorderWidth, 0).x, HeaderMax.y + ImVec2(8 - halfBorderWidth, 0).y),
                ImVec2(0.0f, 0.0f), uv,
#if IMGUI_VERSION_NUM > 18101
                headerColor, GetStyle().NodeRounding, ImDrawFlags_RoundCornersTop);
#else
                headerColor, GetStyle().NodeRounding, 1 | 2);
#endif


            auto headerSeparatorMin = ImVec2(HeaderMin.x, HeaderMax.y);
            auto headerSeparatorMax = ImVec2(HeaderMax.x, HeaderMin.y);

            if ((headerSeparatorMax.x > headerSeparatorMin.x) && (headerSeparatorMax.y > headerSeparatorMin.y))
            {
                drawList->AddLine(
                    ImVec2(headerSeparatorMin.x + ImVec2(-(8 - halfBorderWidth), -0.5f).x, headerSeparatorMin.y + ImVec2(-(8 - halfBorderWidth), -0.5f).y),
                    ImVec2(headerSeparatorMax.x + ImVec2((8 - halfBorderWidth), -0.5f).x, headerSeparatorMax.x + ImVec2((8 - halfBorderWidth), -0.5f).y),
                    ImColor(255, 255, 255, 96 * alpha / (3 * 255)), 1.0f);
            }
        }
    }

    CurrentNodeId = 0;

    ImGui::PopID();

    ed::PopStyleVar();

    SetStage(Stage::Invalid);
}

void util::BlueprintNodeBuilder::Header(const ImVec4& color)
{
    HeaderColor = ImColor(color);
    SetStage(Stage::Header);
}

void util::BlueprintNodeBuilder::EndHeader()
{
    SetStage(Stage::Content);
}

void util::BlueprintNodeBuilder::Input(ed::PinId id)
{
    if (CurrentStage == Stage::Begin)
        SetStage(Stage::Content);

    const auto applyPadding = (CurrentStage == Stage::Input);

    SetStage(Stage::Input);

    if (applyPadding)
        ImGui::Dummy(ImVec2(0, 8)); // Add vertical padding

    Pin(id, PinKind::Input);

    ImGui::Columns(2); // Create two columns
    ImGui::SetColumnWidth(0, ImGui::GetContentRegionAvail().x * 0.3f); // Set the width of the first column
    ImGui::NextColumn(); // Move to the second column
}



void util::BlueprintNodeBuilder::EndInput()
{
    ImGui::EndGroup();
    ImGui::PopID();
    ImGui::SameLine();
    EndPin();
    ImGui::BeginGroup();
}


void util::BlueprintNodeBuilder::Middle()
{
    if (CurrentStage == Stage::Begin)
        SetStage(Stage::Content);

    SetStage(Stage::Middle);
}

void util::BlueprintNodeBuilder::Output(ed::PinId id)
{
    if (CurrentStage == Stage::Begin)
        SetStage(Stage::Content);

    const auto applyPadding = (CurrentStage == Stage::Output);

    SetStage(Stage::Output);

    if (applyPadding)
    {
        ImGui::AlignTextToFramePadding();
        ImGui::Dummy(ImVec2(ImGui::GetStyle().ItemSpacing.x, 0.0f));
    }

    Pin(id, PinKind::Output);

    ImGui::BeginGroup();
}

void util::BlueprintNodeBuilder::EndOutput()
{
    ImGui::EndGroup();
    ImGui::PopID();
    ImGui::SameLine();
    EndPin();
    ImGui::BeginGroup();
}

bool util::BlueprintNodeBuilder::SetStage(Stage stage)
{
    if (stage == CurrentStage)
        return false;

    auto oldStage = CurrentStage;
    CurrentStage = stage;

    ImVec2 cursor;
    switch (oldStage)
    {
    case Stage::Begin:
        break;

    case Stage::Header:
        ImGui::EndGroup();
        HeaderMin = ImGui::GetItemRectMin();
        HeaderMax = ImGui::GetItemRectMax();

        // spacing between header and content
        ImGui::Spacing();

        break;

    case Stage::Content:
        break;

    case Stage::Input:
        ed::PopStyleVar(2);

        ImGui::Dummy(ImVec2(0, 0));
        ImGui::EndGroup();

        // #debug
        // ImGui::GetWindowDrawList()->AddRect(
        //     ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(255, 0, 0, 255));

        break;

    case Stage::Middle:
        ImGui::EndGroup();

        // #debug
        // ImGui::GetWindowDrawList()->AddRect(
        //     ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(255, 0, 0, 255));

        break;

    case Stage::Output:
        ed::PopStyleVar(2);

        ImGui::Dummy(ImVec2(0, 0));
        ImGui::EndGroup();

        // #debug
        // ImGui::GetWindowDrawList()->AddRect(
        //     ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(255, 0, 0, 255));

        break;

    case Stage::End:
        if (oldStage == Stage::Input)
            ImGui::Spacing();
        if (oldStage != Stage::Begin)
            ImGui::EndGroup();
        ContentMin = ImGui::GetItemRectMin();
        ContentMax = ImGui::GetItemRectMax();

        //ImGui::Spacing();
        NodeMin = ImGui::GetItemRectMin();
        NodeMax = ImGui::GetItemRectMax();
        break;

    case Stage::Invalid:
        break;
    }

    switch (stage)
    {
    case Stage::Begin:
        ImGui::BeginGroup();
        break;

    case Stage::Header:
        HasHeader = true;

        ImGui::BeginGroup();
        break;

    case Stage::Content:
        if (oldStage == Stage::Header)
            ImGui::Spacing();

        ImGui::BeginGroup();
        ImGui::Dummy(ImVec2(0, 0));
        break;

    case Stage::Input:
        ImGui::BeginGroup();
        ImGui::BeginChild("inputs", ImVec2(0, 0), true);
        ed::PushStyleVar(ed::StyleVar_PivotAlignment, ImVec2(0, 0.5f));
        ed::PushStyleVar(ed::StyleVar_PivotSize, ImVec2(0, 0));

        if (!HasHeader)
            ImGui::Spacing();
        break;

    case Stage::Middle:
        if (oldStage == Stage::Input)
            ImGui::Spacing();

        ImGui::BeginGroup();
        ImGui::Dummy(ImVec2(0, 20));
        break;

    case Stage::Output:
        if (oldStage == Stage::Middle || oldStage == Stage::Input)
            ImGui::Spacing();
        else
            ImGui::Dummy(ImVec2(0, 0));

        ed::EndPin();
        ImGui::PopStyleVar(2);
        ImGui::EndChild();
        ImGui::EndGroup();
        break;

    default:
        break;
    }
}

void util::BlueprintNodeBuilder::Pin(ed::PinId id, ed::PinKind kind)
{
    ed::BeginPin(id, kind);
}

void util::BlueprintNodeBuilder::EndPin()
{
    ed::EndPin();

    // #debug
    // ImGui::GetWindowDrawList()->AddRectFilled(
    //     ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(255, 0, 0, 64));
}