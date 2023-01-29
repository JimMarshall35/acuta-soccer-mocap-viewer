#include <vector>
#include <string>
#include <fstream>
#include "MocapFile.h"


MocapFile::MocapFile( bool reverseEndianness)
	:_reverseSourceFileEndianness(reverseEndianness)
{
	
}

void MocapFile::ReadFileFloats(std::string filePath, bool reverseEndianness) {
	_floats.clear();
	std::ifstream in(filePath, std::ifstream::binary);
	int begin = in.tellg();
	in.seekg(0, in.end);
	int end = in.tellg();
	in.seekg(0, in.beg);
	int length = end - begin;

	int floatBufferSize = length / 4;
	std::vector<char> bbuffer;
	unsigned int word = 0;
	unsigned int rword = 0;

	for (int i = 0; i < floatBufferSize; i++) {
		char b1, b2, b3, b4;
		in.read(&b1, 1);
		in.read(&b2, 1);
		in.read(&b3, 1);
		in.read(&b4, 1);
		if (_reverseSourceFileEndianness) {
			bbuffer.push_back(b4);
			bbuffer.push_back(b3);
			bbuffer.push_back(b2);
			bbuffer.push_back(b1);
		}
		else {
			bbuffer.push_back(b1);
			bbuffer.push_back(b2);
			bbuffer.push_back(b3);
			bbuffer.push_back(b4);
		}
		
		auto asAFloat = *((float*)&bbuffer[(bbuffer.size() - 4)]);
		_floats.push_back(asAFloat);
	}
}

void MocapFile::ReadFloatsIntoFrameStructs()
{
	_frames.clear();
	int numFrames = _floats.size() / MocapFrameSizeFloats;
	for (int i = 0; i < numFrames; i++) {
		MocapFrame frame;
		//frame.points
		for (int j = 0; j < PlayerPoints; j++) {
			int startIndex = i * MocapFrameSizeFloats + j * 3 + 1;
			auto vec3 = glm::vec3{
				_floats[startIndex],
				_floats[startIndex + 1],
				_floats[startIndex + 2]
			};
			frame.points[j] = vec3;
		}
		_frames.push_back(frame);
	}
}

void MocapFile::Load(std::string path)
{
	ReadFileFloats(path, _reverseSourceFileEndianness);
	ReadFloatsIntoFrameStructs();
}

const std::vector<MocapFrame>& MocapFile::CGetFrames() const
{
	return _frames;
}

std::vector<MocapFrame>& MocapFile::GetFrames()
{
	return _frames;
}
