#pragma once
#include <string>
#include <vector>
struct ImGuiIO;
struct GLFWwindow;
class IFilesystem;
class MocapAnimation;
class MocapFile;
class Config;

enum ToolMode {
	ToolModePlay,
	ToolModeEdit
};

class ToolUi
{
public:
	~ToolUi();
	ToolUi(GLFWwindow* window, IFilesystem* fileSystem, MocapAnimation* animation, MocapFile* file, const Config& config);
	void Update(double deltaT);
	void Draw() const;
	inline bool WantsMouse() const {
		return _wantMouseInput;
	}
	inline bool WantsKeyboard() const {
		return _wantKeyboardInput;
	}
private:
	void LoadFile(std::string fileName);
private:
	void DoPlayModeWindow();
	void DoEditModeWindow();
	void DoUiWindow();
	void SwitchToEditMode();
	void SwitchToPlayMode();
	IFilesystem* _fileSystem;
	ImGuiIO* _io;
	bool _wantMouseInput;
	bool _wantKeyboardInput;
	std::string _mocapFilesFolder;
	MocapAnimation* _animation;
	MocapFile* _file;
	std::vector<std::string> _mocapFiles;
	std::string _loadedFile;
	ToolMode _mode = ToolModePlay;
	bool _paused = false;
};

