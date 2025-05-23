#define IMGUI_USER_CONFIG "my_imgui_config.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_stdlib.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"
#include "misc/freetype/imgui_freetype.h"

#include <SDL.h>
#include <SDL_opengl.h>

#include <iostream>
#include <vector>
#include <algorithm>
#include <map>
#include <fstream>
#include <sstream>
#include <variant>
#include <memory>
#include <filesystem>
#include <regex>

#include "utils.h"
#include "config.h"
#include "error_handler.h"

class RUN1C {
public:
    RUN1C() {
        starterPath = Config::get1CStarterPath();
    };
    RUN1C(std::string starterPath) : starterPath(starterPath) {};
    bool run(std::string input, bool isConfigMode = false);
private:
    std::string starterPath;
};

bool RUN1C::run(std::string input, bool isConfigMode) {
    try {
        // Validate input
        if (input.empty()) {
            ErrorHandler::logError(ErrorType::InvalidPath, "Empty input provided");
            return false;
        }

        // Validate starter path exists
        if (!ErrorHandler::validate1CPath(starterPath)) {
            ErrorHandler::showError(ErrorType::FileNotFound, "1C starter not found at: " + starterPath);
            return false;
        }

        std::vector<std::string> args;
        ErrorHandler::logInfo("Processing input: " + input);

        if (isConfigMode) {
            args.push_back("CONFIG");
        } else {
            args.push_back("ENTERPRISE");
        }

        ErrorHandler::logInfo("Running regex extraction on input");

        std::regex filepathRegex("\\w:.+?((?=\"$)|(?=\";)|($))", std::regex_constants::ECMAScript);
        std::smatch m;

        if (std::regex_search(input, m, filepathRegex)) {
            std::string path = m[0].str();
            ErrorHandler::logInfo("Extracted path: " + path);

            // Validate extracted path
            if (!ErrorHandler::validatePath(path)) {
                ErrorHandler::showError(ErrorType::InvalidPath, "Database path does not exist: " + path);
                return false;
            }

            args.push_back("/F");
            args.push_back("\"" + path + "\"");

            ErrorHandler::logInfo("Launching 1C with path: " + path);
            launchProcess(starterPath, args);
            return true;

        } else {
            ErrorHandler::logError(ErrorType::InvalidPath, "Could not extract valid path from input: " + input);
            return false;
        }

    } catch (const std::exception& e) {
        ErrorHandler::showError(ErrorType::LaunchFailed, "Exception during launch: " + std::string(e.what()));
        return false;
    }
}

float getScreenDPI(SDL_Window* window) {
    float dpi = -1.0f;
    int winIdx = SDL_GetWindowDisplayIndex(window);
    if (winIdx >= 0) {
        SDL_GetDisplayDPI(winIdx, NULL, &dpi, NULL);
    }
    return dpi;
}

using PersistentStorage_StoreItem = std::variant<std::string, std::vector<std::string>>;

class PersistentStorage {
public:
    PersistentStorage();

    // Loads configuration from file
    void load();

    // Saves configuration to file
    void save();

    // Store a string or array value by key
    void put(const std::string& key, const PersistentStorage_StoreItem& value);

    // Get a string value by key (returns empty string if not found or not a string)
    std::string getItem(const std::string& key) const;

    // Get a vector<string> by key (returns empty vector if not found or not an array)
    std::vector<std::string> getArray(const std::string& key) const;

    // Get a modifiable reference to a vector<string> by key (throws if not found or not an array)
    std::vector<std::string>& getArrayRef(const std::string& key);

    // Check if key exists
    bool contains(const std::string& key) const;

private:
    std::string filepath;
    std::map<std::string, PersistentStorage_StoreItem> store;

    // Helper to parse lines like [type:name] or [type:type:name]
    std::vector<std::string> parseBracketedLine(const std::string& line) const;

    // Ensure config file exists
    void createFileIfNotExists(const std::string& path) const;
};

PersistentStorage::PersistentStorage() {
    filepath = Config::getStorageFilePath();
    std::cout << "[config] Using storage path: " << filepath << std::endl;
    
    // Ensure storage directories and file exist
    createFileIfNotExists(filepath);
    std::cout << "[config] Storage initialized successfully" << std::endl;
}

std::vector<std::string> PersistentStorage::parseBracketedLine(const std::string& line) const {
    std::vector<std::string> result;
    if (line.size() < 3 || line.front() != '[' || line.back() != ']')
        return result;

    std::string inner = line.substr(1, line.size() - 2); // Remove [ and ]
    std::stringstream ss(inner);
    std::string part;
    while (std::getline(ss, part, ':')) {
        result.push_back(part);
    }

    return result;
}

