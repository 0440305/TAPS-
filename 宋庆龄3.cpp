#include <windows.h>
#include <commdlg.h>
#include <wchar.h>
#include <mmsystem.h>

#define ID_SELECT_BUTTON 101
#define ID_PAUSE_BUTTON 102
#define ID_RESUME_BUTTON 103
#define ID_STOP_BUTTON 104
#define ID_VOLUME_UP_BUTTON 105
#define ID_VOLUME_DOWN_BUTTON 106

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

HWND hwndVideo;
BOOL isPaused = FALSE;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    MSG msg;
    WNDCLASSW wc = { 0 };
    wc.lpszClassName = L"VideoPlayerApp";
    wc.hInstance = hInstance;
    wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
    wc.lpfnWndProc = WndProc;
    RegisterClassW(&wc);
    CreateWindowW(wc.lpszClassName, L"音频播放器",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        100, 100, 580, 90, 0, 0, hInstance, 0);

    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE:
        CreateWindowW(L"BUTTON", L"音量+",
            WS_VISIBLE | WS_CHILD | WS_DISABLED,
            370, 10, 80, 30,
            hwnd, (HMENU)ID_VOLUME_UP_BUTTON, NULL, NULL);

        CreateWindowW(L"BUTTON", L"音量-",
            WS_VISIBLE | WS_CHILD | WS_DISABLED,
            460, 10, 80, 30,
            hwnd, (HMENU)ID_VOLUME_DOWN_BUTTON, NULL, NULL);

        CreateWindowW(L"BUTTON", L"选择文件",
            WS_VISIBLE | WS_CHILD,
            10, 10, 80, 30,
            hwnd, (HMENU)ID_SELECT_BUTTON, NULL, NULL);

        CreateWindowW(L"BUTTON", L"暂停",
            WS_VISIBLE | WS_CHILD | WS_DISABLED,
            100, 10, 80, 30,
            hwnd, (HMENU)ID_PAUSE_BUTTON, NULL, NULL);

        CreateWindowW(L"BUTTON", L"继续",
            WS_VISIBLE | WS_CHILD | WS_DISABLED,
            190, 10, 80, 30,
            hwnd, (HMENU)ID_RESUME_BUTTON, NULL, NULL);

        CreateWindowW(L"BUTTON", L"停止",
            WS_VISIBLE | WS_CHILD | WS_DISABLED,
            280, 10, 80, 30,
            hwnd, (HMENU)ID_STOP_BUTTON, NULL, NULL);

        break;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case ID_SELECT_BUTTON: // 选择按钮
        {
            OPENFILENAME ofn;
            TCHAR szFile[MAX_PATH] = { 0 };

            ZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.lpstrFile = szFile;
            ofn.lpstrFile[0] = '\0';
            ofn.hwndOwner = hwnd;
            ofn.nMaxFile = sizeof(szFile);
            ofn.lpstrFilter = L"Media Files\0*.mp3;*.mp4\0All Files\0*.*\0";
            ofn.nFilterIndex = 1;
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

            if (GetOpenFileName(&ofn)) {
                mciSendString(L"close all", NULL, 0, NULL); // 关闭当前打开的音频或视频
                wchar_t command[MAX_PATH + 32];
                swprintf_s(command, MAX_PATH + 32, L"open \"%s\" type mpegvideo alias video", szFile);
                mciSendString(command, NULL, 0, NULL);
                mciSendString(L"play video", NULL, 0, NULL);
                EnableWindow(GetDlgItem(hwnd, ID_PAUSE_BUTTON), TRUE);
                EnableWindow(GetDlgItem(hwnd, ID_RESUME_BUTTON), FALSE);
                EnableWindow(GetDlgItem(hwnd, ID_SELECT_BUTTON), FALSE);
                EnableWindow(GetDlgItem(hwnd, ID_STOP_BUTTON), TRUE);
                EnableWindow(GetDlgItem(hwnd, ID_VOLUME_UP_BUTTON), TRUE);
                EnableWindow(GetDlgItem(hwnd, ID_VOLUME_DOWN_BUTTON), TRUE);
            }
        }
        break;

        case ID_PAUSE_BUTTON: // 暂停按钮
            mciSendString(L"pause video", NULL, 0, NULL);
            EnableWindow(GetDlgItem(hwnd, ID_PAUSE_BUTTON), FALSE);
            EnableWindow(GetDlgItem(hwnd, ID_RESUME_BUTTON), TRUE);
            isPaused = TRUE;
            break;

        case ID_RESUME_BUTTON: // 继续按钮
            mciSendString(L"resume video", NULL, 0, NULL);
            EnableWindow(GetDlgItem(hwnd, ID_RESUME_BUTTON), FALSE);
            EnableWindow(GetDlgItem(hwnd, ID_PAUSE_BUTTON), TRUE);
            isPaused = FALSE;
            break;

        case ID_STOP_BUTTON: // 停止按钮
            mciSendString(L"stop video", NULL, 0, NULL);
            mciSendString(L"close video", NULL, 0, NULL);
            EnableWindow(GetDlgItem(hwnd, ID_PAUSE_BUTTON), FALSE);
            EnableWindow(GetDlgItem(hwnd, ID_RESUME_BUTTON), FALSE);
            EnableWindow(GetDlgItem(hwnd, ID_SELECT_BUTTON), TRUE);
            EnableWindow(GetDlgItem(hwnd, ID_STOP_BUTTON), FALSE);
            EnableWindow(GetDlgItem(hwnd, ID_VOLUME_UP_BUTTON), FALSE);
            EnableWindow(GetDlgItem(hwnd, ID_VOLUME_DOWN_BUTTON), FALSE);
            break;

        case ID_VOLUME_UP_BUTTON: // 音量增加按钮
            mciSendString(L"setaudio video volume +500", NULL, 0, NULL);
            break;

        case ID_VOLUME_DOWN_BUTTON: // 音量减少按钮
            mciSendString(L"setaudio video volume -500", NULL, 0, NULL);
            break;
        }
        break;

    case WM_DESTROY:
        mciSendString(L"close all", NULL, 0, NULL);
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProcW(hwnd, msg, wParam, lParam);
    }

    return 0;
}
