#define NOMINMAX
#include <SDL.h>
#include <SDL_render.h>
#include <SDL_image.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer.h"
#include "EngineFonts.h"
#include <cstdio>
#include <string>
#include <iostream>
#include <imgui_internal.h>
#include "ProjectManager.h"
#include "ConsoleLogger.h"
#include "EditorWindow.h"
#include <filesystem>
#include <dirent/dirent.h>
#include <algorithm>
#include <regex>
#include <cstdlib>
#include <unordered_map>
#include <vector>
#include <random>
#include "Behaviours/Behaviour.h"
#include "Behaviours/RigidbodyBehaviour.h";
#include "Behaviours/ColliderBehaviour.h"
#include "PresetBehaviours/MoveBehaviour.h"
#include <chrono>
#include "GameObject.h"
#include "magic_enum.hpp"
#include "SpriteEditorWindow.h"
#include <Windows.h>
#include <Shellapi.h>
#include "BehaviourNodeEditor.h"
#include "Scenes/SceneManager.h"
#include "Scenes/Scene.h"
#include "ScriptCompiler.h"

#include "Behaviours/EventSheetEditor.h"
#include "PaddleMoveBehaviour.h"
#include "EnemyPaddleMoveBehaviour.h"
#include "BallMovementBehaviour.h"
#include "BallColliderBehaviour.h"

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>

using json = nlohmann::json;

// // Rename to LuminousEngine

namespace PhantomEditor {

    EditorWindow* EditorWindow::editorWindow = nullptr;
    SDL_Window* window = nullptr;
    SDL_Window* gameWindow = nullptr;
    static SDL_Renderer* renderer = nullptr;
    static SDL_Renderer* gameRenderer = nullptr;
    ImGuiContext* rendererContext;
    ImGuiContext* gameRendererContext;
    EngineFonts defaultFont;
    ImFont* defaultFont13;
    ImFont* defaultFont16;
    ImFont* defaultFont24;
    ImFont* defaultFont34;
    bool isFirstBehaviourFrame = true;
    bool isViewportFirstFrame = true;
    bool isFileExplorerFirstFrame = true;
    bool isPropertiesFirstFrame = true;
    bool isHierarchyFirstFrame = true;
    std::filesystem::path projectPath;
    std::filesystem::path fileExplorerPath;

    MonoDomain* monoDomain;
    MonoAssembly* monoAssembly;

    SDL_Texture* playIcon;
    SDL_Texture* activePlayIcon;
    SDL_Texture* grayedPlayIcon;
    SDL_Texture* pauseIcon;
    SDL_Texture* activePauseIcon;
    SDL_Texture* grayedPauseIcon;
    SDL_Texture* folderIconTexture;
    SDL_Texture* cppIconTexture;
    SDL_Texture* behaviourIconTexture;
    SDL_Texture* blueBackgroundTexture;
    SDL_Texture* blueGameBackgroundTexture;
    SDL_Texture* yArrowTexture;
    SDL_Texture* xArrowTexture;
    std::unordered_map<std::filesystem::path, SDL_Texture*> userTextures;

    bool isDragging = false;
    bool draggingObject = false;
    ImVec2 dragStartPos;
    ImVec2 dropPos;
    ImVec2 dragPos;
    ImVec2 viewportTransformDragPos;
    bool movingObjectWithXArrow = false;
    bool movingObjectWithYArrow = false;
    bool hoveringMovementArrow = false;
    float objectToMouseDistanceDrag;
    bool behavioursWindowOpen = false;

    GameObject dragData;
    //std::vector<GameObject> gameObjects(0);
    //std::vector<GameObject> gameObjectsUnordered(0);
    GameObject* objectInProperties = nullptr;
    GameObject* selectedObject = nullptr;
    static bool playing = false;
    bool startStopPlaying = false;
    bool paused = false;

    SpriteEditorWindow spriteEditorWindow;
    BehaviourNodeEditor behaviourNodeEditor;
    EventSheetEditor eventSheetEditor;

    bool explorerContextMenuOpen = false;
    bool isDraggingConsoleExplorerWindow = false;
    ImVec2 consoleExplorerWindowOffset = ImVec2(0, 0);
    bool explorerOpen = true;

    bool quitEditor = false;

    std::vector<ConsoleLogger::ConsoleLog> EditorWindow::consoleLogs;

    //using CreateBehaviorFunc = std::unique_ptr<Behavior> (*)();


    void EditorWindow::InitFonts()
    {
        ImGuiIO& io = ImGui::GetIO();

        // Load a font file
        io.Fonts->AddFontFromFileTTF("Fonts/ProggyClean.ttf", 13.0f);

        // Set the font as the default
        io.FontDefault = io.Fonts->Fonts.back();
        defaultFont = LiberationSansRegularFont;

        // Create fonts
        defaultFont13 = GetEngineFont(LiberationSansRegularFont, 13);
        defaultFont16 = GetEngineFont(LiberationSansBoldFont, 26);
        defaultFont24 = GetEngineFont(LiberationSansBoldFont, 24);
        defaultFont34 = GetEngineFont(LiberationSansBoldFont, 34);
    }

    void EditorWindow::InitImages()
    {
        // Initialize SDL_image
        int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG | IMG_INIT_WEBP | IMG_INIT_AVIF | IMG_INIT_JXL | IMG_INIT_TIF;
        if (!(IMG_Init(imgFlags) & imgFlags))
        {
            // Handle error
        }

        std::filesystem::path imagesPath = std::filesystem::path(__FILE__).parent_path() / "Images";

        //SDL_Surface* folderSurface = IMG_Load((imagesPath / "FolderIcon.png").string().c_str());
        //folderIconTexture = SDL_CreateTextureFromSurface(renderer, folderSurface);
        //SDL_FreeSurface(folderSurface);

        //SDL_Surface* cppSurface = IMG_Load((imagesPath / "CppIcon.png").string().c_str());
        //cppIconTexture = SDL_CreateTextureFromSurface(renderer, cppSurface);
        //SDL_FreeSurface(cppSurface);

        //std::string textureFiles[] = { "FolderIcon.png", "CppIcon.png" };
        //for (int i = 0; i < sizeof(textureFiles) / sizeof(textureFiles[0]); i++) {
        //    SDL_Surface* surface = IMG_Load((imagesPath / textureFiles[i]).string().c_str());
        //    if (textureFiles[i] == "FolderIcon.png") folderIconTexture = SDL_CreateTextureFromSurface(renderer, surface);
        //    else if (textureFiles[i] == "CppIcon.png") cppIconTexture = SDL_CreateTextureFromSurface(renderer, surface);
        //    SDL_FreeSurface(surface);
        //}

        // TODO: Instead crature a global textureFiles unordered map array, the if and else if statements is not needed and instead loop directly through the images folder to get the image paths

        std::string textureFiles[] = { "FolderIcon.png", "CppIcon.png", "GreenArrow.png", "RedArrow.png", "BehaviourIcon.png", "PlayIcon.png", "ActivePlayIcon.png", "GrayedPlayIcon.png", "PauseIcon.png", "ActivePauseIcon.png", "GrayedPauseIcon.png"};
        for (int i = 0; i < sizeof(textureFiles) / sizeof(textureFiles[0]); i++) {
            SDL_Surface* surface = IMG_Load((imagesPath / textureFiles[i]).string().c_str());
            if (textureFiles[i] == "FolderIcon.png") folderIconTexture = SDL_CreateTextureFromSurface(renderer, surface);
            else if (textureFiles[i] == "CppIcon.png") cppIconTexture = SDL_CreateTextureFromSurface(renderer, surface);
            else if (textureFiles[i] == "GreenArrow.png") yArrowTexture = SDL_CreateTextureFromSurface(renderer, surface);
            else if (textureFiles[i] == "RedArrow.png") xArrowTexture = SDL_CreateTextureFromSurface(renderer, surface);
            else if (textureFiles[i] == "BehaviourIcon.png") behaviourIconTexture = SDL_CreateTextureFromSurface(renderer, surface);
            else if (textureFiles[i] == "PlayIcon.png") playIcon = SDL_CreateTextureFromSurface(renderer, surface);
            else if (textureFiles[i] == "ActivePlayIcon.png") activePlayIcon = SDL_CreateTextureFromSurface(renderer, surface);
            else if (textureFiles[i] == "GrayedPlayIcon.png") grayedPlayIcon = SDL_CreateTextureFromSurface(renderer, surface);
            else if (textureFiles[i] == "PauseIcon.png") pauseIcon = SDL_CreateTextureFromSurface(renderer, surface);
            else if (textureFiles[i] == "ActivePauseIcon.png") activePauseIcon = SDL_CreateTextureFromSurface(renderer, surface);
            else if (textureFiles[i] == "GrayedPauseIcon.png") grayedPauseIcon = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_FreeSurface(surface);
        }

        SDL_Surface* blueSurface = SDL_CreateRGBSurface(0, 1920, 1080, 32, 0, 0, 0, 0);
        Uint32 blueColor = SDL_MapRGB(blueSurface->format, 3, 136, 252);
        SDL_FillRect(blueSurface, NULL, blueColor);
        blueBackgroundTexture = SDL_CreateTextureFromSurface(renderer, blueSurface);
        blueGameBackgroundTexture = SDL_CreateTextureFromSurface(gameRenderer, blueSurface);
        SDL_FreeSurface(blueSurface);

        // Loads textures
    }


    void EditorWindow::InitSDL()
    {
        // Initialize SDL2
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
            printf("SDL_Init Error: %s\n", SDL_GetError());
            exit(1);
        }

        // get desktop size
        SDL_DisplayMode dm;
        SDL_GetDesktopDisplayMode(0, &dm);
        int width = dm.w;
        int height = dm.h - 50;


