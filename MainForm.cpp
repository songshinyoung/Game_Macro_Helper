//---------------------------------------------------------------------------

#include <vcl.h>
#include <windows.h>
#include <tlhelp32.h>  // ���μ��� ������ ��
#include <vector>
#include <Graphics.hpp>
#include <System.Classes.hpp>
#include <algorithm>
#include <System.IOUtils.hpp>
#include <System.SysUtils.hpp>

#include <mmsystem.h> 	// PlaySound() �Լ� ȣ�� ����.



#include <vector>
#include <cmath>


#pragma hdrstop

#include "MainForm.h"
#include "frmAllScreenCapture.h"


//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfmMain *fmMain;

HHOOK 	g_hMouseHook;
HHOOK	g_hKeyHook;

//int		g_MousePos_X;
//int		g_MousePos_Y;

bool	g_bMacroStarted;

#define _MY_ENABLE_COLOR 	((TColor)0x0D5FFD5)
#define _MY_DISABLE_COLOR 	((TColor)0x0B5B5FF)

//---------------------------------------------------------------------------
// FIND WINDOW HANDLE
//---------------------------------------------------------------------------
// EnumWindows �ݹ�� ����ü
struct EnumData {
    DWORD ProcessId;
    HWND FoundHandle;
};

//---------------------------------------------------------------------------
// ���μ��� �̸����� PID ã��
DWORD GetProcessIdByName(const wchar_t* processName) {
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE)
        return 0;

    PROCESSENTRY32W processEntry;
    processEntry.dwSize = sizeof(PROCESSENTRY32W);

    if (Process32FirstW(snapshot, &processEntry)) {
        do {
            if (_wcsicmp(processEntry.szExeFile, processName) == 0) {
                CloseHandle(snapshot);
                return processEntry.th32ProcessID;
            }
        } while (Process32NextW(snapshot, &processEntry));
    }

    CloseHandle(snapshot);
    return 0;
}
//---------------------------------------------------------------------------
// EnumWindows �ݹ� �Լ� (â �ڵ� �˻�)
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    EnumData* data = (EnumData*)lParam;
    DWORD processId = 0;

    GetWindowThreadProcessId(hwnd, &processId);
    if (processId == data->ProcessId) {
        data->FoundHandle = hwnd;
        return FALSE;  // ã���� ���� �ߴ�
    }
    return TRUE;  // ��� �˻�
}
//---------------------------------------------------------------------------
// ���� ���� �̸����� �ֻ��� â �ڵ� ��ȯ
HWND FindWindowByExeName(const wchar_t* exeName) {
    DWORD pid = GetProcessIdByName(exeName);
    if (pid == 0) return NULL;

    EnumData data;
    data.ProcessId = pid;
    data.FoundHandle = NULL;

    EnumWindows(EnumWindowsProc, (LPARAM)&data);
    return data.FoundHandle;
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
// EVENT SEND.
//---------------------------------------------------------------------------
// Ű���� �̺�Ʈ ����
void SendKeyboardEvent(KBDLLHOOKSTRUCT &KeyInfo, bool keyDown)
{
    INPUT input;
	input.type 			= INPUT_KEYBOARD;
	input.ki.wVk 		= KeyInfo.vkCode;
	input.ki.wScan 		= KeyInfo.scanCode;
	input.ki.dwFlags 	= keyDown ? KeyInfo.flags : KEYEVENTF_KEYUP;
	input.ki.time 		= 0;
    input.ki.dwExtraInfo = 0;
	SendInput(1, &input, sizeof(INPUT));
}

void SendKeyboardEvent(DWORD vkcode)
{
	KBDLLHOOKSTRUCT KeyInfo;

	KeyInfo.vkCode 		= vkcode;
	KeyInfo.scanCode 	= 0;
	KeyInfo.flags 		= 0;

	SendKeyboardEvent(KeyInfo, true);
	SendKeyboardEvent(KeyInfo, false);
}

void SendKeyboardEvent(DWORD vkcode, DWORD scan, DWORD flag)
{
	KBDLLHOOKSTRUCT KeyInfo;

	KeyInfo.vkCode 		= vkcode;
	KeyInfo.scanCode 	= scan;
	KeyInfo.flags 		= flag;

	SendKeyboardEvent(KeyInfo, true);
	SendKeyboardEvent(KeyInfo, false);
}

// ���콺 �̵� �̺�Ʈ ����
void SendMouseMove(int x, int y)
{
    INPUT input;
    input.type = INPUT_MOUSE;
    input.mi.dx = x * (65535 / GetSystemMetrics(SM_CXSCREEN));  // ���� ��ǥ ��ȯ
    input.mi.dy = y * (65535 / GetSystemMetrics(SM_CYSCREEN));
    input.mi.mouseData = 0;
    input.mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;
    input.mi.time = 0;
    input.mi.dwExtraInfo = 0;
    SendInput(1, &input, sizeof(INPUT));
}

// ���콺 Ŭ�� �̺�Ʈ ����
void SendLeftMouseClick(bool ButtonDown)
{
	INPUT input;
	input.type = INPUT_MOUSE;
	input.mi.dwFlags = ButtonDown ? MOUSEEVENTF_LEFTDOWN : MOUSEEVENTF_LEFTUP;
    input.mi.time = 0;
	SendInput(1, &input, sizeof(INPUT));
}

// ���콺 Ŭ�� �̺�Ʈ ����
void SendRightMouseClick(bool ButtonDown)
{
    INPUT input;
    input.type = INPUT_MOUSE;
	input.mi.dwFlags = ButtonDown ? MOUSEEVENTF_RIGHTDOWN : MOUSEEVENTF_RIGHTUP;
    input.mi.time = 0;
	SendInput(1, &input, sizeof(INPUT));
}





//---------------------------------------------------------------------------
// SCREEN CAPTURE
//---------------------------------------------------------------------------
//// ȭ�� ĸó �� �߾� �ȼ� ���� ���� �Լ�
//TColor GetCenterPixelColor(int screenX, int screenY) {
//    const int width = 100;  // ĸó ���� �ʺ�
//    const int height = 100; // ĸó ���� ����
//
//    HDC hScreenDC = GetDC(0); // ��ü ȭ�� DC
//    HDC hMemDC = CreateCompatibleDC(hScreenDC);
//    HBITMAP hBitmap = CreateCompatibleBitmap(hScreenDC, width, height);
//    HBITMAP hOldBmp = (HBITMAP)SelectObject(hMemDC, hBitmap);
//
//    // ȭ�� ���� ĸó
//    BitBlt(hMemDC, 0, 0, width, height, hScreenDC, screenX, screenY, SRCCOPY);
//    SelectObject(hMemDC, hOldBmp); // ��Ʈ�� ���� ����
//
//    // ��Ʈ�� ������ ����
//    BITMAPINFO bmi = {};
//	bmi.bmiHeader.biSize 			= sizeof(BITMAPINFOHEADER);
//	bmi.bmiHeader.biWidth 			= width;
//	bmi.bmiHeader.biHeight 			= -height; // Top-down DIB
//	bmi.bmiHeader.biPlanes 			= 1;
//	bmi.bmiHeader.biBitCount 		= 32;
//	bmi.bmiHeader.biCompression 	= BI_RGB;
//
//	std::vector<BYTE> pixelData(width * height * 4); // 32��Ʈ �ȼ� ����
//    GetDIBits(hMemDC, hBitmap, 0, height, &pixelData[0], &bmi, DIB_RGB_COLORS);
//
//    // �߾� �ȼ� ��ǥ ��� (0-based)
//	const int centerX = width / 2 - 1;  // 49
//    const int centerY = height / 2 - 1; // 49
//
//    // �߾� �ȼ� ���� ���� (BGR ����)
//    const int offset = (centerY * width + centerX) * 4;
//    BYTE blue = pixelData[offset];
//    BYTE green = pixelData[offset + 1];
//    BYTE red = pixelData[offset + 2];
//
//    // ���ҽ� ����
//    DeleteObject(hBitmap);
//    DeleteDC(hMemDC);
//    ReleaseDC(0, hScreenDC);
//
//    return TColor((blue << 16) | (green << 8) | red); // TColor �������� ��ȯ
//}
////---------------------------------------------------------------------------

// ȭ�� ĸó �� HBITMAP ��ȯ �Լ�
Graphics::TBitmap * CaptureScreenRegion(int screenX, int screenY, int width, int height)
{
	HDC hScreenDC 	= GetDC(0); // ��ü ȭ�� DC
	HDC hMemDC 		= CreateCompatibleDC(hScreenDC);
    HBITMAP hBitmap = CreateCompatibleBitmap(hScreenDC, width, height);
	HBITMAP hOldBmp = (HBITMAP)SelectObject(hMemDC, hBitmap);

	// ȭ�� ���� ĸó
    BitBlt(hMemDC, 0, 0, width, height, hScreenDC, screenX, screenY, SRCCOPY);
    SelectObject(hMemDC, hOldBmp); // ��Ʈ�� ���� ����

	Graphics::TBitmap *bmp = new Graphics::TBitmap();
	bmp->Handle 		= hBitmap;  	// HBITMAP ����
//	bmp->HandleType 	= bmDIB;       	// DIB ���� ���
//	bmp->PixelFormat 	= pf32bit;    	// 32��Ʈ ���� ����
//	bmp->Canvas->LineTo(1,1);

	// ���ҽ� ����
    DeleteDC(hMemDC);
	ReleaseDC(0, hScreenDC);

	// DeleteObject(hBitmap);
	//return hBitmap; // HBITMAP ��ȯ

	return bmp;
}



// �߾� �ȼ� ���� ���� �Լ�
TColor GetCenterPixelColor(HBITMAP hBitmap) {
    const int width = 100;
    const int height = 100;

    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height; // Top-down DIB
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    std::vector<BYTE> pixelData(width * height * 4); // 32��Ʈ �ȼ� ����
    HDC hDC = GetDC(0);
    HDC hMemDC = CreateCompatibleDC(hDC);
    HBITMAP hOldBmp = (HBITMAP)SelectObject(hMemDC, hBitmap);

    GetDIBits(hMemDC, hBitmap, 0, height, &pixelData[0], &bmi, DIB_RGB_COLORS);

    SelectObject(hMemDC, hOldBmp);
    DeleteDC(hMemDC);
    ReleaseDC(0, hDC);

    // �߾� �ȼ� ��ǥ ��� (0-based)
    const int centerX = width / 2 - 1;  // 49
    const int centerY = height / 2 - 1; // 49

    // �߾� �ȼ� ���� ���� (BGR ����)
    const int offset = (centerY * width + centerX) * 4;
    BYTE blue = pixelData[offset];
    BYTE green = pixelData[offset + 1];
    BYTE red = pixelData[offset + 2];

    return TColor((blue << 16) | (green << 8) | red); // TColor �������� ��ȯ
}





//---------------------------------------------------------------------------
// Mouse Hooking.
//---------------------------------------------------------------------------
// ���콺 �� ���ν��� (��� ���콺 �̺�Ʈ ����)
LRESULT __stdcall MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode >= 0) {
        MSLLHOOKSTRUCT *pMouse = (MSLLHOOKSTRUCT*)lParam;

		// �̺�Ʈ �α�
        String eventMsg;
        switch (wParam) {
            case WM_MOUSEMOVE:
				eventMsg = "Mouse MOVE";
                break;
            case WM_LBUTTONDOWN:
                eventMsg = "Left Button DOWN";
                break;
            case WM_LBUTTONUP:
                eventMsg = "Left Button UP";
                break;
            case WM_RBUTTONDOWN:
                eventMsg = "Right Button DOWN";
                break;
            case WM_RBUTTONUP:
                eventMsg = "Right Button UP";
                break;
        }

        // �޸� �̺�Ʈ ���
		if (!eventMsg.IsEmpty()) {

			g_MousePos_X = pMouse->pt.x;
			g_MousePos_Y = pMouse->pt.y;

			fmMain->StatusBar1->Panels->Items[3]->Text = g_MousePos_X;
			fmMain->StatusBar1->Panels->Items[4]->Text = g_MousePos_Y;
			fmMain->StatusBar1->Panels->Items[5]->Text = eventMsg;

			if(fmMain->CheckBox_AutoScreenCapture->Checked) {
				fmMain->ScreenCaptureClick(pMouse->pt.x, pMouse->pt.y);
			}

//			Form1->Memo1->Lines->Add(
//				Format("%s : X=%d, Y=%d", ARRAYOFCONST((
//					eventMsg, pMouse->pt.x, pMouse->pt.y
//				)))
//			);
		}
	}

    // ���� ������ ����
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}
//---------------------------------------------------------------------------
bool SetMouseHook()
{
	if (!g_hMouseHook) {
		g_hMouseHook = SetWindowsHookEx(
			WH_MOUSE_LL,        // ������ ���콺 ��
			MouseHookProc,      // �� ���ν���
			NULL, 				// NULL�� �־�� ���� ��Ȼ��ȭ �Ǿ Hooking�� ���������� �ȴ�. ���⿡ �� �ν��Ͻ��� ������ ���� Ȱ��ȭ �� ���¿����� Hooking�� ���� �Ѵ�. HInstance,          // ���� �ν��Ͻ�
			0                   // ��� ������
		);

		if (g_hMouseHook) {
			fmMain->Memo1->Lines->Add("Mouse hook started!");
			return true;
		} else {
			fmMain->Memo1->Lines->Add("Failed to start Mouse hook!");
			return false;
		}
	}

	return true;
}
//---------------------------------------------------------------------------
void RemoveMouseHook()
{
	if (g_hMouseHook != NULL)
	{
		UnhookWindowsHookEx(g_hMouseHook);
		g_hMouseHook = NULL;
		fmMain->Memo1->Lines->Add("Mouse Hook removed");
	}
}
//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
// Keyboard Hooking.
//---------------------------------------------------------------------------