void PersistentStorage::createFileIfNotExists(const std::string& path) const {
    // Create parent directory if it doesn't exist
    std::filesystem::path filePath(path);
    std::filesystem::path parentDir = filePath.parent_path();
    if (!parentDir.empty() && !std::filesystem::exists(parentDir)) {
        std::cout << "[config] Creating directory: " << parentDir.string() << std::endl;
        std::filesystem::create_directories(parentDir);
    }
    
    // Create file if it doesn't exist
    if (!std::filesystem::exists(path)) {
        std::cout << "[config] Creating storage file: " << path << std::endl;
        std::ofstream outfile(path, std::ios::app);
        outfile.close();
    }
}

void PersistentStorage::load() {

    std::ifstream infile(filepath);
    std::vector<std::string> lines;

    {
        std::string line;
        while(std::getline(infile, line)) {
            // Strip leading and trailing whitespace
            line.erase(0, line.find_first_not_of(" \t\r\n"));
            line.erase(line.find_last_not_of(" \t\r\n") + 1);
            if (line.empty()) continue; // Skip empty lines
            lines.push_back(line);
        }
    }

    for (size_t i = 0; i < lines.size(); ++i) {

        const auto& line = lines[i];
        auto parts = parseBracketedLine(line);

        if (parts.size() == 1) {
            std::string key = parts[0];
            if (i + 1 < lines.size()) {
                const auto& nextLine = lines[i + 1];
                if (nextLine.front() == '[' || nextLine.back() == ']') {
                    std::cerr << "[config loading] ERROR: missing value for key: " << key << std::endl;
                    continue;
                }
                std::cout << "[config loading] " << key << " = " << nextLine << std::endl;
                store.emplace(key, nextLine);
                i++;
            } else {
                std::cerr << "[config loading] ERROR: missing value for key: " << key << std::endl;
            }
        } else if (parts.size() == 2 && parts[0] == "array") {
            std::string key = parts[1];
            std::vector<std::string> array;
            while(i + 1 < lines.size()) {
                const auto& nextLine = lines[i + 1];
                if (nextLine.front() == '[' || nextLine.back() == ']') {
                    break;
                }
                std::cout << "[config loading] " << key << " << " << nextLine << std::endl;
                array.push_back(nextLine);
                i++;
            }
            if (array.size() == 0) {
                std::cerr << "[config loading] ERROR: missing items for array: " << key << std::endl;
                continue;
            }
            store.emplace(key, array);
        } else {
            std::cerr << "[config loading] ERROR: wrong file format" << std::endl;
        }

    }

}

void PersistentStorage::save() {
    std::cout << "[config saving] persisting storage to disk" << std::endl;

    std::ofstream outfile(filepath, std::ios::trunc);
    if (!outfile.is_open()) {
        std::cerr << "[config saving] ERROR: Unable to open file for saving: " << filepath << std::endl;
        return;
    }

    for (const auto& [key, value] : store) {
        if (std::holds_alternative<std::string>(value)) {
            outfile << "[" << key << "]" << std::endl;
            outfile << std::get<std::string>(value) << std::endl;
        } else if (std::holds_alternative<std::vector<std::string>>(value)) {
            outfile << "[array:" << key << "]" << std::endl;
            for (const auto& item : std::get<std::vector<std::string>>(value)) {
                outfile << item << std::endl;
            }
        }
    }

    outfile.close();
    std::cout << "[config saving] storage saved successfully" << std::endl;
}

void PersistentStorage::put(const std::string& key, const PersistentStorage_StoreItem& value) {
    store[key] = value;
}

std::string PersistentStorage::getItem(const std::string& key) const {
    auto it = store.find(key);
    if (it != store.end()) {
        if (std::holds_alternative<std::string>(it->second)) {
            std::string value = std::get<std::string>(it->second);
            return value;
        }
    }
    return "";
}

std::vector<std::string> PersistentStorage::getArray(const std::string& key) const {
    auto it = store.find(key);
    if (it != store.end()) {
        if (std::holds_alternative<std::vector<std::string>>(it->second)) {
            const auto& vec = std::get<std::vector<std::string>>(it->second);
            return vec;
        }
    }
    return {}; // Return an empty vector if the key is not found or the value is not a vector
}

std::vector<std::string>& PersistentStorage::getArrayRef(const std::string& key) {
    auto it = store.find(key);
    if (it != store.end()) {
        if (std::holds_alternative<std::vector<std::string>>(it->second)) {
            return std::get<std::vector<std::string>>(it->second);
        }
    }
    // If key does not exist, create it with an empty vector
    store[key] = std::vector<std::string>{};
    return std::get<std::vector<std::string>>(store[key]);
}

