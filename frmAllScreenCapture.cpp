//---------------------------------------------------------------------------

#include <vcl.h>
#include <algorithm>

#pragma hdrstop

#include "frmAllScreenCapture.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

TfmAllScreen *fmAllScreen;


//---------------------------------------------------------------------------
// 화면 캡처 및 HBITMAP 반환 함수
HBITMAP CaptureScreenAll(int left, int top, int width, int height)
{
    HDC hScreenDC     = GetDC(0); // 전체 화면 DC
    HDC hMemDC         = CreateCompatibleDC(hScreenDC);
    HBITMAP hBitmap = CreateCompatibleBitmap(hScreenDC, width, height);
    HBITMAP hOldBmp = (HBITMAP)SelectObject(hMemDC, hBitmap);

    // 화면 영역 캡처
    BitBlt(hMemDC, 0, 0, width, height, hScreenDC, left, top, SRCCOPY);
    SelectObject(hMemDC, hOldBmp); // 비트맵 선택 해제

    // 리소스 정리
    DeleteDC(hMemDC);
    ReleaseDC(0, hScreenDC);

    return hBitmap; // HBITMAP 반환

}
//---------------------------------------------------------------------------
void __fastcall DarkenBitmap(TBitmap *Bitmap, int amount)
{
    if (!Bitmap) return;
    Bitmap->PixelFormat = pf32bit;
    int WidthBytes = Bitmap->Width * 4;

    for (int y = 0; y < Bitmap->Height; y++)
    {
        BYTE *Row = (BYTE *)Bitmap->ScanLine[y];

        for (int x = 0; x < WidthBytes; x += 4)
        {
            Row[x] = std::max(Row[x] - amount, 0);
            Row[x + 1] = std::max(Row[x + 1] - amount, 0);
            Row[x + 2] = std::max(Row[x + 2] - amount, 0);
        }
    }

}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// Form.
//---------------------------------------------------------------------------
__fastcall TfmAllScreen::TfmAllScreen(TComponent* Owner)
    : TForm(Owner)
{
	m_bmp           = NULL;
	m_bmpDark       = new Graphics::TBitmap();;
	m_bmpCapture    = new Graphics::TBitmap();;
	m_bMouseDown    = false;
	m_bCapture		= false;

    int nPrimeMonitorIdx = 0;

    for(int i=0; i<Screen->MonitorCount; i++) {
        if(Screen->Monitors[i]->Primary ) {
            nPrimeMonitorIdx = i;
            break;
        }
    }

    Left   = Screen->Monitors[nPrimeMonitorIdx]->Left;
    Top    = Screen->Monitors[nPrimeMonitorIdx]->Top;
    Width  = Screen->Monitors[nPrimeMonitorIdx]->Width;
    Height = Screen->Monitors[nPrimeMonitorIdx]->Height;

    m_hBitmap = CaptureScreenAll(Left, Top, Width, Height);

    if(m_hBitmap) {
        m_bmp = new Graphics::TBitmap();
        if(m_bmp) {
            m_bmp->Handle = m_hBitmap;          // HBITMAP 설정

            m_bmpDark->Assign(m_bmp);

            DarkenBitmap(m_bmpDark, 50);
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TfmAllScreen::FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
    if(Key == 0x1B) {
        Close();
    }
}
//---------------------------------------------------------------------------
void __fastcall TfmAllScreen::FormCreate(TObject *Sender)
{
    Image1->Picture->Bitmap->Width  = Width;
    Image1->Picture->Bitmap->Height = Height;

    Image1->Picture->Bitmap->Assign(m_bmp);
}
//---------------------------------------------------------------------------
void __fastcall TfmAllScreen::FormShow(TObject *Sender)
{
    Image1->Picture->Bitmap->Canvas->Pen->Color     = clGray;
    Image1->Picture->Bitmap->Canvas->Brush->Style     = bsClear;
    Image1->Picture->Bitmap->Canvas->Rectangle(0,0,99,99);

    Image1->Repaint();
}
//---------------------------------------------------------------------------
void __fastcall TfmAllScreen::FormClose(TObject *Sender, TCloseAction &Action)
{
    // Action = caFree;   // Form Close되면 자동 메로리 삭제됨.
}
//---------------------------------------------------------------------------
void __fastcall TfmAllScreen::FormDestroy(TObject *Sender)
{
    if(m_bmp)            delete m_bmp;
	if(m_bmpDark)        delete m_bmpDark;
	if(m_bmpCapture)	 delete m_bmpCapture;

	if(m_hBitmap)        DeleteObject(m_hBitmap);


}
//---------------------------------------------------------------------------
void __fastcall TfmAllScreen::Image1MouseMove(TObject *Sender, TShiftState Shift,
          int X, int Y)
{
	if(m_bCapture) return;

	// 약간 어둡게 처리한 이미지를 보여준다.
    Image1->Picture->Bitmap->Assign(m_bmpDark);

    TCanvas* pCanvas = Image1->Picture->Bitmap->Canvas;

    if(m_bMouseDown) {
		pCanvas->Pen->Color     	= clRed;
		pCanvas->Pen->Style     	= psDot;
		pCanvas->Brush->Style    	= bsClear;

		int nWidth 	= X - m_StartPoint.X;
		int nHeight	= Y - m_StartPoint.Y;

		// 선택하여 사각형을 그릴 때 사갹형 안쪽은 진짜 이미지를 보여 준다.
		BitBlt(	pCanvas->Handle,
				m_StartPoint.X,
				m_StartPoint.Y,
				nWidth,
				nHeight,
				m_bmp->Canvas->Handle,
				m_StartPoint.X,
				m_StartPoint.Y,
				SRCCOPY);

		pCanvas->Rectangle(m_StartPoint.X, m_StartPoint.Y, X, Y);

		AnsiString sMsg;
		sMsg.printf("%dx%d", nWidth, nHeight);

		pCanvas->Font->Color 	= clWhite;
		pCanvas->Font->Style 	= TFontStyles() << fsBold;
		pCanvas->Brush->Color 	= clBlack;
		pCanvas->Brush->Style 	= bsSolid;
		pCanvas->TextOut(X + 10, Y + 10, sMsg);

	}
	else {
		// 선택되지 않은 상태에서는 십자선과 좌표 값을 보여 준다.
		pCanvas->Brush->Style 	=  bsClear;

		pCanvas->Pen->Color 	= clLime;
		pCanvas->Pen->Style    	= psDot;
        pCanvas->MoveTo(X, 0);
		pCanvas->LineTo(X, Height);
        pCanvas->MoveTo(0, Y);
		pCanvas->LineTo(Width, Y);

		AnsiString sMsg;
		sMsg.printf("(X:%d, Y:%d)", X, Y);

		pCanvas->Font->Color 	= clWhite;
		pCanvas->Font->Style 	= TFontStyles() << fsBold;
		pCanvas->Brush->Color 	= clBlack;
		pCanvas->Brush->Style 	=  bsSolid;
		pCanvas->TextOut(X + 10, Y + 10, sMsg);
	}

}
//---------------------------------------------------------------------------
void __fastcall TfmAllScreen::Image1MouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y)
{
	if(m_bCapture) return;

	m_StartPoint.X     = X;
    m_StartPoint.Y     = Y;

    m_bMouseDown    = true;
}
//---------------------------------------------------------------------------
void __fastcall TfmAllScreen::Image1MouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y)
{
	if(m_bCapture) return;

	m_EndPoint.X     = X;
    m_EndPoint.Y     = Y;

	m_bMouseDown    = false;

	int nWidth  = m_EndPoint.X - m_StartPoint.X;
	int nHeight = m_EndPoint.Y - m_StartPoint.Y;

	if(nWidth > 0 && nHeight > 0 ) {

		m_bmpCapture->Width  = nWidth;
		m_bmpCapture->Height = nHeight;

		BitBlt(	m_bmpCapture->Canvas->Handle,
				0,
				0,
				nWidth,
				nHeight,
				m_bmp->Canvas->Handle,
				m_StartPoint.X,
				m_StartPoint.Y,
				SRCCOPY);

		Image1->Picture->Bitmap->Assign(m_bmpCapture);
		Image1->Repaint();

		m_bCapture = true;
	}



}
//---------------------------------------------------------------------------
