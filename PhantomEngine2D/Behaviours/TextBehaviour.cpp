#include "TextBehaviour.h"
#include "../ConsoleLogger.h"

TextBehaviour::TextBehaviour(ImGuiContext* gameContext, ImGuiContext* editorContext, std::string text)
    : _text(text), gameContext(gameContext), editorContext(editorContext)
{
    SetName("Text Behaviour");
    SetRenderInEditMode(true);
}

void TextBehaviour::Start()
{

}

void TextBehaviour::Update(float deltaTime)
{
    if (editorContext != nullptr)
    {
        ImGui::SetCurrentContext(editorContext);
        // Editor context code here

        // End
        ImGui::SetCurrentContext(gameContext);
    }
    // Game context code here

    // End
    if (editorContext != nullptr) ImGui::SetCurrentContext(editorContext);
}

TextBehaviour::~TextBehaviour()
{
}
