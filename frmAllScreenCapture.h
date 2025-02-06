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
//---------------------------------------------------------------------------
class TfmAllScreen : public TForm
{
__published:	// IDE-managed Components
	TImage *Image1;
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

public:		// User declarations
	__fastcall TfmAllScreen(TComponent* Owner);

	Graphics::TBitmap * m_bmpCapture;

	TPoint	m_StartPoint;
	TPoint	m_EndPoint;

};
//---------------------------------------------------------------------------
extern PACKAGE TfmAllScreen *fmAllScreen;
//---------------------------------------------------------------------------
#endif
