#include "WordsDetectionApp.h"

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nShowCmd) {
	
	WordsDetectionApp wda;

	return DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, (DLGPROC)WordsDetectionApp::DlgProc);
}


//std::wofstream out(L"C:\\Users\\KIKIMORA\\Desktop\\text.txt");
//for (int i = 0; i < filtered_files.size(); i++) {

//	out << "asd\n";
//}

//std::vector<std::wstring> persed_file = fm.ParseFile(L"C:\\Users\\KIKIMORA\\Desktop\\Copies\\TEXT_1.txt");

//for (int i = 0; i < persed_file.size(); i++) {

//	out << persed_file[i] << "\n";
//}


//[target_words](const std::wstring& filepath) -> BOOL {
//	FileManager fm;
//
//	std::set<std::wstring> parsed_file = fm.ParseFileSet(filepath);
//
//	for (const std::wstring& word : target_words) {
//
//		auto it = parsed_file.find(word);
//
//		if (it != parsed_file.end())
//			return TRUE;
//	}
//
//	return FALSE;
//}