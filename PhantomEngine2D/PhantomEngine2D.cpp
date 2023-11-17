#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_render.h>
#include <SDL_syswm.h>
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer.h"
#include <ImGuiFileDialog/ImGuiFileDialog.h>
#include "EngineFonts.h"
#include <cstdio>
#include <string>
#include <iostream>
#include <imgui_internal.h>
#include "ProjectManager.h"
#include "ConsoleLogger.h"
#include "PhantomEngine2D.h"
#include <windows.h>
#include <shobjidl.h> 

// Rename to LuminousEngine

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
EngineFonts defaultFont;
ImFont* defaultFont28;
ImFont* defaultFont34;
bool mainWindowOpen = true;
char projectName[256] = "";
bool creatingProject = false;

void InitFonts()
{
    ImGuiIO& io = ImGui::GetIO();

    // Load a font file
    io.Fonts->AddFontFromFileTTF("Fonts/LiberationSans-Bold.ttf", 16.0f);

    // Set the font as the default
    io.FontDefault = io.Fonts->Fonts.back();
    defaultFont = LiberationSansBoldFont;

    // Create fonts
    defaultFont28 = GetEngineFont(LiberationSansBoldFont, 24);
    defaultFont34 = GetEngineFont(LiberationSansBoldFont, 34);
}


void InitSDL()
{
    // Initialize SDL2
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        exit(1);
    }

    // Create SDL2 window
    window = SDL_CreateWindow("Phantom Engine v0.1",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        450, 250,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    SDL_SetWindowResizable(window, SDL_FALSE);
    //int w = 500;
    //int h = 250;
    //SDL_GetWindowMinimumSize(window, &w, &h);
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

    // Initialize ImGui for SDL2 + SDL_Renderer
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    InitFonts();
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer_Init(renderer);

    // Set up ImGUI style
    ImGui::StyleColorsDark();
}

std::string WINAPI ShowOpenProjectDialog(HINSTANCE hInstance, HWND hWnd)
{
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    std::string path;
    if (SUCCEEDED(hr))
    {
        IFileOpenDialog* pFileOpen;

        // Create the FileOpenDialog object.
        hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
            IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

        if (SUCCEEDED(hr))
        {
            // Set options to select folders only
            FILEOPENDIALOGOPTIONS options;
            pFileOpen->GetOptions(&options);
            pFileOpen->SetOptions(options | FOS_PICKFOLDERS);

            // Show the Open dialog box.
            hr = pFileOpen->Show(NULL);

            // Get the folder path from the dialog box.
            if (SUCCEEDED(hr))
            {
                IShellItem* pItem;
                hr = pFileOpen->GetResult(&pItem);
                if (SUCCEEDED(hr))
                {
                    PWSTR pszFilePath;
                    hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

                    // Display the folder path to the user.
                    if (SUCCEEDED(hr))
                    {
                        int len = WideCharToMultiByte(CP_UTF8, 0, pszFilePath, -1, NULL, 0, NULL, NULL);
                        std::string str(len, '\0');
                        WideCharToMultiByte(CP_UTF8, 0, pszFilePath, -1, &str[0], len, NULL, NULL);
                        path = str;

                        CoTaskMemFree(pszFilePath);
                    }
                    pItem->Release();
                }
            }
            pFileOpen->Release();
        }
        CoUninitialize();
    }
    return path;
}


void CleanupSDL()
{
    ImGui_ImplSDLRenderer_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void ProcessEvents()
{
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        ImGui_ImplSDL2_ProcessEvent(&event);
        if (event.type == SDL_QUIT) {
            exit(0);
        }
    }
}

void Render()
{
    // Start a new ImGUI frame
    ImGui_ImplSDL2_NewFrame(window);
    ImGui_ImplSDLRenderer_NewFrame();
    ImGui::NewFrame();

    // Draw some ImGUI widgets

    if (mainWindowOpen)
    {
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
        ImGui::SetNextWindowBgAlpha(0.0f);
        ImGui::Begin("CreateOpenProjectWindow", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse);

        ImGui::PushFont(defaultFont34);
        ImGui::SetCursorPos(ImVec2(50, 20));
        ImGui::Text("Create Or Open A Project!");
        ImGui::SetCursorPos(ImVec2(50, 100));
        ImGui::PopFont();
        ImGui::PushFont(defaultFont28);
        if (ImGui::Button("Create", ImVec2(150, 75))) {
            mainWindowOpen = false;
        }
        //ImGui::SameLine();
        ImGui::SetCursorPos(ImVec2(250, 100));
        if (ImGui::Button("Open", ImVec2(150, 75))) {
            SDL_SysWMinfo info;
            SDL_VERSION(&info.version);
            if (SDL_GetWindowWMInfo(window, &info)) {
                HWND hwnd = info.info.win.window;
                HINSTANCE hInstance = GetModuleHandle(NULL);
                std::string pathString = ShowOpenProjectDialog(hInstance, hwnd);
                std::filesystem::path path(pathString);
                ProjectManager::OpenProject(path);
            }
        }
        ImGui::PopFont();
        ImGui::End();
    }
    else
    {
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
        ImGui::SetNextWindowBgAlpha(0.0f);
        ImGui::Begin("CreateOpenProjectWindow", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse);

        ImGui::PushFont(defaultFont28);

        ImGui::SetCursorPos(ImVec2(120, 35));
        ImGui::Text("Enter Project Name");

        ImGui::PopFont();

        ImGui::PushFont(defaultFont28);
        //char buffer[256] = ""; // buffer to hold input text
        ImGui::SetNextWindowContentSize(ImVec2(300, 100));
        ImGui::SetCursorPos(ImVec2(65, 75));
        //ImGui::InputText("##ProjectNameLabel", buf, sizeof(buf), ImGuiInputTextFlags_EnterReturnsTrue); // create an input text box
        ImGui::InputTextWithHint("##ProjectNameLabel", "Project Name...", projectName, sizeof(projectName), ImGuiInputTextFlags_EnterReturnsTrue); // create an input text box

        ImGui::PopFont();
        ImGui::PushFont(defaultFont34);

        ImGui::SetCursorPos(ImVec2(250, 125));
        std::string name = projectName;
        if (ImGui::Button("Create", ImVec2(150, 75)) && !creatingProject) {
            // remove spaces
            name.erase(std::remove(name.begin(), name.end(), ' '), name.end());

            // remove special characters
            name.erase(std::remove_if(name.begin(), name.end(), [](char c) {
                return !std::isalnum(c);
                }), name.end());
            if (name != "")
            {
                creatingProject = true;
                if (!ProjectManager::CreateProject(name).empty())
                {
                    creatingProject = false;
                    if (ProjectManager::OpenProject(name))
                    {
                        SDL_DestroyWindow(window);
                    }
                }
                else creatingProject = false;
            }
        }
        ImGui::SetCursorPos(ImVec2(50, 125));
        if (ImGui::Button("Close", ImVec2(150, 75))) {
            mainWindowOpen = true;
            memset(projectName, 0, 256);
        }

        ImGui::PopFont();
        ImGui::End();
    }

    // Render ImGUI
    ImGui::Render();
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
    SDL_RenderPresent(renderer);
}

int main(int argc, char* argv[])
{
    InitSDL();

    // Main loop
    while (true) {
        ProcessEvents();
        Render();
    }

    CleanupSDL();
    return 0;
}