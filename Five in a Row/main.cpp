//Win32���ڳ���. 
#include <ctime>
#include <cstdlib>
#include <windows.h>
#include <windowsx.h>
#include "Map.h"
#include "resource.h"

class Map PlayChess;

LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);

/*���¿�ʼ��Ϸ����������.*/
void ReStartGame(void);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASSEX wc; /* A properties struct of our window */
    HWND hwnd; /* A 'HANDLE', hence the H, or a pointer to our window */
    MSG msg; /* A temporary location for all messages */

    /* zero out the struct and set the stuff we want to modify */
    memset(&wc,0,sizeof(wc));
    wc.style         = CS_HREDRAW|CS_VREDRAW;
    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.lpfnWndProc   = WndProc;/* This is where we will send messages to */
    wc.hInstance     = hInstance;
    wc.hCursor       = LoadCursor(NULL,IDC_ARROW);
    
    wc.hbrBackground = CreateSolidBrush(RGB(85,120,100));
    wc.lpszClassName = "Five_in_a_raw";
    wc.hIcon         = LoadIcon(NULL,IDI_APPLICATION); /* Load a standard icon */
    wc.hIconSm       = LoadIcon(NULL,IDI_APPLICATION); /* use the name "A" to use the project icon */
    wc.lpszMenuName  = MAKEINTRESOURCE(IDR_FIAR_MENU); 
    
    if(!RegisterClassEx(&wc))
    {
        MessageBox(NULL,TEXT("������ע��ʧ��!"),TEXT("����!"),MB_ICONEXCLAMATION|MB_OK);
        return 0;
    }

    hwnd = CreateWindowEx(NULL,"Five_in_a_raw",TEXT("������"),WS_VISIBLE|WS_OVERLAPPEDWINDOW^WS_SIZEBOX^WS_MAXIMIZEBOX,
        CW_USEDEFAULT, /* x */
        CW_USEDEFAULT, /* y */
        700, /* width */
        700, /* height */
        NULL,LoadMenu(hInstance,MAKEINTRESOURCE(IDR_FIAR_MENU)),hInstance,NULL);
        
    ShowWindow(hwnd,nCmdShow);
    UpdateWindow(hwnd);

    if(hwnd == NULL)
    {
        MessageBox(NULL,TEXT("���ڴ���ʧ��!"),TEXT("����!"),MB_ICONEXCLAMATION|MB_OK);
        return 0;
    }

    srand((unsigned int)time(nullptr));
    while(GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg); /* Translate key codes to chars if present */
        DispatchMessage(&msg); /* Send it to WndProc */
    }
    return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
    static enum _ChessMan WhoIsWin;
    static int cxClient;
    static int cyClient;
    static HWND hButtonBlack;
    static HWND hButtonWhite;
    static HWND hStaticText;
    static HDC hdc;
    static HFONT hFont;
    static HPEN hPenOfMap = CreatePen(PS_SOLID,2,RGB(0,0,0));
    static PAINTSTRUCT ps;
    
    switch(Message)
    {
        case WM_CREATE:
        {
            hFont = CreateFont(20,10,0,0,FW_THIN,false,false,false,GB2312_CHARSET,OUT_CHARACTER_PRECIS,
            CLIP_CHARACTER_PRECIS,DEFAULT_QUALITY,FF_MODERN,TEXT("����"));
            cxClient = ((LPCREATESTRUCT)(lParam))->cx;
            cyClient = ((LPCREATESTRUCT)(lParam))->cy;
            break;
        }
        
        case WM_SIZE:
        {
            if(PlayChess.AI_s_chess == None)
            {
                InvalidateRect(hwnd,NULL,false);
                DestroyWindow(hButtonBlack);
                DestroyWindow(hButtonWhite);
                DestroyWindow(hStaticText);
                hButtonBlack = CreateWindowEx(NULL,TEXT("button"),TEXT("��ɫ"),WS_CHILD|BS_PUSHBUTTON,
                                0,cyClient/3,cxClient,50,hwnd,(HMENU)1,(HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),
                                NULL);
                SendMessage(hButtonBlack,WM_SETFONT,(WPARAM)hFont,NULL);
                ShowWindow(hButtonBlack,SW_SHOW);
                hButtonWhite = CreateWindowEx(NULL,TEXT("button"),TEXT("��ɫ"),WS_CHILD|BS_PUSHBUTTON,
                                0,cyClient/3+100,cxClient,50,hwnd,(HMENU)2,(HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),
                                NULL);
                SendMessage(hButtonWhite,WM_SETFONT,(WPARAM)hFont,NULL);
                ShowWindow(hButtonWhite,SW_SHOW);
                
                hStaticText = CreateWindowEx(WS_EX_TRANSPARENT,TEXT("static"),TEXT("ѡ����Ҫѡ�õ�������ɫ:"),WS_CHILD|SS_CENTER|SS_CENTERIMAGE,
                                0,0,cxClient,50,hwnd,(HMENU)3,(HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),
                                NULL);
                SendMessage(hStaticText,WM_SETFONT,(WPARAM)hFont,NULL);
                SetClassLong(hStaticText,GCL_HBRBACKGROUND,(LONG)((HBRUSH)(COLOR_WINDOW+1)));
                ShowWindow(hStaticText,SW_SHOW);    
            }
            break;
        }
        
        case WM_CTLCOLORSTATIC:
        {
            hdc = (HDC)(wParam);
            SetBkMode(hdc,TRANSPARENT);
            SetTextColor(hdc,RGB(255,255,255));
            return (INT_PTR)GetStockObject(NULL_BRUSH); //����ɫ��ˢ.
        }
        
        case WM_CTLCOLORBTN:
        {
            hdc = (HDC)(wParam);
            SetBkMode(hdc,TRANSPARENT);
            return (INT_PTR)GetStockObject(NULL_BRUSH); //����ɫ��ˢ.
        }
        
        case WM_PAINT:
        {
            hdc = BeginPaint(hwnd,&ps);
            if(PlayChess.AI_s_chess != None)
            {
                SelectObject(hdc,hPenOfMap);
                PlayChess.DrawChess(hdc);
                PlayChess.IsWin(hwnd);
            }
            EndPaint(hwnd,&ps);
            break; 
        }
        
        case WM_LBUTTONUP:
        {
            if(PlayChess.PutAChess(GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam)))
            {
                PlayChess.PutAIChess();
                InvalidateRect(hwnd,NULL,false);
            }
            break;
        } 
        
        case WM_COMMAND:
        {
            if(lParam)//��ť�ؼ�. 
            {
                if(PlayChess.AI_s_chess == None)
                {
                    switch(LOWORD(wParam))
                    {
                        case 1:PlayChess.SetChessColor(White,Black);break;
                        case 2:PlayChess.SetChessColor(Black,White);break;
                    }
                    DestroyWindow(hStaticText);
                    DestroyWindow(hButtonBlack);
                    DestroyWindow(hButtonWhite);
                    InvalidateRect(hwnd,NULL,true);
                }
            }
            else //�˵� 
            {
                switch(LOWORD(wParam))
                {
                    case ID_RESTART:ReStartGame();SendMessage(hwnd,WM_SIZE,0,0);break;
                }
            }
            break;
        }
        
        case WM_DESTROY:
        {
            DeleteObject(hFont);
            DeleteObject(hPenOfMap);
            PostQuitMessage(0);
            break;
        }
        default:
            return DefWindowProc(hwnd,Message,wParam,lParam);
    }
    
    return 0;
}

