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
CMesh* CMesh::Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot, const D3DXVECTOR2 block, const D3DXVECTOR2 size, const int type, const bool bCalc)
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
		pMesh->m_pos = pos;
		pMesh->m_rot = rot;
		pMesh->m_scale = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
		pMesh->m_bCalc = bCalc;

		return pMesh;
	}

	OutputDebugStringA("! ! ! メッシュ生成に失敗しました ! ! !\n");

	return NULL;
}

//========================================================================
// メッシュのコンストラクタ
//========================================================================
CMesh::CMesh(const int nPriority) :CObject(nPriority)
{
	// 値のクリア
	m_pos = DEFAULT_VECTOR3;
	m_rot = DEFAULT_VECTOR3;
	m_scale = DEFAULT_VECTOR3;
	m_nNumVtxAll = 0;
	m_nNumIdxAll = 0;
	m_nVtxIdx = 0;
	m_type = 0;
	m_bCalc = true;
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
	if (m_bCalc == false)
	{// 何もしない
		return;
	}

	CInputKeyboard* pInputKeyboard = CManager::GetInputKeyboard();		// キーボード入力の取得
	CInputMouse* pInputMouse = CManager::GetInputMouse();				// マウス入力の取得
	CDebugProc* pDebugProc = CManager::GetDebugProc();					// デバッグ表示の取得
	CEffectBatching* pEffectBatching = CManager::GetEffectBatching();	// バッチ描画エフェクトの取得
	VERTEX_3D* pVtx;						// 頂点情報へのポインタ
	static bool bVtxEffect = true;

	if (m_pVtxBuff == nullptr)
	{// NULLチェック
		return;
	}

	// エフェクトの切り替え
	if (pInputKeyboard->GetTrigger(DIK_F2) == true)
	{// 切り替える
		bVtxEffect = (bVtxEffect) ? false : true;
	}

	// 頂点操作
	if (pInputKeyboard->GetRepeat(DIK_E) == true)
	{// 次の頂点
		m_nVtxIdx = (m_nVtxIdx + 1) % m_nNumVtxAll;
	}
	else if (pInputKeyboard->GetRepeat(DIK_Q) == true)
	{// 前の頂点
		m_nVtxIdx = (m_nVtxIdx + (m_nNumVtxAll - 1)) % m_nNumVtxAll;
	}
	
	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点情報の設定
	for (int nCntVtx = 0; nCntVtx < m_nNumVtxAll; nCntVtx++)
	{
		if (bVtxEffect == true)
		{// 頂点エフェクトをつける場合
			D3DXCOLOR col = (m_nVtxIdx == nCntVtx) ? COLOR_RED : COLOR_GREEN;
			float fSize = ((m_nVtxIdx == nCntVtx) ? 10.0f : 6.0f) * m_scale.x;

			D3DXVECTOR3 pos;
			D3DXVec3TransformCoord(&pos, &pVtx[nCntVtx].pos, &m_mtxWorld);

			pEffectBatching->Set(pos, DEFAULT_VECTOR3, 0.0f, 1.0f, 0, 0.0f, 0.0f, 1, fSize, 0.0f, 0.0f, col);
		}

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
	CRenderer* pRenderer = CManager::GetRenderer();			// レンダラーへのポインタ
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスへのポインタ
	D3DXMATRIX mtxRot, mtxScale, mtxTrans;		// 計算用マトリックス

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	// 向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_rot.y, m_rot.x, m_rot.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

	// スケールを反映
	D3DXMatrixScaling(&mtxScale, m_scale.x, m_scale.y, m_scale.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxScale);

	// 位置を反映
	D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);
}

//========================================================================
// 位置設定
//========================================================================
void CMesh::SetPosition(const D3DXVECTOR3 pos)
{
	// 位置を変更
	m_pos = pos;
}

//========================================================================
// 角度設定
//========================================================================
void CMesh::SetRotation(const D3DXVECTOR3 rot)
{
	// X向きを調整
	CorrectAngle(&m_rot.x, m_rot.x);

	// Y向きを調整
	CorrectAngle(&m_rot.y, m_rot.y);

	// Z向きを調整
	CorrectAngle(&m_rot.z, m_rot.z);

	// 向きを変更
	m_rot = rot;
}

