#if defined(UNICODE) && !defined(_UNICODE)
    #define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
    #define UNICODE
#endif

#include <tchar.h>
#include <windows.h>
#include <gdiplus.h>
#include "PlaneClass.h"
#include "FlyingMonsterClass.h"
#include "BulletClass.h"

using namespace Gdiplus;

PlaneClass Plane;
FlyingMonsterClass Monster[MAX_MONSTER];
int OutPutCount = 0;
int Score = 0;
bool IsPause = false;       //暂停标志变量

LRESULT CALLBACK WindowProcedure (HWND,UINT,WPARAM,LPARAM);

void DrawPicture(HDC,wchar_t*,int,int,int,int);


/*  Make the class name into a global variable  */
TCHAR szClassName[ ] = _T("PlaneHit");

int WINAPI WinMain (HINSTANCE hThisInstance,HINSTANCE hPrevInstance,LPSTR lpszArgument,int nCmdShow)
{
    HWND hwnd;               /* This is the handle for our window */
    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */

    ULONG_PTR Token;
    GdiplusStartupInput Input;

    GdiplusStartup(&Token,&Input,0);

    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
    wincl.cbSize = sizeof (WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;                 /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use Windows's default colour as the background of the window */

    wincl.hbrBackground = 0;//CreatePatternBrush(Backgroud);

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl))
        return 0;

    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx (
           0,                   /* Extended possibilites for variation */
           szClassName,         /* Classname */
           _T("PlaneHit"),       /* Title Text */
           WS_OVERLAPPEDWINDOW^WS_THICKFRAME, /* default window */
           CW_USEDEFAULT,       /* Windows decides the position */
           CW_USEDEFAULT,       /* where the window ends up on the screen */
           800,                 /* The programs width */
           600,                 /* and height in pixels */
           HWND_DESKTOP,        /* The window is a child-window to desktop */
           NULL,                /* No menu */
           hThisInstance,       /* Program Instance handler */
           NULL                 /* No Window Creation data */
           );

    /* Make the window visible on the screen */
    ShowWindow (hwnd,nCmdShow);

    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (GetMessage (&messages,NULL,0,0))
    {
        /* Translate virtual-key messages into character messages */
        TranslateMessage(&messages);
        /* Send message to WindowProcedure */
        DispatchMessage(&messages);
    }

    GdiplusShutdown(Token);
    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return messages.wParam;
}


