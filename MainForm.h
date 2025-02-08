//---------------------------------------------------------------------------

#ifndef MainFormH
#define MainFormH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.ComCtrls.hpp>


#include <Vcl.Menus.hpp>
#include <Vcl.Dialogs.hpp>

#include "FrequenceTimer.h"
#include "BitmapManager.h"
#include <Vcl.ImgList.hpp>


typedef enum {
	SEQ_MAIN_NONE,
	SEQ_RE_GAME_S,
	SEQ_ITEM_SALE_S,

	SEQ_MAIN_MAX
} eMainStepType;

//---------------------------------------------------------------------------
class TfmMain : public TForm
{
__published:	// IDE-managed Components
	TTimer *Timer_Seq;
	TPageControl *PageControl1;
	TTabSheet *TabSheet1;
	TTabSheet *TabSheet2;
	TGroupBox *GroupBox1;
	TGroupBox *GroupBox2;
	TPanel *Panel4;
	TLabel *Label8;
	TLabel *Label9;
	TPanel *Panel5;
	TEdit *Edit_Key_Game_1;
	TEdit *Edit_Key_Game_2;
	TPanel *Panel6;
	TLabel *Label6;
	TLabel *Label7;
	TPanel *Panel7;
	TEdit *Edit_Key_Start;
	TEdit *Edit_Key_End;
	TLabel *Label10;
	TLabel *Label11;
	TLabel *Label12;
	TLabel *Label13;
	TLabel *Label14;
	TLabel *Label15;
	TEdit *Edit_Key_Game_3;
	TEdit *Edit_Key_Game_4;
	TEdit *Edit_Key_Game_5;
	TEdit *Edit_Key_Game_6;
	TEdit *Edit_Key_Game_7;
	TEdit *Edit_Key_Game_8;
	TGroupBox *GroupBox3;
	TPanel *Panel8;
	TLabel *Label16;
	TLabel *Label17;
	TLabel *Label18;
	TLabel *Label19;
	TLabel *Label20;
	TLabel *Label21;
	TLabel *Label22;
	TLabel *Label23;
	TPanel *Panel9;
	TEdit *Edit_Key_Skill_1;
	TEdit *Edit_Key_Skill_2;
	TEdit *Edit_Key_Skill_3;
	TEdit *Edit_Key_Skill_4;
	TEdit *Edit_Key_Skill_5;
	TEdit *Edit_Key_Skill_6;
	TEdit *Edit_Key_Skill_7;
	TEdit *Edit_Key_Skill_8;
	TPanel *Panel10;
	TLabel *Label24;
	TLabel *Label25;
	TLabel *Label26;
	TLabel *Label27;
	TLabel *Label28;
	TLabel *Label29;
	TLabel *Label30;
	TLabel *Label31;
	TLabel *Label32;
	TLabel *Label33;
	TPanel *Panel11;
	TLabel *Label34;
	TEdit *Edit_Delay_Skill_1;
	TEdit *Edit_Delay_Skill_2;
	TEdit *Edit_Delay_Skill_3;
	TEdit *Edit_Delay_Skill_4;
	TEdit *Edit_Delay_Skill_5;
	TEdit *Edit_Delay_Skill_6;
	TEdit *Edit_Delay_Skill_7;
	TEdit *Edit_Delay_Skill_8;
	TLabel *Label35;
	TPanel *Panel12;
	TLabel *Label36;
	TCheckBox *CheckBox_Skill_Enable_1;
	TCheckBox *CheckBox_Skill_Enable_2;
	TCheckBox *CheckBox_Skill_Enable_3;
	TCheckBox *CheckBox_Skill_Enable_4;
	TCheckBox *CheckBox_Skill_Enable_5;
	TCheckBox *CheckBox_Skill_Enable_6;
	TCheckBox *CheckBox_Skill_Enable_7;
	TCheckBox *CheckBox_Skill_Enable_8;
	TGroupBox *GroupBox4;
	TPanel *Panel13;
	TLabel *Label37;
	TLabel *Label38;
	TLabel *Label39;
	TPanel *Panel15;
	TLabel *Label47;
	TLabel *Label48;
	TLabel *Label49;
	TPanel *Panel16;
	TLabel *Label56;
	TEdit *Edit_Delay_Mouse_1;
	TEdit *Edit_Delay_Mouse_2;
	TPanel *Panel17;
	TLabel *Label57;
	TCheckBox *CheckBox_Mouse_Enable_1;
	TCheckBox *CheckBox_Mouse_Enable_2;
	TTimer *Timer_Macro;
	TStatusBar *StatusBar1;
	TPanel *Panel_Macro_Msg;
	TTimer *Timer_Display;
	TMainMenu *MainMenu1;
	TMenuItem *File1;
	TMenuItem *OpenFile1;
	TMenuItem *SaveFile1;
	TMenuItem *N1;
	TMenuItem *Exit1;
	TSaveDialog *SaveDialog1;
	TOpenDialog *OpenDialog1;
	TEdit *Edit_Title;
	TMenuItem *N2;
	TMenuItem *Saveas1;
	TTimer *Timer_WatchDog;
	TEdit *Edit_TargetProName;
	TLabel *Label40;
	TComboBox *ComboBox_FileList;
	TButton *Button_Start_Stop;
	TTimer *Timer_ScreenCapture;
	TPanel *Panel18;
	TListBox *ListBox_Bitmap;
	TPanel *Panel19;
	TPanel *Panel14;
	TImage *Image_Capture;
	TMemo *Memo1;
	TPanel *Panel1;
	TImage *Image1;
	TLabel *Label5;
	TImage *Image_SaveImage;
	TLabel *Label41;
	TLabel *Label42;
	TEdit *Edit_ProcessName;
	TButton *btnFindWindow;
	TEdit *Edit_KeyEvent;
	TButton *Button_SendKeyEvent;
	TPanel *Panel2;
	TLabel *Label2;
	TLabel *Label1;
	TEdit *Edit_Mouse_X;
	TEdit *Edit_Mouse_Y;
	TButton *Button_SendMouseClick;
	TPanel *Panel3;
	TLabel *Label3;
	TLabel *Label4;
	TEdit *Edit_Capture_X;
	TEdit *Edit_Capture_Y;
	TButton *Button_ScreenCapture;
	TButton *Button_MouseHookStart;
	TButton *Button_MouseHookStop;
	TCheckBox *CheckBox_AutoScreenCapture;
	TButton *Button_CompareBitmap;
	TButton *Button_SeqStart;
	TButton *Button_AddBitmap;
	TButton *Button_DeleteBitmap;
	TButton *Button_ChangeBitmap;
	TButton *Button_AllScreenCapture;
	void __fastcall btnFindWindowClick(TObject *Sender);
	void __fastcall Button_SendKeyEventClick(TObject *Sender);
	void __fastcall Button_SendMouseClickClick(TObject *Sender);
	void __fastcall Button_ScreenCaptureClick(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall Button_MouseHookStartClick(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall Button_MouseHookStopClick(TObject *Sender);
	void __fastcall Button_CompareBitmapClick(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall Timer_SeqTimer(TObject *Sender);
	void __fastcall Button_SeqStartClick(TObject *Sender);
	void __fastcall Timer_MacroTimer(TObject *Sender);
	void __fastcall CheckBox_Skill_Enable_1Click(TObject *Sender);
	void __fastcall Edit_Key_Skill_1Click(TObject *Sender);
	void __fastcall Edit_Key_StartClick(TObject *Sender);
	void __fastcall Timer_DisplayTimer(TObject *Sender);
	void __fastcall Edit_Delay_Skill_1Change(TObject *Sender);
	void __fastcall OpenFile1Click(TObject *Sender);
	void __fastcall SaveFile1Click(TObject *Sender);
	void __fastcall Exit1Click(TObject *Sender);
	void __fastcall Saveas1Click(TObject *Sender);
	void __fastcall Timer_WatchDogTimer(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall ComboBox_FileListClick(TObject *Sender);
	void __fastcall Edit_Delay_Mouse_1Change(TObject *Sender);
	void __fastcall CheckBox_Mouse_Enable_1Click(TObject *Sender);
	void __fastcall Button_Start_StopClick(TObject *Sender);
	void __fastcall Button_AllScreenCaptureClick(TObject *Sender);
	void __fastcall Button_AllScreenCaptureMouseMove(TObject *Sender, TShiftState Shift,
          int X, int Y);
	void __fastcall Button_AllScreenCaptureMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall Button_AllScreenCaptureMouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall Timer_ScreenCaptureTimer(TObject *Sender);
	void __fastcall Button_AddBitmapClick(TObject *Sender);
	void __fastcall ListBox_BitmapDrawItem(TWinControl *Control, int Index, TRect &Rect, TOwnerDrawState State);
	void __fastcall ListBox_BitmapClick(TObject *Sender);
	void __fastcall ListBox_BitmapKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall Button_DeleteBitmapClick(TObject *Sender);
	void __fastcall Button_ChangeBitmapClick(TObject *Sender);


private:	// User declarations

	// --------------------------------
	// Sequence


	int __fastcall SeqRegame();
	int __fastcall SeqSaleItem();

	int 	m_nInventoryX;
	int     m_nInventoryY;

	// --------------------------------

	TFrequencyTimer m_DelayTimer;

	TFrequencyTimer m_SkillDelay[10];
	TFrequencyTimer	m_MouseSkillDelay[2];

	TFrequencyTimer m_TargetWindowTimer;

	TEdit * m_pMenuKey[10];
	TEdit * m_pSkillKey[10];
	TEdit * m_pSkillDelay[10];

	TCheckBox * m_pChkBox_SkillEnable[10];

	void __fastcall SaveToFile(const String& filename);
	void __fastcall LoadFromFile(const String& filename);

public:		// User declarations
	__fastcall TfmMain(TComponent* Owner);

	void __fastcall ScreenCaptureClick(int screenX, int screenY);

	String  m_sKeyName[256];

	HWND 	m_hTargetWnd;
	int		m_nSeqStep;
	eMainStepType m_eMainStatus;

	// Save Items --------------------
	KBDLLHOOKSTRUCT m_SkillKeys[10];
	KBDLLHOOKSTRUCT m_SettingKeyHook[10]; 	// 0: Start Key, 1 ~ 9 Stop Key.
	bool	m_bSkillEnabled[10];
	int		m_nSkillDelay[10];

	bool	m_bMouseEnabled[2];
	int     m_nMouseDelay[2];

	TBitmapManager m_BitmapManager;


	String  m_sSaveFileTitle;
	String	m_sSaveFilePath;
	String  m_sSaveFileName;

	String 	m_sRootPath;

	TPoint m_StartPos;
	TPoint m_EndPos;

	//--------------------------------

	bool	m_bMacroKeyInputWait;	// Macro Key 입력 대기 중인 경우.
	int		m_nMacroKeyInputType;	// 0 : Start / End,  1 : Skill Key.
	int		m_nMacroKeyInputIndex;	// 현재 선택된 Key 입력칸 번호.

	bool	m_bTargetProcessFound;	// 목표 프로세스를 찾은 경우 true.

	void __fastcall DisplayUpdate(bool bFirstUpdate = false);

	void __fastcall LoadCfgFilesToComboBox(const UnicodeString &folderPath, const UnicodeString &SelectFileName, TComboBox *comboBox);

};
//---------------------------------------------------------------------------
extern PACKAGE TfmMain *fmMain;

int		g_MousePos_X;
int		g_MousePos_Y;
//---------------------------------------------------------------------------
#endif
