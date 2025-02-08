object fmAllScreen: TfmAllScreen
  Left = 0
  Top = 0
  BorderIcons = []
  BorderStyle = bsNone
  Caption = 'User Capture Image'
  ClientHeight = 378
  ClientWidth = 347
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  KeyPreview = True
  OldCreateOrder = False
  Position = poDefault
  OnClose = FormClose
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  OnKeyDown = FormKeyDown
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Image1: TImage
    Left = 0
    Top = 0
    Width = 347
    Height = 299
    Align = alClient
    OnMouseDown = Image1MouseDown
    OnMouseMove = Image1MouseMove
    OnMouseUp = Image1MouseUp
    ExplicitWidth = 651
    ExplicitHeight = 338
  end
  object Panel_FrameBottom: TPanel
    Left = 0
    Top = 299
    Width = 347
    Height = 60
    Align = alBottom
    BevelOuter = bvNone
    Caption = 'Panel_FrameBottom'
    ShowCaption = False
    TabOrder = 0
    Visible = False
    object BitBtn1: TBitBtn
      Left = 8
      Top = 6
      Width = 89
      Height = 49
      Kind = bkOK
      NumGlyphs = 2
      TabOrder = 0
    end
    object BitBtn2: TBitBtn
      Left = 103
      Top = 6
      Width = 89
      Height = 49
      Kind = bkCancel
      NumGlyphs = 2
      TabOrder = 1
    end
  end
  object StatusBar1: TStatusBar
    Left = 0
    Top = 359
    Width = 347
    Height = 19
    Panels = <
      item
        Text = '0000'
        Width = 100
      end
      item
        Text = '0000'
        Width = 50
      end>
    Visible = False
  end
end
