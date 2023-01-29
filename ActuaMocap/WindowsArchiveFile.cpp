#include "WindowsArchiveFile.h"
#include <fstream>

WindowsArchiveFile::~WindowsArchiveFile()
{
	if (_data) {
		delete[] _data;
	}
}

WindowsArchiveFile::WindowsArchiveFile(std::string offsetFilePath, std::string dataFile)
	:_loadedDataFile(dataFile), _loadedOffsetFilePath(offsetFilePath)
{
	LoadOffsets(_loadedOffsetFilePath);
	LoadDataFile(_loadedOffsetFilePath);
}

void WindowsArchiveFile::LoadOffsets(std::string offsetFilePath)
{
	int numOffsets;
	// load offsets file
	{
		std::ifstream is(offsetFilePath, std::ifstream::binary);
		is.seekg(0, is.end);
		int length = is.tellg();
		is.seekg(0, is.beg);
		is.read((char*)_offsets, length);
		numOffsets = length / sizeof(FileOffset);
	}

	
}

void WindowsArchiveFile::LoadDataFile(std::string dataFile)
{
	std::ifstream dataFileIn(dataFile, std::ifstream::binary);
	dataFileIn.seekg(0, dataFileIn.end);
	int length = dataFileIn.tellg();
	dataFileIn.seekg(0, dataFileIn.beg);
	if (_data) {
		delete[] _data;
	}
	_data = new char[length];
}

ResourceFile WindowsArchiveFile::GetResourceFileFromOffset(size_t offset)
{
	int divided = offset / 8;
	size_t dataFileOffset = _offsets[divided].offset;
	size_t dataFileSize = _offsets[divided].size;
	ResourceFile r;
	r.size = dataFileSize;
	r.data = &_data[dataFileOffset];
	return ResourceFile();
}
