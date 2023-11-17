#pragma once

#include "Behaviour.h"
#include "../GameObject.h"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer.h"

class TextBehaviour : public Behaviour {
public:
    TextBehaviour(ImGuiContext* gameContext, ImGuiContext* editorContext = nullptr, std::string text = "Example Text");
    void Update(float deltaTime) override;
    virtual ~TextBehaviour();
    void Start() override;

private:
    std::string _text;
    ImGuiContext* gameContext;
    ImGuiContext* editorContext;
};