/**
*�ؿ���Ϸ����������.
*Args:
*   None.
*Returns:
*   None.
*/
void ReStartGame(void)
{
    int Tmpx,Tmpy;
    PlayChess.AI_s_chess = None;
    PlayChess.Player_s_chess = None;
    for(Tmpx = 0;Tmpx < 15;Tmpx++)
    {
        for(Tmpy = 0;Tmpy < 15;Tmpy++)
        {
            PlayChess.MapChess[Tmpx][Tmpy] = None;
        }
    }
    return;
}

/**
*class Map���캯��.
*Args:
*   None.
*Returns:
*   None.
*/
Map::Map(void)
{
    int Tmpx,Tmpy;
    for(Tmpx = 0;Tmpx < Height;Tmpx++)
    {
        for(Tmpy = 0;Tmpy < Width;Tmpy++)
        {
            MapChess[Tmpx][Tmpy] = None;
            MapPoint[Tmpx][Tmpy].x = RectWidth*Tmpx+90; //�������ӵĳ�����Ϊ�������ƶ�����. 
            MapPoint[Tmpx][Tmpy].y = RectWidth*Tmpy+85;
        }
    }
}

/**
*����ѡ������������Ӻ͵������ӵ���ɫ.
*Args:
*   AI:AI������ɫ.
*   Person:���������ɫ. 
*Returns:
*   None.
*/
void Map::SetChessColor(enum _ChessMan AI,enum _ChessMan Person)
{
    AI_s_chess = AI;
    Player_s_chess = Person;
    return;
}