DWORD GetMyScanCode(KBDLLHOOKSTRUCT *pKbdLLHook)
{
	if(pKbdLLHook == NULL) return 0;

	DWORD nScanCode = pKbdLLHook->scanCode;

	// GetKeyNameTextW(nScanCode << 16, buffer, 16);

	// Ư�� Ű �˻� (��: F1 Ű)
	if (GetAsyncKeyState(VK_F1) & 0x8000) {
		// ShowMessage(L"F1 Ű�� ���Ƚ��ϴ�!");
	}

	// ���� �ڵ尡 �ٸ� Ű�� ������ Ư�� Ű���� flags�Ǵ� vkcode�� ���Ͽ� ������ scancode ���� �ο��� �ش�.
	if(pKbdLLHook->scanCode == 0x58 && pKbdLLHook->flags == 1) { // Win
		nScanCode = 11 * 8 + 3;
	}
	else if(pKbdLLHook->scanCode == 0x50 && pKbdLLHook->flags == 1) { // Super
		nScanCode = 11 * 8 + 5;
	}
	else if(pKbdLLHook->scanCode == 0x52 && pKbdLLHook->flags == 1) { // Ins
		nScanCode = 12 * 8 + 0;
	}
	else if(pKbdLLHook->scanCode == 0x47 && pKbdLLHook->flags == 1) { // Home
		nScanCode = 12 * 8 + 1;
	}
	else if(pKbdLLHook->scanCode == 0x53 && pKbdLLHook->flags == 1) { // Del
		nScanCode = 12 * 8 + 2;
	}
	else if(pKbdLLHook->scanCode == 0x4F && pKbdLLHook->flags == 1) { // End
		nScanCode = 12 * 8 + 3;
	}
	else if(pKbdLLHook->scanCode == 0x49 && pKbdLLHook->flags == 1) { // PgUp
		nScanCode = 12 * 8 + 4;
	}
	else if(pKbdLLHook->scanCode == 0x51 && pKbdLLHook->flags == 1) { // PgDn
		nScanCode = 12 * 8 + 5;
	}
	else if(pKbdLLHook->scanCode == 0x45 && pKbdLLHook->flags == 1) { // Num
		nScanCode = 12 * 8 + 6;
	}
	else if(pKbdLLHook->scanCode == 0x37 && pKbdLLHook->flags == 1) { // PrtSc
		nScanCode = 12 * 8 + 7;
	}
	else if(pKbdLLHook->scanCode == 0x35 && pKbdLLHook->flags == 1) { // Num /
		nScanCode = 13 * 8 + 0;
	}
	else if(pKbdLLHook->scanCode == 0x1C && pKbdLLHook->flags == 1) { // Num Enter
		nScanCode = 13 * 8 + 1;
	}
	else if(pKbdLLHook->scanCode == 0 && pKbdLLHook->vkCode == 0xAE) { // Vol -
		nScanCode = 13 * 8 + 2;
	}
	else if(pKbdLLHook->scanCode == 0 && pKbdLLHook->vkCode == 0xAF) { // Vol +
		nScanCode = 13 * 8 + 3;
	}
	else if(pKbdLLHook->scanCode == 0 && pKbdLLHook->vkCode == 0xAD) { // Vol X
		nScanCode = 13 * 8 + 4;
	}
	else if(pKbdLLHook->scanCode == 0 && pKbdLLHook->vkCode == 0xB7) { // VCal.
		nScanCode = 13 * 8 + 5;
	}
	else if(pKbdLLHook->scanCode == 0x48 && pKbdLLHook->vkCode == 0x26 && pKbdLLHook->flags == 1) { // Up
		nScanCode = 14 * 8 + 0;
	}
	else if(pKbdLLHook->scanCode == 0x4B && pKbdLLHook->vkCode == 0x25 && pKbdLLHook->flags == 1) { // Left
		nScanCode = 14 * 8 + 1;
	}
	else if(pKbdLLHook->scanCode == 0x4D && pKbdLLHook->vkCode == 0x27 && pKbdLLHook->flags == 1) { // Right
		nScanCode = 14 * 8 + 2;
	}
	else if(pKbdLLHook->scanCode == 0x50 && pKbdLLHook->vkCode == 0x28 && pKbdLLHook->flags == 1) { // Down
		nScanCode = 14 * 8 + 3;
	}

	return nScanCode;
}

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode == HC_ACTION)
    {
		switch (wParam)
		{
			case WM_KEYLAST:
				// Form1->Memo1->Lines->Add("WM_KEYLAST");
				break;

			case WM_KEYDOWN:
				{
					KBDLLHOOKSTRUCT *pKbdLLHook = (KBDLLHOOKSTRUCT *)lParam;
					wchar_t buffer[16];

					DWORD nScanCode = GetMyScanCode(pKbdLLHook);

					fmMain->StatusBar1->Panels->Items[9]->Text = fmMain->m_sKeyName[nScanCode];

					if(fmMain->m_bMacroKeyInputWait) {
						if(pKbdLLHook->scanCode == 0x01) {

							switch(fmMain->m_nMacroKeyInputType) {
								case 0: 	// Start, End and Menu Key/
									fmMain->m_SettingKeyHook[fmMain->m_nMacroKeyInputIndex].vkCode	 	= 0;
									fmMain->m_SettingKeyHook[fmMain->m_nMacroKeyInputIndex].scanCode 	= 0;
									fmMain->m_SettingKeyHook[fmMain->m_nMacroKeyInputIndex].flags		= 0;
									break;

								case 1: 	// Skill Key.
									fmMain->m_SkillKeys[fmMain->m_nMacroKeyInputIndex].vkCode	 		= 0;
									fmMain->m_SkillKeys[fmMain->m_nMacroKeyInputIndex].scanCode 		= 0;
									fmMain->m_SkillKeys[fmMain->m_nMacroKeyInputIndex].flags			= 0;
									fmMain->m_bSkillEnabled[fmMain->m_nMacroKeyInputIndex]				= false;
									break;
							}
						}
						else {
							switch(fmMain->m_nMacroKeyInputType) {
								case 0: 	// Start, End and Menu Key/
									fmMain->m_SettingKeyHook[fmMain->m_nMacroKeyInputIndex] 			= *pKbdLLHook;
									fmMain->m_SettingKeyHook[fmMain->m_nMacroKeyInputIndex].scanCode 	= nScanCode;
									break;

								case 1: 	// Skill Key.
									fmMain->m_SkillKeys[fmMain->m_nMacroKeyInputIndex] 					= *pKbdLLHook;
									fmMain->m_SkillKeys[fmMain->m_nMacroKeyInputIndex].scanCode 		= nScanCode;
									break;
							}
						}

						fmMain->m_bMacroKeyInputWait 	= false;
						fmMain->m_nMacroKeyInputType	= 0;
						fmMain->m_nMacroKeyInputIndex	= 0;

						fmMain->DisplayUpdate();
					}
					else {
						// Macro Start Key Check.
						if((g_bMacroStarted != true)
						&& fmMain->m_bTargetProcessFound
						&& (pKbdLLHook->scanCode == fmMain->m_SettingKeyHook[0].scanCode)
						&& (pKbdLLHook->vkCode == fmMain->m_SettingKeyHook[0].vkCode)) {
							g_bMacroStarted = true;
							PlaySound("C:\\Windows\\Media\\Speech On.wav", NULL, SND_ALIAS | SND_ASYNC);
						}

						// Macro Stop Key Check.
						if(g_bMacroStarted) {
							for(int i=1; i<10; i++) {
								if((fmMain->m_SettingKeyHook[i].scanCode == pKbdLLHook->scanCode)
								&& (fmMain->m_SettingKeyHook[i].vkCode   == pKbdLLHook->vkCode)) {
									// ��� Ű �������� Macro Stop ��Ų��.
									g_bMacroStarted = false;
									PlaySound("C:\\Windows\\Media\\Speech Off.wav", NULL, SND_ALIAS | SND_ASYNC);
								}
							}
						}


						switch(pKbdLLHook->scanCode) {
							case 0x01:
								//fmMain->Timer_Seq->Enabled = false;
								// ESC
								break;

							case 0x37:
								// Print Screen
								if(pKbdLLHook->flags == 1) {
									fmMain->ScreenCaptureClick(g_MousePos_X, g_MousePos_Y);
								}
								break;

							case 0x3F:
								if(pKbdLLHook->vkCode == 0x74) { // F5
									if(fmMain->m_hTargetWnd && (fmMain->Timer_Seq->Enabled != true)) {
										PlaySound("C:\\Windows\\Media\\Speech On.wav", NULL, SND_ALIAS | SND_ASYNC);
										g_bMacroStarted = false;
										fmMain->m_eMainStatus = SEQ_RE_GAME_S;
										fmMain->m_nSeqStep = 0;
										fmMain->Timer_Seq->Enabled = true;

									}
									else if(fmMain->Timer_Seq->Enabled){
										PlaySound("C:\\Windows\\Media\\Speech Off.wav", NULL, SND_ALIAS | SND_ASYNC);
										fmMain->Timer_Seq->Enabled = false;
										fmMain->m_eMainStatus = SEQ_MAIN_NONE;
									}

								}
								break;

							case 0x40:
								if(pKbdLLHook->vkCode == 0x75) { // F6
									if(fmMain->m_hTargetWnd && (fmMain->Timer_Seq->Enabled != true)) {
										PlaySound("C:\\Windows\\Media\\Speech On.wav", NULL, SND_ALIAS | SND_ASYNC);
										g_bMacroStarted = false;
										fmMain->m_eMainStatus = SEQ_ITEM_SALE_S;
										fmMain->m_nSeqStep = 0;
										fmMain->Timer_Seq->Enabled = true;

									}
									else if(fmMain->Timer_Seq->Enabled){
										PlaySound("C:\\Windows\\Media\\Speech Off.wav", NULL, SND_ALIAS | SND_ASYNC);
										fmMain->Timer_Seq->Enabled = false;
										fmMain->m_eMainStatus = SEQ_MAIN_NONE;
									}

								}
								break;
						}
					}



//					if((nScanCode >=0)
//					&& (pKbdLLHook->scanCode < ARRAYSIZE(Form1->m_nScanCodeCound))
//					&& (Form1->m_bScanCodeDown[nScanCode] != true)) {
//						Form1->m_nScanCodeCound[nScanCode] += 1;
//						Form1->m_bNewEvent = true;
//						Form1->m_bScanCodeInput[nScanCode] = true;
//
//						Form1->UpdateKeyData(nScanCode, pKbdLLHook->vkCode, 1);
//
//						Form1->m_bScanCodeDown[nScanCode] = true;
//					}
//
//					String sMsg;
//					sMsg.printf(L"Key Name : %-16s,  VK_Code : 0x%02X,  Scan Code : 0x%02X,  Flags : %d,  Time : %d",
//								fmMain->m_sKeyName[nScanCode].c_str(), pKbdLLHook->vkCode, pKbdLLHook->scanCode, pKbdLLHook->flags, pKbdLLHook->time);
//
//					fmMain->Memo1->Lines->Add(sMsg);
				}
				break;

			case WM_KEYUP:
				{
					// Form1->Memo1->Lines->Add("WM_KEYUP");

					KBDLLHOOKSTRUCT *pKbdLLHook = (KBDLLHOOKSTRUCT *)lParam;

					DWORD nScanCode = GetMyScanCode(pKbdLLHook);

//					if((nScanCode >=0)
//					&& (pKbdLLHook->scanCode < ARRAYSIZE(Form1->m_nScanCodeCound))) {
//						Form1->m_bScanCodeDown[nScanCode] = false;
//					}
				}
				break;
		}
	}

	return CallNextHookEx(g_hKeyHook, nCode, wParam, lParam);
}
//---------------------------------------------------------------------------
bool SetKeyHook()
{
    g_hKeyHook = SetWindowsHookExW(WH_KEYBOARD_LL, KeyboardProc, NULL, 0);
    if (!g_hKeyHook)
	{
		fmMain->Memo1->Lines->Add("Failed to set hook");
		return false;
    }
    else
	{
		fmMain->Memo1->Lines->Add("Hook set successfully");
		return true;
    }
}
//---------------------------------------------------------------------------
void RemoveKeyHook()
{
	if (g_hKeyHook != NULL)
	{
		UnhookWindowsHookEx(g_hKeyHook);
		g_hKeyHook = NULL;
		fmMain->Memo1->Lines->Add("Hook removed");
	}
}
//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
// Compare Bitmap.
//---------------------------------------------------------------------------
// �� ��Ʈ�� �� �� ���絵 ��� �Լ�

