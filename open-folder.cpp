
// minGW to compile the code,
// g++ -O3 -static open-folder.cpp -o open-folder.exe
// or
// g++ -O3 open-folder.cpp -o open-folder.exe

// no uses Windows wincrypt.h
// no uses Windows API，
// no need C++ library
// self coded Base64 encoder, any g++ versions should be able to compile the code, avoid scope error

// JavaScript Base64 convert Unicode to UTF-8, 
// this C++ routine has Base64-to-UTF8，then converted to Wide String (wstring)


#include <windows.h>
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <cstdio>
#include <clocale>



// 1. 純 C++ 實作 Base64 解碼（不依賴任何 Windows 系統庫）
// 1. c++ base64 routine, no windows API/library
std::string PureBase64Decode(const std::string& in) {
    std::string out;
    std::vector<int> T(256, -1);
    for (int i = 0; i < 64; i++) 
        T["ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[i]] = i;

    int val = 0, valb = -8;
    for (char c : in) {
        if (T[c] == -1) continue;
        val = (val << 6) + T[c];
        valb += 6;
        if (valb >= 0) {
            out.push_back(char((val >> valb) & 0xFF));
            valb -= 8;
        }
    }
    return out;
}

// 2. 將解碼出來的 UTF-8 字串轉為 Windows 標準的 wstring
// 2. UTF8 -> Windows wstring
std::wstring Utf8ToWstring(const std::string& str) {
    if (str.empty()) return L"";
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}






int main() {
    std::setlocale(LC_ALL, ".UTF-8");
    SetConsoleOutputCP(CP_UTF8);

    std::wcout << L"[DEBUG] ===== open folder by Browser/webpage (C++ only) =====" << std::endl;
    std::wcout << L"xiaolaba 2021, testing done" << std::endl << std::endl;


    // 互斥鎖防禦
	// mutex, prevent loop of endless opening folder
    HANDLE hMutex = CreateMutexW(NULL, TRUE, L"Global\\MyFolderLauncherMutexLock");
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        if (hMutex) CloseHandle(hMutex);
        return 0; 
    }

    LPWSTR* argv;
    int argc;
    argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    
    if (argc > 1) {
        std::wstring cmdLineW(argv[1]);
        std::wcout << L"[DEBUG] 1. Browser argument: " << std::endl << cmdLineW << std::endl<< std::endl;
        
        // 移除前後引號
		// remove prefix/suffix ""
        while (!cmdLineW.empty() && (cmdLineW.front() == L'"' || cmdLineW.front() == L'\'')) {
            cmdLineW.erase(0, 1);
        }
        while (!cmdLineW.empty() && (cmdLineW.back() == L'"' || cmdLineW.back() == L'\'')) {
            cmdLineW.pop_back();
        }

        std::wcout << L"[DEBUG] 1. removed "" cmdLineW: " << std::endl << cmdLineW << std::endl<< std::endl;

        // 移除協定前綴
		// remove prefic to obtian raw folder string
        std::wstring prefix1 = L"myexplorersubfolder://";
        std::wstring prefix2 = L"myexplorersubfolder:";
        if (cmdLineW.find(prefix1) == 0) cmdLineW.erase(0, prefix1.length());
        else if (cmdLineW.find(prefix2) == 0) cmdLineW.erase(0, prefix2.length());
        
        while (!cmdLineW.empty() && (cmdLineW.back() == L'/' || cmdLineW.back() == L'\\')) {
            cmdLineW.pop_back();
        }

        std::wcout << L"[DEBUG] 1. removed myexplorersubfolder:// cmdLineW: " << std::endl << cmdLineW << std::endl<< std::endl;


        // wstring to string, for Base64 
        std::string base64Str(cmdLineW.begin(), cmdLineW.end());
        std::cout << "[DEBUG] 2. Base64ed string: " << std::endl << base64Str << std::endl << std::endl;

        // 【核心解碼】
		// decoding
        std::string decodedUtf8 = PureBase64Decode(base64Str);
        std::wstring realPath = Utf8ToWstring(decodedUtf8);

        if (!realPath.empty()) {
            std::wcout << L"[DEBUG] 3. realPath string: " << std::endl << realPath << std::endl<< std::endl;
            
            // 安全檢查：防範注入
			// safty check the string
            if (realPath.find(L'"') == std::wstring::npos && realPath.find(L'\'') == std::wstring::npos) {
                std::wcout << L"[DEBUG] Invoke Windows File Expoloer... this debug window will remains 20sec" << std::endl;
                ShellExecuteW(NULL, L"open", realPath.c_str(), NULL, NULL, SW_SHOWNORMAL);
            } else {
                std::wcout << L"[DEBUG] grabage char detected，stop here." << std::endl;
            }
        } else {
            std::wcout << L"[DEBUG] Base64 decode went wrong!" << std::endl;
        }
    }
    
    LocalFree(argv);
    ReleaseMutex(hMutex);
    CloseHandle(hMutex);
    
    Sleep(20000); 
    return 0;
}