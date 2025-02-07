//-----------------------------------------------------------
// BitmapManager.h
//-----------------------------------------------------------
#ifndef BitmapManagerH
#define BitmapManagerH

#include <System.Classes.hpp>
#include <Vcl.Graphics.hpp>
#include <Vcl.Dialogs.hpp>
#include <System.Contnrs.hpp>

#include <map>

class TBitmapData {

public:
	Graphics::TBitmap * bitmap;
	TPoint 				StartPos;
	TPoint 				EndPos;
	int					ID;

public:
	TBitmapData() : bitmap(NULL), StartPos(0, 0), EndPos(0, 0), ID(-1) {

	}

	TBitmapData(const Graphics::TBitmap * pBmp, const TPoint start, const TPoint end, const int id ) {
		bitmap = new Graphics::TBitmap();
		bitmap->Assign(pBmp);
		StartPos 	= start;
		EndPos		= end;
		ID 			= id;
	}

	// ���� ������.
	TBitmapData(const TBitmapData& other) : StartPos(other.StartPos), EndPos(other.EndPos), ID(other.ID) {

		if(bitmap != NULL) delete bitmap;
		bitmap = NULL;

		if (other.bitmap) {
			bitmap = new Graphics::TBitmap();
			bitmap->Assign(other.bitmap);
        } else {
			bitmap = NULL;
        }
	}

    ~TBitmapData() {
		if(bitmap) delete bitmap;
	}

    void SaveToFile(TFileStream* fs) {
        if (!fs) return;

        // StartPos, EndPos ����
		fs->Write(&ID, 			sizeof(ID));
		fs->Write(&StartPos, 	sizeof(TPoint));
		fs->Write(&EndPos, 		sizeof(TPoint));

        // bitmap ����
        if (bitmap) {
            bool hasBitmap = true;
            fs->Write(&hasBitmap, sizeof(bool));
            bitmap->SaveToStream(fs);
        } else {
            bool hasBitmap = false;
            fs->Write(&hasBitmap, sizeof(bool));
        }
    }

    void LoadFromFile(TFileStream* fs) {
        if (!fs) return;

		// StartPos, EndPos �ε�
		fs->Read(&ID, 			sizeof(ID));
		fs->Read(&StartPos, 	sizeof(TPoint));
		fs->Read(&EndPos, 		sizeof(TPoint));

        // bitmap �ε�
        bool hasBitmap;
        fs->Read(&hasBitmap, sizeof(bool));

        if (hasBitmap) {
            if (!bitmap) {
                bitmap = new Graphics::TBitmap();
            }
            bitmap->LoadFromStream(fs);
        } else {
            delete bitmap;
			bitmap = NULL;
        }
	}
};


class TBitmapManager {
private:
	// TObjectList* m_Bitmaps;  // TBitmap ��ü ���� �����̳�

	std::map<int, TBitmapData *> bitmapMap;


public:
	TBitmapManager();
	~TBitmapManager();

    // ��Ʈ�� ����

	void ClearAll();

	int  AddBitmap(int key, Graphics::TBitmap * pBitmap, TPoint start, TPoint end);
	int  DeleteBitmap(int index);
	bool IsKeyExist(int key);

	Graphics::TBitmap * GetBitmap(int key);
	TBitmapData * GetBitmpaData(int key);

	int GetMaxKey();
	int Count() const { return bitmapMap.size(); }

	// ���� �����
	void SaveToFile(TFileStream* fs);
	void LoadFromFile(TFileStream* fs);


};

#endif