std::vector<int> ComputeHistogram(TBitmap* bmp) {
    std::vector<int> histogram(256, 0); // �׷��̽����� ���� (256��)

    bmp->PixelFormat = pf8bit; // 8��Ʈ �׷��̽����� ��ȯ

    int width = bmp->Width;
    int height = bmp->Height;
    int y, x;

    for (y = 0; y < height; y++) {
        Byte* scanLine = (Byte*)bmp->ScanLine[y];
        for (x = 0; x < width; x++) {
            histogram[scanLine[x]]++;
        }
    }
    return histogram;
}

double CompareHistograms(const std::vector<int>& hist1, const std::vector<int>& hist2) {
    double sum = 0.0;
    int i;

    for (i = 0; i < 256; i++) {
        sum += std::abs(hist1[i] - hist2[i]);
    }
    return 1.0 - (sum / (256.0 * 255.0)); // 1�� �������� ����
}

double CompareBitmaps(TBitmap* bmp1, TBitmap* bmp2) {
    if (bmp1->Width != bmp2->Width || bmp1->Height != bmp2->Height) {
        return 0.0;
    }

    std::vector<int> hist1 = ComputeHistogram(bmp1);
    std::vector<int> hist2 = ComputeHistogram(bmp2);

	return CompareHistograms(hist1, hist2) * 100.0;
}



//double CompareBitmaps(Graphics::TBitmap* bmp1, Graphics::TBitmap* bmp2)
//{
//    // 1. �⺻ ���� üũ
//    if(bmp1->Width != bmp2->Width || bmp1->Height != bmp2->Height)
//        return 0.0;  // ũ�Ⱑ �ٸ��� 0% ����
//
//    const int width = bmp1->Width;
//    const int height = bmp1->Height;
//    int matchCount = 0;
//
//    // 2. �ȼ� ���� ���� (32��Ʈ ARGB)
//    bmp1->PixelFormat = pf32bit;
//    bmp2->PixelFormat = pf32bit;
//
//    // 3. �̹��� ���
//	bmp1->Canvas->Lock();
//	bmp2->Canvas->Lock();
//
//    try
//    {
//		// 4. �ȼ� ���� ��
//        for(int y = 0; y < height; y++)
//        {
//            // ��ĵ���� ������ ��������
//            unsigned int* p1 = (unsigned int*)bmp1->ScanLine[y];
//            unsigned int* p2 = (unsigned int*)bmp2->ScanLine[y];
//
//            for(int x = 0; x < width; x++)
//            {
//                // Alpha ä�� �����ϰ� RGB�� ��
//                if((p1[x] & 0x00FFFFFF) == (p2[x] & 0x00FFFFFF))
//                    matchCount++;
//            }
//        }
//    }
//    __finally
//    {
//		bmp1->Canvas->Unlock();
//		bmp2->Canvas->Unlock();
//    }
//
//    // 5. ���絵 ���
//    return (matchCount / (double)(width * height)) * 100.0;
//}
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
// Path ���� "\\"�� ���� ��� �߰��� �ִ� �Լ�.
//---------------------------------------------------------------------------

