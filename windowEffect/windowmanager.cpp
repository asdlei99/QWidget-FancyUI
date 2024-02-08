#include "windowmanager.h"


//下面两条预处理是MSVC的语法，若使用MinGW编译，请修改配置文件，在配置文件中链接库，并删去这两句预处理命令
//参考：https://blog.csdn.net/qqwangfan/article/details/105512532
#pragma comment(lib,"user32.lib")
#pragma comment(lib, "dwmapi.lib")

BOOL(WINAPI* WindowManager::SetWindowCompositionAttributePtr)(HWND, WINDOWCOMPOSITIONATTRIBDATA*)
    {(BOOL(WINAPI*)(HWND, WINDOWCOMPOSITIONATTRIBDATA*))GetProcAddress(GetModuleHandleW(L"user32.dll"), "SetWindowCompositionAttribute")};


LRESULT WindowManager::moveWindow(const HWND hwnd)
{
    ReleaseCapture();
    return SendMessage_(hwnd, WM_SYSCOMMAND, SC_MOVE + HTCAPTION, 0);
}

bool WindowManager::disableMaximize(const HWND hWnd)
{
    LONG_PTR value = SetWindowLongPtr_(hWnd,GWL_STYLE, GetWindowLongPtr_(hWnd,GWL_STYLE)& ~WS_MAXIMIZEBOX);
    return value == 0 && GetLastError() != 0 ? false : !GetLastError();
}

bool WindowManager::disableMinimize(const HWND hWnd)
{
    return SetWindowLongPtr_(hWnd,GWL_STYLE, GetWindowLongPtr_(hWnd,GWL_STYLE)& ~WS_MINIMIZEBOX) == 0 && GetLastError() != 0 ? false : !GetLastError();
}

bool WindowManager::disablesResizingViaBorders(const HWND hWnd)
{
    return SetWindowLongPtr_(hWnd,GWL_STYLE, GetWindowLongPtr_(hWnd,GWL_STYLE)& ~WS_SIZEBOX) == 0 && GetLastError() != 0 ? false : !GetLastError();
}

bool WindowManager::disableTitleBar(const HWND hWnd)
{
    //有缺陷，顶部会留有一条6个像素白色边，暂时未找到较好的消除方法，与DWM有关
    //参考：https://stackoverflow.com/questions/39731497/create-window-without-titlebar-with-resizable-border-and-without-bogus-6px-whit
    //但是使用配合DwmWindowEffect中的效果可以在视觉上消除影响
    //可以通过原先标题栏的位置进行拖动或双击放大一次，拖动后显示标题栏，调整大小后消失且不再可拖动
    LONG_PTR style = GetWindowLongPtr_(hWnd,GWL_STYLE);
    LONG_PTR value = SetWindowLongPtr_(hWnd,GWL_STYLE,style & ~WS_SYSMENU & ~WS_CAPTION);
    return value == 0 && GetLastError() != 0 ? false : !GetLastError();
}

WindowManager::WindowManager() {}

BOOL WindowManager::SetWindowCompositionAttribute( HWND hWnd, WINDOWCOMPOSITIONATTRIBDATA *data)
{
    return SetWindowCompositionAttributePtr(hWnd,data);
}

bool WindowManager::DwmSetWindowAttribute( HWND hwnd, DWORD dwAttribute, LPCVOID pvAttribute, DWORD cbAttribute)
{
    return SUCCEEDED(::DwmSetWindowAttribute(hwnd, dwAttribute, pvAttribute, cbAttribute));
}

bool WindowManager::DwmExtendFrameIntoClientArea(HWND hWnd, const MARGINS *pMarInset)
{
    return SUCCEEDED(::DwmExtendFrameIntoClientArea(hWnd,pMarInset));
}

LRESULT WindowManager::SendMessage_(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    return SendMessage(hWnd,Msg,wParam,lParam);
}

bool WindowManager::DwmEnableBlurBehindWindow(HWND hWnd, const DWM_BLURBEHIND *pBlurBehind)
{
    //从 Windows 8 开始，由于窗口的呈现方式发生了样式更改，因此调用此函数不会产生模糊效果，而是单纯的透明效果。
    return  SUCCEEDED(::DwmEnableBlurBehindWindow(hWnd, pBlurBehind));
}

LONG_PTR WindowManager::SetWindowLongPtr_(HWND hWnd, int nIndex, LONG_PTR dwNewLong)
{
    SetLastError(0);
    return SetWindowLongPtr(hWnd,nIndex,dwNewLong);
}

LONG_PTR WindowManager::GetWindowLongPtr_(HWND hWnd, int nIndex)
{
    return GetWindowLongPtr(hWnd,nIndex);
}
