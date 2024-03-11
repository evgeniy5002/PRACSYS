#pragma once
#include "Header.h"
#include "FileManager.h"
#include "Report.h"

struct WordsDetectionApp {
    VOID ResumeSearch();
    VOID StartSearch();
    VOID SuspendSearch();
    VOID StopSearch();

	VOID LoadTargetWords(const std::wstring filepath);

	static WordsDetectionApp* wda_ptr;
	WordsDetectionApp();
	static BOOL CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	BOOL WDA_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
	VOID WDA_OnCommand(HWND hwnd, INT id, HWND hwndCtl, UINT codeNotify);
	VOID WDA_OnClose(HWND hwnd);

	HWND hTargetWords;

	HWND hFilesFolderPath, hForCopyPath, hForCensoredPath, hForReportPath;
	HWND hFileInfo;
	HANDLE hFilterThread, hCopyThread, hCensorThread, hReportThread;
};