/**
*�����������һ������.
*Args:
*   x:����X����.
*   y:����Y����. 
*Returns:
*   TRUE:����һ�����ӣ�����TRUE.
*   FALSE:û�з���һ�����ӣ�����FALSE. 
*/
bool Map::PutAChess(int x,int y)
{
    int Tmpx,Tmpy;
    for(Tmpx = 0;Tmpx < Width;Tmpx++)
    {
        for(Tmpy = 0;Tmpy < Height;Tmpy++)
        {
            //����MapPoint�е�ĳһλ�ý����ݴ���. 
            if((x - MapPoint[Tmpx][Tmpy].x < 10 && x - MapPoint[Tmpx][Tmpy].x > 0 && y - MapPoint[Tmpx][Tmpy].y > 0 && y - MapPoint[Tmpx][Tmpy].y < 10) 
            || (MapPoint[Tmpx][Tmpy].x - x < 10 && MapPoint[Tmpx][Tmpy].x - x > 0 &&  MapPoint[Tmpx][Tmpy].y - y > 0 && MapPoint[Tmpx][Tmpy].y - y < 10)
            || (MapPoint[Tmpx][Tmpy].x - x < 10 && MapPoint[Tmpx][Tmpy].x - x > 0 &&  MapPoint[Tmpx][Tmpy].y - y < 0 && MapPoint[Tmpx][Tmpy].y - y > -10)
            || (x - MapPoint[Tmpx][Tmpy].x < 10 && x - MapPoint[Tmpx][Tmpy].x > 0 &&  MapPoint[Tmpx][Tmpy].y - y > 0 && MapPoint[Tmpx][Tmpy].y - y < 10))
            {
                if(MapChess[Tmpx][Tmpy] == None)    //��������Ǹ���λ. 
                {
                    MapChess[Tmpx][Tmpy] = Player_s_chess;
                    return true;
                }
                return false;
            }
        }
    }
    return false;
}

/**
*��������.
*Args:
*   hdc:�豸�����ľ��.
*Returns:
*   None.
*/
void Map::DrawChess(HDC& hdc)
{
    int Tmp;
    for(Tmp = 0;Tmp < 15;Tmp++)
    {
        MoveToEx(hdc,PlayChess.MapPoint[Tmp][0].x,PlayChess.MapPoint[Tmp][0].y,nullptr);
        LineTo(hdc,PlayChess.MapPoint[Tmp][14].x,PlayChess.MapPoint[Tmp][14].y);
        MoveToEx(hdc,PlayChess.MapPoint[0][Tmp].x,PlayChess.MapPoint[0][Tmp].y,nullptr);
        LineTo(hdc,PlayChess.MapPoint[14][Tmp].x,PlayChess.MapPoint[14][Tmp].y);
    }
    int Tmpx,Tmpy;
    for(Tmpx = 0;Tmpx < 15;Tmpx++)
    {
        for(Tmpy = 0;Tmpy < 15;Tmpy++)
        {
            if(MapChess[Tmpx][Tmpy] == Black)
            {
                SelectObject(hdc,hBlackBrush);
                Ellipse(hdc,MapPoint[Tmpx][Tmpy].x - Radii,MapPoint[Tmpx][Tmpy].y - Radii,MapPoint[Tmpx][Tmpy].x + Radii,MapPoint[Tmpx][Tmpy].y + Radii);
            }
            else if(MapChess[Tmpx][Tmpy] == White)
            {
                SelectObject(hdc,(HBRUSH)GetStockObject(NULL_BRUSH));
                Ellipse(hdc,MapPoint[Tmpx][Tmpy].x - Radii,MapPoint[Tmpx][Tmpy].y - Radii,MapPoint[Tmpx][Tmpy].x + Radii,MapPoint[Tmpx][Tmpy].y + Radii);
            }
        }
    }
}

