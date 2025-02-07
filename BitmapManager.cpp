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
	std::map<int, TBitmapData *>::iterator it;

    for (it = bitmapMap.begin(); it != bitmapMap.end(); ++it)
	{
		TBitmapData * pbitmapdata = (TBitmapData *)(it->second);
		delete pbitmapdata;
	}

	bitmapMap.clear();
}

int TBitmapManager::GetMaxKey()
{
	if (bitmapMap.empty())
    {
        return -1; // 비어있을 경우 -1 반환
    }

    std::map<int, TBitmapData *>::iterator it = bitmapMap.end();
    --it; // map은 key 기준으로 정렬되므로 마지막 요소가 가장 큰 key
    return it->first;
}

bool TBitmapManager::IsKeyExist(int key)
{
	std::map<int, TBitmapData *>::iterator it = bitmapMap.find(key);

	if (it != bitmapMap.end())
	{
		return true;
	}

	return false;
}

int TBitmapManager::AddBitmap(int key, Graphics::TBitmap * pBitmap, TPoint start, TPoint end)
{
	std::map<int, TBitmapData *>::iterator it = bitmapMap.find(key);

	if (it != bitmapMap.end())
	{
		TBitmapData * pBitmapData = (TBitmapData *)(it->second);
		delete pBitmapData;
	}

	TBitmapData * pbitmapData = new  TBitmapData(pBitmap, start, end, key);

	bitmapMap[key] = pbitmapData;

	return bitmapMap.size();
}

int TBitmapManager::DeleteBitmap(int key)
{
	std::map<int, TBitmapData *>::iterator it = bitmapMap.find(key);

	if (it != bitmapMap.end())
    {
		TBitmapData * pBitmapData = (TBitmapData *)(it->second);
		delete pBitmapData;

		bitmapMap.erase(it);
	}

	return  bitmapMap.size();
}

// 특정 인덱스 비트맵 반환
Graphics::TBitmap* TBitmapManager::GetBitmap(int key)
{
	std::map<int, TBitmapData *>::iterator it = bitmapMap.find(key);

	if (it != bitmapMap.end())
	{
		TBitmapData * pbitmapData = (TBitmapData *)(it->second);
		return pbitmapData->bitmap;
	}

	return NULL;
}

TBitmapData * TBitmapManager::GetBitmpaData(int key)
{
	std::map<int, TBitmapData *>::iterator it = bitmapMap.find(key);

	if (it != bitmapMap.end())
	{
		TBitmapData * pbitmapData = (TBitmapData *)(it->second);
		return pbitmapData;
	}

	return NULL;
}

// 사용자 정의 파일 포맷 저장
void TBitmapManager::SaveToFile(TFileStream* fs)
{

    try {
		int count = bitmapMap.size();
		fs->Write(&count, sizeof(int));

		std::map<int, TBitmapData *>::iterator it;
		for (it = bitmapMap.begin(); it != bitmapMap.end(); ++it)
		{
			int key = it->first;
			TBitmapData * bmpData = it->second;

			fs->Write(&key, sizeof(int));
			bmpData->SaveToFile(fs);
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

			TBitmapData* bmpData = new TBitmapData();
			bmpData->LoadFromFile(fs);

			bitmapMap[key] = bmpData;
		}

    }
	__finally {

    }
}