bool PersistentStorage::contains(const std::string& key) const {
    return store.find(key) != store.end();
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

    const int baseFontSize = Config::getBaseFontSize();
    const float fontSize = floorf(baseFontSize * dpiScale);
    std::cout << "[font size] = " << fontSize << std::endl;

	if (dpi != -1.0f) {
		ImFontConfig fontCfg;
        fontCfg.FontBuilderFlags |= ImGuiFreeTypeBuilderFlags::ImGuiFreeTypeBuilderFlags_MonoHinting | ImGuiFreeTypeBuilderFlags_Monochrome; //отключает антиалиасинг и дает строгий алгоритм хинта
		fontCfg.PixelSnapH = true;
		fontCfg.RasterizerDensity = dpiScale;
        ImFont* font = io.Fonts->AddFontFromFileTTF(Config::getFontPath().c_str(), fontSize, &fontCfg, io.Fonts->GetGlyphRangesCyrillic());
        IM_ASSERT(font != nullptr);
	}

    ImGui::GetStyle().ScaleAllSizes(dpiScale);

    auto run1c = std::make_unique<RUN1C>();
    auto storage = std::make_unique<PersistentStorage>();
    storage->load();

    // Our state
    bool show_demo_window = false;
    bool showHelpWindow = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    std::string inputBuffer;
    bool regexError = false;
    bool isInputFocused = false;
    bool isSetFocusOnInput = true;
    bool isSetFocusOnCurrentHistoryItem = false;
    std::vector<std::string>& history = storage->getArrayRef("basesHistory");
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

            ImGui::Begin("RUN1C_MainWindow", NULL, ImGuiWindowFlags_NoDecoration);

            if (ImGui::Button("Help")) showHelpWindow = !showHelpWindow;
            ImGui::SameLine();
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

            ImGui::Checkbox("Demo Window", &show_demo_window);

            ImGui::Separator();

            ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(255,255,255,0));
            ImGui::SetNextItemWidth(-FLT_MIN); // Make the input field take the full width of the window

            if (isSetFocusOnInput && ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) && !ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0)) {
                ImGui::SetKeyboardFocusHere(0);
                isSetFocusOnInput = false;
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
                    storage->save();
                    historySelectedItem = &history.back();

                    inputBuffer = "";
                }
            }

            isInputFocused = ImGui::IsItemActiveAsInputText();

            if (isInputFocused) {
                if (ImGui::IsKeyPressed(ImGuiKey_UpArrow) || ImGui::IsKeyPressed(ImGuiKey_DownArrow)) {
                    if (!historySelectedItem) historySelectedItem = &history.back();
                    isSetFocusOnCurrentHistoryItem = true;
                }
            }

            ImGuiID inputID = ImGui::GetItemID();

            if (regexError) {
                ImGui::Separator();
                ImGui::TextColored(ImVec4(1.0f, 0.35f, 0.35f, 1.0f), "Wrong input");
            }

            ImGui::Separator();

            if (ImGui::BeginListBox("##listbox_history", ImVec2(-FLT_MIN, -FLT_MIN))) {

                for (auto it = history.rbegin(); it != history.rend(); it++) {

                    std::string* currentItemRef = &(*it);
                    ImGui::PushID(currentItemRef);

                    bool isSelected = historySelectedItem == currentItemRef;

                    ImGuiSelectableFlags flags = (historySelectedItem == currentItemRef && !isInputFocused) ? ImGuiSelectableFlags_Highlight : 0;

                    if (isSelected && isSetFocusOnCurrentHistoryItem) {
                        ImGui::SetKeyboardFocusHere();
                        isSetFocusOnCurrentHistoryItem = false;
                    }

                    if (ImGui::Selectable(it->c_str(), isSelected, flags)) {
                        historySelectedItem = currentItemRef;
                        inputBuffer = *historySelectedItem;
                        isSetFocusOnInput = true;
                    }

                    if (isSelected) {
                        ImGui::SetItemDefaultFocus();
                    }

                    ImGui::PopID();

                }
                ImGui::EndListBox();
            }

            ImGui::PopStyleColor();

            if (ImGui::GetActiveID() != inputID && ImGui::IsKeyDown(ImGuiKey_F)) {
                isSetFocusOnInput = true;
            }

            ImGui::End();
        }

        if (showHelpWindow) {

            ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f));
            ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x / 2, io.DisplaySize.y / 2), ImGuiCond_Always, ImVec2(0.5f, 0.5f));

            ImGui::Begin("Help##RUN1C_HelpPopupWindow", &showHelpWindow, ImGuiWindowFlags_None);

            if (!ImGui::IsWindowFocused()) showHelpWindow = false;

            ImGui::Text("[f] - focus search bar");
            ImGui::Text("[up/down arrow] - move to history");

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

    storage->save();

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
