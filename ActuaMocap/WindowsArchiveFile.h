#pragma once
#include <string>
struct FileOffset { int offset, size; };
struct ResourceFile {
	char* data;
	size_t size;
};
#define MAX_FILES 500

class WindowsArchiveFile
{
public:
	~WindowsArchiveFile();
	WindowsArchiveFile(std::string offsetFilePath, std::string dataFile);
	void LoadOffsets(std::string offsetFilePath);
	void LoadDataFile(std::string dataFile);
	ResourceFile GetResourceFileFromOffset(size_t offset);
private:
	FileOffset _offsets[MAX_FILES];
	char* _data;
	std::string _loadedOffsetFilePath;
	std::string _loadedDataFile;

};

