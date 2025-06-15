#include <iostream>
#include <windows.h>

#define internal static
#define local_persist static
#define global_variable static
#define WINDOW_NAME "HomemadeHeroWindow"

global_variable bool Running;
global_variable BITMAPINFO BitmapInfo;
global_variable void *BitmapMemory; // bitmap memory to write to from windows
global_variable HBITMAP BitmapHandle;
global_variable HDC BitmapDeviceContext;

// resize DIB (Device Independant Bitmap (image to write to))
internal void win32_ResizeDIBSection(int width, int height)
{
    if(BitmapHandle) DeleteObject(BitmapHandle);
    if(!BitmapDeviceContext) BitmapDeviceContext = CreateCompatibleDC(0);

    BITMAPINFOHEADER bmiHeader = {};
    bmiHeader.biSize = sizeof(bmiHeader);
    bmiHeader.biWidth = width;
    bmiHeader.biHeight = height;
    bmiHeader.biPlanes = 1;
    bmiHeader.biBitCount = 32;
    bmiHeader.biCompression = BI_RGB;
    bmiHeader.biSizeImage = 0;
    bmiHeader.biXPelsPerMeter = 0;
    bmiHeader.biYPelsPerMeter = 0;
    bmiHeader.biClrUsed = 0;
    bmiHeader.biClrImportant = 0;
    BitmapInfo.bmiHeader = bmiHeader;

    BitmapHandle =  CreateDIBSection(
        BitmapDeviceContext,
        &BitmapInfo,
        DIB_RGB_COLORS,
        &BitmapMemory, // requires ** - overwrites pointer to allocate new resource
        0, 0 // advanced Palette colors options -- irrelevant since using rgb
    );
}

internal void win32_UpdateWindow(HDC DeviceContext, int x, int y, int width, int height)
{
    // coppies rectangle to rectangle
    StretchDIBits(
        DeviceContext,
        x, y, width, height,
        x, y, width, height,
        BitmapMemory, // req *
        &BitmapInfo,
        DIB_RGB_COLORS,
        SRCCOPY // bitwise operation
    );
}

LRESULT CALLBACK win32_MainWindowCallback(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT Result = 0;
    switch (message)
    {
        case WM_SIZE:
        {
            RECT clientRect;
            GetClientRect(window, &clientRect);
            int width = clientRect.right - clientRect.left;
            int height = clientRect.bottom - clientRect.top;
            win32_ResizeDIBSection(width, height);
            printf("WM_SIZE\n");
        }
        break;
        case WM_DESTROY:
        {
            Running = false;
            printf("WM_DESTROY\n");
        }
        break;
        case WM_CLOSE:
        {
            Running = false;
            printf("WM_CLOSE\n");
        }
        break;
        case WM_ACTIVATEAPP:
        {
            printf("WM_ACTIVATEAPP\n");
        }
        break;
        case WM_PAINT:
        {
            PAINTSTRUCT Paint;
            HDC DeviceContext = BeginPaint(window, &Paint);
            int x = Paint.rcPaint.left;
            int y = Paint.rcPaint.top;
            int width = Paint.rcPaint.right - Paint.rcPaint.left;
            int height = Paint.rcPaint.bottom - Paint.rcPaint.top;
            win32_UpdateWindow(DeviceContext, x, y, width, height);
            EndPaint(window, &Paint);
        }
        break;
        default:
        {
            // printf("default\n");
            Result = DefWindowProc(window, message, wParam, lParam);
        }
        break;
    }
    return (Result);
}

void InitConsole()
{
    AllocConsole();
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);
    freopen("CONIN$", "r", stdin);
    std::cout << "Console initialized\n";
}

int CALLBACK WinMain(HINSTANCE appInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow)
{
    InitConsole();
    WNDCLASSEXA WindowClass = {};

    WindowClass.cbSize = sizeof(WNDCLASSEX);
    WindowClass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    WindowClass.lpfnWndProc = win32_MainWindowCallback; // window proceducre - controls behaviour
    WindowClass.hInstance = appInstance;                // application instance (the window)
    WindowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    WindowClass.lpszClassName = WINDOW_NAME;

    // register class
    if (!RegisterClassExA(&WindowClass))
    {
        MessageBoxA(NULL, "Failed to register Window", "ERROR", MB_OK);
        return -1;
    }

    // creating window
    HWND WindowHandle =
        CreateWindowExA(
            0, // Optional window styles.
            WINDOW_NAME,
            "Handmade Hero",                  // Window text
            WS_OVERLAPPEDWINDOW | WS_VISIBLE, // Window style
            CW_USEDEFAULT,
            CW_USEDEFAULT, // pos (x, y)
            CW_USEDEFAULT,
            CW_USEDEFAULT, // size (w, h)
            NULL,          // Parent window
            NULL,          // Menu
            appInstance,   // Instance handle
            NULL           // Additional application data
        );

    // message box and fail on failed to make window
    if (!WindowHandle)
    {
        MessageBoxA(NULL, "Failed to create window", "ERROR", MB_OK);
        return -1;
    }

    // create message queue
    Running = true;
    MSG message;
    while (Running)
    {
        // infinitely loop until exit msg (window closed)
        BOOL MessageResult = GetMessage(&message, 0, 0, 0);
        if (MessageResult > 0)
        {
            TranslateMessage(&message);
            DispatchMessageA(&message);
        }
        else
        {
            break;
        }
    }

    return 0;
}