// ���ڿ��� ���� "\\"�� �ִ��� Ȯ���ϰ� ������ �߰�
UnicodeString EnsureTrailingBackslash(const UnicodeString &path)
{
    // ���ڿ��� ��� ���� �ʰ� ������ ���ڰ� '\\'�� �ƴϸ� �߰�
    if (!path.IsEmpty() && AnsiLastChar(path.c_str())[0] != '\\')
    {
        return path + "\\";
    }
    return path;
}
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
// FORM...
//---------------------------------------------------------------------------
__fastcall TfmMain::TfmMain(TComponent* Owner) : TForm(Owner)
{
	m_hTargetWnd 			= NULL;
	g_hMouseHook 			= NULL;
	g_hKeyHook      		= NULL;

	g_MousePos_X			= 0;
	g_MousePos_Y			= 0;

	g_bMacroStarted			= false;

	m_bTargetProcessFound	= false;

	m_eMainStatus			= SEQ_MAIN_NONE;


	ZeroMemory(m_bSkillEnabled, 	sizeof(m_bSkillEnabled));
	ZeroMemory(m_nSkillDelay, 		sizeof(m_nSkillDelay));
	ZeroMemory(m_SkillKeys, 		sizeof(m_SkillKeys));

	ZeroMemory(m_SettingKeyHook, 	sizeof(m_SettingKeyHook));

	ZeroMemory(m_bMouseEnabled, 	sizeof(m_bMouseEnabled));
	ZeroMemory(m_nMouseDelay, 		sizeof(m_nMouseDelay));

	ZeroMemory(m_pSkillKey, 		sizeof(m_pSkillKey));
	ZeroMemory(m_pSkillDelay, 		sizeof(m_pSkillDelay));
	ZeroMemory(m_pMenuKey, 			sizeof(m_pMenuKey));
	ZeroMemory(m_pChkBox_SkillEnable, 	sizeof(m_pChkBox_SkillEnable));

	ZeroMemory(m_nInventoryStatus, 	sizeof(m_nInventoryStatus));


	m_bMacroKeyInputWait		= false;	// Macro Key �Է� ��� ���� ���.
	m_nMacroKeyInputType		= 0;	// 0 : Start / End. 1 : Menu Key, 2 : Skill Key.
	m_nMacroKeyInputIndex		= 0;	// ���� ���õ� Key �Է�ĭ ��ȣ.

	//----------------------------------------------------
	// Start Key. (F2)
	m_SettingKeyHook[0].vkCode		= 0x71;
	m_SettingKeyHook[0].scanCode	= 0x3C;
	m_SettingKeyHook[0].flags		= 0;

	// Start Key. (F3)
	m_SettingKeyHook[1].vkCode		= 0x72;
	m_SettingKeyHook[1].scanCode	= 0x3D;
	m_SettingKeyHook[1].flags		= 0;

	// �κ��丮 �޴� Ű (I)
	m_SettingKeyHook[2].vkCode		= 0x49;
	m_SettingKeyHook[2].scanCode	= 0x17;
	m_SettingKeyHook[2].flags		= 0;

	// ��� �޴� Ű (S)
	m_SettingKeyHook[3].vkCode		= 0x53;
	m_SettingKeyHook[3].scanCode	= 0x1F;
	m_SettingKeyHook[3].flags		= 0;

	// ������ �޴� Ű (F)
	m_SettingKeyHook[4].vkCode		= 0x46;
	m_SettingKeyHook[4].scanCode	= 0x21;
	m_SettingKeyHook[4].flags		= 0;

	// ���� �޴� Ű (Tab)
	m_SettingKeyHook[5].vkCode		= 0x09;
	m_SettingKeyHook[5].scanCode	= 0x0F;
	m_SettingKeyHook[5].flags		= 0;

	// �������� �޴� Ű (M)
	m_SettingKeyHook[6].vkCode		= 0x4D;
	m_SettingKeyHook[6].scanCode	= 0x32;
	m_SettingKeyHook[6].flags		= 0;

	// ������ �޴� Ű (T)
	m_SettingKeyHook[7].vkCode		= 0x54;
	m_SettingKeyHook[7].scanCode	= 0x14;
	m_SettingKeyHook[7].flags		= 0;

	// ä�� �޴� Ű (Enter)
	m_SettingKeyHook[8].vkCode		= 0x0D;
	m_SettingKeyHook[8].scanCode	= 0x1C;
	m_SettingKeyHook[8].flags		= 0;

	// �Ӹ� �޴� Ű (R)
	m_SettingKeyHook[9].vkCode		= 0x52;
	m_SettingKeyHook[9].scanCode	= 0x13;
	m_SettingKeyHook[9].flags		= 0;

	//----------------------------------------------------
	// Start Key. 1,2,3,4...

	for (unsigned int i = 0; i < ARRAYSIZE(m_SkillKeys); i++)
    {
		m_SkillKeys[i].vkCode 	= 0x31 + i;
		m_SkillKeys[i].scanCode = 0x02 + i;
		m_SkillKeys[i].flags 	= 0;
	}


	//----------------------------------------------------
	// Mouse init. data.
	for(unsigned int i=0; i<ARRAYSIZE(m_nMouseDelay); i++) {
		m_nMouseDelay[i] = 5000;
	}

}
//---------------------------------------------------------------------------
void __fastcall TfmMain::FormCreate(TObject *Sender)
{
	this->Width = 858;

	if(Screen->MonitorCount >= 2) {
		for(int i=0; i<Screen->MonitorCount ; i++) {
			if(Screen->Monitors[i]->Primary != true) {
				this->Left  = Screen->Monitors[i]->Left + ((Screen->Monitors[i]->Width / 2) - (this->Width / 2));
				break;
			}
		}
	}
	//------------------------------------------------------
	// create Key Nama table.
	DWORD   scancode = 0;
	wchar_t buffer[16];

	for(int row=0; row<32; row++) {
		for(int col=0; col<16; col += 2) {
			scancode = (row * 8) + (col / 2) ;
			ZeroMemory(buffer, sizeof(buffer));
			GetKeyNameTextW(scancode << 16, buffer, 16);
			m_sKeyName[scancode] = buffer;
		}
	}

	// Ư�� Ű ��..
	// ���� �ڵ尡 �ٸ� Ű�� ������ Ư�� Ű���� flags�Ǵ� vkcode�� ���Ͽ� ������ scancode ���� �ο��� �ش�.
	scancode = 11 * 8 + 3;  m_sKeyName[scancode] = "Win";
	scancode = 11 * 8 + 5;  m_sKeyName[scancode] = "Super";
	scancode = 12 * 8 + 0;  m_sKeyName[scancode] = "Ins";
	scancode = 12 * 8 + 1;  m_sKeyName[scancode] = "Home";
	scancode = 12 * 8 + 2;  m_sKeyName[scancode] = "Del";
	scancode = 12 * 8 + 3;  m_sKeyName[scancode] = "End";
	scancode = 12 * 8 + 4;  m_sKeyName[scancode] = "PgUp";
	scancode = 12 * 8 + 5;  m_sKeyName[scancode] = "PgDn";
	scancode = 12 * 8 + 6;  m_sKeyName[scancode] = "Num";
	scancode = 12 * 8 + 7;  m_sKeyName[scancode] = "PrtSc";
	scancode = 13 * 8 + 0;  m_sKeyName[scancode] = "Num /";
	scancode = 13 * 8 + 1;  m_sKeyName[scancode] = "Num Enter";
	scancode = 13 * 8 + 2;  m_sKeyName[scancode] = "Vol -";
	scancode = 13 * 8 + 3;  m_sKeyName[scancode] = "Vol +";
	scancode = 13 * 8 + 4;  m_sKeyName[scancode] = "Vol X";
	scancode = 13 * 8 + 5;  m_sKeyName[scancode] = "VCal.";
	scancode = 14 * 8 + 0;  m_sKeyName[scancode] = "Up";
	scancode = 14 * 8 + 1;  m_sKeyName[scancode] = "Left";
	scancode = 14 * 8 + 2;  m_sKeyName[scancode] = "Right";
	scancode = 14 * 8 + 3;  m_sKeyName[scancode] = "Down";
	//------------------------------------------------------


//	m_SkillKey[0] = '1';
//	m_SkillKey[1] = '2';
//	m_SkillKey[2] = '3';
//	m_SkillKey[3] = '4';
//	m_SkillKey[4] = '5';
//	m_SkillKey[5] = '6';
//	m_SkillKey[6] = '7';
//	m_SkillKey[7] = '8';

	for(unsigned int i=0; i<ARRAYSIZE(m_nSkillDelay); i++) {
		m_nSkillDelay[i] = 500;
	}

	m_pMenuKey[0] = Edit_Key_Start;
	m_pMenuKey[1] = Edit_Key_End;
	m_pMenuKey[2] = Edit_Key_Game_1;
	m_pMenuKey[3] = Edit_Key_Game_2;
	m_pMenuKey[4] = Edit_Key_Game_3;
	m_pMenuKey[5] = Edit_Key_Game_4;
	m_pMenuKey[6] = Edit_Key_Game_5;
	m_pMenuKey[7] = Edit_Key_Game_6;
	m_pMenuKey[8] = Edit_Key_Game_7;
	m_pMenuKey[9] = Edit_Key_Game_8;

	for(unsigned int i=0; i<ARRAYSIZE(m_pMenuKey); i++) {
		if(m_pMenuKey[i]) m_pMenuKey[i]->Tag = i;
	}

	m_pSkillKey[0] = Edit_Key_Skill_1;
	m_pSkillKey[1] = Edit_Key_Skill_2;
	m_pSkillKey[2] = Edit_Key_Skill_3;
	m_pSkillKey[3] = Edit_Key_Skill_4;
	m_pSkillKey[4] = Edit_Key_Skill_5;
	m_pSkillKey[5] = Edit_Key_Skill_6;
	m_pSkillKey[6] = Edit_Key_Skill_7;
	m_pSkillKey[7] = Edit_Key_Skill_8;

	for(unsigned int i=0; i<ARRAYSIZE(m_pSkillKey); i++) {
		if(m_pSkillKey[i]) m_pSkillKey[i]->Tag = i;
	}

	m_pSkillDelay[0] = Edit_Delay_Skill_1;
	m_pSkillDelay[1] = Edit_Delay_Skill_2;
	m_pSkillDelay[2] = Edit_Delay_Skill_3;
	m_pSkillDelay[3] = Edit_Delay_Skill_4;
	m_pSkillDelay[4] = Edit_Delay_Skill_5;
	m_pSkillDelay[5] = Edit_Delay_Skill_6;
	m_pSkillDelay[6] = Edit_Delay_Skill_7;
	m_pSkillDelay[7] = Edit_Delay_Skill_8;

	for(unsigned int i=0; i<ARRAYSIZE(m_pSkillDelay); i++) {
		if(m_pSkillDelay[i]) m_pSkillDelay[i]->Tag = i;
	}

	m_pChkBox_SkillEnable[0] = CheckBox_Skill_Enable_1;
	m_pChkBox_SkillEnable[1] = CheckBox_Skill_Enable_2;
	m_pChkBox_SkillEnable[2] = CheckBox_Skill_Enable_3;
	m_pChkBox_SkillEnable[3] = CheckBox_Skill_Enable_4;
	m_pChkBox_SkillEnable[4] = CheckBox_Skill_Enable_5;
	m_pChkBox_SkillEnable[5] = CheckBox_Skill_Enable_6;
	m_pChkBox_SkillEnable[6] = CheckBox_Skill_Enable_7;
	m_pChkBox_SkillEnable[7] = CheckBox_Skill_Enable_8;

	for(unsigned int i=0; i<ARRAYSIZE(m_pChkBox_SkillEnable); i++) {
		if(m_pChkBox_SkillEnable[i]) m_pChkBox_SkillEnable[i]->Tag = i;
	}


	//-------------------------------------------
	// ������ ���� ���� ��� �о� ����.
	TStringList * pFileNameList = new TStringList;

	if(FileExists("LastSettingData.dat")) {
		pFileNameList->LoadFromFile("LastSettingData.dat");

		m_sSaveFilePath = pFileNameList->Strings[0];
		m_sSaveFileName = ExtractFileName(m_sSaveFilePath);

		if(FileExists(m_sSaveFilePath)) {
			LoadFromFile(pFileNameList->Strings[0]);
		}
		else {
			m_sSaveFilePath = "";
			m_sSaveFileName = "";
		}
	}

	delete pFileNameList;
	//-------------------------------------------
	// Save ���ϵ��� ����Ʈ�� �ۼ� �Ѵ�.
	m_sRootPath = ExtractFilePath(ParamStr(0));

	LoadCfgFilesToComboBox(m_sRootPath, m_sSaveFileName, ComboBox_FileList);

	//-------------------------------------------
}
//---------------------------------------------------------------------------

