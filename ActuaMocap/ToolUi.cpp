#include "ToolUi.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "Config.h"
#include <iostream>
#include "IFilesystem.h"
#include "MocapFile.h"
#include "MocapAnimation.h"

ToolUi::~ToolUi()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

ToolUi::ToolUi(GLFWwindow* window, IFilesystem* fileSystem, MocapAnimation* animation, MocapFile* file, const Config& config)
	:_fileSystem(fileSystem),
    _mocapFilesFolder(config.MocapFilesFolder),
    _animation(animation),
    _file(file)
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    _io = &ImGui::GetIO(); (void)_io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    if (config.Theme == "Light") {
        ImGui::StyleColorsLight();
    }
    else if (config.Theme == "Dark") {
        ImGui::StyleColorsDark();
    }
    else {
        std::cout << "Unrecognised theme in config file" << std::endl;
    }
    _mocapFiles = _fileSystem->ListFilesInDirectory(config.MocapFilesFolder);
    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    const char* glsl_version = "#version 130";
    ImGui_ImplOpenGL3_Init(glsl_version);
}

void ToolUi::Update(double deltaT)
{
    _wantMouseInput = _io->WantCaptureMouse;
    _wantKeyboardInput = _io->WantCaptureKeyboard;

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    if (!_paused) {
        _animation->Update(deltaT);
    }
    
    DoUiWindow();
    //ImGui::ShowDemoWindow();
    
    // render
    // ------
    ImGui::Render();
}

void ToolUi::Draw() const
{
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ToolUi::LoadFile(std::string fileName)
{
    _animation->ResetAfterNewFileLoad();
    _file->Load(_mocapFilesFolder + "\\" + fileName);
    _loadedFile = fileName;
}

void ToolUi::DoPlayModeWindow()
{
    ImGui::Text(_loadedFile.c_str());
    ImGui::Text("length %f", _animation->GetCurrentLengthSeconds());
    ImGui::Text("progress %f", _animation->GetAnimationProgressSeconds());
    if (ImGui::Button(_paused ? "Play" : "Pause")) {
        if (_paused) {
            _paused = false;
        }
        else {
            _animation->PopulateSkeleton();
            _paused = true;
        }
    }

    static int item_current_idx = 0; // Here we store our selection data as an index.
    if (ImGui::BeginListBox("mocap files"))
    {
        for (int i = 1; i < _mocapFiles.size(); i++) { // skip first file as it's ".."
            const bool is_selected = (item_current_idx == i);
            if (ImGui::Selectable(_mocapFiles[i].c_str(), is_selected)) {
                item_current_idx = i;
                LoadFile(_mocapFiles[i]);
            }

            // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndListBox();
    }

    static char buf[64] = "";
    if (ImGui::InputText("fps", buf, 64, ImGuiInputTextFlags_CharsDecimal)) {
        _animation->SetFps(atof(buf));
    }

}

void ToolUi::DoEditModeWindow()
{
    static int sliderVal = _animation->GetCurrentFrameNumber();
    if (ImGui::SliderInt("frame", &sliderVal, 0, _animation->GetNumFrames() - 1)) {
        _animation->SetToFrame(sliderVal);
    }
}

void ToolUi::DoUiWindow()
{
    ImGui::Begin("Motion capture tool");

    if(ImGui::Button(_mode == ToolModePlay ? "Edit" : "Playback")) {
        if (_mode == ToolModePlay) {
            _mode = ToolModeEdit;
            SwitchToEditMode();
        }
        else {
            SwitchToPlayMode();
            _mode = ToolModePlay;
        }
    }

    switch (_mode) {
    break; case ToolModePlay:
        DoPlayModeWindow();
    break; case ToolModeEdit:
        DoEditModeWindow();
    }

    
    ImGui::End();
}

void ToolUi::SwitchToEditMode()
{
    _paused = true;
    _animation->SetToFrame(_animation->GetCurrentFrameNumber());
}

void ToolUi::SwitchToPlayMode()
{
    _paused = false;
}
