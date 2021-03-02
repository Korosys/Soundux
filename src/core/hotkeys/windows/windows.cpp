#if defined(_WIN32)
#include "../../global/globals.hpp"
#include "../hotkeys.hpp"
#include <Windows.h>
#include <chrono>

using namespace std::chrono_literals;

namespace Soundux::Objects
{
    HHOOK oKeyBoardProc;
    LRESULT CALLBACK keyBoardProc(int nCode, WPARAM wParam, LPARAM lParam)
    {
        if (nCode == HC_ACTION)
        {
            if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)
            {
                auto *info = reinterpret_cast<PKBDLLHOOKSTRUCT>(lParam);
                Globals::gHotKeys.onKeyDown(info->vkCode);
            }
            else if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP)
            {
                auto *info = reinterpret_cast<PKBDLLHOOKSTRUCT>(lParam);
                Globals::gHotKeys.onKeyUp(info->vkCode);
            }
        }
        return CallNextHookEx(oKeyBoardProc, nCode, wParam, lParam);
    }

    void Hotkeys::listen()
    {
        oKeyBoardProc = SetWindowsHookEx(WH_KEYBOARD_LL, keyBoardProc, nullptr, NULL);

        MSG message;
        while (!kill)
        {
            if (PeekMessage(&message, nullptr, 0, 0, PM_REMOVE) != 0)
            {
                TranslateMessage(&message);
                DispatchMessage(&message);
            }
        }

        UnhookWindowsHookEx(oKeyBoardProc);
    }

    std::string Hotkeys::getKeyName(const int &key)
    {
        auto scanCode = MapVirtualKey(key, MAPVK_VK_TO_VSC);

        CHAR name[128];
        int result = 0;
        switch (key)
        {
        case VK_LEFT:
        case VK_UP:
        case VK_RIGHT:
        case VK_DOWN:
        case VK_RCONTROL:
        case VK_RMENU:
        case VK_LWIN:
        case VK_RWIN:
        case VK_APPS:
        case VK_PRIOR:
        case VK_NEXT:
        case VK_END:
        case VK_HOME:
        case VK_INSERT:
        case VK_DELETE:
        case VK_DIVIDE:
        case VK_NUMLOCK:
            scanCode |= KF_EXTENDED;
        default:
            result = GetKeyNameTextA(scanCode << 16, name, 128);
        }

        return name;
    }
} // namespace Soundux::Objects
#endif