//========================================================================
// 
// メッシュマネージャー [ mesh_manager.cpp ]
// Author : Nakazawa Yuna
// 
//========================================================================
#include "mesh_manager.h"

#include "manager.h"
#include "debugproc.h"
#include "input.h"
#include "sound.h"
#include "object.h"

#include "mesh.h"

#include <fstream>
#include <iostream>

//************************************************************************
// マクロ定義
//************************************************************************

//************************************************************************
// 静的メンバ変数宣言
//************************************************************************
CMesh* CMeshManager::m_apMesh = {};					// 配置したオブジェクトの情報
int CMeshManager::m_nNumObject = 0;					// 現在オブジェクトの総数

//========================================================================
// 生成処理
//========================================================================
void CMeshManager::Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot, const D3DXVECTOR2 block, const D3DXVECTOR2 size, const int type)
{
	if (m_apMesh == NULL)
	{// NULLチェック
		m_apMesh = CMesh::Create(pos, rot, block, size, type);

		if (m_apMesh != NULL)
		{// 生成できた
			// 総数をカウントアップ
			m_nNumObject++;
		}
	}
}

//========================================================================
// メッシュマネージャーのコンストラクタ
//========================================================================
CMeshManager::CMeshManager()
{
	// 値のクリア
	memset(&m_apMesh, NULL, sizeof(m_apMesh));
	m_nNumObject = 0;
}

//========================================================================
// メッシュマネージャーのデストラクタ
//========================================================================
CMeshManager::~CMeshManager()
{
}

//========================================================================
// メッシュマネージャーの初期化処理
//========================================================================
HRESULT CMeshManager::Init(void)
{
	// 値の初期化
	memset(&m_apMesh, NULL, sizeof(m_apMesh));
	m_nNumObject = 0;

	return S_OK;
}

//========================================================================
// メッシュマネージャーの終了処理
//========================================================================
void CMeshManager::Uninit(void)
{
	if (m_apMesh != NULL)
	{// NULLチェック
		m_apMesh = NULL;
	}
}

//========================================================================
// メッシュマネージャーの更新処理
//========================================================================
void CMeshManager::Update(void)
{
}

//========================================================================
// メッシュマネージャーのリセット処理
//========================================================================
void CMeshManager::ResetData(void)
{
	// 現在のオブジェクトを破棄
	if (m_apMesh != NULL)
	{// NULLチェック
		// 終了処理
		m_apMesh->Uninit();

		m_apMesh = NULL;
	}

	// カウントリセット
	m_nNumObject = 0;
}

//========================================================================
// メッシュマネージャーの書き込み処理
//========================================================================
HRESULT CMeshManager::WriteData(const char* pFilename)
{
	if (m_apMesh == nullptr)
	{// NULLチェック
		return E_FAIL;
	}

	HRESULT hr = m_apMesh->WriteData(pFilename);

	return hr;
}

//========================================================================
// メッシュマネージャーの読み込み処理
//========================================================================
HRESULT CMeshManager::ReadData(const char* pFilename)
{
	// バイナリ形式でファイルオープン
	std::ifstream file(pFilename, std::ios_base::in | std::ios_base::binary);

	if (file.is_open() == true)
	{// ファイルが開けた
		// 現在のオブジェクトを破棄
		Uninit();

		// カウントリセット
		int type = 0;
		m_nNumObject = 0;

		// ファイルから読み込む
		file.read((char*)&type, sizeof(type));

		// オブジェクトの生成
		Create(DEFAULT_VECTOR3, DEFAULT_VECTOR3, DEFAULT_VECTOR2, DEFAULT_VECTOR2, type);

		HRESULT hr = m_apMesh->ReadData(pFilename);

		// 総数を確認
		if (hr != S_OK)
		{// もし違ったら
			OutputDebugStringA("! ! ! メッシュマネージャーの読み込みに失敗しました ! ! !\n");
		}

		// ファイルを閉じる
		file.close();
	}
	else
	{// ファイルが開けなかった
		OutputDebugStringA("! ! ! ファイルを開けませんでした ! ! !\n");

		return E_FAIL;
	}

	return S_OK;
}