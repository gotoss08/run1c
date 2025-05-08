#define IMGUI_USER_CONFIG "my_imgui_config.h"
#include "imgui.h"
#include "imgui_stdlib.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"
#include "misc/freetype/imgui_freetype.h"

#include <SDL.h>
#include <SDL_opengl.h>

#include <iostream>
#include <vector>
#include <algorithm>

#include "utils.h"

class RUN1C {
public:
    RUN1C() {
        starterPath = getEnvironmentVariable("PROGRAMFILES") + "\\1cv8\\common\\1cestart.exe";
    };
    RUN1C(std::string starterPath) : starterPath(starterPath) {};
    bool run(std::string input, bool isConfigMode = false);
private:
    std::string starterPath;
};

bool RUN1C::run(std::string input, bool isConfigMode) {

    std::vector<std::string> args;

    if (isConfigMode) {
        args.push_back("CONFIG");
    } else {
        args.push_back("ENTERPRISE");
    }

    std::cout << "running regex on " << input << std::endl;

    std::regex filepathRegex("\\w:.+?((?=\"$)|(?=\";)|($))", std::regex_constants::ECMAScript);
    std::smatch m;

    if (std::regex_search(input, m, filepathRegex)) {

        std::string path = m[0].str();
        std::cout << "extracted path: " << path << std::endl;
        
        args.push_back("/F");
        args.push_back("\"" + path + "\"");

    } else {
        return false;
    }

    launchProcess(starterPath, args);

    return true;

}

float getScreenDPI(SDL_Window* window) {
    float dpi = -1.0f;
    int winIdx = SDL_GetWindowDisplayIndex(window);
    if (winIdx >= 0) {
        SDL_GetDisplayDPI(winIdx, NULL, &dpi, NULL);
    }
    return dpi;
}

int main(int,char**) {
    
    SetConsoleOutputCP(CP_UTF8);
    setvbuf(stdout, nullptr, _IOFBF, 1000);

    // Setup SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0) {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }

    // Decide GL+GLSL versions
#if defined(__APPLE__)
    // GL 3.2 Core + GLSL 150
    const char* glsl_version = "#version 150";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif

    // From 2.0.18: Enable native IME.
#ifdef SDL_HINT_IME_SHOW_UI
    SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
#endif

    // Create window with graphics context
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_Window* window = SDL_CreateWindow("Dear ImGui SDL2+OpenGL3 example", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);
    if (window == nullptr) {
        std::cerr << "Error: SDL_CreateWindow(): " << SDL_GetError() << std::endl;
        return -1;
    }

    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    if (gl_context == nullptr) {
        std::cerr << "Error: SDL_GL_CreateContext(): " << SDL_GetError() << std::endl;
        return -1;
    }

    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init(glsl_version);

    float dpi = getScreenDPI(window);
    std::cout << "[DPI] = " << dpi << std::endl;

    const float windowsDefaultDPI = 96.0f;
    float dpiScale = round(dpi / windowsDefaultDPI * 2.0f) * 0.5f;
    std::cout << "[DPI scale] = " << dpiScale << std::endl;

    const int baseFontSize = 18;
    const float fontSize = floorf(baseFontSize * dpiScale);
    std::cout << "[font size] = " << fontSize << std::endl;

	if (dpi != -1.0f) {
		ImFontConfig fontCfg;
        fontCfg.FontBuilderFlags |= ImGuiFreeTypeBuilderFlags::ImGuiFreeTypeBuilderFlags_MonoHinting | ImGuiFreeTypeBuilderFlags_Monochrome; //отключает антиалиасинг и дает строгий алгоритм хинта
		fontCfg.PixelSnapH = true;
		fontCfg.RasterizerDensity = dpiScale;
        ImFont* font = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\segoeui.ttf", fontSize, &fontCfg, io.Fonts->GetGlyphRangesCyrillic());
        IM_ASSERT(font != nullptr);
	}

    ImGui::GetStyle().ScaleAllSizes(dpiScale);

    auto run1c = new RUN1C();

    // Our state
    bool show_demo_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    std::string inputBuffer;
    bool regexError = false;
    bool setFocusOnInput = true;
    std::vector<std::string> history;
    std::string* historySelectedItem = nullptr;

    // Main loop
    bool done = false;
    while (!done) {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                done = true;
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
                done = true;
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) // Check for Escape key press
                done = true;                                                     // Set done flag to true
        }
        if (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED) {
            SDL_Delay(10);
            continue;
        }

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
        {
            ImGui::SetNextWindowSize(io.DisplaySize);
            ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);

            ImGui::Begin("run1c", NULL, ImGuiWindowFlags_NoDecoration);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state

            ImGui::Separator();

            ImGui::SetNextItemWidth(-1); // Make the input field take the full width of the window

            if (setFocusOnInput && ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) && !ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0)) {
                ImGui::SetKeyboardFocusHere(0);
                setFocusOnInput = false;
            }

            if (ImGui::InputTextWithHint("##input", "1C path...", &inputBuffer, ImGuiInputTextFlags_EnterReturnsTrue, nullptr, nullptr)) {
                if (ImGui::IsKeyDown(ImGuiKey_ModShift)) {
                    regexError = !run1c->run(inputBuffer, true);
                } else {
                    regexError = !run1c->run(inputBuffer, false);
                }
                if (!regexError) {
                    // Move found item to the top of history
                    auto it = std::find(history.begin(), history.end(), inputBuffer);
                    if (it != history.end()) {
                        std::string found = *it;
                        history.erase(it);
                    }
                    history.push_back(inputBuffer);
                    historySelectedItem = &history.back();

                    inputBuffer = "";
                }
            }

            if (regexError) {
                ImGui::TextColored(ImVec4(255,150,150,255), "regex error");
            }

            if (ImGui::BeginListBox("##listbox_history", ImVec2(-FLT_MIN, -FLT_MIN))) {
                for (auto it = history.rbegin(); it != history.rend(); ++it) {
                    std::string* currentItemRef = &(*it);
                    bool isSelected = (historySelectedItem == currentItemRef);
                    ImGuiSelectableFlags flags = (historySelectedItem == currentItemRef) ? ImGuiSelectableFlags_Highlight : 0;
                    if (ImGui::Selectable(it->c_str(), isSelected, flags)) {
                        historySelectedItem = currentItemRef;
                        inputBuffer = *historySelectedItem;
                    }
                    if (isSelected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndListBox();
            }

            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
