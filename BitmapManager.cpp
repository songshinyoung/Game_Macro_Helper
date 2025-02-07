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

// ��� ��Ʈ�� ����
void TBitmapManager::ClearAll() {

	// ����� ��� ��Ʈ���� ����
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
        return -1; // ������� ��� -1 ��ȯ
    }

    std::map<int, TBitmapData *>::iterator it = bitmapMap.end();
    --it; // map�� key �������� ���ĵǹǷ� ������ ��Ұ� ���� ū key
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

// Ư�� �ε��� ��Ʈ�� ��ȯ
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

// ����� ���� ���� ���� ����
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

// ����� ���� ���� ���� �ε�
void TBitmapManager::LoadFromFile(TFileStream* fs)
{
	// Object List �ʱ�ȭ.
	ClearAll();

	if(fs == NULL) return;

	if(fs->Position >= fs->Size) return; // �̹� �� �о���.

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

