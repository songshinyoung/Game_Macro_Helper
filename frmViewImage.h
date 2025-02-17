//---------------------------------------------------------------------------

#ifndef frmViewImageH
#define frmViewImageH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.Buttons.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Imaging.jpeg.hpp>
//---------------------------------------------------------------------------
class TfmImageView : public TForm
{
__published:	// IDE-managed Components
	TImage *Image1;
	TPanel *Panel_Title;
	TBitBtn *BitBtn1;
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall FormDestroy(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TfmImageView(TComponent* Owner);

	AnsiString m_sFileName;
	AnsiString m_sTitle;
};
//---------------------------------------------------------------------------
extern PACKAGE TfmImageView *fmImageView;
//---------------------------------------------------------------------------
#endif
