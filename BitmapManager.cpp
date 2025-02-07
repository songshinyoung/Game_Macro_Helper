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
        return -1; // ������� ��� -1 ��ȯ
    }

    std::map<int, Graphics::TBitmap*>::iterator it = bitmapMap.end();
    --it; // map�� key �������� ���ĵǹǷ� ������ ��Ұ� ���� ū key
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

// Ư�� �ε��� ��Ʈ�� ��ȯ
Graphics::TBitmap* TBitmapManager::GetBitmap(int key)
{
	std::map<int, Graphics::TBitmap*>::iterator it = bitmapMap.find(key);

    if (it != bitmapMap.end())
    {
		return (Graphics::TBitmap *)(it->second);
	}

	return NULL;
}

// ����� ���� ���� ���� ����
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

			Graphics::TBitmap* bmp = new Graphics::TBitmap();
			bmp->LoadFromStream(fs);

			bitmapMap[key] = bmp;
		}

    }
    __finally {

    }
}