//========================================================================
// スケール設定
//========================================================================
void CMesh::SetScale(const D3DXVECTOR3 scale)
{
	// スケールを変更
	m_scale = scale;
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
HRESULT CMesh::WriteTxtData(const char* pFilename)
{
	// ファイルオープン
	std::ofstream filetxt(pFilename);

	if (filetxt.is_open() == true)
	{// ファイルが開けた
		VERTEX_3D* pVtx;
		DWORD* pIdx;
		std::string str;		// 情報書き込み用

		// 頂点バッファをロックし、頂点情報へのポインタを取得
		GetVtxBuff()->Lock(0, 0, (void**)&pVtx, 0);

		for (int nCntVtx = 0; nCntVtx < m_nNumVtxAll; nCntVtx++)
		{
			D3DXVECTOR3 pos;
			D3DXVec3TransformCoord(&pos, &pVtx[nCntVtx].pos, &m_mtxWorld);

			str.clear();

			str = "\tVERTEX\n";
			str += "\t\tPOS = ";
			str += std::to_string(pos.x);
			str += " ";
			str += std::to_string(pos.y);
			str += " ";
			str += std::to_string(pos.z);
			str += "\n";
			str += "\t\tBONE_REF = 0 1\n";
			str += "\t\tWEIGHT_VALUE = 0.0 1.0\n";
			str += "\t\tTEX = ";
			str += std::to_string(pVtx[nCntVtx].tex.x);
			str += " ";
			str += std::to_string(pVtx[nCntVtx].tex.y);
			str += "\n";
			str += "\tEND_VERTEX\n\n";

			filetxt.write(str.c_str(), str.size());
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

			filetxt.write(str.c_str(), str.size());
		}

		// インデックスバッファをアンロックする
		GetIdxBuff()->Unlock();

		// ファイルを閉じる
		filetxt.close();
	}
	else
	{// ファイルが開けなかった
		OutputDebugStringA("! ! ! ファイルを開けませんでした ! ! !\n");

		return E_FAIL;
	}

	return S_OK;
}

//========================================================================
// メッシュの書き込み処理
//========================================================================
HRESULT CMesh::WriteBinData(const char* pFilename)
{
	// ファイルオープン
	std::ofstream filebin(pFilename, std::ios_base::out | std::ios_base::binary);

	if (filebin.is_open() == true)
	{// ファイルが開けた
		VERTEX_3D* pVtx;
		DWORD* pIdx;

		// 書き出す
		filebin.write((const char*)&m_type, sizeof(m_type));
		filebin.write((const char*)&m_pos, sizeof(m_pos));
		filebin.write((const char*)&m_rot, sizeof(m_rot));
		filebin.write((const char*)&m_block, sizeof(m_block));
		filebin.write((const char*)&m_size, sizeof(m_size));
		filebin.write((const char*)&m_nNumVtxAll, sizeof(m_nNumVtxAll));
		filebin.write((const char*)&m_nNumIdxAll, sizeof(m_nNumIdxAll));

		// 頂点バッファをロックし、頂点情報へのポインタを取得
		GetVtxBuff()->Lock(0, 0, (void**)&pVtx, 0);

		for (int nCntVtx = 0; nCntVtx < m_nNumVtxAll; nCntVtx++)
		{
			filebin.write((const char*)&pVtx[nCntVtx], sizeof(*pVtx));
		}

		// 頂点バッファをアンロックする
		GetVtxBuff()->Unlock();

		// インデックスバッファをロックし、頂点番号データへのポインタを取得
		GetIdxBuff()->Lock(0, 0, (void**)&pIdx, 0);

		for (int nCntIdx = 0; nCntIdx < m_nNumIdxAll; nCntIdx++)
		{
			filebin.write((const char*)&pIdx[nCntIdx], sizeof(*pIdx));
		}

		// インデックスバッファをアンロックする
		GetIdxBuff()->Unlock();

		// ファイルを閉じる
		filebin.close();
	}
	else
	{// ファイルが開けなかった
		OutputDebugStringA("! ! ! ファイルを開けませんでした ! ! !\n");

		return E_FAIL;
	}

	return S_OK;
}