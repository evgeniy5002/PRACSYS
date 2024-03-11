#pragma once
#include "Header.h"

struct FileInfo {
	unsigned long fileSize;
	std::wstring fileName;
	std::wstring filePath;
	std::wstring replacements;

	FileInfo(unsigned long size, std::wstring name, std::wstring path, std::wstring replacements);
	FileInfo() = default;
};

struct Report {
	std::vector<FileInfo> files;
	std::vector<std::string> mostRepeatedWords;
	VOID add(FileInfo file);
};

