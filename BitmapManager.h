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
	TBitmapData() : bitmap(NULL), StartPos(0, 0), EndPos(0, 0) {

    }

    TBitmapData(const TBitmapData& other)
        : StartPos(other.StartPos), EndPos(other.EndPos) {
        if (other.bitmap) {
            bitmap = new Graphics::TBitmap();
            bitmap->Assign(other.bitmap);
        } else {
			bitmap = NULL;
        }
	}

    ~TBitmapData() {
        delete bitmap;
	}

    void SaveToFile(TFileStream* fs) {
        if (!fs) return;

        // StartPos, EndPos 저장
        fs->Write(&StartPos, sizeof(TPoint));
        fs->Write(&EndPos, sizeof(TPoint));

        // bitmap 저장
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

        // StartPos, EndPos 로드
        fs->Read(&StartPos, sizeof(TPoint));
        fs->Read(&EndPos, sizeof(TPoint));

        // bitmap 로드
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

	Graphics::TBitmap * bitmap;
	TPoint StartPos;
	TPoint EndPos;
};


class TBitmapManager {
private:
	// TObjectList* m_Bitmaps;  // TBitmap 객체 저장 컨테이너

	std::map<int, Graphics::TBitmap*> bitmapMap;


public:
	TBitmapManager();
	~TBitmapManager();

    // 비트맵 관리

	void ClearAll();

	int  AddBitmap(int key, Graphics::TBitmap * pBitmap);
	int  DeleteBitmap(int index);
	bool IsKeyExist(int key);

	Graphics::TBitmap * GetBitmap(int key);

	int GetMaxKey();
	int Count() const { return bitmapMap.size(); }

	// 파일 입출력
	void SaveToFile(TFileStream* fs);
	void LoadFromFile(TFileStream* fs);


};

#endif