        // Create SDL2 window
        window = SDL_CreateWindow(("Phantom Engine v0.1 - " + projectPath.stem().string()).c_str(),
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            width, height,
            SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
        //SDL_SetWindowResizable(window, SDL_FALSE);
        SDL_SetWindowMinimumSize(window, 960, 540);
        //SDL_SetWindowMaximumSize(window, 500, 250);
        if (!window) {
            printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
            SDL_Quit();
            exit(1);
        }

        // Create SDL2 renderer
        renderer = SDL_CreateRenderer(window, -1,
            SDL_RENDERER_ACCELERATED |
            SDL_RENDERER_PRESENTVSYNC);
        if (!renderer) {
            printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
            SDL_DestroyWindow(window);
            SDL_Quit();
            exit(1);
        }


        // Game Window/Renderer
        gameWindow = SDL_CreateWindow(("Phantom Engine v0.1 - " + projectPath.stem().string()).c_str(),
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            1366, 768,
            SDL_WINDOW_HIDDEN | SDL_WINDOW_RESIZABLE);
        //SDL_SetWindowResizable(window, SDL_FALSE);
        SDL_SetWindowMinimumSize(window, 480, 270);
        //SDL_SetWindowMaximumSize(window, 500, 250);
        if (!gameWindow) {
            printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
            SDL_Quit();
            exit(1);
        }

        // Create SDL2 renderer
        gameRenderer = SDL_CreateRenderer(gameWindow, -1,
            SDL_RENDERER_ACCELERATED |
            SDL_RENDERER_PRESENTVSYNC);
        if (!gameRenderer) {
            printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
            SDL_DestroyWindow(gameWindow);
            SDL_Quit();
            exit(1);
        }


        // Initialize ImGui for SDL2 + SDL_Renderer
        rendererContext = ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        (void)io;
        io.ConfigWindowsMoveFromTitleBarOnly = true;
        InitFonts();
        ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
        ImGui_ImplSDLRenderer_Init(renderer);

        // Dark Theme
        ImGuiStyle& style = ImGui::GetStyle();
        ImVec4* colors = style.Colors;

        // Set up ImGUI style
        ImGui::StyleColorsDark();

        // Global Colors
        colors[ImGuiCol_WindowBg] = ImVec4(0.09f, 0.09f, 0.09f, 1.00f);
        colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.23f, 0.23f, 0.23f, 1.00f);
        colors[ImGuiCol_FrameBgActive] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
        colors[ImGuiCol_TitleBg] = ImVec4(0.13f, 0.13f, 0.13f, 1.00f);
        colors[ImGuiCol_TitleBgActive] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
        colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.09f, 0.09f, 0.09f, 1.00f);
        colors[ImGuiCol_MenuBarBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
        colors[ImGuiCol_ScrollbarBg] = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
        colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.45f, 0.45f, 0.45f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.55f, 0.55f, 0.55f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.65f, 0.65f, 0.65f, 1.00f);
        colors[ImGuiCol_CheckMark] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
        colors[ImGuiCol_SliderGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
        colors[ImGuiCol_SliderGrabActive] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);

        // Headers
        colors[ImGuiCol_Header] = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
        colors[ImGuiCol_HeaderHovered] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
        colors[ImGuiCol_HeaderActive] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
        colors[ImGuiCol_Separator] = colors[ImGuiCol_Border];

        // Buttons
        colors[ImGuiCol_Button] = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
        colors[ImGuiCol_ButtonActive] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);

        // Dropdowns
        //colors[ImGuiCol_DropdownBg] = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
        //colors[ImGuiCol_DropdownHovered] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
        //colors[ImGuiCol_DropdownActive] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
        colors[ImGuiCol_MenuBarBg] = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);

        // Text
        colors[ImGuiCol_Text] = ImVec4(0.95f, 0.95f, 0.95f, 1.00f);
        colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
        colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);

        // Popups
        colors[ImGuiCol_PopupBg] = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);
        colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

        // Modals
        //colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
        //colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);



        // Initialize ImGui for SDL2 + SDL_Renderer     gameRenderer
        gameRendererContext = ImGui::CreateContext();
        ImGui::SetCurrentContext(gameRendererContext);
        ImGuiIO& gameIO = ImGui::GetIO();
        (void)gameIO;
        io.ConfigWindowsMoveFromTitleBarOnly = true;
        ImGui_ImplSDL2_InitForSDLRenderer(gameWindow, gameRenderer);
        ImGui_ImplSDLRenderer_Init(gameRenderer);
        ImGui::SetCurrentContext(rendererContext);

    }

    std::string PathToString(std::filesystem::path path)
    {
        std::string pathString = path.string();
        std::replace(pathString.begin(), pathString.end(), '\\', '/');
        pathString.erase(pathString.find_last_not_of('\0') + 1); // Check if this is even needed after I added the new hidden character removal below.
        pathString.erase(std::remove_if(pathString.begin(), pathString.end(), [](char c) {
            return !std::isprint(static_cast<unsigned char>(c));
            }), pathString.end());
        return pathString;
    }


    void EditorWindow::CleanupSDL()
    {
        ImGui_ImplSDLRenderer_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();
        IMG_Quit();

        mono_jit_cleanup(monoDomain);

        SDL_DestroyTexture(blueBackgroundTexture);
        SDL_DestroyTexture(folderIconTexture);
        SDL_DestroyTexture(cppIconTexture);
        SDL_DestroyTexture(behaviourIconTexture);
        for (auto& texturePair : userTextures) {
            SDL_DestroyTexture(texturePair.second);
        }
        for (auto& object : SceneManager::GetActiveScene()->GetGameObjects()) {
            SDL_DestroyTexture(object.GetTexture());
            SDL_DestroyTexture(object.GetGameTexture());
        }
        SDL_DestroyRenderer(gameRenderer);
        SDL_DestroyWindow(gameWindow);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

    void HandleWindowResize(int windowWidth, int windowHeight)
    {
        // Calculate the new scale factor based on the window size
        float scaleX = (float)windowWidth / 1920;
        float scaleY = (float)windowHeight / 1080;

        // Update the positions and scales of all game objects
        for (GameObject& object : SceneManager::GetActiveScene()->GetGameObjects())
        {
            int originalWidth = object.GetSize().x;
            int originalHeight = object.GetSize().y;
            int originalX = object.GetPosition().x;
            int originalY = object.GetPosition().y;

            // Compute the new size and position of the object based on the current window size
            float scaleX = (float)windowWidth / (float)originalWidth;
            float scaleY = (float)windowHeight / (float)originalHeight;
            int newX = (int)(originalX * scaleX);
            int newY = (int)(originalY * scaleY);
            object.SetPosition(ImVec2(newX, newY));
            object.SetSize(ImVec2(scaleX, scaleY));
        }
    }

    void EditorWindow::ProcessEvents()
    {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.window.windowID != SDL_GetWindowID(window)) return;
            switch (event.type) {
            case SDL_QUIT:
                ConsoleLogger::InfoLog("Closing editor");
                quitEditor = true;
                exit(0);
            break;
            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    // Start the drag and drop operation
                    //dragData = button_data;
                    dragStartPos = ImVec2(event.button.x, event.button.y);
                    isDragging = true;
                }
                break;
            case SDL_MOUSEBUTTONUP:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    isDragging = false;
                }
                break;
            case SDL_MOUSEMOTION:
                if (isDragging) {
                    // Update the drag position
                    dragPos = ImVec2(event.motion.x, event.motion.y);
                }
                if (movingObjectWithXArrow || movingObjectWithYArrow) {
                    // Update the drag position
                    viewportTransformDragPos = ImVec2(event.motion.x, event.motion.y);
                }
                break;
            }
        }
    }

    void EditorWindow::ProcessGameEvents()
    {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.window.windowID != SDL_GetWindowID(gameWindow)) return;
            switch (event.type) {
            case SDL_QUIT:
                exit(0);
            break;
            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                    HandleWindowResize(event.window.data1, event.window.data2);
                }
            }
        }
    }

    bool CompareGameObjects(GameObject* object, GameObject* otherObject)
    {
        return std::find_if(SceneManager::GetActiveScene()->GetGameObjects().begin(), SceneManager::GetActiveScene()->GetGameObjects().end(), [&](const auto& obj) { return obj.GetId() == object->GetId() && otherObject->GetId() == obj.GetId(); }) != SceneManager::GetActiveScene()->GetGameObjects().end();
    }

    void LoadUserImages()
    {
        // Recursive directory iterator
        std::filesystem::recursive_directory_iterator iter(projectPath / "Assets");
        std::filesystem::recursive_directory_iterator end;

        // Loop through all files in the directory tree
        while (iter != end) {
            if (std::filesystem::is_regular_file(*iter) && iter->path().extension() == ".png" || iter->path().extension() == ".webp" || iter->path().extension() == ".jpg") {
                // Load image into SDL texture
                SDL_Texture* texture = IMG_LoadTexture(renderer, iter->path().string().c_str());

                if (texture)
                {
                    // Add texture to unordered_map with its name as the key
                    std::string texturePath = PathToString(iter->path());
                    std::replace(texturePath.begin(), texturePath.end(), '\\', '/');
                    texturePath.erase(texturePath.find_last_not_of('\0') + 1);
                    // ConsoleLogger::InfoLog("Added " + texturePath + " " + std::to_string(texturePath.length()));
                    //userTextures[texturePath] = texture;
                    //ConsoleLogger::InfoLog("Loaded texture " + texturePath)
                    ;
                }
            }
            ++iter;
        }
    }

    void UnloadUserImages()
    {
        //for (const auto& [name, texture] : userTextures) {
        //    SDL_DestroyTexture(texture);
        //}
        //userTextures.clear();
    }

    SDL_Texture* GetUserTexture(std::filesystem::path path)
    {
        SDL_Texture* texture = nullptr;
        auto it = userTextures.find(path);
        if (it == userTextures.end()) {
            SDL_Surface* surface = IMG_Load(path.string().c_str());
            texture = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_FreeSurface(surface);
            userTextures[path] = texture;
        }
        return userTextures[path];
    }

    void DrawPositionArrows(GameObject& gameObject)
    {
        if ((movingObjectWithXArrow || movingObjectWithYArrow) && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
        {
            movingObjectWithXArrow = false;
            movingObjectWithYArrow = false;
            //selectedObject = nullptr;
            //objectToMouseDistanceDrag = (viewportTransformDragPos.x - ImGui::GetWindowPos().x) - selectedObject->positon.x;
        }
        if (selectedObject == nullptr || !CompareGameObjects(selectedObject, &gameObject)) return;

        bool yHovering = false;
        bool xHovering = false;

        ImGui::SameLine();
        ImGui::SetCursorPos(ImVec2(gameObject.GetPosition().x + gameObject.GetSize().x, gameObject.GetPosition().y + gameObject.GetSize().y / 2));
        ImGui::Image((ImTextureID)xArrowTexture, ImVec2(100, 25), ImVec2(0, 0), ImVec2(1, 1), ImColor(255, 255, 255, 200), ImColor(0, 0, 0, 0));
        if (ImGui::IsItemHovered()) xHovering = true;
        if (movingObjectWithXArrow || (ImGui::IsItemHovered() && ImGui::IsMouseDown(ImGuiMouseButton_Left)))
        {
            if (!movingObjectWithXArrow)
            {
                movingObjectWithXArrow = true;
                objectToMouseDistanceDrag = (viewportTransformDragPos.x - ImGui::GetWindowPos().x) - selectedObject->GetPosition().x;
            }
            float mousePos = viewportTransformDragPos.x - ImGui::GetWindowPos().x;
            selectedObject->SetPosition(ImVec2(mousePos - objectToMouseDistanceDrag, selectedObject->GetPosition().y));
            //ConsoleLogger::InfoLog(std::to_string(viewportTransformDragPos.x - ImGui::GetWindowPos().x));
        }

        ImGui::SetCursorPos(ImVec2(gameObject.GetPosition().x + gameObject.GetSize().x / 2, gameObject.GetPosition().y - 100));
        ImGui::Image((ImTextureID)yArrowTexture, ImVec2(25, 100), ImVec2(0, 0), ImVec2(1, 1), ImColor(255, 255, 255, 200), ImColor(0, 0, 0, 0));
        if (ImGui::IsItemHovered()) yHovering = true;
        if (movingObjectWithYArrow || (ImGui::IsItemHovered() && ImGui::IsMouseDown(ImGuiMouseButton_Left)))
        {
            if (!movingObjectWithYArrow)
            {
                movingObjectWithYArrow = true;
                objectToMouseDistanceDrag = (viewportTransformDragPos.y - ImGui::GetWindowPos().y) - selectedObject->GetPosition().y;
            }
            float mousePos = viewportTransformDragPos.y - ImGui::GetWindowPos().y;
            selectedObject->SetPosition(ImVec2(selectedObject->GetPosition().x, mousePos - objectToMouseDistanceDrag));
        }

        if (xHovering || yHovering) hoveringMovementArrow = true;
        else hoveringMovementArrow = false;
    }

    void DrawToViewport(GameObject& gameObject) {
        // SDL_Texture* texture, const ImVec2& size, const ImVec2& position

                //Use this to indicate the object is selected
        //float borderSize = 1.0f;
        //ImVec2 borderOffset(borderSize, borderSize);
        //ImVec2 imageMin = windowPos + borderOffset;
        //ImVec2 imageMax = windowPos + windowSize - borderOffset;

        //ImGui::GetWindowDrawList()->AddRect(windowPos, windowPos + windowSize, ImColor(255, 255, 255, 255));
        //ImGui::GetWindowDrawList()->AddImage(bgTextureId, imageMin, imageMax, ImVec2(0, 1), ImVec2(1, 0), ImColor(255, 255, 255, 255));

        //ConsoleLogger::InfoLog(std::to_string(gameObject.GetRealSize().x));
        //ConsoleLogger::InfoLog(std::to_string(gameObject.GetRealSize().y));

        ImTextureID textureId = (ImTextureID)gameObject.GetTexture();

        ImGui::SetCursorPos(gameObject.GetPosition());
        ImGui::Image((ImTextureID)textureId, gameObject.GetSize(), ImVec2(0, 0), ImVec2(1, 1), gameObject.GetTint(), gameObject.GetBorder()
        );

        //ImGui::GetWindowDrawList()->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), ImColor(255, 0, 0, 255));

        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !hoveringMovementArrow && ImGui::IsItemHovered()) {
            selectedObject = &gameObject;
            objectInProperties = &gameObject;
        }

        //DrawPositionArrows(gameObject); // Moved to fix issue
    }

    void DrawBackground()
    {
        ImTextureID bgTextureId = (ImTextureID)blueBackgroundTexture;
        ImVec2 windowPos = ImGui::GetWindowPos();
        ImVec2 windowSize = ImGui::GetWindowSize();
        ImGuiStyle& style = ImGui::GetStyle();
        ImVec2 prevPadding = style.WindowPadding;
        style.WindowPadding = ImVec2(0, 0);
        //ImGui::GetWindowDrawList()->AddImage(bgTextureId, windowPos, ImVec2(windowPos.x + windowSize.x, windowPos.y + windowSize.y), ImVec2(0, 1), ImVec2(1, 0), ImColor(255, 255, 255, 255));
        ImGui::SetCursorPos(ImVec2(0,0));
        ImGui::Image((ImTextureID)bgTextureId, ImVec2(windowPos.x + windowSize.x, windowPos.y + windowSize.y), ImVec2(0, 0), ImVec2(1, 1), ImColor(255, 255, 255, 255));
        style.WindowPadding = prevPadding;

        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && ImGui::IsItemHovered() && !hoveringMovementArrow)
        {
            selectedObject = nullptr;
            objectInProperties = nullptr;
        }

        //if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && ImGui::IsMouseHoveringRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax())) {
        //    selectedObject = nullptr;
        //    objectInProperties = nullptr;
        //}
    }

    void ShowExplorerConsole()
    {
        static bool show = true;

        if (isFileExplorerFirstFrame)
        {
            isFileExplorerFirstFrame = false;
            ImGui::SetNextWindowSize(ImVec2(1920, 282));
            ImGui::SetNextWindowPos(ImVec2(0, 788));
        }

        ImGui::Begin("ExplorerConsole", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove);
        ImVec2 pos = ImGui::GetWindowPos();
        ImVec2 size = ImGui::GetWindowSize();
        ImDrawList* drawList = ImGui::GetWindowDrawList();

        // Custom title bar
        const float titleBarHeight = 20.0f;
        //drawList->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + titleBarHeight), IM_COL32(50, 50, 50, 255));
        drawList->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + titleBarHeight), ImGui::GetColorU32(ImVec4(0.13f, 0.13f, 0.13f, 1.00f)));
        drawList->AddRectFilled(pos, ImVec2(pos.x + size.x + 1, pos.y + titleBarHeight), IM_COL32(19, 19, 19, 255));
        //ImGui::GetCurrentWindow()->DrawList->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + titleBarHeight), ImGui::GetColorU32(ImGuiCol_TitleBg));

        //drawList->AddText(ImVec2(pos.x + 10.0f, pos.y + 3.0f), IM_COL32(255, 255, 255, 255), "File Explorer");

        float width = ImGui::GetContentRegionAvail().x;
        float height = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;
        float buttonWidth = 100.0f;
        float buttonHeight = height - ImGui::GetStyle().FramePadding.y;

        ImGui::SetCursorScreenPos(ImVec2(pos.x + 10, pos.y));
        if (ImGui::Button("Explorer", ImVec2(buttonWidth, buttonHeight))) {
            explorerOpen = true;
        }

        ImGui::SetCursorScreenPos(ImVec2(pos.x + 30 + buttonWidth, pos.y));
        if (ImGui::Button("Console", ImVec2(buttonWidth, buttonHeight))) {
            explorerOpen = false;
        }

        if (isDraggingConsoleExplorerWindow && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
        {
            isDraggingConsoleExplorerWindow = false;
        }

        if (ImGui::IsMouseHoveringRect(pos, ImVec2(pos.x + size.x, pos.y + 20)) && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
        {
            isDraggingConsoleExplorerWindow = true;
            consoleExplorerWindowOffset = ImGui::GetMousePos() - pos;
        }

        if (isDraggingConsoleExplorerWindow && ImGui::IsMouseDown(ImGuiMouseButton_Left))
        {
            pos = ImGui::GetMousePos() - consoleExplorerWindowOffset;
            ImGui::SetWindowPos(pos);
        }

        if (explorerOpen) // File Explorer
        {
            SDL_Texture* texture;
            float nextX = 10;
            float nextY = 30;
            if (fileExplorerPath.filename().string() != "Assets" || (fileExplorerPath.filename().string() == "Assets" && fileExplorerPath.parent_path().filename().string() != projectPath.filename().string())) // Checks to see if its not the main Asset folder
            {
                ImGui::PushID(fileExplorerPath.string().c_str()); // set unique ID based on the path string
                // Creates back button
                ImGui::SetCursorPosY(nextY);
                ImGui::SetCursorPosX(nextX);
                if (ImGui::ImageButton(
                    (ImTextureID)folderIconTexture,           // Texture ID (cast from an integer)
                    ImVec2(32, 32),                   // Size of the image button
                    ImVec2(0, 0),                     // UV coordinate for the top-left corner of the image
                    ImVec2(1, 1),                     // UV coordinate for the bottom-right corner of the image
                    -1,                               // Frame padding (default value)
                    ImVec4(0, 0, 0, 0),               // Background color (transparent)
                    ImVec4(1, 1, 1, 1)                // Tint color (default value)
                ))
                {
                    fileExplorerPath = fileExplorerPath.parent_path();
                    ImGui::PopID();
                    ImGui::End();
                    return;
                }
                ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetStyle().ItemSpacing.y); // Move cursor to the bottom of the image
                ImGui::SetCursorPosX(nextX + 5);
                ImGui::Text("Back");             // Text to display below the image
                nextX += 60;
                ImGui::PopID(); // reset the unique ID
            }

            for (const auto& entry : std::filesystem::directory_iterator(fileExplorerPath))
            {
                ImGui::PushID(entry.path().string().c_str()); // set unique ID based on the path string
                std::string fileName = entry.path().filename().string();
                //ConsoleLogger::InfoLog("Found " + fileName);
                if (fileName.length() > 6) {
                    fileName = fileName.substr(0, 6);
                    fileName = fileName + "..";
                }
                ImGui::SetCursorPosY(nextY);
                ImGui::SetCursorPosX(nextX);
                ImVec2 pos = ImGui::GetCursorPos();
                if (entry.is_directory())
                {
                    if (ImGui::ImageButton(
                        (ImTextureID)folderIconTexture,           // Texture ID (cast from an integer)
                        ImVec2(32, 32),                   // Size of the image button
                        ImVec2(0, 0),                     // UV coordinate for the top-left corner of the image
                        ImVec2(1, 1),                     // UV coordinate for the bottom-right corner of the image
                        -1,                               // Frame padding (default value)
                        ImVec4(0, 0, 0, 0),               // Background color (transparent)
                        ImVec4(1, 1, 1, 1)                // Tint color (default value)
                    ))
                    {
                        fileExplorerPath = entry.path();
                        ImGui::PopID();
                        ImGui::End();
                        return;
                    }
                    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetStyle().ItemSpacing.y); // Move cursor to the bottom of the image
                    ImGui::SetCursorPosX(nextX - 2);
                    ImGui::Text(fileName.c_str());             // Text to display below the image

                    nextX += 60;
                    if (nextX > 1900)
                    {
                        nextX = 10;
                        nextY = nextY + 75;
                    }
                }
                else if (entry.is_regular_file())
                {
                    if (entry.path().extension().string() == ".cpp")
                    {
                        if (ImGui::ImageButton(
                            (ImTextureID)cppIconTexture,           // Texture ID (cast from an integer)
                            ImVec2(32, 32),                   // Size of the image button
                            ImVec2(0, 0),                     // UV coordinate for the top-left corner of the image
                            ImVec2(1, 1),                     // UV coordinate for the bottom-right corner of the image
                            -1,                               // Frame padding (default value)
                            ImVec4(0, 0, 0, 0),               // Background color (transparent)
                            ImVec4(1, 1, 1, 1)                // Tint color (default value)
                        ))
                        {
                            std::string command = "code " + entry.path().string();
                            std::system(command.c_str());
                        }
                        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetStyle().ItemSpacing.y); // Move cursor to the bottom of the image
                        ImGui::SetCursorPosX(nextX - 2);
                        ImGui::Text(fileName.c_str());             // Text to display below the image

                        nextX += 60;
                        if (nextX > 1900)
                        {
                            nextX = 10;
                            nextY = nextY + 75;
                        }
                    }
                    else if (entry.path().extension().string() == ".png" || entry.path().extension().string() == ".jpg" || entry.path().extension().string() == ".webp")
                    {
                        //SDL_Surface* surface = IMG_Load((entry.path()).string().c_str());
                        //texture = SDL_CreateTextureFromSurface(renderer, surface);
                        //SDL_FreeSurface(surface);
                        SDL_Texture* texture = GetUserTexture(entry.path());

                        if (ImGui::ImageButton(
                            (ImTextureID)texture,
                            ImVec2(32, 32),
                            ImVec2(0, 0),
                            ImVec2(1, 1),
                            -1,
                            ImVec4(0, 0, 0, 0),
                            ImVec4(1, 1, 1, 1) // Todo: See what happens when I set this to 0,0,0,0
                        ))
                        { // Todo: Fix this
                            //ConsoleLogger::WarningLog("Clicked:" + PathToString(entry.path()));
                            //std::string command = "start " + PathToString(entry.path());
                            //std::system(command.c_str());
                        }

                        ImVec2 dropPos = ImGui::GetCursorScreenPos();
                        ImVec2 dropSize = ImVec2(100, 100);
                        if (ImGui::IsMouseHoveringRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax())) {
                            if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
                                draggingObject = true;
                                dragData.SetTexture(texture);
                                dragData.SetName(entry.path().stem().string());
                                dragData.SetPath(entry.path().generic_string());
                                objectInProperties = nullptr;
                                selectedObject = nullptr;
                            }
                            else if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                            {
                                ConsoleLogger::WarningLog("Clicked:" + PathToString(entry.path()));
                                std::string command = "start " + PathToString(entry.path());
                                std::system(command.c_str());
                            }
                        }
                        if (isDragging && draggingObject) {
                            // Set the next window position, position, and background alpha

                            int width, height;
                            SDL_QueryTexture(folderIconTexture, nullptr, nullptr, &width, &height);
                            //ConsoleLogger::InfoLog("Width" + std::to_string(width) + " Height" + std::to_string(height));

                            ImGui::SetNextWindowSize(ImVec2(width, height));
                            ImGui::SetNextWindowPos(dragPos);
                            ImGui::SetNextWindowBgAlpha(0.0f);

                            // Set the padding and margins for the button and the window
                            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
                            ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.5f, 0.5f));
                            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
                            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

                            // Begin the window and draw the button
                            ImGui::Begin("DragSpriteWindow", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse);
                            ImGui::Image((void*)dragData.GetTexture(), ImGui::GetWindowSize());
                            ImGui::End();
                            ImGui::PopStyleVar(4);
                        }

                        if (draggingObject && ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
                            draggingObject = false;
                            isDragging = false;
                            ConsoleLogger::InfoLog("Dropped on file explorer");
                        }
                        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetStyle().ItemSpacing.y); // Move cursor to the bottom of the image
                        ImGui::SetCursorPosX(nextX - 2);
                        ImGui::Text(fileName.c_str());             // Text to display below the image

                        nextX += 60;
                        if (nextX > 1900)
                        {
                            nextX = 10;
                            nextY = nextY + 75;
                        }
                    }
                    else if (entry.path().extension().string() == ".behaviour")
                    {
                        if (ImGui::ImageButton(
                            (ImTextureID)behaviourIconTexture,           // Texture ID (cast from an integer)
                            ImVec2(32, 32),                   // Size of the image button
                            ImVec2(0, 0),                     // UV coordinate for the top-left corner of the image
                            ImVec2(1, 1),                     // UV coordinate for the bottom-right corner of the image
                            -1,                               // Frame padding (default value)
                            ImVec4(0, 0, 0, 0),               // Background color (transparent)
                            ImVec4(1, 1, 1, 1)                // Tint color (default value)
                        ))
                        {
                            //eventSheetEditor.SetPath(entry.path());
                            //eventSheetEditor.eventSheetEditorOpen = true;
                            eventSheetEditor.OpenEventSheet(entry.path());
                        }
                        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetStyle().ItemSpacing.y); // Move cursor to the bottom of the image
                        ImGui::SetCursorPosX(nextX - 2);
                        ImGui::Text(fileName.c_str());             // Text to display below the image

                        nextX += 60;
                        if (nextX > 1900)
                        {
                            nextX = 10;
                            nextY = nextY + 75;
                        }
                    }
                }
                //ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetStyle().ItemSpacing.y); // Move cursor to the bottom of the image
                //ImGui::SetCursorPosX(nextX - 2);
                //ImGui::Text(fileName.c_str());             // Text to display below the image

                //nextX += 60;
                //if (nextX > 1900)
                //{
                //    nextX = 10;
                //    nextY = nextY + 75;
                //}
                ImGui::PopID(); // reset the unique ID
            }
            // Add a scrollable text box
            //ImGui::BeginChild("Scrolling Region", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar);
            //ImGui::Text("Scrollable Text");
            //ImGui::EndChild();

            if (explorerContextMenuOpen || (ImGui::IsWindowHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right)))
            {
                explorerContextMenuOpen = true;
                if (ImGui::BeginPopupContextWindow())
                {
                    if (ImGui::MenuItem("Create Behaviour"))
                    {
                        explorerContextMenuOpen = false;
                        eventSheetEditor.eventSheetCreatorOpen = true;
                        eventSheetEditor.SetPath(fileExplorerPath);
                    }

                    if (ImGui::MenuItem("Create Sprite"))
                    {
                        explorerContextMenuOpen = false;
                        spriteEditorWindow.spriteCreatorOpen = true;
                        spriteEditorWindow.SetPath(fileExplorerPath);
                    }

                    ImGui::Separator();

                    if (ImGui::MenuItem("Open Folder In File Explorer"))
                    {
                        explorerContextMenuOpen = false;
                        //ConsoleLogger::InfoLog("Opened Folder: " + PathToString(fileExplorerPath));
                        std::wstring widePath = fileExplorerPath.wstring();
                        ShellExecute(NULL, L"open", widePath.c_str(), NULL, NULL, SW_SHOWDEFAULT);
                    }

                    ImGui::EndPopup();
                }

            }
        }
        else // Console
        {
            for (const ConsoleLogger::ConsoleLog log : EditorWindow::consoleLogs) {
                switch (log.type) {
                case ConsoleLogger::ConsoleLogType::INFO:
                    ImGui::Text(log.message.c_str());
                    break;
                case ConsoleLogger::ConsoleLogType::WARNING:
                    ImGui::TextColored(ImVec4(1, 1, 0, 1), log.message.c_str());
                    break;
                case ConsoleLogger::ConsoleLogType::ERROR_:
                    ImGui::TextColored(ImVec4(1, 0, 0, 1), log.message.c_str());
                    break;
                }
                if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
                    ImGui::SetScrollHereY(1.0f);
            }
        }

        ImGui::End();
    }

    void ShowFileExplorer()
    {
        static bool show = true;
        SDL_Texture* texture;

        if (isFileExplorerFirstFrame)
        {
            isFileExplorerFirstFrame = false;
            ImGui::SetNextWindowSize(ImVec2(1920, 282));
            ImGui::SetNextWindowPos(ImVec2(0, 788));
        }
        ImGuiWindowFlags windowFlags = ImGuiTableFlags_NoSavedSettings;

        float nextX = 10;
        float nextY = 30;

        if (ImGui::Begin("File Explorer", nullptr, windowFlags))
        {
            if (fileExplorerPath.filename().string() != "Assets" || (fileExplorerPath.filename().string() == "Assets" && fileExplorerPath.parent_path().filename().string() != projectPath.filename().string())) // Checks to see if its not the main Asset folder
            {
                ImGui::PushID(fileExplorerPath.string().c_str()); // set unique ID based on the path string
                // Creates back button
                ImGui::SetCursorPosY(nextY);
                ImGui::SetCursorPosX(nextX);
                if (ImGui::ImageButton(
                    (ImTextureID)folderIconTexture,           // Texture ID (cast from an integer)
                    ImVec2(32, 32),                   // Size of the image button
                    ImVec2(0, 0),                     // UV coordinate for the top-left corner of the image
                    ImVec2(1, 1),                     // UV coordinate for the bottom-right corner of the image
                    -1,                               // Frame padding (default value)
                    ImVec4(0, 0, 0, 0),               // Background color (transparent)
                    ImVec4(1, 1, 1, 1)                // Tint color (default value)
                ))
                {
                    fileExplorerPath = fileExplorerPath.parent_path();
                    ImGui::PopID();
                    ImGui::End();
                    return;
                }
                ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetStyle().ItemSpacing.y); // Move cursor to the bottom of the image
                ImGui::SetCursorPosX(nextX + 5);
                ImGui::Text("Back");             // Text to display below the image
                nextX += 60;
                ImGui::PopID(); // reset the unique ID
            }

            for (const auto& entry : std::filesystem::directory_iterator(fileExplorerPath))
            {
                ImGui::PushID(entry.path().string().c_str()); // set unique ID based on the path string
                std::string fileName = entry.path().filename().string();
                //ConsoleLogger::InfoLog("Found " + fileName);
                if (fileName.length() > 7) {
                    fileName = fileName.substr(0, 8);
                    fileName = fileName + "..";
                }
                ImGui::SetCursorPosY(nextY);
                ImGui::SetCursorPosX(nextX);
                ImVec2 pos = ImGui::GetCursorPos();
                if (entry.is_directory())
                {
                    if (ImGui::ImageButton(
                        (ImTextureID)folderIconTexture,           // Texture ID (cast from an integer)
                        ImVec2(32, 32),                   // Size of the image button
                        ImVec2(0, 0),                     // UV coordinate for the top-left corner of the image
                        ImVec2(1, 1),                     // UV coordinate for the bottom-right corner of the image
                        -1,                               // Frame padding (default value)
                        ImVec4(0, 0, 0, 0),               // Background color (transparent)
                        ImVec4(1, 1, 1, 1)                // Tint color (default value)
                    ))
                    {
                        fileExplorerPath = entry.path();
                        ImGui::PopID();
                        ImGui::End();
                        return;
                    }
                }
                else if (entry.is_regular_file())
                {
                    if (entry.path().extension().string() == ".cpp")
                    {
                        if (ImGui::ImageButton(
                            (ImTextureID)cppIconTexture,           // Texture ID (cast from an integer)
                            ImVec2(32, 32),                   // Size of the image button
                            ImVec2(0, 0),                     // UV coordinate for the top-left corner of the image
                            ImVec2(1, 1),                     // UV coordinate for the bottom-right corner of the image
                            -1,                               // Frame padding (default value)
                            ImVec4(0, 0, 0, 0),               // Background color (transparent)
                            ImVec4(1, 1, 1, 1)                // Tint color (default value)
                        ))
                        {
                            std::string command = "code " + entry.path().string();
                            std::system(command.c_str());
                        }
                    }
                    else if (entry.path().extension().string() == ".png" || entry.path().extension().string() == ".jpg" || entry.path().extension().string() == ".webp")
                    {
                        //SDL_Surface* surface = IMG_Load((entry.path()).string().c_str());
                        //texture = SDL_CreateTextureFromSurface(renderer, surface);
                        //SDL_FreeSurface(surface);
                        SDL_Texture* texture = GetUserTexture(entry.path());

                        if (ImGui::ImageButton(
                            (ImTextureID)texture,
                            ImVec2(32, 32),
                            ImVec2(0, 0),
                            ImVec2(1, 1),
                            -1,
                            ImVec4(0, 0, 0, 0),
                            ImVec4(1, 1, 1, 1) // Todo: See what happens when I set this to 0,0,0,0
                        ))
                        {
                            ConsoleLogger::WarningLog("Clicked:" + PathToString(entry.path()));
                            std::string command = "start " + PathToString(entry.path());
                            std::system(command.c_str());
                        }

                        ImVec2 dropPos = ImGui::GetCursorScreenPos();
                        ImVec2 dropSize = ImVec2(100, 100);
                        if (ImGui::IsMouseHoveringRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax())) {
                            if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
                                draggingObject = true;
                                dragData.SetTexture(texture);
                                dragData.SetName(entry.path().stem().string());
                                dragData.SetPath(entry.path().string());
                                objectInProperties = nullptr;
                                selectedObject = nullptr;
                            }
                            else if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                            {
                                ConsoleLogger::WarningLog("Clicked:" + PathToString(entry.path()));
                                std::string command = "start " + PathToString(entry.path());
                                std::system(command.c_str());
                            }
                        }
                        if (isDragging && draggingObject) {
                            // Set the next window position, position, and background alpha

                            int width, height;
                            SDL_QueryTexture(folderIconTexture, nullptr, nullptr, &width, &height);
                            //ConsoleLogger::InfoLog("Width" + std::to_string(width) + " Height" + std::to_string(height));

                            ImGui::SetNextWindowSize(ImVec2(width, height));
                            ImGui::SetNextWindowPos(dragPos);
                            ImGui::SetNextWindowBgAlpha(0.0f);

                            // Set the padding and margins for the button and the window
                            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
                            ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.5f, 0.5f));
                            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
                            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

                            // Begin the window and draw the button
                            ImGui::Begin("DragSpriteWindow", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse);
                            ImGui::Image((void*)dragData.GetTexture(), ImGui::GetWindowSize());
                            ImGui::End();
                            ImGui::PopStyleVar(4);
                        }

                        if (draggingObject && ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
                            draggingObject = false;
                            isDragging = false;
                            ConsoleLogger::InfoLog("Dropped on file explorer");
                        }
                    }
                }
                ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetStyle().ItemSpacing.y); // Move cursor to the bottom of the image
                ImGui::SetCursorPosX(nextX - 2);
                ImGui::Text(fileName.c_str());             // Text to display below the image

                nextX += 60;

                if (nextX > 1900)
                {
                    nextX = 10;
                    nextY = nextY + 75;
                }
                ImGui::PopID(); // reset the unique ID
            }
            // Add a scrollable text box
            //ImGui::BeginChild("Scrolling Region", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar);
            //ImGui::Text("Scrollable Text");
            //ImGui::EndChild();

            if (explorerContextMenuOpen || (ImGui::IsWindowHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right)))
            {
                explorerContextMenuOpen = true;
                if (ImGui::BeginPopupContextWindow())
                {
                    if (ImGui::MenuItem("Create Behaviour"))
                    {
                        explorerContextMenuOpen = false;
                        eventSheetEditor.eventSheetCreatorOpen = true;
                        eventSheetEditor.SetPath(fileExplorerPath);
                    }
                    if (ImGui::MenuItem("Create Sprite"))
                    {
                        explorerContextMenuOpen = false;
                        spriteEditorWindow.spriteCreatorOpen = true;
                        spriteEditorWindow.SetPath(fileExplorerPath);
                    }

                    ImGui::Separator();

                    if (ImGui::MenuItem("Open Folder In File Explorer"))
                    {
                        explorerContextMenuOpen = false;
                        //ConsoleLogger::InfoLog("Opened Folder: " + PathToString(fileExplorerPath));
                        std::wstring widePath = fileExplorerPath.wstring();
                        ShellExecute(NULL, L"open", widePath.c_str(), NULL, NULL, SW_SHOWDEFAULT);
                    }

                    ImGui::EndPopup();
                }
            }
        }
        ImGui::End();
    }

    void KeyShortcuts()
    {
        if (ImGui::IsKeyReleased(ImGuiKey_Delete) && selectedObject != nullptr)
        {
            SceneManager::GetActiveScene()->RemoveGameObject(selectedObject);
            selectedObject = nullptr;
            objectInProperties = nullptr;
        }
        //else if (ImGui::IsKeyReleased(ImGuiKey_F2) && selectedObject != nullptr)
        //{
        //    SceneManager::GetActiveScene()->RemoveGameObject(selectedObject);
        //    selectedObject = nullptr;
        //    objectInProperties = nullptr;
        //}
    }

    void ShowHierarchy()
    {
        static bool show = true;

        if (isHierarchyFirstFrame)
        {
            ImGui::SetNextWindowSize(ImVec2(277, 736));
            ImGui::SetNextWindowPos(ImVec2(0, 52));
            isHierarchyFirstFrame = false;
        }
        ImGuiWindowFlags windowFlags = ImGuiTableFlags_NoSavedSettings;
        if (ImGui::Begin("Hierachy", nullptr, windowFlags))
        {
            for (int i = 0; i < SceneManager::GetActiveScene()->GetGameObjects().size(); i++) // Todo: I am doing this both in ShowProperties and ShowViewport, maybe merge them to use the same loop?
            {
                std::string objectName = SceneManager::GetActiveScene()->GetGameObjects()[i].GetName();
                if (objectName.length() > round(ImGui::GetWindowWidth() / 11)) {
                    objectName = objectName.substr(0, round(ImGui::GetWindowWidth() / 11));
                    objectName = objectName + "...";
                }
                if (ImGui::Button((objectName + "##" + std::to_string(SceneManager::GetActiveScene()->GetGameObjects()[i].GetId())).c_str(), ImVec2((ImGui::GetWindowWidth() - 15), 0)))
                {
                    objectInProperties = &SceneManager::GetActiveScene()->GetGameObjects()[i];
                    selectedObject = &SceneManager::GetActiveScene()->GetGameObjects()[i];
                }
                ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetStyle().ItemSpacing.y);
            }
            // Add a scrollable text box
            //ImGui::BeginChild("Scrolling Region", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar);
            //ImGui::Text("Scrollable Text");
            //ImGui::EndChild();
        }
        ImGui::End();
    }

    void ShowBehaviourWindow()
    {
        if (isFirstBehaviourFrame)
        {
            int width, height;
            SDL_GetWindowSize(window, &width, &height);
            ImGui::SetNextWindowSize(ImVec2(300, 400));
            ImGui::SetNextWindowPos(ImVec2((width - 300) / 2.0f, (height - 400) / 2.0f));
            isFirstBehaviourFrame = false;
        }
        if (ImGui::Begin("Add Behaviour", &behavioursWindowOpen, ImGuiWindowFlags_NoResize))
        {
            float buttonWidth = ImGui::GetWindowWidth() - 15;
            float textWidth = ImGui::CalcTextSize("Rigidbody").x;
            int numSpaces = (int)((buttonWidth - textWidth) / ImGui::CalcTextSize(" ").x / 2);
            numSpaces = std::max(numSpaces, 0);
            std::string physicsText = std::string(numSpaces, ' ') + "Rigidbody" + std::string(numSpaces, ' ');
            if (ImGui::Button(physicsText.c_str(), ImVec2(buttonWidth, 0)))
            {
                objectInProperties->AddBehaviour(new RigidbodyBehaviour());
                behavioursWindowOpen = false;
            }

            float textWidth2 = ImGui::CalcTextSize("Movement").x;
            int numSpaces2 = (int)((buttonWidth - textWidth2) / ImGui::CalcTextSize(" ").x / 2);
            numSpaces2 = std::max(numSpaces2, 0);
            std::string movementText = std::string(numSpaces2, ' ') + "Movement" + std::string(numSpaces2, ' ');
            if (ImGui::Button(movementText.c_str(), ImVec2(buttonWidth, 0)))
            {
                objectInProperties->AddBehaviour(new MoveBehaviour());
                behavioursWindowOpen = false;
                isFirstBehaviourFrame = true;
            }

            float textWidth3 = ImGui::CalcTextSize("Box Collider").x;
            int numSpaces3 = (int)((buttonWidth - textWidth3) / ImGui::CalcTextSize(" ").x / 2);
            numSpaces3 = std::max(numSpaces3, 0);
            std::string colliderText = std::string(numSpaces3, ' ') + "Box Collider" + std::string(numSpaces3, ' ');
            if (ImGui::Button(colliderText.c_str(), ImVec2(buttonWidth, 0)))
            {
                objectInProperties->AddBehaviour(new ColliderBehaviour());
                behavioursWindowOpen = false;
                isFirstBehaviourFrame = true;
            }

            float textWidth4 = ImGui::CalcTextSize("Paddle Movement").x;
            int numSpaces4 = (int)((buttonWidth - textWidth4) / ImGui::CalcTextSize(" ").x / 2);
            numSpaces4 = std::max(numSpaces4, 0);
            std::string paddleMoveText = std::string(numSpaces4, ' ') + "Paddle Movement" + std::string(numSpaces4, ' ');
            if (ImGui::Button(paddleMoveText.c_str(), ImVec2(buttonWidth, 0)))
            {
                objectInProperties->AddBehaviour(new PaddleMoveBehaviour());
                behavioursWindowOpen = false;
                isFirstBehaviourFrame = true;
            }

            float textWidth5 = ImGui::CalcTextSize("Enemy Paddle Movement").x;
            int numSpaces5 = (int)((buttonWidth - textWidth5) / ImGui::CalcTextSize(" ").x / 2);
            numSpaces5 = std::max(numSpaces5, 0);
            std::string paddleMoveText1 = std::string(numSpaces5, ' ') + "Enemy Paddle Movement" + std::string(numSpaces5, ' ');
            if (ImGui::Button(paddleMoveText1.c_str(), ImVec2(buttonWidth, 0)))
            {
                objectInProperties->AddBehaviour(new EnemyPaddleMoveBehaviour());
                behavioursWindowOpen = false;
                isFirstBehaviourFrame = true;
            }

            float textWidth6 = ImGui::CalcTextSize("Ball Movement").x;
            int numSpaces6 = (int)((buttonWidth - textWidth6) / ImGui::CalcTextSize(" ").x / 2);
            numSpaces6 = std::max(numSpaces6, 0);
            std::string paddleMoveText2 = std::string(numSpaces6, ' ') + "Ball Movement" + std::string(numSpaces6, ' ');
            if (ImGui::Button(paddleMoveText2.c_str(), ImVec2(buttonWidth, 0)))
            {
                objectInProperties->AddBehaviour(new BallMovementBehaviour());
                behavioursWindowOpen = false;
                isFirstBehaviourFrame = true;
            }

            float textWidth7 = ImGui::CalcTextSize("Ball Collider").x;
            int numSpaces7 = (int)((buttonWidth - textWidth7) / ImGui::CalcTextSize(" ").x / 2);
            numSpaces7 = std::max(numSpaces7, 0);
            std::string paddleMoveText3 = std::string(numSpaces7, ' ') + "Ball Collider" + std::string(numSpaces7, ' ');
            if (ImGui::Button(paddleMoveText3.c_str(), ImVec2(buttonWidth, 0)))
            {
                objectInProperties->AddBehaviour(new BallColliderBehaviour());
                behavioursWindowOpen = false;
                isFirstBehaviourFrame = true;
            }

            //std::unordered_map<std::string, CreateBehaviorFunc> behaviorFactoryMap;
            //behaviorFactoryMap.emplace("MyBehavior", &MyBehavior::Create);

            //// Loop through all subdirectories of the project path
            //for (const auto& entry : std::filesystem::recursive_directory_iterator(projectPath)) {
            //    if (entry.is_directory()) {
            //        // Loop through all files in the directory
            //        for (const auto& file : std::filesystem::directory_iterator(entry)) {
            //            // Check if the file is a C++ source file
            //            if (file.path().extension() == ".cpp") {
            //                // Load the source file and look for a behavior class
            //                std::ifstream sourceFile(file.path().string());
            //                std::string line;
            //                while (std::getline(sourceFile, line)) {
            //                    // Check if the line contains the string "class" and "Behavior"
            //                    if (line.find("class") != std::string::npos && line.find("Behavior") != std::string::npos) {
            //                        // Extract the class name
            //                        std::string className = line.substr(line.find("class") + 6, line.find(":") - line.find("class") - 7);
            //                        // Check if the class name is in the behavior factory map
            //                        if (behaviorFactoryMap.find(className) != behaviorFactoryMap.end()) {
            //                            // Create the button
            //                            ImGui::Button(className.c_str(), ImVec2(100, 20));
            //                            if (ImGui::IsItemClicked()) {
            //                                objectInProperties->AddBehavior(behaviorFactoryMap[className]());
            //                            }
            //                        }
            //                    }
            //                }
            //            }
            //        }
            //    }
            //}

        }
        ImGui::End();
    }

    void ShowProperties()
    {
        static bool show = true;

        if (isPropertiesFirstFrame)
        {
            ImGui::SetNextWindowSize(ImVec2(300, 736));
            ImGui::SetNextWindowPos(ImVec2(1643, 52));
            isPropertiesFirstFrame = false;
        }
        ImGuiWindowFlags windowFlags = ImGuiTableFlags_NoSavedSettings;
        if (ImGui::Begin("Properties", nullptr, windowFlags) && objectInProperties != nullptr && std::find_if(SceneManager::GetActiveScene()->GetGameObjects().begin(), SceneManager::GetActiveScene()->GetGameObjects().end(), [&](const auto& obj) { return &obj == objectInProperties; }) != SceneManager::GetActiveScene()->GetGameObjects().end())
        {
            ImGui::BeginGroup();
            // Name
            char nameBuffer[256] = {};
            strcpy_s(nameBuffer, objectInProperties->GetName().c_str());
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - 38);
            if (ImGui::InputText("##ObjectNameText", nameBuffer, sizeof(nameBuffer), ImGuiInputTextFlags_EnterReturnsTrue)) {
                objectInProperties->SetName(std::string(nameBuffer));
            }
            //ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetStyle().ItemSpacing.y); ------ USE THIS FOR THE THEM TO BE CLOSER TOGETHER
            // Position
            ImGui::NewLine();
            ImGui::Text("Position:   ");
            ImGui::SameLine();
            ImGui::Text("X");
            ImGui::SameLine();
            char xBuffer[256] = {};
            sprintf_s(xBuffer, sizeof(xBuffer), "%.4g", objectInProperties->GetPosition().x);
            ImGui::SetNextItemWidth(50);
            if (ImGui::InputText("##ObjectXPos", xBuffer, sizeof(xBuffer), ImGuiInputTextFlags_EnterReturnsTrue)) {
                objectInProperties->SetPosition(ImVec2(std::stof(xBuffer), objectInProperties->GetPosition().y));
            }
            ImGui::SameLine();
            ImGui::Text("Y");
            ImGui::SameLine();
            char yBuffer[256] = {};
            sprintf_s(yBuffer, sizeof(yBuffer), "%.4g", objectInProperties->GetPosition().y);
            ImGui::SetNextItemWidth(50);
            if (ImGui::InputText("##ObjectYPos", yBuffer, sizeof(yBuffer), ImGuiInputTextFlags_EnterReturnsTrue)) {
                objectInProperties->SetPosition(ImVec2(objectInProperties->GetPosition().x, std::stof(yBuffer)));
            }
            // Scale
            ImGui::NewLine();
            ImGui::Text("Scale:      ");
            ImGui::SameLine();
            ImGui::Text("X");
            ImGui::SameLine();
            char xBufferScale[256] = {};
            float newSizeX = objectInProperties->GetSize().x / objectInProperties->GetRealSize().x;
            sprintf_s(xBufferScale, sizeof(xBufferScale), "%.4g", newSizeX);
            ImGui::SetNextItemWidth(50);
            if (ImGui::InputText("##ObjectXScale", xBufferScale, sizeof(xBufferScale), ImGuiInputTextFlags_EnterReturnsTrue)) {
                objectInProperties->SetSize(ImVec2(std::stof(xBufferScale) * objectInProperties->GetRealSize().x, objectInProperties->GetSize().y));
            }
            ImGui::SameLine();
            ImGui::Text("Y");
            ImGui::SameLine();
            char yBufferScale[256] = {};
            float newSizeY = objectInProperties->GetSize().y / objectInProperties->GetRealSize().y;
            sprintf_s(yBufferScale, sizeof(yBufferScale), "%.4g", newSizeY);
            ImGui::SetNextItemWidth(50);
            if (ImGui::InputText("##ObjectYScale", yBufferScale, sizeof(yBufferScale), ImGuiInputTextFlags_EnterReturnsTrue)) {
                objectInProperties->SetSize(ImVec2(objectInProperties->GetSize().x, std::stof(yBufferScale) * objectInProperties->GetRealSize().y));
            }
            // Rotation
            ImGui::NewLine();
            ImGui::Text("Rotation:   ");
            ImGui::SameLine();
            ImGui::Text("X");
            ImGui::SameLine();
            char xBufferRotation[256] = {};
            sprintf_s(xBufferRotation, sizeof(xBufferRotation), "%.4g", objectInProperties->GetRotation().x);
            ImGui::SetNextItemWidth(50);
            if (ImGui::InputText("##ObjectXRotation", xBufferRotation, sizeof(xBufferRotation), ImGuiInputTextFlags_EnterReturnsTrue)) {
                objectInProperties->SetRotation(ImVec2(std::stof(xBufferRotation), objectInProperties->GetRotation().y));
            }
            ImGui::SameLine();
            ImGui::Text("Y");
            ImGui::SameLine();
            char yBufferRotation[256] = {};
            sprintf_s(yBufferRotation, sizeof(yBufferRotation), "%.4g", objectInProperties->GetRotation().y);
            ImGui::SetNextItemWidth(50);
            if (ImGui::InputText("##ObjectYRotation", yBufferRotation, sizeof(yBufferRotation), ImGuiInputTextFlags_EnterReturnsTrue)) {
                objectInProperties->SetRotation(ImVec2(objectInProperties->GetRotation().x, std::stof(yBufferRotation)));
            }
            // Order
            ImGui::NewLine();
            ImGui::Text("Order:      ");
            ImGui::SameLine();
            char orderBuffer[256] = {};
            sprintf_s(orderBuffer, sizeof(orderBuffer), "%d", objectInProperties->GetZOrder());
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - 15);
            ImGui::SetNextItemWidth(50);
            if (ImGui::InputText("##ObjectOrder", orderBuffer, sizeof(orderBuffer), ImGuiInputTextFlags_EnterReturnsTrue)) {
                objectInProperties->SetZOrder(std::stof(orderBuffer));
            }
            // Tint
            ImGui::NewLine();
            ImGui::Text("Colour:     ");
            ImGui::SameLine();
            ImGui::Text("R");
            ImGui::SameLine();
            char rBufferTint[256] = {};
            float tintValue = objectInProperties->GetTint().Value.x;
            sprintf_s(rBufferTint, sizeof(rBufferTint), "%.0f", tintValue * 255);
            ImGui::SetNextItemWidth(27);
            if (ImGui::InputText("##ObjectRTint", rBufferTint, sizeof(rBufferTint), ImGuiInputTextFlags_EnterReturnsTrue)) {
                objectInProperties->SetTint(ImColor(std::stof(rBufferTint) / 255, objectInProperties->GetTint().Value.y, objectInProperties->GetTint().Value.z, objectInProperties->GetTint().Value.w));
            }
            ImGui::SameLine();
            ImGui::Text("G");
            ImGui::SameLine();
            char gBufferTint[256] = {};
            sprintf_s(gBufferTint, sizeof(gBufferTint), "%.0f", objectInProperties->GetTint().Value.y * 255);
            ImGui::SetNextItemWidth(27);
            if (ImGui::InputText("##ObjectGTint", gBufferTint, sizeof(gBufferTint), ImGuiInputTextFlags_EnterReturnsTrue)) {
                objectInProperties->SetTint(ImColor(objectInProperties->GetTint().Value.x, std::stof(gBufferTint) / 255, objectInProperties->GetTint().Value.z, objectInProperties->GetTint().Value.w));
            }
            ImGui::SameLine();
            ImGui::Text("B");
            ImGui::SameLine();
            char bBufferTint[256] = {};
            sprintf_s(bBufferTint, sizeof(bBufferTint), "%.0f", objectInProperties->GetTint().Value.z * 255);
            ImGui::SetNextItemWidth(27);
            if (ImGui::InputText("##ObjectBTint", bBufferTint, sizeof(bBufferTint), ImGuiInputTextFlags_EnterReturnsTrue)) {
                objectInProperties->SetTint(ImColor(objectInProperties->GetTint().Value.x, objectInProperties->GetTint().Value.y, std::stof(bBufferTint) / 255, objectInProperties->GetTint().Value.w));
            }
            ImGui::SameLine();
            ImGui::Text("A");
            ImGui::SameLine();
            char aBufferTint[256] = {};
            sprintf_s(aBufferTint, sizeof(aBufferTint), "%.0f", objectInProperties->GetTint().Value.w * 255);
            ImGui::SetNextItemWidth(27);
            if (ImGui::InputText("##ObjectATint", aBufferTint, sizeof(aBufferTint), ImGuiInputTextFlags_EnterReturnsTrue)) {
                objectInProperties->SetTint(ImColor(objectInProperties->GetTint().Value.x, objectInProperties->GetTint().Value.y, objectInProperties->GetTint().Value.z, std::stof(aBufferTint) / 255));
            }
            //// Border
            //ImGui::NewLine();
            //ImGui::Text("Border:     ");
            //ImGui::SameLine();
            //ImGui::Text("R");
            //ImGui::SameLine();
            //char rBufferBorder[256] = {};
            //sprintf_s(rBufferBorder, sizeof(rBufferBorder), "%.0f", objectInProperties->GetBorder().Value.x * 255);
            //ImGui::SetNextItemWidth(27);
            //if (ImGui::InputText("##ObjectRBorder", rBufferBorder, sizeof(rBufferBorder), ImGuiInputTextFlags_EnterReturnsTrue)) {
            //    objectInProperties->border.Value.x = std::stof(rBufferBorder) / 255;
            //}
            //ImGui::SameLine();
            //ImGui::Text("G");
            //ImGui::SameLine();
            //char gBufferBorder[256] = {};
            //sprintf_s(gBufferBorder, sizeof(gBufferBorder), "%.0f", objectInProperties->GetBorder().Value.y * 255);
            //ImGui::SetNextItemWidth(27);
            //if (ImGui::InputText("##ObjectGBorder", gBufferBorder, sizeof(gBufferBorder), ImGuiInputTextFlags_EnterReturnsTrue)) {
            //    objectInProperties->border.Value.y = std::stof(gBufferBorder) / 255;
            //}
            //ImGui::SameLine();
            //ImGui::Text("B");
            //ImGui::SameLine();
            //char bBufferBorder[256] = {};
            //sprintf_s(bBufferBorder, sizeof(bBufferBorder), "%.0f", objectInProperties->GetBorder().Value.z * 255);
            //ImGui::SetNextItemWidth(27);
            //if (ImGui::InputText("##ObjectBBorder", bBufferBorder, sizeof(bBufferBorder), ImGuiInputTextFlags_EnterReturnsTrue)) {
            //    objectInProperties->border.Value.z = std::stof(bBufferBorder) / 255;
            //}
            //ImGui::SameLine();
            //ImGui::Text("A");
            //ImGui::SameLine();
            //char aBufferBorder[256] = {};
            //sprintf_s(aBufferBorder, sizeof(aBufferBorder), "%.0f", objectInProperties->GetBorder().Value.w * 255);
            //ImGui::SetNextItemWidth(27);
            //if (ImGui::InputText("##ObjectABorder", aBufferBorder, sizeof(aBufferBorder), ImGuiInputTextFlags_EnterReturnsTrue)) {
            //    objectInProperties->border.Value.w = std::stof(aBufferBorder) / 255;
            //}
            // Behaviours
            int behaviourNum = 0;
            for (Behaviour* behaviour : objectInProperties->GetBehaviours())
            {
                behaviourNum++;
                float buttonWidth = ImGui::GetWindowWidth() - 15;
                float textWidth = ImGui::CalcTextSize(behaviour->GetName().c_str()).x;
                int numSpaces = (int)((buttonWidth - textWidth) / ImGui::CalcTextSize(" ").x / 2);
                std::string centeredText = std::string(numSpaces, ' ') + behaviour->GetName().c_str() + std::string(numSpaces, ' ');
                ImGui::Separator();
                //ImGui::NewLine();
                //ImGui::CollapseButton();
                ImGui::Text(centeredText.c_str());
                ImGui::SameLine();

                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0)); // Set button color to transparent
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0)); // Set button hover color to transparent
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0)); // Set button active color to transparent
                ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 40);
                if (ImGui::Button(("X##" + std::to_string(behaviourNum)).c_str()))
                {
                    objectInProperties->RemoveBehaviour(behaviour);
                }
                ImGui::PopStyleColor(3);
                // Configeration properties here
            }
            // Behaviour button
            ImGui::NewLine();
            // Calculate the button width
            float buttonWidth = ImGui::GetWindowWidth() - 15;
            // Calculate the width of the button text
            float textWidth = ImGui::CalcTextSize("Add Behaviour").x;
            // Calculate the number of spaces needed to center the text
            int numSpaces = (int)((buttonWidth - textWidth) / ImGui::CalcTextSize(" ").x / 2);
            // Create the centered button text
            std::string centeredText = std::string(numSpaces, ' ') + "Add Behaviour " + std::string(numSpaces, ' ');

            // Create the button with the centered text
            if (ImGui::Button(centeredText.c_str(), ImVec2(buttonWidth, 0))) {
                behavioursWindowOpen = true;
            }
            ImGui::EndGroup();
        }
        ImGui::End();
    }

    void ShowViewport()
    {
        static bool show = true;

        if (isViewportFirstFrame)
        {
            ImGui::SetNextWindowSize(ImVec2(1366, 736));
            ImGui::SetNextWindowPos(ImVec2(277, 52));
            isViewportFirstFrame = false;
        }
        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoScrollbar | ImGuiTableFlags_NoSavedSettings;
        if (ImGui::Begin("Viewport", nullptr, windowFlags))
        {
            if (draggingObject && ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
                ImVec2 windowPos = ImGui::GetWindowPos();
                ImVec2 windowSize = ImGui::GetWindowSize();
                if (ImGui::IsMouseHoveringRect(windowPos, ImVec2(windowPos.x + windowSize.x, windowPos.y + windowSize.y)))
                {
                    //ConsoleLogger::InfoLog("Dropped on Viewport");
                    int width, height;
                    SDL_QueryTexture(dragData.GetTexture(), nullptr, nullptr, &width, &height);

                    std::random_device rd;
                    std::mt19937 gen(rd());
                    std::uniform_int_distribution<> dis(10000000, 1000000000);

                    GameObject object = GameObject(dis(gen)); // TODO: BEFORE ASIGNING ID, CHECK IF IT IS ACTUALLY UNIQUE ---------------------------------------------------------
                    object.SetName(dragData.GetName());
                    object.SetPosition(ImVec2(ImGui::GetMousePos()));
                    object.SetSize(ImVec2(width, height));
                    object.SetRealSize(ImVec2(width, height));
                    object.SetRotation(ImVec2(0, 0));
                    object.SetTexture(dragData.GetTexture());
                    object.SetPath(dragData.GetPath());
                    //ConsoleLogger::InfoLog("Active scene: " + SceneManager::GetActiveScene()->GetPath().string());
                    SceneManager::GetActiveScene()->GetGameObjects().push_back(object);
                    //for (int i = 0; i < SceneManager::GetActiveScene()->GetGameObjects().size(); i++)
                    //{
                    //    ConsoleLogger::InfoLog("Gameobject: " + SceneManager::GetActiveScene()->GetGameObjects()[i].GetName());
                    //}
                    //if (SceneManager::GetActiveScene()->GetGameObjects().empty())
                    //{
                    //    ConsoleLogger::InfoLog("EMPTY");
                    //}
                    //ConsoleLogger::InfoLog("Just added game object: " + object.GetName());
                    objectInProperties = &SceneManager::GetActiveScene()->GetGameObjects().back();
                    selectedObject = &SceneManager::GetActiveScene()->GetGameObjects().back();
                }
                draggingObject = false;
                isDragging = false;
            }

            DrawBackground();

            // sort gameObjects by zIndex
            if (!SceneManager::GetActiveScene()->GetGameObjects().empty())
            {
                std::sort(SceneManager::GetActiveScene()->GetGameObjects().begin(), SceneManager::GetActiveScene()->GetGameObjects().end(), [](const GameObject& a, const GameObject& b) {
                    return a.GetZOrder() < b.GetZOrder();
                    });

                for (int i = 0; i < SceneManager::GetActiveScene()->GetGameObjects().size(); i++)
                {
                    DrawToViewport(SceneManager::GetActiveScene()->GetGameObjects()[i]);
                }
            }

            DrawPositionArrows(*selectedObject);
        }
        ImGui::End();
    }

    void RenderTopbar()
    {
        int currentWidth, currentHeight;
        SDL_GetWindowSize(window, &currentWidth, &currentHeight);
        ImGui::SetNextWindowSize(ImVec2(currentWidth, 25));
        ImGui::SetNextWindowPos(ImVec2(0, 20));
        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoScrollbar | ImGuiTableFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize;
        if (ImGui::Begin("Top Bar", nullptr, windowFlags))
        {
            // Play Button
            ImTextureID playIconTexture;
            if (playing) playIconTexture = activePlayIcon;
            else playIconTexture = playIcon;

            ImGui::SetCursorPos(ImVec2(currentWidth/2-20, 2));
            if (ImGui::ImageButton(
                (ImTextureID)playIconTexture,           // Texture ID (cast from an integer)
                ImVec2(20, 20),                   // Size of the image button
                ImVec2(0, 0),                     // UV coordinate for the top-left corner of the image
                ImVec2(1, 1),                     // UV coordinate for the bottom-right corner of the image
                -1,                               // Frame padding (default value)
                ImVec4(0, 0, 0, 0),               // Background color (transparent)
                ImVec4(1, 1, 1, 1)                // Tint color (default value)
            ))
            {
                startStopPlaying = true;
                paused = false;
            }

            // Pause Button
            ImTextureID pauseIconTexture;
            if (paused) pauseIconTexture = activePauseIcon;
            else if (playing) pauseIconTexture = pauseIcon;
            else pauseIconTexture = grayedPauseIcon;

            ImGui::SetCursorPos(ImVec2(currentWidth/2+20, 2));
            if (ImGui::ImageButton(
                (ImTextureID)pauseIconTexture,           // Texture ID (cast from an integer)
                ImVec2(20, 20),                   // Size of the image button
                ImVec2(0, 0),                     // UV coordinate for the top-left corner of the image
                ImVec2(1, 1),                     // UV coordinate for the bottom-right corner of the image
                -1,                               // Frame padding (default value)
                ImVec4(0, 0, 0, 0),               // Background color (transparent)
                ImVec4(1, 1, 1, 1)                // Tint color (default value)
            ))
            {
                if (playing) paused = !paused;
            }
        }
        ImGui::End();
    }

    void RunBehaviours(GameObject* object, bool startFunction = false)
    {
        if (!playing || paused) return;
        //ConsoleLogger::InfoLog("Object: " + object->GetName());
        static auto lastTime = std::chrono::high_resolution_clock::now();
        auto currentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> delta = currentTime - lastTime;
        lastTime = currentTime;
        for (Behaviour* behaviour : object->GetBehaviours())
        {
            if (behaviour == nullptr)
            {
                ConsoleLogger::ErrorLog("A behaviour on the game object '" + object->GetName() + "' is invalid. If you removed it, please restart the play mode.");
                continue;
            }
            if (startFunction)
            {
                behaviour->SetGameObject(object);
                behaviour->Start();
            }
            else behaviour->Update(delta.count());
        }
    }

    void RenderGame()
    {
        //ImGui::SetCurrentContext(gameRendererContext);
        //ImGui::NewFrame();
        //ImGui::SetNextWindowPos(ImVec2(0, 0));
        //int width, height;
        //SDL_GetWindowSize(gameWindow, &width, &height);
        //ImGui::SetNextWindowSize(ImVec2(width, height));
        //ImGui::Begin("Game GUI Window", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBackground);
        // Clear the screen
        SDL_SetRenderDrawColor(gameRenderer, 0, 0, 0, 255);
        SDL_RenderClear(gameRenderer);

        // Background
        int currentWidth, currentHeight;
        SDL_GetWindowSize(window, &currentWidth, &currentHeight);
        SDL_Rect srcRect = { 0, 0, currentWidth, currentHeight };
        SDL_Rect dstrect = { 0, 0, currentWidth, currentHeight };
        SDL_RenderCopy(gameRenderer, blueGameBackgroundTexture, &srcRect, &dstrect);

        // Sort gameObjects by zIndex
        std::sort(SceneManager::GetActiveScene()->GetGameObjects().begin(), SceneManager::GetActiveScene()->GetGameObjects().end(), [](const GameObject& a, const GameObject& b) {
            return a.GetZOrder() < b.GetZOrder();
            });

        for (GameObject& object : SceneManager::GetActiveScene()->GetGameObjects())
        {
            RunBehaviours(&object);
            SDL_Rect srcRect = { 0, 0, object.GetRealSize().x, object.GetRealSize().y };
            SDL_Rect dstrect = { object.GetPosition().x,object.GetPosition().y, object.GetSize().x, object.GetSize().y };
            SDL_RenderCopy(gameRenderer, object.GetGameTexture(), &srcRect, &dstrect);

            // Debug information
            //SDL_SetRenderDrawColor(gameRenderer, 255, 0, 0, 255);
            //SDL_RenderDrawRect(gameRenderer, &dstrect);

            //std::string debugInfo = "Object " + object.GetName() + ": Pos=(" + std::to_string(object.GetPosition().x) + "," + std::to_string(object.GetPosition().y) + ") Size=(" + std::to_string(object.GetSize().x) + "," + std::to_string(object.GetSize().y) + ")";
        }

        // Present the renderer
        SDL_RenderPresent(gameRenderer);
        //ImGui::End();
        //ImGui::SetCurrentContext(rendererContext);
        //ImGui::NewFrame();
    }


    void StartStopGame()
    {
        if (ImGui::IsKeyReleased(ImGuiKey_P) && ImGui::IsKeyReleased(ImGuiKey_LeftCtrl) || startStopPlaying)
        {
            startStopPlaying = false;
            playing = !playing;

            if (playing)
            { 
                // TODO: REPLACE gameObjects AND MAKE THE EDITOR USE SCENES

                //Scene scene = Scene(projectPath / "Assets" / "Scenes" / "Default.json", gameObjects);
                //SceneManager::AddScene(scene);
                //SceneManager::SetActiveScene(&SceneManager::GetScenes()->back());

                //std::copy(gameObjects.begin(), gameObjects.end(), scene.m_GameObjects.begin());

                ConsoleLogger::InfoLog("Started Play Mode");

                CompileScripts(projectPath, projectPath);
                monoAssembly = mono_domain_assembly_open(monoDomain, (projectPath.string() + "/GameScripts.dll").c_str());

                EditorWindow::editorWindow->ExecuteMonoFunction("GameManager", "Start");

                for (Scene& scene : *SceneManager::GetScenes()) // Todo: In settings list scenes to be included in play mode. Then only backup those scenes.
                {
                    SceneManager::BackupScene(&scene);
                }

                for (GameObject& object : SceneManager::GetActiveScene()->GetGameObjects())
                {
                    //int texW = 0;
                    //int texH = 0;
                    //SDL_QueryTexture(object.GetTexture(), NULL, NULL, &texW, &texH);
                    //SDL_Texture* texture = SDL_CreateTexture(gameRenderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, texW, texH);

                    //// ensure texture dimensions match renderer dimensions
                    //int rendererWidth, rendererHeight;
                    //SDL_GetRendererOutputSize(gameRenderer, &rendererWidth, &rendererHeight);
                    //SDL_Rect dstRect = { 0, 0, rendererWidth, rendererHeight };

                    //// create surface from renderer texture
                    //SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormat(0, texW, texH, 32, SDL_PIXELFORMAT_ARGB8888);
                    //SDL_RenderReadPixels(renderer, NULL, surface->format->format, surface->pixels, surface->pitch);

                    //// copy surface to texture and free the surface
                    //SDL_UpdateTexture(texture, NULL, surface->pixels, surface->pitch);
                    //SDL_FreeSurface(surface);

                    //for (auto& behaviour : object.GetBehaviours())
                    //{
                    //    behaviour->SetGameObject(&object);
                    //}

                    SDL_Texture* texture = nullptr;
                    SDL_Surface* surface = IMG_Load(object.GetPath().c_str());
                    if (surface == nullptr)
                    {
                        std::string warning = "Error loading surface for texture: ";
                        warning.append(IMG_GetError());
                        ConsoleLogger::ErrorLog(warning);
                        continue;
                    }
                    if (surface->w == 0 || surface->h == 0)
                    {
                        SDL_FreeSurface(surface);
                        ConsoleLogger::ErrorLog("A game object with the name " + object.GetName() + " has 0 width or height so it was not loaded in.");
                        continue;
                    }
                    texture = SDL_CreateTextureFromSurface(gameRenderer, surface);
                    SDL_FreeSurface(surface);

                    object.SetGameTexture(texture);

                    RunBehaviours(&object, true);
                }
                SDL_SetWindowSize(gameWindow, 1366, 768);
                SDL_ShowWindow(gameWindow);
            }
            else
            {
                ConsoleLogger::InfoLog("Stopped Play Mode");

                SDL_HideWindow(gameWindow);

                for (auto& object : SceneManager::GetActiveScene()->GetGameObjects()) {
                    SDL_DestroyTexture(object.GetGameTexture());
                }

                for (Scene& scene : *SceneManager::GetScenes())
                {
                    SceneManager::ResetScene(&scene);
                    scene.gameObjectsBackup.clear();
                }
            }
        }
    }

    void EditorWindow::ExecuteMonoFunction(std::string className, std::string methodName) // Todo: Move this to a new class
    {
        if (!monoAssembly) return;

        MonoImage* monoImage = mono_assembly_get_image(monoAssembly);
        if (!monoImage) return;

        MonoClass* monoClass = mono_class_from_name(monoImage, "PhantomEngine", className.c_str());
        if (!monoClass) return;

        MonoObject* monoObject = mono_object_new(monoDomain, monoClass);
        mono_runtime_object_init(monoObject);
        if (!monoObject) return;

        MonoMethod* monoMethod = mono_class_get_method_from_name(monoClass, methodName.c_str(), 0);
        if (!monoMethod) return;

        MonoObject* exception = nullptr;
        mono_runtime_invoke(monoMethod, monoObject, nullptr, &exception);

        if (exception)
        {
            MonoString* errorString = mono_object_to_string(exception, nullptr);
            const char* errorMessage = mono_string_to_utf8(errorString);
            ConsoleLogger::ErrorLog(errorMessage);
        }
    }

    bool EditorWindow::IsPlaying()
    {
        return playing;
    }

    SDL_Renderer* EditorWindow::GameRenderer()
    {
        return gameRenderer;
    }

    SDL_Renderer* EditorWindow::GetRenderer()
    {
        return renderer;
    }

    void EditorWindow::Render()
    {
        // Start a new ImGUI frame
        ImGui_ImplSDL2_NewFrame(window);
        ImGui_ImplSDLRenderer_NewFrame();
        ImGui::NewFrame();

        // Draw some ImGUI widgets

        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
        ImGui::SetNextWindowBgAlpha(0.0f);
        ImGui::Begin("EditorWindow", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoBringToFrontOnFocus);

        // Add a menu bar
        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("Open Project", "Ctrl+O")) {}
                if (ImGui::MenuItem("Save Project", "Ctrl+S"))
                {
                    SceneManager::SaveScene(SceneManager::GetActiveScene());
                }
                if (ImGui::MenuItem("Build Project", "")) {}
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Edit")) {
                if (ImGui::MenuItem("Project Settings", "")) {}
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Window")) {
                if (ImGui::MenuItem("Hierarchy", "")) {}
                if (ImGui::MenuItem("File Explorer", "")) {}
                if (ImGui::MenuItem("Properties", "")) {}
                if (ImGui::MenuItem("Sprite Editor", "")) {}
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Help")) {
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        ImGui::End();

        //LoadUserImages();

        KeyShortcuts();
        ShowViewport();
        //ShowFileExplorer();
        ShowHierarchy();
        ShowProperties();
        ShowExplorerConsole();
        RenderTopbar();
        if (behavioursWindowOpen) ShowBehaviourWindow();
        eventSheetEditor.DrawEventSheetCreator();
        eventSheetEditor.DrawEventSheetEditor();
        eventSheetEditor.DrawEventSheetEventList();
        spriteEditorWindow.DrawSpriteEditorCreation();
        spriteEditorWindow.DrawSpriteEditor();
        behaviourNodeEditor.DrawBehaviourNodeEditor();
        StartStopGame();
        if (playing) RenderGame();

        //UnloadUserImages();

        // Render ImGUI
        ImGui::Render();
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);  // -------------------- Dont think I need this
        ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
        SDL_RenderPresent(renderer);
    }

    void EditorWindow::Init(std::filesystem::path path) {
        editorWindow = this;

        projectPath = PathToString(path);
        fileExplorerPath = projectPath / "Assets";

        if (renderer == nullptr) InitSDL();
        InitImages();

        spriteEditorWindow = SpriteEditorWindow();
        spriteEditorWindow.SpriteEditorSetup(window);

        eventSheetEditor = EventSheetEditor();
        eventSheetEditor.EventSheetEditorSetup(window);

        behaviourNodeEditor = BehaviourNodeEditor();
        behaviourNodeEditor.BehaviourNodeEditorSetup(window);

        monoDomain = mono_jit_init("PhantomEngine");

        //CompileScripts(projectPath, projectPath);

        // Main loop
        while (!quitEditor) {
            ProcessEvents();
            ProcessGameEvents();
            Render();
        }

        CleanupSDL();
    }
}