/**
*AI����һ������.
*Args:
*   None.
*Returns:
*   TRUE:Meaningless.
*/
bool Map::PutAIChess(void)
{
    int Posx,Posy;
    CountPlayerChess(Posx,Posy);
    if(Posx == -1)
    {
        CountAIChess(Posx,Posy);
        if(Posx == -1)
        {
            do{
                Posx = rand()%15;
                Posy = rand()%15;
            }while(MapChess[Posx][Posy] != None);
        }
        MapChess[Posx][Posy] = AI_s_chess;
    }
    else
    {
        MapChess[Posx][Posy] = AI_s_chess;
        return true;
    }
    return true;
}

/**
*�ж�AI�������Ƿ�������һ�еĿ���.
*Args:
*   Tmpx:�����λX����[�ҵ�������]��δ�ҵ������ԣ�Ϊ-1.
*   Tmpx:�����λY����[�ҵ�������]��δ�ҵ������ԣ�Ϊ-1.
*Returns:
*   None.
*/
int Map::CountAIChess(int &Tmpx,int &Tmpy)
{
    int x,y;
    int PossiblePosx,PossiblePosy;
    int count = 0;
    int Tmp = 0;
    int TmpPos;
    for(x = 0;x < Width;x++)
    {
        for(y = 0;y < Height;y++)
        {
            if(MapChess[x][y] == AI_s_chess)
            {
                /*��һ����:�������Ƿ�AI���ӳ�����ȫ����*/
                count = 0;
                PossiblePosx = x;
                PossiblePosy = y;
                Tmp = 0;
                while(Tmp <= 5)
                {
                    if(MapChess[PossiblePosx][PossiblePosy] == AI_s_chess)
                    {
                        count++;
                    }
                    Tmp++;
                    PossiblePosy++;
                }
                while(MapChess[x][PossiblePosy] == None)
                {
                    PossiblePosy--;
                }
                if(count >= 3)
                {
                    TmpPos = PossiblePosy;
                    PossiblePosx = x;
                    PossiblePosy = y-1;
                    Tmp = 0;
                    while(Tmp <= 5)
                    {
                        if(MapChess[PossiblePosx][PossiblePosy] == Player_s_chess && Tmp != 0)
                        {
                            break;
                        }
                        if(MapChess[PossiblePosx][PossiblePosy] == None)
                        {
                            Tmpx = PossiblePosx;
                            Tmpy = PossiblePosy;
                            return 0;
                        }
                        Tmp++;
                        PossiblePosy++;
                    }
                }
                /*�ڶ�����:�������Ƿ�AI���ӳ�����ȫ����*/
                count = 0;
                PossiblePosx = x;
                PossiblePosy = y;
                Tmp = 0;
                while(Tmp <= 5)
                {
                    if(MapChess[PossiblePosx][PossiblePosy] == AI_s_chess)
                    {
                        count++;
                    }
                    Tmp++;
                    PossiblePosx++;
                }
                while(MapChess[PossiblePosx][y] == None)
                {
                    PossiblePosx--;
                }
                if(count >= 3)  //��ȫ����. 
                {
                    TmpPos = PossiblePosx;
                    PossiblePosx = x-1;
                    PossiblePosy = y;
                    Tmp = 0;
                    while(Tmp <= 5)
                    {
                        if(MapChess[PossiblePosx][PossiblePosy] == Player_s_chess && Tmp != 0)
                        {
                            break;
                        }
                        if(MapChess[PossiblePosx][PossiblePosy] == None)
                        {
                            Tmpx = PossiblePosx;
                            Tmpy = PossiblePosy;
                            return 0;
                        }
                        Tmp++;
                        PossiblePosx++;
                    }
                }
                /*��������:����б�����Ƿ�AI���ӳ�����ȫ����*/
                count = 0;
                PossiblePosx = x;
                PossiblePosy = y;
                Tmp = 0;
                while(Tmp <= 5)
                {
                    if(MapChess[PossiblePosx][PossiblePosy] == AI_s_chess)
                    {
                        count++;
                    }
                    Tmp++;
                    PossiblePosx++;
                    PossiblePosy++;
                }
                while(MapChess[PossiblePosx][PossiblePosy] == None)
                {
                    PossiblePosx--;
                    PossiblePosy--;
                }
                if(count >= 3)
                {
                    PossiblePosx = x-1;
                    PossiblePosy = y-1;
                    Tmp = 0;
                    while(Tmp <= 5)
                    {
                        if(MapChess[PossiblePosx][PossiblePosy] == Player_s_chess && Tmp != 0)
                        {
                            break;
                        }
                        if(MapChess[PossiblePosx][PossiblePosy] == None)
                        {
                            Tmpx = PossiblePosx;
                            Tmpy = PossiblePosy;
                            return 0;
                        }
                        Tmp++;
                        PossiblePosx++;
                        PossiblePosy++;
                    }
                }
                /*���Ĳ���:����б�����Ƿ�AI���ӳ�����ȫ����*/
                count = 0;
                PossiblePosx = x;
                PossiblePosy = y;
                Tmp = 0;
                while(Tmp <= 5)
                {
                    if(MapChess[PossiblePosx][PossiblePosy] == AI_s_chess)
                    {
                        count++;
                    }
                    Tmp++;
                    PossiblePosx--;
                    PossiblePosy++;
                }
                while(MapChess[PossiblePosx][PossiblePosy] == None)
                {
                    PossiblePosx++;
                    PossiblePosy--;
                }
                if(count >= 3)
                {
                    PossiblePosx = x+1;
                    PossiblePosy = y-1;
                    Tmp = 0;
                    while(Tmp <= 5)
                    {
                        if(MapChess[PossiblePosx][PossiblePosy] == Player_s_chess && Tmp != 0)
                        {
                            break;
                        }
                        if(MapChess[PossiblePosx][PossiblePosy] == None)
                        {
                            Tmpx = PossiblePosx;
                            Tmpy = PossiblePosy;
                            return 0;
                        }
                        Tmp++;
                        PossiblePosx--;
                        PossiblePosy++;
                    }
                }
            }
        }
    }
    /*����-1��AI����ʱ�������*/
    Tmpx = -1;
    Tmpy = -1;
    return 0;
}

