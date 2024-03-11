#include "WordsDetectionApp.h"

#define wstr_vector std::vector<std::wstring>

HANDLE ahtung_mutex;

HANDLE h_filter_thread_done_event = CreateEvent(NULL, TRUE, FALSE, NULL);
HANDLE h_censored_thread_done_event = CreateEvent(NULL, TRUE, FALSE, NULL);

Report report;

wstr_vector gl_target_words;

std::wstring gl_files_folder_path;
std::wstring gl_for_copy_path;
std::wstring gl_for_censored_path;
std::wstring gl_for_report_path;

BOOL stop_threads = FALSE;

VOID WriteToFile(const Report& report, const std::wstring& filepath);
std::wstring SelectFile();
std::wstring SelectFolder();
wstr_vector  ParseString(const std::wstring& text);
VOID ShowFile(HWND hFileInfo, const std::wstring& filepath);


// === === === === === === === === === === //
struct ThreadParams {
	wstr_vector files;
};

DWORD WINAPI Copy_Files_Thread(LPVOID lpParam) {
	WaitForSingleObject(ahtung_mutex, INFINITE);

	if (WaitForSingleObject(h_filter_thread_done_event, INFINITE) == WAIT_OBJECT_0) {
		FileManager fm;
		ThreadParams* files = (ThreadParams*)lpParam;

		fm.CopyFiles(files->files, gl_for_copy_path);
	}

	if (stop_threads)
		ExitThread(0);


	ReleaseMutex(ahtung_mutex);
	return 0;
}

DWORD WINAPI Censor_Files_Thread(LPVOID lpParam) {
	WaitForSingleObject(ahtung_mutex, INFINITE);

	if (WaitForSingleObject(h_filter_thread_done_event, INFINITE) == WAIT_OBJECT_0) {
		FileManager fm;
		ThreadParams* thread_params = (ThreadParams*)lpParam;

		fm.CensorFiles(thread_params->files, gl_target_words, gl_for_censored_path);
		thread_params->files = fm.SearchFiles(gl_for_censored_path);
	}

	if (stop_threads)
		ExitThread(0);

	ReleaseMutex(ahtung_mutex);
	SetEvent(h_censored_thread_done_event);
	return 0;
}

DWORD WINAPI Filter_Files_Thread(LPVOID lpParam) {
	WaitForSingleObject(ahtung_mutex, INFINITE);

	FileManager fm;
	ThreadParams* thread_params = (ThreadParams*)lpParam;

	std::vector<std::wstring> target_words = gl_target_words;

	thread_params->files = fm.FilterFiles(thread_params->files,
		[target_words](const std::wstring& filepath) -> BOOL {
			FileManager fm;

			std::set<std::wstring> parsed_file = fm.ParseFileSet(filepath);

			for (const std::wstring& word : target_words) {

				auto it = parsed_file.find(word);

				if (it != parsed_file.end())
					return TRUE;
			}

			return FALSE;
		}
	);

	if (stop_threads)
		ExitThread(0);

	ReleaseMutex(ahtung_mutex);
	SetEvent(h_filter_thread_done_event);
	return 0;
}

DWORD WINAPI Report_Thread(LPVOID lpParam) {
	FileManager fm;
	ThreadParams* thread_params = (ThreadParams*)lpParam;

	if (WaitForSingleObject(h_censored_thread_done_event, INFINITE) == WAIT_OBJECT_0) {

		for (size_t i = 0; i < thread_params->files.size(); i++) {
			FileInfo file;

			file.filePath = thread_params->files[i];
			file.fileName = fm.ExtractFileName(thread_params->files[i]);
			file.fileSize = fm.GetFileSize(thread_params->files[i]);

			report.add(file);
		}

		WriteToFile(report, gl_for_report_path);
	}

	return 0;
}

// === === === === === === === === === === //




// === === === === === === === === === === //

VOID WordsDetectionApp::StartSearch() {
	stop_threads = FALSE;
	FileManager fm;
	ThreadParams* thread_params = new ThreadParams();

	size_t text_len = GetWindowTextLength(hTargetWords);

	wchar_t* buffer = new wchar_t[text_len + 1];

	GetWindowText(hTargetWords, buffer, text_len + 1);

	wstr_vector words = ParseString(buffer);

	gl_target_words.insert(gl_target_words.end(), words.begin(), words.end());

	delete[] buffer;

	thread_params->files = fm.SearchFiles(gl_files_folder_path);

	hFilterThread = CreateThread(NULL, 0, Filter_Files_Thread, LPVOID(thread_params), 0, NULL);
	hCopyThread = CreateThread(NULL, 0, Copy_Files_Thread, LPVOID(thread_params), 0, NULL);
	hCensorThread = CreateThread(NULL, 0, Censor_Files_Thread, LPVOID(thread_params), 0, NULL);
	hReportThread = CreateThread(NULL, 0, Report_Thread, LPVOID(thread_params), 0, NULL);
}

VOID WordsDetectionApp::StopSearch() {
	stop_threads = TRUE;
	TerminateThread(hFilterThread, 0);
	TerminateThread(hCopyThread, 0);
	TerminateThread(hCensorThread, 0);
}

VOID WordsDetectionApp::ResumeSearch() {
	ResumeThread(hFilterThread);
	ResumeThread(hCopyThread);
	ResumeThread(hCensorThread);
}

VOID WordsDetectionApp::SuspendSearch() {
	SuspendThread(hFilterThread);
	SuspendThread(hCopyThread);
	SuspendThread(hCensorThread);
}

// === === === === === === === === === === //





WordsDetectionApp* WordsDetectionApp::wda_ptr = nullptr;

WordsDetectionApp::WordsDetectionApp() {
	wda_ptr = this;
}

