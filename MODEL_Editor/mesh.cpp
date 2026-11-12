//========================================================================
// 
// メッシュ [ mesh.cpp ]
// Author : Nakazawa Yuna
// 
//========================================================================
#include "mesh.h"

#include "manager.h"
#include "debugproc.h"
#include "renderer.h"
#include "input.h"
#include "sound.h"
#include "object.h"

#include "meshwall.h"
#include "meshfield.h"
#include "meshring.h"
#include "meshcylinder.h"
#include "meshdome.h"

#include "effect_batching.h"

#include <fstream>
#include <iostream>
#include <string>

//************************************************************************
// マクロ定義
//************************************************************************

//************************************************************************
// 静的メンバ変数宣言
//************************************************************************

//========================================================================
// 生成処理
//========================================================================
CMesh* CMesh::Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot, const D3DXVECTOR2 block, const D3DXVECTOR2 size, const int type)
{
	CMesh* pMesh = NULL;

	if (pMesh == NULL)
	{// NULLチェック
		// メッシュの生成
		switch (type)
		{
		case MESHWALL:			// メッシュウォール
			pMesh = CMeshWall::Create(pos, rot, block, size, nullptr, CObject::TYPE_MESH, 3);

			break;

		case MESHFIELD:			// メッシュフィールド
			pMesh = CMeshField::Create(pos, rot, block, size, CObject::TYPE_MESH, nullptr, 3);

			break;

		case MESHRING:			// メッシュリング
			pMesh = CMeshRing::Create(pos, DEFAULT_VECTOR3, rot, block, size, COLOR_WHITE, nullptr, CObject::TYPE_MESH, 3);

			break;

		case MESHCYLINDER:		// メッシュシリンダー
			pMesh = CMeshCylinder::Create(pos, rot, block, size, COLOR_WHITE, false, nullptr, CObject::TYPE_MESH, 3);

			break;

		case MESHDOME:			// メッシュドーム
			pMesh = CMeshDome::Create(pos, rot, block, size.x, nullptr, CObject::TYPE_MESH, 3);

			break;
		}
	}

	if (pMesh != NULL)
	{// NULLチェック
		return pMesh;
	}

	OutputDebugStringA("! ! ! シーンの生成に失敗しました ! ! !\n");

	return NULL;
}

//========================================================================
// メッシュのコンストラクタ
//========================================================================
CMesh::CMesh(const int nPriority) :CObject(nPriority)
{
	// 値のクリア
	m_nNumVtxAll = 0;
	m_nNumIdxAll = 0;
	m_nVtxIdx = 0;
	m_type = 0;
}

//========================================================================
// メッシュのデストラクタ
//========================================================================
CMesh::~CMesh()
{
}

//========================================================================
// メッシュの初期化処理
//========================================================================
HRESULT CMesh::Init(void)
{
	return S_OK;
}

//========================================================================
// メッシュの終了処理
//========================================================================
void CMesh::Uninit(void)
{
	// 頂点バッファの破棄
	if (m_pVtxBuff != NULL)
	{
		m_pVtxBuff->Release();
		m_pVtxBuff = NULL;
	}

	// インデックスバッファの破棄
	if (m_pIdxBuff != NULL)
	{
		m_pIdxBuff->Release();
		m_pIdxBuff = NULL;
	}

	// 自分自身を破棄
	CObject::Release();
}

//========================================================================
// メッシュの更新処理
//========================================================================
void CMesh::Update(void)
{
	CInputKeyboard* pInputKeyboard = CManager::GetInputKeyboard();		// キーボード入力の取得
	CInputMouse* pInputMouse = CManager::GetInputMouse();				// マウス入力の取得
	CDebugProc* pDebugProc = CManager::GetDebugProc();					// デバッグ表示の取得
	CEffectBatching* pEffectBatching = CManager::GetEffectBatching();	// バッチ描画エフェクトの取得
	VERTEX_3D* pVtx;						// 頂点情報へのポインタ

	// 頂点操作
	if (pInputKeyboard->GetRepeat(DIK_D) == true)
	{// 次の頂点
		m_nVtxIdx = (m_nVtxIdx + 1) % m_nNumVtxAll;
	}
	else if (pInputKeyboard->GetRepeat(DIK_A) == true)
	{// 前の頂点
		m_nVtxIdx = (m_nVtxIdx + (m_nNumVtxAll - 1)) % m_nNumVtxAll;
	}

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点情報の設定
	for (int nCntVtx = 0; nCntVtx < m_nNumVtxAll; nCntVtx++)
	{
		D3DXCOLOR col = (m_nVtxIdx == nCntVtx) ? COLOR_RED : COLOR_GREEN;
		float fSize = (m_nVtxIdx == nCntVtx) ? 10.0f : 6.0f;

		pEffectBatching->Set(pVtx[nCntVtx].pos, DEFAULT_VECTOR3, 0.0f, 1.0f, 0, 0.0f, 0.0f, 1, fSize, 0.0f, 0.0f, col);

		if (m_nVtxIdx != nCntVtx)
		{// 操作されている番号じゃなければ飛ばす
			continue;
		}

		// X軸移動
		if (pInputKeyboard->GetPress(DIK_T) == true)
		{// 大きく
			pVtx[nCntVtx].pos.x += 1.0f;
		}
		else if (pInputKeyboard->GetPress(DIK_G) == true)
		{// 小さく
			pVtx[nCntVtx].pos.x -= 1.0f;
		}

		// Y軸移動
		if (pInputKeyboard->GetPress(DIK_Y) == true)
		{// 大きく
			pVtx[nCntVtx].pos.y += 1.0f;
		}
		else if (pInputKeyboard->GetPress(DIK_H) == true)
		{// 小さく
			pVtx[nCntVtx].pos.y -= 1.0f;
		}

		// Z軸移動
		if (pInputKeyboard->GetPress(DIK_U) == true)
		{// 大きく
			pVtx[nCntVtx].pos.z += 1.0f;
		}
		else if (pInputKeyboard->GetPress(DIK_J) == true)
		{// 小さく
			pVtx[nCntVtx].pos.z -= 1.0f;
		}
	}

	// 頂点バッファをアンロックする
	m_pVtxBuff->Unlock();
}