/**
*�ж���ҵ������Ƿ�������һ�еĿ���.
*Args:
*   Tmpx:�����λX����[�ҵ�������]��δ�ҵ������ԣ�Ϊ-1.
*   Tmpx:�����λY����[�ҵ�������]��δ�ҵ������ԣ�Ϊ-1.
*Returns:
*   None.
*/
int Map::CountPlayerChess(int &Tmpx,int &Tmpy)
{
    int x,y;
    int PossiblePosx,PossiblePosy;
    int count = 0;
    int Tmp = 0;
    int TmpPos;
    for(x = 0;x < Width;x++)
    {
        for(y = 0;y < Height;y++)
        {
            if(MapChess[x][y] == Player_s_chess)
            {
                /*��һ����:�������Ƿ�������ӳ�����ȫ����*/
                count = 0;
                PossiblePosx = x;
                PossiblePosy = y;
                Tmp = 0;
                while(Tmp <= 5)
                {
                    if(MapChess[PossiblePosx][PossiblePosy] == Player_s_chess)
                    {
                        count++;
                    }
                    Tmp++;
                    PossiblePosy++;
                }
                while(MapChess[x][PossiblePosy] == None)
                {
                    PossiblePosy--;
                }
                if(count >= SafeNum)
                {
                    TmpPos = PossiblePosy;
                    PossiblePosx = x;
                    PossiblePosy = y-1;
                    Tmp = 0;
                    while(Tmp <= 5)
                    {
                        if(MapChess[PossiblePosx][PossiblePosy] == AI_s_chess && Tmp != 0)
                        {
                            break;
                        }
                        if(MapChess[PossiblePosx][PossiblePosy] == None)
                        {
                            Tmpx = PossiblePosx;
                            Tmpy = PossiblePosy;
                            return 0;
                        }
                        Tmp++;
                        PossiblePosy++;
                    }
                }
                /*�ڶ�����:�������Ƿ�������ӳ�����ȫ����*/
                count = 0;
                PossiblePosx = x;
                PossiblePosy = y;
                Tmp = 0;
                while(Tmp <= 5)
                {
                    if(MapChess[PossiblePosx][PossiblePosy] == Player_s_chess)
                    {
                        count++;
                    }
                    Tmp++;
                    PossiblePosx++;
                }
                while(MapChess[PossiblePosx][y] == None)
                {
                    PossiblePosx--;
                }
                if(count >= SafeNum)    //��ȫ����. 
                {
                    TmpPos = PossiblePosx;
                    PossiblePosx = x-1;
                    PossiblePosy = y;
                    Tmp = 0;
                    while(Tmp <= 5)
                    {
                        if(MapChess[PossiblePosx][PossiblePosy] == AI_s_chess && Tmp != 0)
                        {
                            break;
                        }
                        if(MapChess[PossiblePosx][PossiblePosy] == None)
                        {
                            Tmpx = PossiblePosx;
                            Tmpy = PossiblePosy;
                            return 0;
                        }
                        Tmp++;
                        PossiblePosx++;
                    }
                }
                /*��������:����б�����Ƿ�������ӳ�����ȫ����*/
                count = 0;
                PossiblePosx = x;
                PossiblePosy = y;
                Tmp = 0;
                while(Tmp <= 5)
                {
                    if(MapChess[PossiblePosx][PossiblePosy] == Player_s_chess)
                    {
                        count++;
                    }
                    Tmp++;
                    PossiblePosx++;
                    PossiblePosy++;
                }
                while(MapChess[PossiblePosx][PossiblePosy] == None)
                {
                    PossiblePosx--;
                    PossiblePosy--;
                }
                if(count >= SafeNum)
                {
                    PossiblePosx = x-1;
                    PossiblePosy = y-1;
                    Tmp = 0;
                    while(Tmp <= 5)
                    {
                        if(MapChess[PossiblePosx][PossiblePosy] == AI_s_chess && Tmp != 0)
                        {
                            break;
                        }
                        if(MapChess[PossiblePosx][PossiblePosy] == None)
                        {
                            Tmpx = PossiblePosx;
                            Tmpy = PossiblePosy;
                            return 0;
                        }
                        Tmp++;
                        PossiblePosx++;
                        PossiblePosy++;
                    }
                }
                /*���Ĳ���:����б�����Ƿ�������ӳ�����ȫ����*/
                count = 0;
                PossiblePosx = x;
                PossiblePosy = y;
                Tmp = 0;
                while(Tmp <= 5)
                {
                    if(MapChess[PossiblePosx][PossiblePosy] == Player_s_chess)
                    {
                        count++;
                    }
                    Tmp++;
                    PossiblePosx--;
                    PossiblePosy++;
                }
                while(MapChess[PossiblePosx][PossiblePosy] == None)
                {
                    PossiblePosx++;
                    PossiblePosy--;
                }
                if(count >= SafeNum)
                {
                    PossiblePosx = x+1;
                    PossiblePosy = y-1;
                    Tmp = 0;
                    while(Tmp <= 5)
                    {
                        if(MapChess[PossiblePosx][PossiblePosy] == AI_s_chess && Tmp != 0)
                        {
                            break;
                        }
                        if(MapChess[PossiblePosx][PossiblePosy] == None)
                        {
                            Tmpx = PossiblePosx;
                            Tmpy = PossiblePosy;
                            return 0;
                        }
                        Tmp++;
                        PossiblePosx--;
                        PossiblePosy++;
                    }
                }
            }
        }
    }
    /*������̰�ȫ������÷���-1��AI����ʱ�������*/
    Tmpx = -1;
    Tmpy = -1;
    return 0;
}