void __fastcall TfmMain::FormShow(TObject *Sender)
{
	Image1->Picture->Bitmap->Width  = 100;
	Image1->Picture->Bitmap->Height = 100;

	Image_SaveImage->Picture->Bitmap->Width  = 100;
	Image_SaveImage->Picture->Bitmap->Height = 100;

	SetMouseHook();
	SetKeyHook();

	DisplayUpdate(true);

}
//---------------------------------------------------------------------------
void __fastcall TfmMain::FormClose(TObject *Sender, TCloseAction &Action)
{
	Timer_Display->Enabled 	= false;
	Timer_Macro->Enabled 	= false;
	Timer_Seq->Enabled 		= false;
	Timer_WatchDog->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall TfmMain::FormDestroy(TObject *Sender)
{
	RemoveMouseHook();
	RemoveKeyHook();
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::DisplayUpdate(bool bFirstUpdate)
{

	Edit_Title->Text = m_sSaveFileTitle;

	Caption = m_sSaveFileName;

	// Menu Key.
	for(unsigned int i=0; i<ARRAYSIZE(m_pMenuKey); i++) {
		if(m_pMenuKey[i]) {
			m_pMenuKey[i]->Text = m_sKeyName[m_SettingKeyHook[i].scanCode];
		}
	}

	// Skill Key.
	for(unsigned int i=0; i<ARRAYSIZE(m_pSkillKey); i++) {
		if(m_pSkillKey[i]) {
			m_pSkillKey[i]->Text = m_sKeyName[m_SkillKeys[i].scanCode];
			m_pSkillKey[i]->Color = m_bSkillEnabled[i] ? _MY_ENABLE_COLOR : clBtnFace;
		}

		if(m_pSkillDelay[i]) {
			m_pSkillDelay[i]->OnChange = NULL;
			m_pSkillDelay[i]->Text = m_nSkillDelay[i];
			m_pSkillDelay[i]->OnChange = Edit_Delay_Skill_1Change;
		}

		if(m_pSkillDelay[i]) {
			m_pSkillDelay[i]->OnChange = NULL;
			m_pSkillDelay[i]->Text = m_nSkillDelay[i];
			m_pSkillDelay[i]->OnChange = Edit_Delay_Skill_1Change;
		}

		if(m_pChkBox_SkillEnable[i]) {
			m_pChkBox_SkillEnable[i]->OnClick = NULL;
			m_pChkBox_SkillEnable[i]->Checked = m_bSkillEnabled[i];
			m_pChkBox_SkillEnable[i]->OnClick = CheckBox_Skill_Enable_1Click;
		}
		else {
			if(m_pSkillKey[i]) m_pSkillKey[i]->Color = _MY_ENABLE_COLOR;
		}

	}

	// Mouse.
	CheckBox_Mouse_Enable_1->OnClick = NULL;
	CheckBox_Mouse_Enable_2->OnClick = NULL;
	CheckBox_Mouse_Enable_1->Checked = m_bMouseEnabled[0];
	CheckBox_Mouse_Enable_2->Checked = m_bMouseEnabled[1];
	CheckBox_Mouse_Enable_1->OnClick = CheckBox_Mouse_Enable_1Click;
	CheckBox_Mouse_Enable_2->OnClick = CheckBox_Mouse_Enable_1Click;


	Edit_Delay_Mouse_1->OnChange 	= NULL;
	Edit_Delay_Mouse_2->OnChange 	= NULL;
	Edit_Delay_Mouse_1->Text 		= m_nMouseDelay[0];
	Edit_Delay_Mouse_2->Text 		= m_nMouseDelay[1];
	Edit_Delay_Mouse_1->Color 		= m_bMouseEnabled[0] ? _MY_ENABLE_COLOR : clBtnFace;
	Edit_Delay_Mouse_2->Color 		= m_bMouseEnabled[1] ? _MY_ENABLE_COLOR : clBtnFace;
	Edit_Delay_Mouse_1->OnChange 	= Edit_Delay_Mouse_1Change;
	Edit_Delay_Mouse_2->OnChange 	= Edit_Delay_Mouse_1Change;

	if(bFirstUpdate) {
		ListBox_Bitmap->Clear();

		for(int i=0; i <= m_BitmapManager.GetMaxKey(); i++) {
			if(m_BitmapManager.IsKeyExist(i)) {
				ListBox_Bitmap->Items->Add(i);
			}
		}
	}

}
//---------------------------------------------------------------------------
void __fastcall TfmMain::btnFindWindowClick(TObject *Sender)
{
	TFrequencyTimer tmTimer;
	tmTimer.StartTimer(0);

	String sProcessName = Edit_ProcessName->Text;

	const wchar_t* targetExe = sProcessName.c_str();  // ã�� ���� ���� �̸�

	HWND hTarget = FindWindowByExeName(targetExe);

	double dTickTime = tmTimer.GetLapTimeMM();

	Memo1->Lines->Add(dTickTime);

	if (hTarget) {
		m_hTargetWnd = hTarget;
		ShowMessage("â �ڵ� ã��: " + IntToHex((int)hTarget, 8));

		m_bTargetProcessFound = true;

	} else {
		ShowMessage("��� ���μ����� â�� ã�� ���߽��ϴ�.");
		m_hTargetWnd = NULL;

		m_bTargetProcessFound = false;
    }
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::Button_SendKeyEventClick(TObject *Sender)
{
	WORD key;

	AnsiString sKeys = Edit_KeyEvent->Text;
	//AnsiString asKeys = sKeys;

	if(m_hTargetWnd != NULL) {
		// Ÿ�� �����츦 �������� ����
		SetForegroundWindow(m_hTargetWnd);

		Sleep(500);

		for(int i=0; i<sKeys.Length(); i++) {

			key =  sKeys.c_str()[i];

			Memo1->Lines->Add(key);

			KBDLLHOOKSTRUCT KeyInfo;

			KeyInfo.vkCode 		= key;
			KeyInfo.scanCode 	= 0;
			KeyInfo.flags 		= 0;

			SendKeyboardEvent(KeyInfo, true);
			SendKeyboardEvent(KeyInfo, false);
			Sleep(30);
		}
	}

}
//---------------------------------------------------------------------------

void __fastcall TfmMain::Button_SendMouseClickClick(TObject *Sender)
{
	if(m_hTargetWnd != NULL) {
		// Ÿ�� �����츦 �������� ����
		SetForegroundWindow(m_hTargetWnd);

		Sleep(500);

		int nX = Edit_Mouse_X->Text.ToIntDef(0);
		int nY = Edit_Mouse_Y->Text.ToIntDef(0);

		// ���콺 �̵� (ȭ�� �»�� ���� ���� ��ǥ)
		SendMouseMove(nX, nY);

		// LEFT ���콺 Ŭ��
		SendLeftMouseClick(true);
		SendLeftMouseClick(false);

		// RIGHT ���콺 Ŭ��
		SendRightMouseClick(true);
		SendRightMouseClick(false);
	}

}
//---------------------------------------------------------------------------

void __fastcall TfmMain::ScreenCaptureClick(int screenX, int screenY)
{
	screenX -= 50;
	screenY -= 50;

	// HBITMAP hBitmap = CaptureScreenRegion(screenX, screenY, 100, 100);
	Graphics::TBitmap *bmp = CaptureScreenRegion(screenX, screenY, 100, 100);
	if (!bmp) {
        ShowMessage("ȭ�� ĸó ����");
        return;
    }

//	TColor centerColor = GetCenterPixelColor(hBitmap);
//
//	String sColor = "0x" + IntToHex(centerColor, 6);
//	StatusBar1->Panels->Items[1]->Text = sColor;


//    // TImage�� ���
//	Graphics::TBitmap *bmp = new Graphics::TBitmap();
//	bmp->Handle = hBitmap;  		// HBITMAP ����

	Image1->Picture->Bitmap = bmp;  // TImage�� ���
	Image1->Picture->Bitmap->Canvas->Pen->Color 	= clGray;
	Image1->Picture->Bitmap->Canvas->Brush->Style 	= bsClear;
	Image1->Picture->Bitmap->Canvas->Rectangle(0,0,99,99);

	Image1->Repaint();

	delete bmp;  					// TBitmap ����
//	DeleteObject(hBitmap);  		// HBITMAP ���� (�߿�!)
}
//---------------------------------------------------------------------------
void __fastcall TfmMain::Button_ScreenCaptureClick(TObject *Sender)
{
	int screenX = Edit_Capture_X->Text.ToIntDef(0);
	int screenY = Edit_Capture_Y->Text.ToIntDef(0);

	ScreenCaptureClick(screenX, screenY);

}
//---------------------------------------------------------------------------

void __fastcall TfmMain::Button_MouseHookStartClick(TObject *Sender)
{
	SetMouseHook();
}

//---------------------------------------------------------------------------

void __fastcall TfmMain::Button_MouseHookStopClick(TObject *Sender)
{
	RemoveMouseHook();
}
//---------------------------------------------------------------------------

void __fastcall TfmMain::Button_CompareBitmapClick(TObject *Sender)
{
    // 1. ȭ�� ĸó (���� �ڵ��� GetCenterPixelColor �Լ� ���� �ʿ�)
	Graphics::TBitmap* bmpCapture = new Graphics::TBitmap();
	bmpCapture->Width 	= 100;
	bmpCapture->Height 	= 100;

	// ĸó ���� (���� �亯�� ȭ�� ĸó �ڵ� Ȱ��)
	bmpCapture->Assign(Image1->Picture->Bitmap);

	// 2. ���� �̹��� ��������


	// 3. �� ����
	double similarity = CompareBitmaps(Image1->Picture->Bitmap, Image_SaveImage->Picture->Bitmap);



	// 4. ��� ǥ��
	AnsiString sMsg;

	if(similarity == 100.0) {
		sMsg = "100 %";
		// Memo1->Lines->Add("�̹����� �����մϴ�.");
	}
	else {
		sMsg.printf("%0.2f %", similarity);
		// Memo1->Lines->Add(sMsg);
	}

	StatusBar1->Panels->Items[7]->Text = sMsg;

    // 5. ���ҽ� ����
	delete bmpCapture;

}
//---------------------------------------------------------------------------




void __fastcall TfmMain::Timer_SeqTimer(TObject *Sender)
{
	switch(m_eMainStatus) {
		case SEQ_MAIN_NONE:
			break;

		case SEQ_RE_GAME_S:
			SeqRegame();
			break;

		case SEQ_ITEM_SALE_S:
			SeqSaleItem();
			break;

		default:
			break;
	}
}
//---------------------------------------------------------------------------

void __fastcall TfmMain::Button_SeqStartClick(TObject *Sender)
{
	m_nSeqStep = 0;
	g_bMacroStarted = false;
	Timer_Seq->Enabled = true;
}
//---------------------------------------------------------------------------


void __fastcall TfmMain::Timer_MacroTimer(TObject *Sender)
{
	if(g_bMacroStarted != true) return;

	// 0.5�ʿ� �� �� �� Target Windows�� Forground�� �������ش�.
	if(m_hTargetWnd != NULL) {
		if(m_TargetWindowTimer.IsDelayEnd()) {
			SetForegroundWindow(m_hTargetWnd);
			m_TargetWindowTimer.StartDelay(500);
		}
		else if(m_TargetWindowTimer.IsStarted() != true) {
			SetForegroundWindow(m_hTargetWnd);
			m_TargetWindowTimer.StartDelay(500);
		}
	}

	// Key Macro.
	for(int i=0; i<8; i++) {
		if(m_bSkillEnabled[i] && (m_SkillKeys[i].vkCode != 0)) {
			if(m_SkillDelay[i].IsDelayEnd() && g_bMacroStarted) {
				SendKeyboardEvent(m_SkillKeys[i], true);
				SendKeyboardEvent(m_SkillKeys[i], false);
				m_SkillDelay[i].StartDelay(m_nSkillDelay[i]);
			}
			else if(m_SkillDelay[i].IsStarted() != true) {
				m_SkillDelay[i].StartDelay(m_nSkillDelay[i]);
			}
		}
	}


	// LEFT ���콺 Ŭ��
	if(m_bMouseEnabled[0]) {
		if(m_MouseSkillDelay[0].IsDelayEnd() && g_bMacroStarted) {
			SendLeftMouseClick(true);
			SendLeftMouseClick(false);
			m_MouseSkillDelay[0].StartDelay(m_nMouseDelay[0]);
		}
		else if(m_MouseSkillDelay[0].IsStarted() != true) {
			m_MouseSkillDelay[0].StartDelay(m_nMouseDelay[0]);
		}
	}


	// RIGHT ���콺 Ŭ��
	if(m_bMouseEnabled[1]) {
		if(m_MouseSkillDelay[1].IsDelayEnd() && g_bMacroStarted) {
			SendRightMouseClick(true);
			SendRightMouseClick(false);
			m_MouseSkillDelay[1].StartDelay(m_nMouseDelay[0]);
		}
		else if(m_MouseSkillDelay[1].IsStarted() != true) {
			m_MouseSkillDelay[1].StartDelay(m_nMouseDelay[1]);
		}
	}


}
//---------------------------------------------------------------------------

void __fastcall TfmMain::CheckBox_Skill_Enable_1Click(TObject *Sender)
{
	TCheckBox * pCheckBox = dynamic_cast<TCheckBox *>(Sender);

	if(pCheckBox == NULL) return;

	int nIndex = pCheckBox->Tag;

	m_bSkillEnabled[nIndex] = pCheckBox->Checked ? true : false;

	DisplayUpdate();

}
//---------------------------------------------------------------------------
void __fastcall TfmMain::CheckBox_Mouse_Enable_1Click(TObject *Sender)
{
	TCheckBox * pCheckBox = dynamic_cast<TCheckBox *>(Sender);

	if(pCheckBox == NULL) return;

	int nIndex = pCheckBox->Tag;

	m_bMouseEnabled[nIndex] = pCheckBox->Checked ? true : false;

	DisplayUpdate();
}
//---------------------------------------------------------------------------



void __fastcall TfmMain::Edit_Key_StartClick(TObject *Sender)
{
	TEdit * pEdit = dynamic_cast<TEdit *>(Sender);

	if(pEdit == NULL) return;

	DisplayUpdate(false);

	pEdit->Text = "";

	m_bMacroKeyInputWait		= true;			// Macro Key �Է� ��� ���� ���.
	m_nMacroKeyInputType		= 0;			// 0 : Start / End. 1 : Skill Key.
	m_nMacroKeyInputIndex		= pEdit->Tag;	// ���� ���õ� Key �Է�ĭ ��ȣ.
}
//---------------------------------------------------------------------------

void __fastcall TfmMain::Edit_Key_Skill_1Click(TObject *Sender)
{
	TEdit * pEdit = dynamic_cast<TEdit *>(Sender);

	if(pEdit == NULL) return;

	DisplayUpdate(false);

	pEdit->Text = "";

	m_bMacroKeyInputWait		= true;			// Macro Key �Է� ��� ���� ���.
	m_nMacroKeyInputType		= 1;			// 0 : Start / End. 1 : Skill Key.
	m_nMacroKeyInputIndex		= pEdit->Tag;	// ���� ���õ� Key �Է�ĭ ��ȣ.
}
//---------------------------------------------------------------------------

void __fastcall TfmMain::Timer_DisplayTimer(TObject *Sender)
{
	Edit_Key_Start->Color = g_bMacroStarted  ? clLime : clBtnFace;
	Edit_Key_End->Color   = !g_bMacroStarted ? _MY_DISABLE_COLOR  : clBtnFace;


	if(m_bMacroKeyInputWait) {
		Panel_Macro_Msg->Caption = "Key�� �Է��ϼ���. (ESC : ����)";
	}
	else {
    	Panel_Macro_Msg->Caption = "";
	}

	for(int i=0; i<10; i++) {
		if(m_pMenuKey[i]) 				m_pMenuKey[i]->Enabled 				= !g_bMacroStarted;
		if(m_pSkillKey[i]) 				m_pSkillKey[i]->Enabled 			= !g_bMacroStarted;
		if(m_pSkillDelay[i]) 			m_pSkillDelay[i]->Enabled 			= !g_bMacroStarted;
		if(m_pChkBox_SkillEnable[i]) 	m_pChkBox_SkillEnable[i]->Enabled 	= !g_bMacroStarted;
	}

	Edit_Delay_Mouse_1->Enabled 		= !g_bMacroStarted;
	Edit_Delay_Mouse_2->Enabled 		= !g_bMacroStarted;

	CheckBox_Mouse_Enable_1->Enabled 	= !g_bMacroStarted;
	CheckBox_Mouse_Enable_2->Enabled 	= !g_bMacroStarted;

	Edit_TargetProName->Enabled 		= !g_bMacroStarted;
	Edit_Title->Enabled 				= !g_bMacroStarted;

	if(Timer_Seq->Enabled) {
		Button_SeqStart->Caption = Button_SeqStart->Tag != 0 ? "Started" : "";
		Button_SeqStart->Tag = Button_SeqStart->Tag != 0 ? 0 : 1;
	}
	else {
		Button_SeqStart->Tag = 0;
        Button_SeqStart->Caption = "Start";
	}

}
//---------------------------------------------------------------------------

void __fastcall TfmMain::Edit_Delay_Skill_1Change(TObject *Sender)
{
	TEdit * pEdit = dynamic_cast<TEdit *>(Sender);

	if(pEdit == NULL) return;

	int nDelay = pEdit->Text.ToIntDef(100);
	int nIndex = pEdit->Tag;

	m_nSkillDelay[nIndex] =  nDelay;

}
//---------------------------------------------------------------------------
void __fastcall TfmMain::Edit_Delay_Mouse_1Change(TObject *Sender)
{
	TEdit * pEdit = dynamic_cast<TEdit *>(Sender);

	if(pEdit == NULL) return;

	int nDelay = pEdit->Text.ToIntDef(1000);
	int nIndex = pEdit->Tag;

	m_nMouseDelay[nIndex] =  nDelay;
}
//---------------------------------------------------------------------------

void __fastcall TfmMain::SaveToFile(const String& filename)
{
	TFileStream* stream = new TFileStream(filename, fmCreate);

	if(stream == NULL) return;

	try
	{
		int nRet = 0;

		m_sSaveFileTitle = Edit_Title->Text.Trim();

		nRet = stream->Write(m_SkillKeys, 				sizeof(m_SkillKeys));
		nRet = stream->Write(m_SettingKeyHook,  		sizeof(m_SettingKeyHook));
		nRet = stream->Write(m_bSkillEnabled, 			sizeof(m_bSkillEnabled));
		nRet = stream->Write(m_nSkillDelay, 			sizeof(m_nSkillDelay));

		nRet = stream->Write(m_bMouseEnabled, 			sizeof(m_bMouseEnabled));
		nRet = stream->Write(m_nMouseDelay, 			sizeof(m_nMouseDelay));


		int nLen = m_sSaveFileTitle.Length();
		nRet = stream->Write(&nLen, 					sizeof(nLen));
		nRet = stream->Write(m_sSaveFileTitle.c_str(), 	m_sSaveFileTitle.Length() * sizeof(WideChar));


		//-------------------------------------------
		// bitmap.
		m_BitmapManager.SaveToFile(stream);

	}
	__finally
	{
		delete stream;
	}


	TStringList * pFileNameList = new TStringList;

	pFileNameList->Add(filename);
	pFileNameList->SaveToFile("LastSettingData.dat");

	delete pFileNameList;

}
//---------------------------------------------------------------------------

void __fastcall TfmMain::LoadFromFile(const String& filename)
{
	TFileStream* stream = new TFileStream(filename, fmOpenRead);

	if(stream == NULL) return;

    try
	{
		int nRet = 0;

		nRet = stream->Read(m_SkillKeys, 		sizeof(m_SkillKeys));
		nRet = stream->Read(m_SettingKeyHook,  	sizeof(m_SettingKeyHook));
		nRet = stream->Read(m_bSkillEnabled, 	sizeof(m_bSkillEnabled));
		nRet = stream->Read(m_nSkillDelay, 		sizeof(m_nSkillDelay));

		nRet = stream->Read(m_bMouseEnabled, 	sizeof(m_bMouseEnabled));
		nRet = stream->Read(m_nMouseDelay, 		sizeof(m_nMouseDelay));

		int len = 0;
		nRet = stream->Read(&len, sizeof(len)); // ���� �б�

		if(len > 0) {
			WCHAR *buffer = new WCHAR[len + 1];
			try {
				stream->Read(buffer, len * sizeof(WideChar)); // ������ �б�
				buffer[len] = L'\0'; // Null-terminate
				m_sSaveFileTitle = String(buffer);
			} __finally {
				delete[] buffer;
			}
		}

		//-------------------------------------------
		// bitmap.
		m_BitmapManager.LoadFromFile(stream);

	}
	__finally
	{
		delete stream;
	}

	TStringList * pFileNameList = new TStringList;

	pFileNameList->Add(filename);
	pFileNameList->SaveToFile("LastSettingData.dat");

	delete pFileNameList;
}
//---------------------------------------------------------------------------

void __fastcall TfmMain::OpenFile1Click(TObject *Sender)
{
	LoadFromFile("Setting.cfg");

	OpenDialog1->Filter = "���� ���� (*.cfg)|*.cfg|��� ���� (*.*)|*.*";

	if(OpenDialog1->Execute()) {

		m_sSaveFilePath = OpenDialog1->FileName;
		m_sSaveFileName = ExtractFileName(m_sSaveFilePath);

		LoadFromFile(m_sSaveFilePath);

		Edit_Title->Text = m_sSaveFileTitle;
	}

	DisplayUpdate(true);
}
//---------------------------------------------------------------------------

void __fastcall TfmMain::SaveFile1Click(TObject *Sender)
{
	if(m_sSaveFilePath != "" && FileExists(m_sSaveFilePath)) {

		SaveToFile(m_sSaveFilePath);
	}
	else {
		SaveDialog1->Filter = "���� ���� (*.cfg)|*.cfg|��� ���� (*.*)|*.*";

		if(SaveDialog1->Execute()) {

			m_sSaveFilePath = SaveDialog1->FileName;
			m_sSaveFileName = ExtractFileName(m_sSaveFilePath);

			SaveToFile(m_sSaveFilePath);
		}
	}

	DisplayUpdate(true);

}
//---------------------------------------------------------------------------
void __fastcall TfmMain::Saveas1Click(TObject *Sender)
{
	SaveDialog1->Filter = "���� ���� (*.cfg)|*.cfg|��� ���� (*.*)|*.*";

	if(SaveDialog1->Execute()) {

		m_sSaveFilePath = SaveDialog1->FileName;
		m_sSaveFileName = ExtractFileName(m_sSaveFilePath);

		// cfg ���Ϸ� ����.
		SaveToFile(m_sSaveFilePath);

		// ComboBox List ����.
		LoadCfgFilesToComboBox(m_sRootPath, m_sSaveFileName, ComboBox_FileList);

		DisplayUpdate(true);
	}
}
//---------------------------------------------------------------------------


void __fastcall TfmMain::Exit1Click(TObject *Sender)
{
	Close();
}
//---------------------------------------------------------------------------


void __fastcall TfmMain::Timer_WatchDogTimer(TObject *Sender)
{

	String sProcessName = Edit_TargetProName->Text;

	const wchar_t* targetExe = sProcessName.c_str();  // ã�� ���� ���� �̸�

	HWND hTarget = FindWindowByExeName(targetExe);


	if((m_hTargetWnd != NULL) && m_bTargetProcessFound) {

		if (hTarget == NULL) {
			g_bMacroStarted			= false;
			m_hTargetWnd 			= NULL;
			m_bTargetProcessFound 	= false;

			DisplayUpdate(false);

			ShowMessage("��� ���μ����� â�� ã�� ���߽��ϴ�.");
		}
	}
	else {
		if (hTarget != NULL) {

			m_hTargetWnd 			= hTarget;
			m_bTargetProcessFound 	= true;

			DisplayUpdate(false);
		}
	}

	if(m_bTargetProcessFound)
		Edit_TargetProName->Color = _MY_ENABLE_COLOR;
	else
		Edit_TargetProName->Color = Edit_TargetProName->Color != _MY_DISABLE_COLOR ? _MY_DISABLE_COLOR : clWhite;
}
//---------------------------------------------------------------------------

#include <System.IOUtils.hpp>

// �־��� �������� .cfg ������ �˻��Ͽ� �޺��ڽ��� �߰�
void __fastcall TfmMain::LoadCfgFilesToComboBox(const UnicodeString &folderPath, const UnicodeString &SelectFileName, TComboBox *comboBox)
{
    try
    {
        // �޺��ڽ� �ʱ�ȭ
        comboBox->Clear();

        // .cfg ���� ��� ��������
        TSearchOption searchOption = TSearchOption::soTopDirectoryOnly; // �������� �˻� �� ��
        TStringDynArray files = TDirectory::GetFiles(folderPath, "*.cfg", searchOption);

		// ���� ����� �޺��ڽ��� �߰�
		int nSelectFileIndex = 0;

        for (int i = 0; i < files.Length; i++)
		{
			String sFileName = ExtractFileName(files[i]);
			comboBox->Items->Add(sFileName);

			if(sFileName == SelectFileName) {
				nSelectFileIndex = i;
			}

		}

		// �⺻ ���� ����
        if (comboBox->Items->Count > 0)
        {
            comboBox->ItemIndex = nSelectFileIndex;
        }
    }
    catch (const Exception &e)
    {
        ShowMessage(L"���� �˻� �� ������ �߻��߽��ϴ�: " + e.Message);
	}
}


//---------------------------------------------------------------------------
void __fastcall TfmMain::ComboBox_FileListClick(TObject *Sender)
{
	String sSelectFileName 	= ComboBox_FileList->Text;
	String sFullPath		= EnsureTrailingBackslash(m_sRootPath) + sSelectFileName;

	if(FileExists(sFullPath)) {
		m_sSaveFileName = sSelectFileName;
		m_sSaveFilePath = sFullPath;

		LoadFromFile(sFullPath);
	}
	else {
		LoadCfgFilesToComboBox(m_sRootPath, m_sSaveFileName, ComboBox_FileList);
	}

	DisplayUpdate(true);
}
//---------------------------------------------------------------------------



void __fastcall TfmMain::Button_Start_StopClick(TObject *Sender)
{

	if(g_bMacroStarted != true) {
		// Macro Start.
		if(fmMain->m_bTargetProcessFound) {
			g_bMacroStarted = true;
			PlaySound("C:\\Windows\\Media\\Speech On.wav", NULL, SND_ALIAS | SND_ASYNC);
		}
	}
	else {
		// Macro Stop.
		g_bMacroStarted = false;
		PlaySound("C:\\Windows\\Media\\Speech Off.wav", NULL, SND_ALIAS | SND_ASYNC);
	}

	DisplayUpdate(true);
}

//---------------------------------------------------------------------------

void __fastcall TfmMain::Button_AllScreenCaptureMouseMove(TObject *Sender, TShiftState Shift,
          int X, int Y)
{
   //
}
//---------------------------------------------------------------------------

void __fastcall TfmMain::Button_AllScreenCaptureMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y)
{
//
}
//---------------------------------------------------------------------------

void __fastcall TfmMain::Button_AllScreenCaptureMouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y)
{
//
}
//---------------------------------------------------------------------------


void __fastcall TfmMain::Button_AllScreenCaptureClick(TObject *Sender)
{
	Hide();

	Timer_ScreenCapture->Enabled = true;

}
//---------------------------------------------------------------------------
void __fastcall TfmMain::Timer_ScreenCaptureTimer(TObject *Sender)
{
	Timer_ScreenCapture->Enabled = false;

	fmAllScreen = new TfmAllScreen(this);
	int nRet = fmAllScreen->ShowModal();

	if(nRet == mrOk) {
		//Graphics::TBitmap * m_bmpCapture = new Graphics::TBitmap;
		//m_bmpCapture->Assign(fmAllScreen->m_bmpCapture);

		m_StartPos = fmAllScreen->m_MouseStart;
		m_EndPos   = fmAllScreen->m_MouseEnd;

		Image_Capture->Picture->Bitmap->Assign(fmAllScreen->m_bmpCapture);
		Image_Capture->Repaint();

		//delete m_bmpCapture;
	}

	delete fmAllScreen;

	Show();
}
//---------------------------------------------------------------------------


void __fastcall TfmMain::Button_AddBitmapClick(TObject *Sender)
{
	if(Image_Capture->Picture->Bitmap->Empty) return;

	int nKey   = m_BitmapManager.GetMaxKey() + 1;

	m_BitmapManager.AddBitmap(nKey, Image_Capture->Picture->Bitmap, m_StartPos, m_EndPos);

	ListBox_Bitmap->Items->Add(nKey);

	StatusBar1->Panels->Items[10]->Text = nKey;

}
//---------------------------------------------------------------------------
void __fastcall TfmMain::Button_DeleteBitmapClick(TObject *Sender)
{
	int nLastIndex = ListBox_Bitmap->ItemIndex;

	if(nLastIndex >= 0
	&& nLastIndex < ListBox_Bitmap->Count) {
		int nKey = ListBox_Bitmap->Items->Strings[ListBox_Bitmap->ItemIndex].ToIntDef(-1);

		if(nKey == -1 ) return;

		m_BitmapManager.DeleteBitmap(nKey);
		ListBox_Bitmap->DeleteSelected();

		if(nLastIndex < ListBox_Bitmap->Count)
			ListBox_Bitmap->ItemIndex = nLastIndex;
		else if(nLastIndex > 0)
            ListBox_Bitmap->ItemIndex = nLastIndex - 1;
	}
}
//---------------------------------------------------------------------------

void __fastcall TfmMain::Button_ChangeBitmapClick(TObject *Sender)
{
	if(Image_Capture->Picture->Bitmap->Empty) return;

	if(ListBox_Bitmap->ItemIndex >= 0
	&& ListBox_Bitmap->ItemIndex < ListBox_Bitmap->Count) {
		int nKey = ListBox_Bitmap->Items->Strings[ListBox_Bitmap->ItemIndex].ToIntDef(-1);

		if(nKey <= -1 ) return;

		m_BitmapManager.AddBitmap(nKey, Image_Capture->Picture->Bitmap, m_StartPos, m_EndPos);

		DisplayUpdate(true);
	}
}
//---------------------------------------------------------------------------

void __fastcall TfmMain::ListBox_BitmapDrawItem(TWinControl *Control, int Index, TRect &Rect, TOwnerDrawState State)
{
    TListBox *ListBox = dynamic_cast<TListBox*>(Control);
    if (!ListBox) return;

    TCanvas *Canvas = ListBox->Canvas;
	Canvas->FillRect(Rect);

	int nKey = ListBox_Bitmap->Items->Strings[Index].ToIntDef(-1);

	if(nKey == -1) return;

	// �̹��� �ε� �� ���
	Graphics::TBitmap *bmp = m_BitmapManager.GetBitmap(nKey);

	if (bmp)
	{
		Canvas->StretchDraw(TRect(Rect.Left + 2, Rect.Top + 2, Rect.Left + 50, Rect.Bottom - 2), bmp);

		Canvas->Brush->Style = bsClear;
		Canvas->Rectangle(Rect.Left + 2, Rect.Top + 2, Rect.Left + 50, Rect.Bottom - 2);
	}

	// �ؽ�Ʈ ���
	Canvas->TextOut(Rect.Left + 55, Rect.Top + 15, ListBox->Items->Strings[Index]);

}
//---------------------------------------------------------------------------

void __fastcall TfmMain::ListBox_BitmapClick(TObject *Sender)
{
	try {
		int nIndex = ListBox_Bitmap->ItemIndex;

		if(nIndex < 0 || nIndex >= ListBox_Bitmap->Count) return;

		int nKey = ListBox_Bitmap->Items->Strings[nIndex].ToIntDef(-1);

		if(nKey == -1) return;

		Image_SaveImage->Picture->Bitmap->Assign(m_BitmapManager.GetBitmap(nKey));
	}
	__finally {

	}


}
//---------------------------------------------------------------------------

void __fastcall TfmMain::ListBox_BitmapKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
	if(Key == 46) { // delete key.
		Button_DeleteBitmapClick(NULL);
	}
}
//---------------------------------------------------------------------------
int __fastcall TfmMain::SeqRegame()
{
	switch(m_nSeqStep) {
		case 0:
			// ������ �̵�.
			SetForegroundWindow(m_hTargetWnd);
			::Sleep(100);

			SendKeyboardEvent('T');
			m_DelayTimer.StartDelay(6000);
			m_nSeqStep = 10;
			break;

		case 10:
			if(m_DelayTimer.IsDelayEnd()) {
				// ESC Key
				SendKeyboardEvent(0x1b, 0x01, 0); // ESC.
				m_DelayTimer.StartDelay(1000);
				m_nSeqStep = 20;
			}
			break;

		case 20:
			if(m_DelayTimer.IsDelayEnd()) {
				// ���� ������ ����
				SendMouseMove(244, 481);
				::Sleep(200);
				SendLeftMouseClick(true);
				::Sleep(200);
				SendLeftMouseClick(false);
				m_DelayTimer.StartDelay(5000);
				m_nSeqStep = 30;
			}
			break;

		case 30:
			if(m_DelayTimer.IsDelayEnd()) {
				// ���� ���� ����.
				SendMouseMove(236, 515);
				::Sleep(50);
				SendLeftMouseClick(true);
				::Sleep(100);
				SendLeftMouseClick(false);
				m_DelayTimer.StartDelay(8000);
				m_nSeqStep = 90;
			}
			break;

		case 90:
			if(m_DelayTimer.IsDelayEnd()) {
				SendKeyboardEvent('M');  // ���� ���� ����
				m_DelayTimer.StartDelay(100);
				m_nSeqStep = 100;
			}
			break;

		case 100:
			if(m_DelayTimer.IsDelayEnd()) {
				// ������������ ��ü ���� ���� ����
				SendMouseMove(897, 128);
				SendLeftMouseClick(true);
				SendLeftMouseClick(false);
				m_DelayTimer.StartDelay(100);
				m_nSeqStep = 200;
			}
			break;

		case 200:
			if(m_DelayTimer.IsDelayEnd()) {
				// 1�� ����
				//SendMouseMove(736, 619);

				// 2��
				//SendMouseMove(1088, 520);

				// 3��
				SendMouseMove(710, 388);



				SendLeftMouseClick(true);
				SendLeftMouseClick(false);
				m_DelayTimer.StartDelay(100);
				m_nSeqStep = 300;
			}
			break;

		case 300:
			if(m_DelayTimer.IsDelayEnd()) {
				// �������� ���� �ȶ� ����.
				// SendMouseMove(583, 583);

				// �뼺�� ���� 1�� (���� ����)
				// SendMouseMove(1100, 289);

				// �ޱ��� ���ƽý� (��������)
				// SendMouseMove(484, 519);

				// �ڸ���ũ ���� (��������)
				SendMouseMove(801, 461);

				SendLeftMouseClick(true);
				SendLeftMouseClick(false);
				m_DelayTimer.StartDelay(3000);
				m_nSeqStep = 400;
			}
			break;

		case 400:
			if(m_DelayTimer.IsDelayEnd()) {




				// ���� ���콺 ��ų ��� ���. (�ٹ� ���)
				KBDLLHOOKSTRUCT KeyInfo;

				KeyInfo.vkCode 		= 0xC0;
				KeyInfo.scanCode 	= 0x29;
				KeyInfo.flags 		= 0;

				SendKeyboardEvent(KeyInfo, true);

				::Sleep(100);
				SendLeftMouseClick(true);
				::Sleep(1000);
				SendLeftMouseClick(false);

				SendKeyboardEvent(KeyInfo, false);

				SendKeyboardEvent('1');  //
				SendKeyboardEvent('2');  //
				SendKeyboardEvent('3');  //
				SendKeyboardEvent('4');  //

				m_nSeqStep = 9000;
				break;
				//////////////////////////////////////////////////////////


				// �������� ���� �ȶ� �� �̵�...

				SendMouseMove(450, 100);
				SendRightMouseClick(true);
				m_DelayTimer.StartDelay(2000);
				m_nSeqStep = 500;
			}
			break;

		case 500:
			if(m_DelayTimer.IsDelayEnd()) {
				// �̵� �Ϸ�...
				SendRightMouseClick(false);
				m_DelayTimer.StartDelay(500);
				m_nSeqStep = 600;
			}
			break;

		case 600:
			if(m_DelayTimer.IsDelayEnd()) {
				// �������� ���� �ȶ� ����
				SendMouseMove(954, 445);
				::Sleep(300);
				SendLeftMouseClick(true);
				::Sleep(100);
				SendLeftMouseClick(false);
				m_DelayTimer.StartDelay(10);
				m_nSeqStep = 700;
			}
			break;

		case 700:
			if(m_DelayTimer.IsDelayEnd()) {
				// �������� ���� �߰� �� �̵�. ��ų ����
				SendMouseMove(550, 103);
				::Sleep(100);

				SendKeyboardEvent('1');  //
				SendKeyboardEvent('2');  //
				SendKeyboardEvent('3');  //
				SendKeyboardEvent('4');  //

				SendRightMouseClick(true);

				m_DelayTimer.StartDelay(3000);
				m_nSeqStep = 800;
			}
			break;

		case 800:
			if(m_DelayTimer.IsDelayEnd()) {
				// �� ���� �ձ��� �̵�.
				SendRightMouseClick(false);
				::Sleep(100);

				SendMouseMove(1283, 137);
				::Sleep(100);

				SendKeyboardEvent('1');  // ���� ���� ����
				SendKeyboardEvent('2');  // ���� ���� ����

				SendRightMouseClick(true);

				m_DelayTimer.StartDelay(2500);
				m_nSeqStep = 900;

			}
			else {
            	SendKeyboardEvent('2');  //
			}
			break;

		case 900:
			if(m_DelayTimer.IsDelayEnd()) {
				// �� ���� �ձ��� �̵� �Ϸ�.
				SendRightMouseClick(false);

				m_DelayTimer.StartDelay(100);
				m_nSeqStep = 9000;

			}
			break;

		case 9000:
			Timer_Seq->Enabled = false;
			m_nSeqStep = 0;
			break;

	}

	StatusBar1->Panels->Items[10]->Text =  m_nSeqStep;

	return m_nSeqStep;
}

