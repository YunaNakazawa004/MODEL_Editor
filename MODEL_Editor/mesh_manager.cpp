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
#include <string>

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
HRESULT CMeshManager::WriteData(const char* pFilenameTxt, const char* pFilenameBin)
{
	if (m_apMesh == nullptr)
	{// NULLチェック
		return E_FAIL;
	}

	HRESULT hr = m_apMesh->WriteTxtData(pFilenameTxt);

	hr = m_apMesh->WriteBinData(pFilenameBin);

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
		ResetData();

		D3DXVECTOR3 pos, rot;
		D3DXVECTOR2 block, size;
		int type;
		int nNumVtx = 0, nNumIdx = 0;

		// 読み込む
		file.read((char*)&type, sizeof(type));
		file.read((char*)&pos, sizeof(pos));
		file.read((char*)&rot, sizeof(rot));
		file.read((char*)&block, sizeof(block));
		file.read((char*)&size, sizeof(size));
		file.read((char*)&nNumVtx, sizeof(nNumVtx));
		file.read((char*)&nNumIdx, sizeof(nNumIdx));

		// オブジェクトの生成
		Create(pos, rot, block, size, type);

		// 頂点情報の読み込み
		VERTEX_3D* pVtx;
		DWORD* pIdx;

		// 頂点バッファをロックし、頂点情報へのポインタを取得
		m_apMesh->GetVtxBuff()->Lock(0, 0, (void**)&pVtx, 0);

		for (int nCntVtx = 0; nCntVtx < nNumVtx; nCntVtx++)
		{
			file.read((char*)&pVtx[nCntVtx], sizeof(*pVtx));
		}

		// 頂点バッファをアンロックする
		m_apMesh->GetVtxBuff()->Unlock();

		// インデックスバッファをロックし、頂点番号データへのポインタを取得
		m_apMesh->GetIdxBuff()->Lock(0, 0, (void**)&pIdx, 0);

		for (int nCntIdx = 0; nCntIdx < nNumIdx; nCntIdx++)
		{
			file.read((char*)&pIdx[nCntIdx], sizeof(*pIdx));
		}

		// インデックスバッファをアンロックする
		m_apMesh->GetIdxBuff()->Unlock();

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