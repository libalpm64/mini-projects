#include "codevault.h"
#include "Includes.h"


std::string promo = "codevault Protection v1.0.0";
std::string promo2 = "Please note that we are not responsible for any actions, outcomes, or consequences that occur after the point of interaction.";

codevault_Export void codevault(std::string key, std::string webhook, bool imgui_support, HWND imgui_window_handle)
{
    Hide_Function(); srand(static_cast<unsigned int>(time(nullptr))); char buffer[MAX_PATH]; webhook_link = std::wstring(webhook.begin(), webhook.end());
    bool selfdelete = false;
    bool antitamper = false;

    if (!main_called)
    {
         main_called = Encrypt(1); ban_system = true; imgui = imgui_support;  SetPrivileges(); PreventUnload(); HideProcess();
         for (int i = Encrypt(0); i < Encrypt(25); ++i) { int3bs(); } Crash_Hide(); disable_titan_hide(); if (IsHyperHideDebuggingProcess()) { HandleAttack(Encrypt("Hyperhide #1"), Encrypt(1)); }
         ProtectProcess(); NoAccess(); Wipe_Section_Infomation(); if (!selfdelete) { RenameExecutable(); } else { SelfDelete(); } if (!imgui_support) { Antidll(); } Initalize_Object();
         if (antitamper) { integrityChecker.get_initial_checksum(); } Imports.hash_api_calls();  Initialize_Threads();
         for (int i = Encrypt(0); i < Encrypt(25); ++i) { int3bs(); } Crash_Hide(); disable_titan_hide(); if (IsHyperHideDebuggingProcess()) { HandleAttack(Encrypt("Hyperhide #1"), Encrypt(1)); }
    }
    else
    {
        flag = Encrypt(1); HandleAttack(Encrypt("Need To Call codevault(), Before Other codevault Functions"), Encrypt(0));
    }
    Hide_Function_End();
}

codevault_Export void check()
{
    Hide_Function(); 
    if (main_called && tls_ran)
    {
        CheckDebugger(0); 
    }
    else
    {
        flag = Encrypt(1); HandleAttack(Encrypt("Failure To Call codevault(), Before Other codevault Functions"), Encrypt(0));
    }
    Hide_Function_End();
}

codevault_Export void unban()
{
    RemoveBans();
}

codevault_Export void Print(const std::string text)
{
    Hide_Function();
    if (authorized && main_called && tls_ran)
    {
        for (int i = Encrypt(0); i < Encrypt(25); ++i) { int3bs(); } CheckDebugger(0);
        DWORD written;
        HANDLE hConsole = Imports.get_std_handle(STD_OUTPUT_HANDLE);
        Lazy(WriteFile).get()(hConsole, text.c_str(), text.size(), &written, NULL);
    }
    else
    {
        flag = Encrypt(1); HandleAttack(Encrypt("Failure To Call codevault(), Before Other codevault Functions"), Encrypt(0));
    }
    Hide_Function_End();
}

codevault_Export void Sleeptime(int milliseconds)
{
    Hide_Function();
    if (authorized && main_called && tls_ran)
    {
        for (int i = Encrypt(0); i < Encrypt(25); ++i) { int3bs(); } CheckDebugger(0);
        auto start = std::chrono::steady_clock::now();
        auto duration = std::chrono::milliseconds(milliseconds);
        while (std::chrono::steady_clock::now() - start < duration) {}
    }
    else
    {
        flag = Encrypt(1); HandleAttack(Encrypt("Failure To Call codevault(), Before Other codevault Functions"), Encrypt(0));
    }
    Hide_Function_End();
}

codevault_Export void SafeThreads(codevaultFunction func)
{
    Hide_Function();

    if (authorized && main_called && tls_ran)
    {
        for (int i = Encrypt(0); i < Encrypt(25); ++i) { int3bs(); } CheckDebugger(0);  ThreadData* data = new ThreadData{ func }; 
        for (int i = Encrypt(0); i < Encrypt(3); ++i) { CALL(&Create_Hashed, NULL, Encrypt(0), &codevault_切都将变得毫无乐趣和更多变得爱得毫无变得毫变得得, NULL, Encrypt(0), NULL); } // fake threads
        Create_Hashed(NULL, Encrypt(0), &ThreadpoolCallback, data, Encrypt(0), NULL);
        for (int i = Encrypt(0); i < Encrypt(3); ++i) { CALL(&Create_Hashed, NULL, Encrypt(0), &codevault_切都将变得毫无乐趣和更多变得爱得毫无变得毫变得得, NULL, Encrypt(0), NULL); } // fake threads
    }
    else
    {
        flag = Encrypt(1); HandleAttack(Encrypt("Failure To Call codevault(), Before Other codevault Functions"), Encrypt(0));
    }

    Hide_Function_End();
}

codevault_Export void Exit()
{
    Hide_Function();
    Imports.exit_application();
    Hide_Function_End();
}

codevault_Export void AllThreads()
{
    Hide_Function();

    if (authorized && main_called && tls_ran)
    {
        for (int i = Encrypt(0); i < Encrypt(25); ++i) { int3bs(); } CheckDebugger(0);
        Protect_All_Threads();
    }
    else
    {
        flag = Encrypt(1); HandleAttack(Encrypt("Failure To Call codevault(), Before Other codevault Functions"), Encrypt(0));
    }

    Hide_Function_End();
}


