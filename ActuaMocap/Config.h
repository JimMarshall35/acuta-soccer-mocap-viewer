#pragma once
#include <string>
class Config
{
public:
	Config();
	std::string MocapFilesFolder;
	std::string Theme;
	std::string Font;
	bool ReverseFileEndianness;
};

