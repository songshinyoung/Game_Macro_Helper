//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "frmViewImage.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfmImageView *fmImageView;
//---------------------------------------------------------------------------
__fastcall TfmImageView::TfmImageView(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TfmImageView::FormCreate(TObject *Sender)
{
//
}
//---------------------------------------------------------------------------
void __fastcall TfmImageView::FormShow(TObject *Sender)
{
	Panel_Title->Caption = m_sTitle;

	if(m_sFileName != "") {
		if(FileExists(m_sFileName)) {
			Image1->Picture->LoadFromFile(m_sFileName);
		}
	}


}
//---------------------------------------------------------------------------
void __fastcall TfmImageView::FormClose(TObject *Sender, TCloseAction &Action)
{
//
}
//---------------------------------------------------------------------------
void __fastcall TfmImageView::FormDestroy(TObject *Sender)
{
//
}
//---------------------------------------------------------------------------