//========================================================================
// メッシュの描画処理
//========================================================================
void CMesh::Draw(void)
{
}

//========================================================================
// 頂点バッファを生成
//========================================================================
HRESULT CMesh::CreateVtxBuff(const int nValue)
{
	if (nValue < 0)
	{// 範囲外の数値
		return E_FAIL;
	}

	CRenderer* pRenderer = CManager::GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスへのポインタ

	// 頂点バッファの生成
	HRESULT hr = pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * nValue,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		NULL);

	// 頂点数を保存
	m_nNumVtxAll = nValue;
	m_nVtxIdx = 0;

	return hr;
}

//========================================================================
// 頂点バッファを生成
//========================================================================
HRESULT CMesh::CreateVtxBuffMulti(const int nValue)
{
	if (nValue < 0)
	{// 範囲外の数値
		return E_FAIL;
	}

	CRenderer* pRenderer = CManager::GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスへのポインタ

	// 頂点バッファの生成
	HRESULT hr = pDevice->CreateVertexBuffer(sizeof(VERTEX_3D_MULTI) * nValue,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D_MULTI,
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		NULL);

	// 頂点数を保存
	m_nNumVtxAll = nValue;
	m_nVtxIdx = 0;

	return hr;
}

//========================================================================
// インデックスバッファを生成
//========================================================================
HRESULT CMesh::CreateIdxBuff(const int nValue)
{
	if (nValue < 0)
	{// 範囲外の数値
		return E_FAIL;
	}

	CRenderer* pRenderer = CManager::GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスへのポインタ

	// インデックスバッファの生成
	HRESULT hr = pDevice->CreateIndexBuffer(sizeof(DWORD) * nValue,
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX32,
		D3DPOOL_MANAGED,
		&m_pIdxBuff,
		NULL);

	// インデックス数を保存
	m_nNumIdxAll = nValue;

	return hr;
}

//========================================================================
// 選択された頂点の座標
//========================================================================
D3DXVECTOR3 CMesh::GetVtxPos(void)
{
	D3DXVECTOR3 pos = DEFAULT_VECTOR3;
	VERTEX_3D* pVtx;						// 頂点情報へのポインタ

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	pos = pVtx[m_nVtxIdx].pos;

	// 頂点バッファをアンロックする
	m_pVtxBuff->Unlock();

	return pos;
}

//========================================================================
// メッシュの書き込み処理
//========================================================================
HRESULT CMesh::WriteData(const char* pFilename)
{
	// ファイルオープン
	std::ofstream file(pFilename);

	if (file.is_open() == true)
	{// ファイルが開けた
		VERTEX_3D* pVtx;
		DWORD* pIdx;
		std::string str;		// 情報書き込み用

		// 頂点バッファをロックし、頂点情報へのポインタを取得
		GetVtxBuff()->Lock(0, 0, (void**)&pVtx, 0);

		for (int nCntVtx = 0; nCntVtx < m_nNumVtxAll; nCntVtx++)
		{
			str.clear();
			str = std::to_string(pVtx[nCntVtx].pos.x);
			str += " ";
			str += std::to_string(pVtx[nCntVtx].pos.y);
			str += " ";
			str += std::to_string(pVtx[nCntVtx].pos.z);
			str += "\n";

			file.write(str.c_str(), str.size());
		}

		// 頂点バッファをアンロックする
		GetVtxBuff()->Unlock();

		// インデックスバッファをロックし、頂点番号データへのポインタを取得
		GetIdxBuff()->Lock(0, 0, (void**)&pIdx, 0);

		for (int nCntIdx = 0; nCntIdx < m_nNumIdxAll; nCntIdx++)
		{
			str.clear();
			str = std::to_string(pIdx[nCntIdx]);
			str += "\n";

			file.write(str.c_str(), str.size());
		}

		// インデックスバッファをアンロックする
		GetIdxBuff()->Unlock();

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

//========================================================================
// メッシュの読み込み処理
//========================================================================
HRESULT CMesh::ReadData(const char* pFilename)
{
	// ファイルオープン
	std::ifstream file(pFilename);

	if (file.is_open() == true)
	{// ファイルが開けた
		//// 現在のオブジェクトを破棄
		//Uninit();

		//// カウントリセット
		//int type = 0;
		//m_nNumObject = 0;

		//// ファイルから読み込む
		//file.read((char*)&type, sizeof(type));

		//// オブジェクトの生成
		//Create(DEFAULT_VECTOR3, DEFAULT_VECTOR3, DEFAULT_VECTOR2, DEFAULT_VECTOR2, type);

		//HRESULT hr = m_Mesh.apMesh->ReadData();

		//// 総数を確認
		//if (hr != S_OK)
		//{// もし違ったら
		//	OutputDebugStringA("! ! ! メッシュの読み込みに失敗しました ! ! !\n");
		//}

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