/**
*�ж���Ϸ���Ƿ������ʤ��.
*Args:
*   hwnd:�����ھ��. 
*Returns:
*   None.
*/
enum _ChessMan Map::IsWin(HWND hwnd)
{
    int Tmpx,Tmpy;
    for(Tmpx = 0;Tmpx < Height;Tmpx++)
    {
        for(Tmpy = 0;Tmpy < Height;Tmpy++)
        {
            if(MapChess[Tmpx][Tmpy] == Player_s_chess && (Tmpy + 4) <= 14)                    //�����̣����϶����жϡ�
            {
                if(MapChess[Tmpx][Tmpy + 1] == Player_s_chess)
                {
                    if(MapChess[Tmpx][Tmpy + 2] == Player_s_chess)
                    {
                        if(MapChess[Tmpx][Tmpy + 3] == Player_s_chess)
                        {
                            if(MapChess[Tmpx][Tmpy + 4] == Player_s_chess)
                            {
                                MessageBox(hwnd,TEXT("���ʤ��!"),TEXT("��ʾ"),MB_OK);
                                DestroyWindow(hwnd);
                                return Player_s_chess;
                            }
                        }
                    }
                }
            }
            if(MapChess[Tmpx][Tmpy] == Player_s_chess && (Tmpx + 4) <= 14)
            {
                if(MapChess[Tmpx + 1][Tmpy] == Player_s_chess)
                {
                    if(MapChess[Tmpx + 2][Tmpy] == Player_s_chess)
                    {
                        if(MapChess[Tmpx + 3][Tmpy] == Player_s_chess)
                        {
                            if(MapChess[Tmpx + 4][Tmpy] == Player_s_chess)
                            {
                                MessageBox(hwnd,TEXT("���ʤ��!"),TEXT("��ʾ"),MB_OK);
                                DestroyWindow(hwnd);
                                return Player_s_chess;
                            }
                        }
                    }
                }
            }
            if(MapChess[Tmpx][Tmpy] == Player_s_chess && (Tmpx + 4) <= 14 && (Tmpy + 4) <= 14)            //����ʽ��б���ж�
            {
                if(MapChess[Tmpx + 1][Tmpy + 1] == Player_s_chess)
                {
                    if(MapChess[Tmpx + 2][Tmpy + 2] == Player_s_chess)
                    {
                        if(MapChess[Tmpx + 3][Tmpy + 3] == Player_s_chess)
                        {
                            if(MapChess[Tmpx + 4][Tmpy + 4] == Player_s_chess)
                            {
                                MessageBox(hwnd,TEXT("���ʤ��!"),TEXT("��ʾ"),MB_OK);
                                DestroyWindow(hwnd);
                                return Player_s_chess;
                            }
                        }
                    }
                }
            }
            if(MapChess[Tmpx][Tmpy] == Player_s_chess&& (Tmpx - 4) >= 0 && (Tmpy + 4) <= 14)            //����ʽ��б���ж�
            {
                if(MapChess[Tmpx - 1][Tmpy + 1] == Player_s_chess)
                {
                    if(MapChess[Tmpx - 2][Tmpy + 2] == Player_s_chess)
                    {
                        if(MapChess[Tmpx - 3][Tmpy + 3] == Player_s_chess)
                        {
                            if(MapChess[Tmpx - 4][Tmpy + 4] == Player_s_chess)
                            {
                                MessageBox(hwnd,TEXT("���ʤ��!"),TEXT("��ʾ"),MB_OK);
                                DestroyWindow(hwnd);
                                return Player_s_chess;
                            }
                        }
                    }
                }
            }
            if(MapChess[Tmpx][Tmpy] == Player_s_chess && (Tmpx + 4) <= 14 && (Tmpy - 4) >= 0)            //����ʽ��б���ж�
            {
                if(MapChess[Tmpx + 1][Tmpy - 1] == Player_s_chess)
                {
                    if(MapChess[Tmpx + 2][Tmpy - 2] == Player_s_chess)
                    {
                        if(MapChess[Tmpx + 3][Tmpy - 3] == Player_s_chess)
                        {
                            if(MapChess[Tmpx + 4][Tmpy - 4] == Player_s_chess)
                            {
                                MessageBox(hwnd,TEXT("���ʤ��!"),TEXT("��ʾ"),MB_OK);
                                DestroyWindow(hwnd);
                                return Player_s_chess;
                            }
                        }
                    }
                }
            }
            if(MapChess[Tmpx][Tmpy] == Player_s_chess && (Tmpx - 4) >= 0 && (Tmpy - 4) >= 0)            //����ʽ��б���ж�
            {
                if(MapChess[Tmpx - 1][Tmpy - 1] == Player_s_chess)
                {
                    if(MapChess[Tmpx - 2][Tmpy - 2] == Player_s_chess)
                    {
                        if(MapChess[Tmpx - 3][Tmpy - 3] == Player_s_chess)
                        {
                            if(MapChess[Tmpx - 4][Tmpy - 4] == Player_s_chess)
                            {
                                MessageBox(hwnd,TEXT("���ʤ��!"),TEXT("��ʾ"),MB_OK);
                                DestroyWindow(hwnd);
                                return Player_s_chess;
                            }
                        }
                    }
                }
            }
            
            //���Ե��ж�. 
            if(MapChess[Tmpx][Tmpy] == AI_s_chess && (Tmpy + 4) <= 14)                    //�����̣����϶����жϡ�
            {
                if(MapChess[Tmpx][Tmpy + 1] == AI_s_chess)
                {
                    if(MapChess[Tmpx][Tmpy + 2] == AI_s_chess)
                    {
                        if(MapChess[Tmpx][Tmpy + 3] == AI_s_chess)
                        {
                            if(MapChess[Tmpx][Tmpy + 4] == AI_s_chess)
                            {
                                MessageBox(hwnd,TEXT("����ʤ��!"),TEXT("��ʾ"),MB_OK);
                                DestroyWindow(hwnd);
                                return AI_s_chess;
                            }
                        }
                    }
                }
            }
            if(MapChess[Tmpx][Tmpy] == AI_s_chess && (Tmpx + 4) <= 14 && (Tmpy + 4) <= 14)            //����ʽ��б���ж�
            {
                if(MapChess[Tmpx + 1][Tmpy + 1] == AI_s_chess)
                {
                    if(MapChess[Tmpx + 2][Tmpy + 2] == AI_s_chess)
                    {
                        if(MapChess[Tmpx + 3][Tmpy + 3] == AI_s_chess)
                        {
                            if(MapChess[Tmpx + 4][Tmpy + 4] == AI_s_chess)
                            {
                                MessageBox(hwnd,TEXT("����ʤ��!"),TEXT("��ʾ"),MB_OK);
                                DestroyWindow(hwnd);
                                return AI_s_chess;
                            }
                        }
                    }
                }
            }
            if(MapChess[Tmpx][Tmpy] == AI_s_chess  && (Tmpx + 4) <= 14)            //����ʽ��б���ж�
            {
                if(MapChess[Tmpx + 1][Tmpy] == AI_s_chess)
                {
                    if(MapChess[Tmpx + 2][Tmpy] == AI_s_chess)
                    {
                        if(MapChess[Tmpx + 3][Tmpy] == AI_s_chess)
                        {
                            if(MapChess[Tmpx + 4][Tmpy] == AI_s_chess)
                            {
                                MessageBox(hwnd,TEXT("����ʤ��!"),TEXT("��ʾ"),MB_OK);
                                DestroyWindow(hwnd);
                                return AI_s_chess;
                            }
                        }
                    }
                }
            }
            if(MapChess[Tmpx][Tmpy] == AI_s_chess && (Tmpx - 4) >= 0 && (Tmpy + 4) <= 14)            //����ʽ��б���ж�
            {
                if(MapChess[Tmpx - 1][Tmpy + 1] == AI_s_chess)
                {
                    if(MapChess[Tmpx - 2][Tmpy + 2] == AI_s_chess)
                    {
                        if(MapChess[Tmpx - 3][Tmpy + 3] == AI_s_chess)
                        {
                            if(MapChess[Tmpx - 4][Tmpy + 4] == AI_s_chess)
                            {
                                MessageBox(hwnd,TEXT("����ʤ��!"),TEXT("��ʾ"),MB_OK);
                                DestroyWindow(hwnd);
                                return AI_s_chess;
                            }
                        }
                    }
                }
            }
        }
    }
    return None;
}