/*  This function is called by the Windows function DispatchMessage()  */

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static int cxClient;
    static int cyClient;
    static HDC hDCBuffer;
    static HBITMAP hBitmap;
    switch (message)                  /* handle the messages */
    {
        case WM_SIZE:
        {
            cxClient = LOWORD(lParam);
            cyClient = HIWORD(lParam);
            break;
        }

        case WM_TIMER:
        {
            if(!IsPause)
            {
                Plane.FillBullet();
                HDC hdc = GetDC(hwnd);

                if(OutPutCount % 5 == 0 && OutPutCount >= 5)
                {
                    Plane.Bullet[OutPutCount/5-1].IsOutPut = true;
                }
                OutPutCount++;
                if(OutPutCount > 30)
                {
                    OutPutCount = 0;
                }

                Plane.ReFillBullet();
                Plane.MoveBullet();

                for(int i = 0;i < MAX_MONSTER;i++)
                {
                    Plane.Destroyed(Monster[i]);
                    if(Plane.IsDead)
                    {
                        KillTimer(hwnd,1);
                        DrawPicture(hdc,(wchar_t*)L"Res\\Boom.png",Monster[i].x,Monster[i].y,Monster[i].Width,Monster[i].Height);
                    }
                }

                for(int i = 0;i < MAX_MONSTER;i++)
                {
                    Monster[i].Move(Plane.x,Plane.y);
                    for(int j = 0;j < MAX_BULLET;j++)
                    {
                        Monster[i].Destroyed(Plane.Bullet[j]);
                    }
                    if(Monster[i].IsDead)
                    {
                        DrawPicture(hdc,(wchar_t*)L"Res\\Boom.png",Monster[i].x,Monster[i].y,Monster[i].Width,Monster[i].Height);
                        Monster[i].ReLive(Plane.x,Plane.y,Plane.Width,Plane.Height);
                        Score+=10;
                    }
                }

                InvalidateRect(hwnd,0,true);

                ReleaseDC(hwnd,hdc);
            }
            else
            {
                SetWindowText(hwnd,"PlaneHit 游戏暂停中");
            }

            break;
        }

        case WM_PAINT:
        {
            char ScoreBuffer[12];

            HFONT hFont = CreateFont(40,14,0,0,400,FALSE, FALSE, FALSE,DEFAULT_CHARSET,OUT_CHARACTER_PRECIS,CLIP_CHARACTER_PRECIS,DEFAULT_QUALITY,FF_DONTCARE,TEXT("微软雅黑"));

            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd,&ps);
            hDCBuffer = CreateCompatibleDC(hdc);
            hBitmap = (HBITMAP)LoadImage(0,"Res//sky.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE);

            wsprintf(ScoreBuffer,"分数:%d",Score);
            SelectObject(hDCBuffer,hFont);
            SelectObject(hDCBuffer,hBitmap);

            DrawPicture(hDCBuffer,Plane.PngName,Plane.x,Plane.y,Plane.Width,Plane.Height);

            DrawPicture(hDCBuffer,Plane.Bullet[0].PngName,Plane.Bullet[0].x,Plane.Bullet[0].y,Plane.Bullet[0].width,Plane.Bullet[0].height);
            DrawPicture(hDCBuffer,Plane.Bullet[1].PngName,Plane.Bullet[1].x,Plane.Bullet[1].y,Plane.Bullet[1].width,Plane.Bullet[1].height);
            DrawPicture(hDCBuffer,Plane.Bullet[2].PngName,Plane.Bullet[2].x,Plane.Bullet[2].y,Plane.Bullet[2].width,Plane.Bullet[2].height);
            DrawPicture(hDCBuffer,Plane.Bullet[3].PngName,Plane.Bullet[3].x,Plane.Bullet[3].y,Plane.Bullet[3].width,Plane.Bullet[3].height);
            DrawPicture(hDCBuffer,Plane.Bullet[4].PngName,Plane.Bullet[4].x,Plane.Bullet[4].y,Plane.Bullet[4].width,Plane.Bullet[4].height);

            SetBkMode(hDCBuffer,TRANSPARENT);
            TextOut(hDCBuffer,0,0,ScoreBuffer,strlen(ScoreBuffer));
            if(Plane.IsDead)
            {
                DrawPicture(hdc,(wchar_t*)L"Res\\Boom.png",Plane.x,Plane.y,Plane.Width,Plane.Height);
                MessageBox(hwnd,"您已被击中!","游戏结束",MB_OK);
                SendMessage(hwnd,WM_DESTROY,0,0);
                return 0;
            }

            if(!IsPause)
            {
                SetWindowText(hwnd,"PlaneHit By Vmpy");
            }

            for(int i = 0; i < MAX_MONSTER;i++)
            {
                DrawPicture(hDCBuffer,Monster[i].PngName,Monster[i].x,Monster[i].y,Monster[i].Width,Monster[i].Height);
            }

            BitBlt(hdc,0,0,cxClient,cyClient,hDCBuffer,0,0,SRCCOPY);

            EndPaint(hwnd,&ps);
            DeleteDC(hDCBuffer);
            DeleteObject(hBitmap);
            break;
        }

        case WM_CREATE:
        {
            SetTimer(hwnd,1,100,0);
            break;
        }

        case WM_KEYDOWN:
        {
            switch(wParam)
            {
                case 'P':IsPause = !IsPause;break;  //当P键按下时，游戏暂停
                default:
                {
                    Plane.Move(wParam);
                    break;
                }
            }

            if(GetKeyState(VK_SHIFT) < 0)   //当Shift按下时，加速移动.
            {
                Plane.Move(wParam);
                Plane.Move(wParam);
                Plane.Move(wParam);
                Plane.Move(wParam);
            }
            break;
        }

        case WM_DESTROY:
        {
            PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
            break;
        }
        default:                      /* for messages that we don't deal with */
            return DefWindowProc (hwnd, message, wParam, lParam);
    }
    return 0;
}

void DrawPicture(HDC hdc,wchar_t* FileName,int x,int y,int width,int height)
{
    Image image(FileName);
    Graphics Graph(hdc);
    Graph.DrawImage(&image,Rect(x,y,width,height));
}
