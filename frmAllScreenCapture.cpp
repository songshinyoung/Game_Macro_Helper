//---------------------------------------------------------------------------

#include <vcl.h>
#include <algorithm>

#pragma hdrstop

#include "frmAllScreenCapture.h"
// #include "MainForm.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

TfmAllScreen *fmAllScreen;


//---------------------------------------------------------------------------
// 화면 캡처 및 HBITMAP 반환 함수
HBITMAP CaptureScreenAll(int left, int top, int width, int height)
{
    HDC hScreenDC  	= GetDC(0); // 전체 화면 DC
	HDC hMemDC    	= CreateCompatibleDC(hScreenDC);
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
// 모니터 전체 영역의 크기를 구하는 함수.
void GetTotalScreenSize(int &left, int &top, int &totalWidth, int &totalHeight)
{
	if (Screen->MonitorCount == 0) {
        totalWidth = 0;
        totalHeight = 0;
        return;
    }

    // 모니터 좌표값을 저장할 변수
    int minX = Screen->Monitors[0]->Left;
    int maxX = minX + Screen->Monitors[0]->Width;
    int minY = Screen->Monitors[0]->Top;
    int maxY = minY + Screen->Monitors[0]->Height;

    // 모든 모니터를 순회하면서 최대 영역을 찾음
    for (int i = 1; i < Screen->MonitorCount; i++) {

        // X 범위 갱신
		if (Screen->Monitors[i]->Left < minX) {
			minX = Screen->Monitors[i]->Left;
        }
		if (Screen->Monitors[i]->Left + Screen->Monitors[i]->Width > maxX) {
			maxX = Screen->Monitors[i]->Left + Screen->Monitors[i]->Width;
        }

        // Y 범위 갱신
		if (Screen->Monitors[i]->Top < minY) {
			minY = Screen->Monitors[i]->Top;
        }
		if (Screen->Monitors[i]->Top + Screen->Monitors[i]->Height > maxY) {
			maxY = Screen->Monitors[i]->Top + Screen->Monitors[i]->Height;
        }
    }

	// 최종 전체 너비 및 높이 계산
	totalWidth  = maxX - minX;
	totalHeight = maxY - minY;

	left		= minX;
	top			= minY;
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

	DoubleBuffered 	= true; // 폼 더블 버퍼링 활성화


	m_MonitorCount 	= Screen->MonitorCount;

	ZeroMemory(m_MonitorIndex, sizeof(m_MonitorIndex));

	for(int i=0; i<Screen->MonitorCount; i++) {
		if(Screen->Monitors[i]->Primary ) {
			m_MonitorIndex[0] = i;
			break;
		}
	}

	for(int i=0; i<Screen->MonitorCount; i++) {
		if(Screen->Monitors[i]->Primary != true) {
			m_MonitorIndex[1] = i;
            break;
		}
	}


	GetTotalScreenSize(m_nAllScreenLeft, m_nAllScreenTop, m_nAllScreenWidth, m_nAllScreenHeight);

	Left   = m_nAllScreenLeft; 		// Screen->Monitors[m_MonitorIndex[1]]->Left; // std::min(Screen->Monitors[m_MonitorIndex[0]]->Left , Screen->Monitors[m_MonitorIndex[1]]->Left );
	Top    = m_nAllScreenTop; 		// Screen->Monitors[m_MonitorIndex[1]]->Top;  //  std::min(Screen->Monitors[m_MonitorIndex[0]]->Top , Screen->Monitors[m_MonitorIndex[1]]->Top );
	Width  = m_nAllScreenWidth; 	// Screen->Monitors[m_MonitorIndex[1]]->Width;
	Height = m_nAllScreenHeight; 	// Screen->Monitors[m_MonitorIndex[1]]->Height;

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
	Image1->Picture->Bitmap->Modified = true;
}
//---------------------------------------------------------------------------
void __fastcall TfmAllScreen::FormShow(TObject *Sender)
{
	Left   = m_nAllScreenLeft; 		// Screen->Monitors[m_MonitorIndex[1]]->Left; // std::min(Screen->Monitors[m_MonitorIndex[0]]->Left , Screen->Monitors[m_MonitorIndex[1]]->Left );
	Top    = m_nAllScreenTop; 		// Screen->Monitors[m_MonitorIndex[1]]->Top;  //  std::min(Screen->Monitors[m_MonitorIndex[0]]->Top , Screen->Monitors[m_MonitorIndex[1]]->Top );
	Width  = m_nAllScreenWidth; 	// Screen->Monitors[m_MonitorIndex[1]]->Width;
	Height = m_nAllScreenHeight; 	// Screen->Monitors[m_MonitorIndex[1]]->Height;


	// 약간 어둡게 처리한 이미지를 보여준다.
	Image1->Picture->Bitmap->Assign(m_bmpDark);

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
void __fastcall TfmAllScreen::ClearPreDrawLines(TPoint & point)
{
	BitBlt(	Image1->Picture->Bitmap->Canvas->Handle, point.X, 0, 1,	Height,	m_bmpDark->Canvas->Handle,	point.X, 0, SRCCOPY);
	BitBlt(	Image1->Picture->Bitmap->Canvas->Handle, 0, point.Y, Width,	1,	m_bmpDark->Canvas->Handle,	0, point.Y, SRCCOPY);

}
//---------------------------------------------------------------------------
void __fastcall TfmAllScreen::ClearPreDrawRect(TRect  & rec)
{
	BitBlt(	Image1->Picture->Bitmap->Canvas->Handle, rec.Left-5, rec.Top-5, rec.Width() + 20, rec.Height()+10,	m_bmpDark->Canvas->Handle,	rec.Left-5, rec.Top-5, SRCCOPY);
}
//---------------------------------------------------------------------------
void __fastcall TfmAllScreen::Image1MouseMove(TObject *Sender, TShiftState Shift,
		  int X, int Y)
{
	if(m_bCapture) return;

	Image1->Picture->Bitmap->Canvas->Lock();

	// 약간 어둡게 처리한 이미지를 보여준다.
	// Image1->Picture->Bitmap->Assign(m_bmpDark);

	// 속도를 빠르게 하기 위해 이전에 그려진 부분만 다시 갱신 한다.
	ClearPreDrawLines(m_CurrentMousePos);
	ClearPreDrawRect(m_CurrentTextRect);
	ClearPreDrawRect(m_CurrentBoxRect);

	TCanvas* pCanvas = Image1->Picture->Bitmap->Canvas;

	AnsiString sMsg;
	int nTxtLeft;
	int nTxtTop;

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


		sMsg.printf("%dx%d", nWidth, nHeight);

		pCanvas->Font->Color 	= clWhite;
		pCanvas->Font->Style 	= TFontStyles() << fsBold;
		pCanvas->Brush->Color 	= clBlack;
		pCanvas->Brush->Style 	= bsSolid;

		nTxtLeft = X + 10;
		nTxtTop	 = Y + 10;

		// 마우스를 왼쪽 위로 동작할 경우 좌표 계산이 거꾸로 되기 때문에 올바른 좌표를 넣어 준다.
		m_CurrentBoxRect.Left 	= m_StartPoint.X < X ? m_StartPoint.X : X;
		m_CurrentBoxRect.Top  	= m_StartPoint.Y < Y ? m_StartPoint.Y : Y;
		m_CurrentBoxRect.Right 	= m_StartPoint.X < X ? m_StartPoint.X + X : m_StartPoint.X;
		m_CurrentBoxRect.Bottom = m_StartPoint.Y < Y ? m_StartPoint.Y + Y : m_StartPoint.Y;

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


		sMsg.printf("(X:%d, Y:%d)", X + m_nAllScreenLeft, Y + m_nAllScreenTop);

		pCanvas->Font->Color 	= clWhite;
		pCanvas->Font->Style 	= TFontStyles() << fsBold;
		pCanvas->Brush->Color 	= clBlack;
		pCanvas->Brush->Style 	=  bsSolid;

		if((X + 200) > m_nAllScreenWidth) {
			nTxtLeft = X - 200;
		}
		else {
			nTxtLeft = X + 10;
		}

		if((Y + 50) > m_nAllScreenHeight) {
			nTxtTop = Y - 50;
		}
		else {
			nTxtTop = Y + 10;
		}

		m_CurrentMousePos.X = X;
		m_CurrentMousePos.Y = Y;
	}



    // 현재 폰트 기준으로 텍스트 크기 계산
	int textWidth  = Canvas->TextWidth(sMsg);
	int textHeight = Canvas->TextHeight(sMsg);

	m_CurrentTextRect.Left   = nTxtLeft;
	m_CurrentTextRect.Top    = nTxtTop;
	m_CurrentTextRect.Right  = nTxtLeft + textWidth;
	m_CurrentTextRect.Bottom = nTxtTop + textHeight;

	pCanvas->TextOut(nTxtLeft, nTxtTop, sMsg);

	Image1->Picture->Bitmap->Canvas->Unlock();

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

	// 박스를 거꾸로 그렸을 경우도 대비하기 위해 값을 비교후 작은 값을 Start로 해준다.
	int nStartX = std::min(m_StartPoint.X, m_EndPoint.X);
	int nStartY = std::min(m_StartPoint.Y, m_EndPoint.Y);
	int nEndX   = std::max(m_StartPoint.X, m_EndPoint.X);
	int nEndY   = std::max(m_StartPoint.Y, m_EndPoint.Y);

	m_StartPoint.X = nStartX;
	m_StartPoint.Y = nStartY;

	m_EndPoint.X   = nEndX;
	m_EndPoint.Y   = nEndY;

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
				nStartX,
				nStartY,
				SRCCOPY);

		Image1->Picture->Bitmap->Assign(m_bmpCapture);
		Image1->Repaint();

		m_bCapture = true;

		Image1->AlignWithMargins 	= true;
		Image1->Width  				= nWidth;
		Image1->Height 				= nHeight;

		Panel_FrameBottom->Visible 	= true;

		this->Width 		= nWidth  < 200 ? 210 : nWidth  + 10;
		this->Height 		= nHeight < 100 ? 250 : nHeight + 150;
		this->BorderStyle 	= bsSingle;

		StatusBar1->Visible	= true;

		AnsiString sMsg;
		sMsg.printf("%d,%d", m_StartPoint.X, m_StartPoint.Y);
		StatusBar1->Panels->Items[0]->Text = sMsg;
		sMsg.printf("%d,%d", m_EndPoint.X, m_EndPoint.Y);
		StatusBar1->Panels->Items[1]->Text = sMsg;


		this->Left = Screen->Monitors[m_MonitorIndex[0]]->Left + ( Screen->Monitors[m_MonitorIndex[0]]->Width  - this->Width)  / 2;
		this->Top  = Screen->Monitors[m_MonitorIndex[0]]->Top  + ( Screen->Monitors[m_MonitorIndex[0]]->Height - this->Height) / 2;


	}

}
//---------------------------------------------------------------------------


