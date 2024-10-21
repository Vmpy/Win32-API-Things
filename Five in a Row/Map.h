#include <windows.h>

enum _ChessMan{None,Black,White};	//����ÿ�������״̬. 
const int RectWidth = 35;	//����������̸��ӿ��.
const int Radii = 10;	//��������������Ӱ뾶. 
const int SafeNum = 3;	//����������̼�ⰲȫ����. 

const HBRUSH hBlackBrush = CreateSolidBrush(RGB(0,0,0));
const HBRUSH hWhiteBrush = CreateSolidBrush(RGB(255,255,255));

class Map
{
	private:
		int Height = 15;
		int Width = 15;
	public:
		enum _ChessMan MapChess[15][15];
		POINT MapPoint[15][15];
		enum _ChessMan AI_s_chess = None; 
		enum _ChessMan Player_s_chess = None;
		Map(void);
		int CountPlayerChess(int &Tmpx,int &Tmpy);
		int CountAIChess(int &Tmpx,int &Tmpy);
		void SetChessColor(enum _ChessMan,enum _ChessMan);
		bool PutAChess(int x,int y);
		bool PutAIChess(void);
		void DrawChess(HDC& hdc);
		enum _ChessMan IsWin(HWND hwnd);
};
