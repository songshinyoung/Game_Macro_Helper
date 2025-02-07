//---------------------------------------------------------------------------

#ifndef frmAllScreenCaptureH
#define frmAllScreenCaptureH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Graphics.hpp>
#include <Vcl.Buttons.hpp>
#include <Vcl.ComCtrls.hpp>
//---------------------------------------------------------------------------
class TfmAllScreen : public TForm
{
__published:	// IDE-managed Components
	TImage *Image1;
	TPanel *Panel_FrameBottom;
	TBitBtn *BitBtn1;
	TBitBtn *BitBtn2;
	TStatusBar *StatusBar1;
	void __fastcall FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall Image1MouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
	void __fastcall Image1MouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
	void __fastcall Image1MouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);


private:	// User declarations
	HBITMAP m_hBitmap;
	Graphics::TBitmap * m_bmp;
	Graphics::TBitmap * m_bmpDark;


	bool	m_bMouseDown;
	bool	m_bCapture;		// 캡처가 완료된 경우 true.

	int		m_MonitorCount;
	int		m_MonitorIndex[2];  	// 0: Prime, 1 : Sub Monitor
	int		m_nAllScreenLeft;
	int		m_nAllScreenTop;
	int		m_nAllScreenWidth;
	int		m_nAllScreenHeight;

	// 이전 십자선과 텍스트 출력 좌표를 기억 후 새로운 그림 그릴 때 전체를 갱신하지 않고 그려진 부분만 다시 갱신하는 방법으로 속도를 개선 한다.
	TPoint	m_CurrentMousePos;
	TRect   m_CurrentTextRect;
	TRect	m_CurrentBoxRect;

	void __fastcall ClearPreDrawLines(TPoint & point);
	void __fastcall ClearPreDrawRect(TRect  & rec);

public:		// User declarations
	__fastcall TfmAllScreen(TComponent* Owner);

	//------------------------------------------
	// Main 쪽으로 넘겨줄 Capture한 이미지 정보와 좌표 정보.
	Graphics::TBitmap * m_bmpCapture;

	TPoint	m_StartPoint;	// Image 상에서의 좌표기 때문에 - 값은 없다.
	TPoint	m_EndPoint;  	// Image 상에서의 좌표기 때문에 - 값은 없다.�
	//------------------------------------------
};
//---------------------------------------------------------------------------
extern PACKAGE TfmAllScreen *fmAllScreen;
//---------------------------------------------------------------------------
#endif
