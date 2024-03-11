#pragma once
#include "Header.h"
#include <functional>

#define wstr_vector std::vector<std::wstring>
#define wstr_set	std::set<std::wstring>

typedef BOOL(*FilterIf)(const std::wstring&);

struct FileManager {
	wstr_vector SearchFiles(const std::wstring& directory_path) const;
	/*wstr_vector FilterFiles(const wstr_vector& files, FilterIf condition) const;*/

	wstr_vector FilterFiles(const wstr_vector& files, std::function<BOOL(std::wstring)> condition) const;

	wstr_vector ParseFileVec(const std::wstring& filepath);
	wstr_set ParseFileSet(const std::wstring& filepath);

	std::wstring FileToString(const std::wstring& filepath) const;
	VOID CopyFiles(const wstr_vector& files, const std::wstring& directory_path) const;
	VOID CensorFiles(const wstr_vector& files, const wstr_vector& target_words, const std::wstring& directory_path) const;
	std::wstring ExtractFileName(const std::wstring& filepath);
	unsigned long GetFileSize(const std::wstring& filepath);
};

