#include "Config.h"
#include "TextFileResourceListParser.h"

Config::Config()
{
	ParseResourcesTextFile("config.txt", [this](std::string key, std::string value) {
			if (key == "SourceEndiannessReversed") {
				ReverseFileEndianness = atoi(value.c_str()) == 0 ? false : true;
			}
			else if (key == "BaseMocapFilesDirectory") {
				MocapFilesFolder = value;
			}
			else if (key == "Theme") {
				Theme = value;
			}
			else if (key == "Font") {
				Font = value;
			}
		});
}