//------------------------------------------------------------------------------
int __fastcall TfmMain::SeqSaleItem()
{
	// ���� â�� �̹� ���� �־�� �Ѵ�.

	switch(m_nSeqStep) {
		case 0:
			// ���� Tab ����.
			SendMouseMove(515, 485);
			::Sleep(100);
			SendLeftMouseClick(true);
			::Sleep(50);
			SendLeftMouseClick(false);
			m_nSeqStep = 100;
			break;

		case 100:
			// ��� �ȱ�.
			SendMouseMove(254, 290);
			::Sleep(100);
			SendLeftMouseClick(true);
			::Sleep(50);
			SendLeftMouseClick(false);

			m_DelayTimer.StartTimer(500);
			m_nSeqStep = 200;
			break;

		case 200:
			// Ȯ�� â ��� Ȯ��.
			{
				TBitmapData *pBitmapData = m_BitmapManager.GetBitmpaData(4); // Ȯ��â ��ư data ���� ����.

				if(pBitmapData) {
					Graphics::TBitmap * pBitmap = CaptureScreenRegion(	pBitmapData->StartPos.X,
																		pBitmapData->StartPos.Y,
																		pBitmapData->EndPos.X - pBitmapData->StartPos.X,
																		pBitmapData->EndPos.Y - pBitmapData->StartPos.Y);

					Image1->Picture->Bitmap = pBitmap;
					Image1->Repaint();


					double similarity = CompareBitmaps(pBitmap, pBitmapData->bitmap);

					delete pBitmap;

					Memo1->Lines->Add(similarity);

					if(similarity >= 95 ) {
						// Ȯ�� â�� ��.

						SendMouseMove(852, 375);
						::Sleep(50);
						SendLeftMouseClick(true);
						::Sleep(50);
						SendLeftMouseClick(false);

						m_DelayTimer.StartTimer(300);
						m_nSeqStep = 300;
					}
					else if(m_DelayTimer.IsTimeOut()) {
						// â�� ���� ����.
						m_DelayTimer.StartTimer(100);
						m_nSeqStep = 300;
					}
				}
				else {
					m_nSeqStep = 300;
				}
			}
			break;

		case 300:
			// �ķ��� �ȱ�.
			SendMouseMove(317, 290);
			::Sleep(100);
			SendLeftMouseClick(true);
			::Sleep(50);
			SendLeftMouseClick(false);

			m_DelayTimer.StartTimer(500);
			m_nSeqStep = 400;
			break;

		case 400:
			// Ȯ�� â ��� Ȯ��.
			{
				TBitmapData *pBitmapData = m_BitmapManager.GetBitmpaData(4); // Ȯ��â ��ư data ���� ����.

				if(pBitmapData) {
					Graphics::TBitmap * pBitmap = CaptureScreenRegion(	pBitmapData->StartPos.X,
																		pBitmapData->StartPos.Y,
																		pBitmapData->EndPos.X - pBitmapData->StartPos.X,
																		pBitmapData->EndPos.Y - pBitmapData->StartPos.Y);

					Image1->Picture->Bitmap->Assign(pBitmap);

					double similarity = CompareBitmaps(pBitmap, pBitmapData->bitmap);

					delete pBitmap;

					Memo1->Lines->Add(similarity);

					if(similarity >= 95 ) {
						// Ȯ�� â�� ��.

						SendMouseMove(852, 375);
						::Sleep(50);
						SendLeftMouseClick(true);
						::Sleep(50);
						SendLeftMouseClick(false);

						m_DelayTimer.StartTimer(300);
						m_nSeqStep = 500;
					}
					else if(m_DelayTimer.IsTimeOut()) {
						// â�� ���� ����.
						m_DelayTimer.StartTimer(100);
						m_nSeqStep = 500;
					}
				}
				else {
					m_nSeqStep = 500;
				}
			}
			break;


		case 500:
			// �뷩�� �ȱ�.
			SendMouseMove(387, 290);
			::Sleep(100);
			SendLeftMouseClick(true);
			::Sleep(50);
			SendLeftMouseClick(false);

			m_DelayTimer.StartTimer(500);
			m_nSeqStep = 600;
			break;

		case 600:
			// Ȯ�� â ��� Ȯ��.
			{
				TBitmapData *pBitmapData = m_BitmapManager.GetBitmpaData(4); // Ȯ��â ��ư data ���� ����.

				if(pBitmapData) {
					Graphics::TBitmap * pBitmap = CaptureScreenRegion(	pBitmapData->StartPos.X,
																		pBitmapData->StartPos.Y,
																		pBitmapData->EndPos.X - pBitmapData->StartPos.X,
																		pBitmapData->EndPos.Y - pBitmapData->StartPos.Y);

					Image1->Picture->Bitmap = pBitmap;

					double similarity = CompareBitmaps(pBitmap, pBitmapData->bitmap);

					delete pBitmap;

					Memo1->Lines->Add(similarity);

					if(similarity >= 95 ) {
						// Ȯ�� â�� ��.

						SendMouseMove(852, 375);
						::Sleep(50);
						SendLeftMouseClick(true);
						::Sleep(50);
						SendLeftMouseClick(false);

						m_DelayTimer.StartTimer(300);
						m_nSeqStep = 700;
					}
					else if(m_DelayTimer.IsTimeOut()) {
						// â�� ���� ����.
						m_DelayTimer.StartTimer(100);
						m_nSeqStep = 700;
					}
				}
				else {
					m_nSeqStep = 700;
				}
			}
			break;

		//---------------------------------------------------
		// ���� �ȱ�.


		case 700:
			// ���� �ȱ� ����.
			SendMouseMove(169, 290);
			::Sleep(50);
			SendLeftMouseClick(true);
			::Sleep(50);
			SendLeftMouseClick(false);

			m_nInventoryX = 0;
			m_nInventoryY = 0;

			m_TackTimer.StartTimer(0);
			m_DelayTimer.StartTimer(50);

			m_nSeqStep = 800;
			break;

		case 800:
			if(m_DelayTimer.IsTimeOut()) {

				TBitmapData *pBitmapData = m_BitmapManager.GetBitmpaData(5); // Inventory Empty Image ���� ����.

				Graphics::TBitmap * pBitmap = NULL;
				int nCaptureWidht  = pBitmapData->EndPos.X - pBitmapData->StartPos.X;
				int nCaptureHeight = pBitmapData->EndPos.Y - pBitmapData->StartPos.Y;
				int nCaptureStartX = 0;
				int nCaptureStartY = 0;
				bool bFound = false;
				AnsiString sMsg;

				for(int y=m_nInventoryY; y<6; y++) {
					m_nInventoryX = 2;

					for(int x=m_nInventoryX; x<10; x++) {

						nCaptureStartX = pBitmapData->StartPos.X + (int)((double)x * 50.33);
						nCaptureStartY = pBitmapData->StartPos.Y + (int)((double)y * 50.33);

						pBitmap = CaptureScreenRegion(	nCaptureStartX,	nCaptureStartY,
														nCaptureWidht,  nCaptureHeight);

						Image1->Picture->Bitmap = pBitmap;

						double similarity = CompareBitmaps(pBitmap, pBitmapData->bitmap);

						delete pBitmap;

						if(similarity < 99.0) {
							bFound = true;

							m_nInventoryX = x;
							m_nInventoryY = y;

							sMsg.printf("Inventory (%d, %d) : %0.2f Item found. ", x, y, similarity);
							Memo1->Lines->Add(sMsg);

							break;
						}
						else {
							sMsg.printf("Inventory (%d, %d) : %0.2f Empty. ", x, y, similarity);
							Memo1->Lines->Add(sMsg);
						}
					}

					if(bFound) {
                    	break;
					}
				}

				if(bFound) {
					SendMouseMove(nCaptureStartX + 15, nCaptureStartY + 20);
					::Sleep(30);
					SendLeftMouseClick(true);
					::Sleep(30);
					SendLeftMouseClick(false);

					m_DelayTimer.StartTimer(200);
					m_nSeqStep = 900;

					m_nInventoryX += 1;
					if(m_nInventoryX >= 10) {
						m_nInventoryY += 1;
                        m_nInventoryX = 0;
					}
				}
				else {
					double dLapTime = m_TackTimer.GetLapTimeMM();
					AnsiString sMsg;
					sMsg.printf("Inventory Sale Time : %d", (int)dLapTime);
					Memo1->Lines->Add(sMsg);

					m_DelayTimer.StartTimer(200);

					// �Ϸ� ��.
					m_nSeqStep = 1000;
				}
			}
			break;

		case 900:
			// Ȯ�� â ��� Ȯ��.
			{
#if 0
				SendKeyboardEvent(0x0D);
				m_DelayTimer.StartTimer(50);
				m_nSeqStep = 800;
#else

				TBitmapData *pBitmapData = m_BitmapManager.GetBitmpaData(4); // Ȯ��â ��ư data ���� ����.

				if(pBitmapData) {
					Graphics::TBitmap * pBitmap = CaptureScreenRegion(	pBitmapData->StartPos.X,
																		pBitmapData->StartPos.Y,
																		pBitmapData->EndPos.X - pBitmapData->StartPos.X,
																		pBitmapData->EndPos.Y - pBitmapData->StartPos.Y);

					Image1->Picture->Bitmap = pBitmap;

					double similarity = CompareBitmaps(pBitmap, pBitmapData->bitmap);

					delete pBitmap;

					Memo1->Lines->Add(similarity);

					if(similarity >= 95 ) {
						// Ȯ�� â�� ��.
						SendMouseMove(852, 375);
						::Sleep(50);
						SendLeftMouseClick(true);
						::Sleep(30);
						SendLeftMouseClick(false);

						m_DelayTimer.StartTimer(100);
						m_nSeqStep = 800;
					}
					else if(m_DelayTimer.IsTimeOut()) {
						// â�� ���� ����.
						m_DelayTimer.StartTimer(100);
						m_nSeqStep = 800;
					}
				}
				else {
					m_nSeqStep = 9000;
				}

#endif
			}
			break;


		case 1000:
			SendMouseMove(513, 613);
			::Sleep(30);
			SendLeftMouseClick(true);
			::Sleep(30);
			SendLeftMouseClick(false);

			m_DelayTimer.StartTimer(50);
			m_nSeqStep = 1100;
			break;

		case 1100:
			if(m_DelayTimer.IsDelayEnd()) {
				SendMouseMove(269, 531);
				::Sleep(30);
				SendLeftMouseClick(true);
				::Sleep(30);
				SendLeftMouseClick(false);

				m_DelayTimer.StartTimer(200);
				m_nSeqStep = 9000;
			}
			break;

		case 9000:
			if(m_DelayTimer.IsDelayEnd()) {
				SendKeyboardEvent(0x1b, 0x01, 0); // ESC.
				m_nSeqStep = 0;
				Timer_Seq->Enabled = false;
			}
			break;
	}

	StatusBar1->Panels->Items[10]->Text =  m_nSeqStep;

	return m_nSeqStep;
}


//------------------------------------------------------------------------------


