//-----------------------------------------------------------
// BitmapManager.cpp
//-----------------------------------------------------------
#include "BitmapManager.h"
#include <System.SysUtils.hpp>

TBitmapManager::TBitmapManager()
{

}

TBitmapManager::~TBitmapManager() {

 	ClearAll();

}

// 모든 비트맵 삭제
void TBitmapManager::ClearAll() {

	// 저장된 모든 비트맵을 해제
	std::map<int, Graphics::TBitmap *>::iterator it;

    for (it = bitmapMap.begin(); it != bitmapMap.end(); ++it)
	{
		Graphics::TBitmap * pBitmap = (Graphics::TBitmap*)(it->second);
		delete pBitmap;
	}

	bitmapMap.clear();
}

int TBitmapManager::GetMaxKey()
{
	if (bitmapMap.empty())
    {
        return -1; // 비어있을 경우 -1 반환
    }

    std::map<int, Graphics::TBitmap*>::iterator it = bitmapMap.end();
    --it; // map은 key 기준으로 정렬되므로 마지막 요소가 가장 큰 key
    return it->first;
}

bool TBitmapManager::IsKeyExist(int key)
{
	std::map<int, Graphics::TBitmap*>::iterator it = bitmapMap.find(key);

	if (it != bitmapMap.end())
	{
		return true;
	}

	return false;
}

int TBitmapManager::AddBitmap(int key, Graphics::TBitmap * pBitmap)
{
	std::map<int, Graphics::TBitmap*>::iterator it = bitmapMap.find(key);

	if (it != bitmapMap.end())
	{
		Graphics::TBitmap * pBitmap = (Graphics::TBitmap*)(it->second);
		delete pBitmap;
	}

	Graphics::TBitmap * bmp = new  Graphics::TBitmap();
	bmp->Assign(pBitmap);

	bitmapMap[key] = bmp;
}

int TBitmapManager::DeleteBitmap(int key)
{
	std::map<int, Graphics::TBitmap*>::iterator it = bitmapMap.find(key);

	if (it != bitmapMap.end())
    {
		Graphics::TBitmap * pBitmap = (Graphics::TBitmap*)(it->second);
		delete pBitmap;

		bitmapMap.erase(it);
	}

	return  bitmapMap.size();
}

// 특정 인덱스 비트맵 반환
Graphics::TBitmap* TBitmapManager::GetBitmap(int key)
{
	std::map<int, Graphics::TBitmap*>::iterator it = bitmapMap.find(key);

    if (it != bitmapMap.end())
    {
		return (Graphics::TBitmap *)(it->second);
	}

	return NULL;
}

// 사용자 정의 파일 포맷 저장
void TBitmapManager::SaveToFile(TFileStream* fs)
{

    try {
		int count = bitmapMap.size();
		fs->Write(&count, sizeof(int));

		std::map<int, Graphics::TBitmap*>::iterator it;
		for (it = bitmapMap.begin(); it != bitmapMap.end(); ++it)
		{
			int key = it->first;
			Graphics::TBitmap* bmp = it->second;

			fs->Write(&key, sizeof(int));
			bmp->SaveToStream(fs);
		}
    }
    __finally {

    }
}

// 사용자 정의 파일 포맷 로드
void TBitmapManager::LoadFromFile(TFileStream* fs)
{
	// Object List 초기화.
	ClearAll();

	if(fs == NULL) return;

	if(fs->Position >= fs->Size) return; // 이미 다 읽었다.

	try {
		int count;

		fs->Read(&count, sizeof(int));

		for (int i = 0; i < count; i++)
		{
			int key;
			fs->Read(&key, sizeof(int));

			Graphics::TBitmap* bmp = new Graphics::TBitmap();
			bmp->LoadFromStream(fs);

			bitmapMap[key] = bmp;
		}

    }
    __finally {

    }
}