BOOL CALLBACK WordsDetectionApp::DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	switch (uMsg) {
		HANDLE_MSG(hwnd, WM_INITDIALOG, wda_ptr->WDA_OnInitDialog);
		HANDLE_MSG(hwnd, WM_COMMAND, wda_ptr->WDA_OnCommand);
		HANDLE_MSG(hwnd, WM_CLOSE, wda_ptr->WDA_OnClose);
	}

	return FALSE;
}

BOOL WordsDetectionApp::WDA_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam) {
	ahtung_mutex = CreateMutex(NULL, FALSE, L"A_H_T_U_N_G__M_U_T_E_X");

	hTargetWords = GetDlgItem(hwnd, IDC_TARGETWORDS);

	hFilesFolderPath = GetDlgItem(hwnd, IDC_FILESFOLDER_PATH);
	hForCopyPath = GetDlgItem(hwnd, IDC_FORCOPY_PATH);
	hForCensoredPath = GetDlgItem(hwnd, IDC_FORCENSORED_PATH);
	hForReportPath = GetDlgItem(hwnd, IDC_FORREPORT_PATH);
	hFileInfo = GetDlgItem(hwnd, IDC_FILEINFO);

	return TRUE;
}

VOID WordsDetectionApp::WDA_OnCommand(HWND hwnd, INT id, HWND hwndCtl, UINT codeNotify) {

	switch (id) {

	case IDC_START: {
		StartSearch();
		break;
	}
	case IDC_STOP: {
		StopSearch();
		break;
	}
	case IDC_SUSPEND: {
		SuspendSearch();
		break;
	}
	case IDC_RESUME: {
		ResumeSearch();
		break;
	}
	case IDC_FILESFOLDER_BTN: {

		std::wstring folder_path = SelectFolder();

		gl_files_folder_path = folder_path;

		SetWindowText(hFilesFolderPath, folder_path.c_str());
		break;
	}
	case IDC_OUTPUT_BTN: {

		ShowFile(hFileInfo, gl_for_report_path);

		break;
	}
	case IDC_FORCOPY_BTN: {

		std::wstring folder_path = SelectFolder();

		gl_for_copy_path = folder_path;

		SetWindowText(hForCopyPath, folder_path.c_str());

		break;
	}
	case IDC_FORREPORT_BTN: {
		
		std::wstring folder_path = SelectFile();

		gl_for_report_path = folder_path;

		SetWindowText(hForReportPath, folder_path.c_str());

		break;
	}
	case IDC_FORCENSORED_BTN: {

		std::wstring folder_path = SelectFolder();

		gl_for_censored_path = folder_path;

		SetWindowText(hForCensoredPath, folder_path.c_str());

		break;
	}
	case IDC_LOADWORDS: {
		FileManager fm;

		std::wstring selectedFile = SelectFile();

		if (!selectedFile.empty()) {
			MessageBox(NULL, selectedFile.c_str(), L"Выбранный файл", MB_OK);
			gl_target_words = fm.ParseFileVec(selectedFile);
		}
		break;
	}
	}
}

VOID WordsDetectionApp::WDA_OnClose(HWND hwnd) {
	EndDialog(hwnd, 0);
}




// === === === === === === === === === === //
wstr_vector ParseString(const std::wstring& text) {
	wstr_vector parsed_file;

	size_t position = text.find(L" ");
	size_t current_position = 0;

	if (position == std::wstring::npos) {
		parsed_file.push_back(text.substr(current_position));
	}

	while (position != std::wstring::npos) {
		parsed_file.push_back(text.substr(current_position, position - current_position));

		current_position = position + 1;

		position = text.find(L" ", current_position);
	}

	parsed_file.push_back(text.substr(current_position));

	return parsed_file;
}

std::wstring SelectFile() {
	OPENFILENAME ofn;
	wchar_t szFileName[MAX_PATH] = L"";

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFilter = L"Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	ofn.lpstrDefExt = L"txt";

	if (GetOpenFileName(&ofn)) {
		return std::wstring(szFileName);
	}

	return L"";
}

std::wstring SelectFolder() {

	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

	BROWSEINFO browseInfo = { 0 };
	browseInfo.hwndOwner = NULL;
	browseInfo.pidlRoot = NULL;
	browseInfo.pszDisplayName = NULL;
	browseInfo.lpszTitle = L"";
	browseInfo.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;

	LPITEMIDLIST pidl = SHBrowseForFolder(&browseInfo);

	if (pidl != NULL) {
		WCHAR path[MAX_PATH];
		if (SHGetPathFromIDList(pidl, path)) {
			return path;
		}

		CoTaskMemFree(pidl);
	}

	CoUninitialize();
}

VOID ShowFile(HWND hFileInfo, const std::wstring& filepath) {

	std::wstring fp = SelectFile();

	std::wifstream file(fp);

	if (file.is_open()) {
		std::wstring line;

		while (std::getline(file, line)) {
			SendMessage(hFileInfo, LB_ADDSTRING, 0, (LPARAM)line.c_str());
		}

		file.close();
	}
	else {
		MessageBox(NULL, L"AHTUNG!!!! Не удалось открыть файл", L"AHTUNG!!!", MB_OK | MB_ICONERROR);
	}
}

VOID WriteToFile(const Report& report, const std::wstring& filepath) {
	std::wofstream out(gl_for_report_path);

	for (size_t i = 0; i < report.files.size(); i++) {
		out << "FILE " << i + 1 << "\n";
		out << "\tNAME:: " << report.files[i].fileName << "\n";
		out << "\tPATH:: " << report.files[i].filePath << "\n";
		out << "\tSIZE:: " << report.files[i].fileSize << " bytes\n\n";
	}
}

// === === === === === === === === === === //