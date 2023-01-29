#pragma once
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "MocapFrame.h"

class MocapFile {
public:
	MocapFile(bool reverseEndianness);
	void Load(std::string path);
	const std::vector<MocapFrame>& CGetFrames() const;
	std::vector<MocapFrame>& GetFrames();
private:
	void ReadFileFloats(std::string filePath, bool reverseEndianness);
	void ReadFloatsIntoFrameStructs();
private:
	std::vector<MocapFrame> _frames;
	std::vector<float> _floats;
	bool _reverseSourceFileEndianness;
};
