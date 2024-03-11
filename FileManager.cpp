#include "FileManager.h"




wstr_vector FileManager::SearchFiles(const std::wstring& directory_path) const {
	wstr_vector file_paths;

	WIN32_FIND_DATA data;
	HANDLE hFind = FindFirstFile((directory_path + L"\\*").c_str(), &data);

	if (hFind != INVALID_HANDLE_VALUE) {

		do {
			std::wstring file_name = data.cFileName;

			if (wcscmp(file_name.c_str(), L".") != 0 && wcscmp(file_name.c_str(), L"..") != 0) {

				file_paths.push_back(directory_path + L"\\" + data.cFileName);
			}

		} while (FindNextFile(hFind, &data) != 0);
	}

	FindClose(hFind);

	return file_paths;
}

//wstr_vector FileManager::FilterFiles(const wstr_vector& files, FilterIf condition) const {
//
//	wstr_vector filtered_files;
//
//	for (size_t i = 0; i < files.size(); i++) {
//		if (condition(files[i])) {
//			filtered_files.push_back(files[i]);
//		}
//	}
//
//	return filtered_files;
//}

wstr_vector FileManager::FilterFiles(const wstr_vector& files, std::function<BOOL(std::wstring)> condition) const {

	wstr_vector filtered_files;

	for (size_t i = 0; i < files.size(); i++) {
		if (condition(files[i])) {
			filtered_files.push_back(files[i]);
		}
	}

	return filtered_files;
}

VOID FileManager::CopyFiles(const wstr_vector& files, const std::wstring& directory_path) const {
	for (size_t i = 0; i < files.size(); i++) {
		INT index_of_file_name = files[i].find_last_of(L"\\");
		std::wstring file_name = files[i].substr(index_of_file_name);

		if (!CopyFile(files[i].c_str(), (directory_path + file_name).c_str(), FALSE))
			MessageBox(NULL, L"Ахтунг! Проблема с копированием!!!!", NULL, MB_OK);
	}
}

wstr_vector FileManager::ParseFileVec(const std::wstring& filepath) {
	std::wstring data = FileToString(filepath);

	wstr_vector parsed_file;

	size_t position = data.find(L" ");
	size_t current_position = 0;

	if (position == std::wstring::npos) {
		parsed_file.push_back(data.substr(current_position));
	}

	while (position != std::wstring::npos) {
		parsed_file.push_back(data.substr(current_position, position - current_position));

		current_position = position + 1;

		position = data.find(L" ", current_position);
	}

	parsed_file.push_back(data.substr(current_position));

	return parsed_file;
}

wstr_set FileManager::ParseFileSet(const std::wstring& filepath) {
	std::wstring data = FileToString(filepath);

	wstr_set parsed_file;

	size_t position = data.find(L" ");
	size_t current_position = 0;

	if (position == std::wstring::npos) {
		parsed_file.insert(data.substr(current_position));
	}

	while (position != std::wstring::npos) {
		parsed_file.insert(data.substr(current_position, position - current_position));

		current_position = position + 1;

		position = data.find(L" ", current_position);
	}

	parsed_file.insert(data.substr(current_position));

	return parsed_file;
}

std::wstring FileManager::FileToString(const std::wstring& filepath) const {
	std::wifstream in(filepath);
	std::wstring data = L"";

	if (in.is_open()) {

		in.seekg(0, std::ios::end);
		std::streamoff len = in.tellg();
		in.seekg(0, std::ios::beg);

		wchar_t* content = new wchar_t[len];

		in.read(content, len);

		data = content;

		delete[] content;
	}
	else {
		std::wstring error = L"AHTUNG!!!!! " + filepath + L" file is not open!";
		MessageBox(NULL, error.c_str(), NULL, MB_OK);
	}


	in.close();

	return data;
}

VOID FileManager::CensorFiles(const wstr_vector& files, const wstr_vector& target_words, const std::wstring& directory_path) const {
	std::wstring line;

	for (size_t i = 0; i < files.size(); i++) {
		std::wifstream in(files[i]);

		INT index_of_file_name = files[i].find_last_of(L"\\");
		std::wstring file_name = files[i].substr(index_of_file_name);

		std::wofstream out(directory_path + L"\\" + file_name);

		while (std::getline(in, line)) {
		
			for (size_t i = 0; i < target_words.size(); i++) {
				
				size_t pos = 0;
				while ((pos = line.find(target_words[i], pos)) != std::wstring::npos) {
					line.replace(pos, target_words[i].length(), L"*******");
					pos += 7;
				}
			}

			out << line << std::endl;

		}
	}
}

std::wstring FileManager::ExtractFileName(const std::wstring& filepath) {
	INT index_of_file_name = filepath.find_last_of(L"\\") + 1;
	std::wstring file_name = filepath.substr(index_of_file_name);

	return file_name;
}

unsigned long FileManager::GetFileSize(const std::wstring& filepath) {
	std::wifstream in(filepath);

	in.seekg(0, std::ios::end);
	std::streamoff len = in.tellg();
	in.seekg(0, std::ios::beg);

	return len;
}



