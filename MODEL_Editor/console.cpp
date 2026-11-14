//========================================================================
// 
// コンソール [console.cpp]
// Author : Nakazawa Yuna
// 
//========================================================================
#include "console.h"

#include "renderer.h"
#include "manager.h"
#include "input.h"
#include "debugproc.h"
#include "texture.h"

#include "objectX.h"
#include "camera.h"
#include "bone.h"
#include "mesh_manager.h"
#include "mesh.h"
#include "meshwall.h"
#include "meshfield.h"
#include "meshring.h"

#include <fstream>
#include <iostream>
#include <string>

//************************************************************************
// マクロ定義
//************************************************************************
#define MOVEMENT				(D3DXVECTOR3(3.0f, 3.0f, 3.0f))			// 移動量
#define ROT						(D3DXVECTOR3(0.05f, 0.05f, 0.05f))		// 向き移動量

//************************************************************************
// 静的メンバ変数宣言
//************************************************************************

//========================================================================
// コンソールクラスの生成処理
//========================================================================
CConsole* CConsole::Create(const D3DXVECTOR3 pos)
{
#ifndef LIST
	if (CObject::GetNumAll() >= MAX_OBJECT)
	{// 最大数のオブジェクトが存在する
		OutputDebugStringA("! ! ! オブジェクトの最大数に達しています ! ! !\n");

		return NULL;
	}
#endif

	CConsole* pConsole = NULL;

	if (pConsole == NULL)
	{// NULLチェック
		// コンソールの生成
		pConsole = new CConsole;
	}

	if (pConsole != NULL)
	{// NULLチェック
		// 初期化処理
		if (FAILED(pConsole->Init(pos)))
		{// もし失敗した場合
			OutputDebugStringA("! ! ! コンソールの初期化に失敗しました ! ! !\n");

			return NULL;
		}

		return pConsole;
	}

	OutputDebugStringA("! ! ! コンソールの生成に失敗しました ! ! !\n");

	return NULL;
}

//========================================================================
// コンソールクラスのコンストラクタ
//========================================================================
CConsole::CConsole(const int nPriority) :CObject(nPriority)
{
	// コンソールクラスの値をクリア
	m_pSample = nullptr;
	m_vpBone.clear();
	m_vnParent.clear();
	m_vpModel.clear();
	m_nNumBone = 0;
	m_mode = 0;
	m_type = 0;
	m_prs = 0;
	m_block = DEFAULT_VECTOR2;
	m_size = DEFAULT_VECTOR2;
}

//========================================================================
// コンソールクラスのデストラクタ
//========================================================================
CConsole::~CConsole()
{
}

//========================================================================
// コンソールクラスの初期化処理
//========================================================================
HRESULT CConsole::Init(const D3DXVECTOR3 pos)
{
	// 模型を生成
	if (m_pSample == nullptr)
	{// NULLチェック
		m_pSample = CObjectX::Create(pos, CObject::TYPE_NONE, "data\\MODEL\\bone.x");

		if (m_pSample != nullptr)
		{// NULLチェック
			m_pSample->SetColor(COLOR_WHITE_ALPHA);
		}
	}

	return S_OK;
}

//========================================================================
// コンソールクラスの終了処理
//========================================================================
void CConsole::Uninit(void)
{
	// ボーンの破棄
	for (int nCntBone = 0; nCntBone < m_nNumBone; nCntBone++)
	{
		if (m_vpBone.at(nCntBone) != nullptr)
		{// NULLチェック
			// 終了処理
			m_vpBone.at(nCntBone)->Uninit();

			delete[] m_vpBone.at(nCntBone);
			m_vpBone.at(nCntBone) = nullptr;
		}
	}

	// 模型を破棄
	if (m_pSample != nullptr)
	{// NULLチェック
		m_pSample = nullptr;
	}

	// 自分自身を破棄
	CObject::Release();
}

//========================================================================
// コンソールクラスの更新処理
//========================================================================
void CConsole::Update(void)
{
	// ローカル変数
	CCamera* pCamera = CManager::GetCamera();							// カメラの取得
	CInputKeyboard* pInputKeyboard = CManager::GetInputKeyboard();		// キーボード入力の取得
	CInputMouse* pInputMouse = CManager::GetInputMouse();				// マウス入力の取得
	CDebugProc* pDebugProc = CManager::GetDebugProc();					// デバッグ表示の取得
	CMeshManager* pMeshManager = CManager::GetMeshManager();			// メッシュマネージャーの取得

	// モード切替
	if (pInputKeyboard->GetTrigger(DIK_F3) == true)
	{// モード切替
		m_mode = (m_mode + 1) % MODE_MAX;
	}

	pDebugProc->Print("\n****** モード ******\n");
	pDebugProc->Print("現在のモード [ F3 ] : ( %s )\n",
		(m_mode == MODE_SKIN) ? "スキンメッシュ" : "ボーン");

	switch (m_mode)
	{
	case MODE_SKIN:			// スキンメッシュ
		// スキンメッシュ処理
		Skin();

		m_pSample->SetDisp(false);

		break;

	case MODE_BONE:			// ボーン
		// ボーン処理
		Bone();

		m_pSample->SetDisp(true);

		break;
	}

	if (pInputKeyboard->GetTrigger(DIK_F9) == true)
	{// ファイル書き出し
		pMeshManager->WriteData("data\\mesh.txt", "data\\mesh.bin");
		WriteData("data\\bone.txt");
	}

	if (pInputKeyboard->GetTrigger(DIK_F10) == true)
	{// ファイル読み込み
		pMeshManager->ReadData("data\\mesh.bin");
		ReadData("data\\bone.txt");
	}

	if (pInputKeyboard->GetTrigger(DIK_F6) == true)
	{// 配置リセット
		pMeshManager->ResetData();
	}

	// 外部ファイルに関するコメント
	pDebugProc->Print("\n****** 外部ファイル ******\n");
	pDebugProc->Print("リセット [ F6 ]\n");
	pDebugProc->Print("書き出し [ F9 ]\n");
	pDebugProc->Print("読み込み [ F10 ]\n");
}

//========================================================================
// コンソールクラスの描画処理
//========================================================================
void CConsole::Draw(void)
{
	for (int nCntBone = 0; nCntBone < m_nNumBone; nCntBone++)
	{
		m_vpBone.at(nCntBone)->Draw();
	}
}

//========================================================================
// スキンモードの処理
//========================================================================
void CConsole::Skin(void)
{
	CDebugProc* pDebugProc = CManager::GetDebugProc();					// デバッグ表示の取得
	CInputKeyboard* pInputKeyboard = CManager::GetInputKeyboard();		// キーボード入力の取得
	CMeshManager* pMeshManager = CManager::GetMeshManager();			// メッシュマネージャーの取得
	CMesh* pMesh = pMeshManager->GetMesh();			// メッシュの取得

	// オブジェクト切り替え
	if (pInputKeyboard->GetTrigger(DIK_1) == true)
	{// モード切替
		m_type = (m_type + (MESH_MAX - 1)) % MESH_MAX;
	}
	else if (pInputKeyboard->GetTrigger(DIK_2) == true)
	{// モード切替
		m_type = (m_type + 1) % MESH_MAX;
	}

	if (pInputKeyboard->GetTrigger(DIK_RETURN) == true)
	{// エンター
		pMeshManager->Create(DEFAULT_VECTOR3, DEFAULT_VECTOR3, m_block, m_size, m_type);
	}

	// 配置に関するコメント
	pDebugProc->Print("\n*** 配置 ***\n");
	pDebugProc->Print("オブジェクト配置 [ ENTER ]\n");
	pDebugProc->Print("オブジェクトの種類 [ 1 ←→ 2 ] : ( %s )\n",
		(m_type == MESHWALL) ? "メッシュウォール" :
		(m_type == MESHFIELD) ? "メッシュフィールド" :
		(m_type == MESHRING) ? "メッシュリング" :
		(m_type == MESHCYLINDER) ? "メッシュシリンダー" : "メッシュドーム");

	// 分割数、サイズを設定
	ObjectSetting();

	if (pMesh != nullptr)
	{// NULLチェック
		// メッシュをコントロール
		pMesh->Controll();

		// モード切替
		if (pInputKeyboard->GetTrigger(DIK_TAB) == true)
		{// モード切替
			m_prs = (m_prs + 1) % PRS_MAX;
		}

		// 全体操作に関するコメント
		pDebugProc->Print("\n****** 全体操作 ******\n");
		pDebugProc->Print("現在の操作モード [ TAB ] : ( %s )\n",
			(m_prs == PRS_POS) ? "移動" :
			(m_prs == PRS_ROT) ? "回転" : "拡縮");

		// 位置、回転、スケールをコントロール
		PosRotScaleControll();

		// 頂点操作に関するコメント
		pDebugProc->Print("\n****** 頂点操作 ******\n");
		pDebugProc->Print("頂点の選択 [ Q ←→ E ]\n");
		pDebugProc->Print("現在の頂点番号 ( %d )\n", pMesh->GetVtxIdx());
		pDebugProc->Print("X軸移動 [ T > G ] : ( %f )\n", pMesh->GetVtxPos().x);
		pDebugProc->Print("Y軸移動 [ Y > H ] : ( %f )\n", pMesh->GetVtxPos().y);
		pDebugProc->Print("Z軸移動 [ U > J ] : ( %f )\n", pMesh->GetVtxPos().z);
	}
}

//========================================================================
// 分割数、サイズを設定
//========================================================================
void CConsole::ObjectSetting(void)
{
	CInputKeyboard* pInputKeyboard = CManager::GetInputKeyboard();		// キーボード入力の取得
	CDebugProc* pDebugProc = CManager::GetDebugProc();					// デバッグ表示の取得
	static bool bXY = true;

	// X/Y
	if (pInputKeyboard->GetTrigger(DIK_RSHIFT) == true)
	{
		bXY = (bXY) ? false : true;
	}

	// 分割数
	if (pInputKeyboard->GetRepeat(DIK_UP) == true)
	{// 上げる
		if (bXY == true)
		{// X
			m_block.x += 1.0f;
		}
		else
		{// Y
			m_block.y += 1.0f;
		}
	}
	else if (pInputKeyboard->GetRepeat(DIK_DOWN) == true)
	{// 下げる
		if (bXY == true)
		{// X
			m_block.x -= 1.0f;
		}
		else
		{// Y
			m_block.y -= 1.0f;
		}
	}

	// サイズ
	if (pInputKeyboard->GetRepeat(DIK_RIGHT) == true)
	{// 上げる
		if (bXY == true)
		{// X
			m_size.x += 10.0f;
		}
		else
		{// Y
			m_size.y += 10.0f;
		}
	}
	else if (pInputKeyboard->GetRepeat(DIK_LEFT) == true)
	{// 下げる
		if (bXY == true)
		{// X
			m_size.x -= 10.0f;
		}
		else
		{// Y
			m_size.y -= 10.0f;
		}
	}

	// 分割数、サイズに関するコメント
	pDebugProc->Print("X/Y切り替え [ 右SHIFT ] : ( %s )\n", (bXY) ? "X" : "Y");
	pDebugProc->Print("分割数 [ ↑/↓ ] : ( %f %f )\n", m_block.x, m_block.y);
	pDebugProc->Print("サイズ [ ←/→ ] : ( %f %f )\n", m_size.x, m_size.y);
}

//========================================================================
// 位置、回転、スケールをコントロール
//========================================================================
void CConsole::PosRotScaleControll(void)
{
	CInputKeyboard* pInputKeyboard = CManager::GetInputKeyboard();		// キーボード入力の取得
	CDebugProc* pDebugProc = CManager::GetDebugProc();					// デバッグ表示の取得
	CMeshManager* pMeshManager = CManager::GetMeshManager();			// メッシュマネージャーの取得
	CMesh* pMesh = pMeshManager->GetMesh();			// メッシュの取得
	D3DXVECTOR3 pos, rot, scale;
	pos = pMesh->GetPosition();
	rot = pMesh->GetRotation();
	scale = pMesh->GetScale();

	switch (m_prs)
	{
	case PRS_POS:			// 移動
		// 移動操作
		if (pInputKeyboard->GetRepeat(DIK_W) == true)
		{// X軸移動
			pos.x += 5.0f;
		}
		else if (pInputKeyboard->GetRepeat(DIK_S) == true)
		{// X軸移動
			pos.x -= 5.0f;
		}
		if (pInputKeyboard->GetRepeat(DIK_A) == true)
		{// Y軸移動
			pos.y += 5.0f;
		}
		else if (pInputKeyboard->GetRepeat(DIK_D) == true)
		{// Y軸移動
			pos.y -= 5.0f;
		}
		if (pInputKeyboard->GetRepeat(DIK_Z) == true)
		{// Z軸移動
			pos.z += 5.0f;
		}
		else if (pInputKeyboard->GetRepeat(DIK_C) == true)
		{// Z軸移動
			pos.z -= 5.0f;
		}

		// 位置を設定
		pMesh->SetPosition(pos);

		break;

	case PRS_ROT:			// 回転
		// 向き操作
		if (pInputKeyboard->GetRepeat(DIK_W) == true)
		{// X軸回転
			rot.x += D3DX_PI * 0.1f;
		}
		else if (pInputKeyboard->GetRepeat(DIK_S) == true)
		{// X軸回転
			rot.x -= D3DX_PI * 0.1f;
		}
		if (pInputKeyboard->GetRepeat(DIK_A) == true)
		{// Y軸回転
			rot.y += D3DX_PI * 0.1f;
		}
		else if (pInputKeyboard->GetRepeat(DIK_D) == true)
		{// Y軸回転
			rot.y -= D3DX_PI * 0.1f;
		}
		if (pInputKeyboard->GetRepeat(DIK_Z) == true)
		{// Z軸回転
			rot.z += D3DX_PI * 0.1f;
		}
		else if (pInputKeyboard->GetRepeat(DIK_C) == true)
		{// Z軸回転
			rot.z -= D3DX_PI * 0.1f;
		}

		// 向きを設定
		pMesh->SetRotation(rot);

		break;

	case PRS_SCALE:		// 拡縮
		// 拡縮操作
		if (pInputKeyboard->GetRepeat(DIK_W) == true)
		{// X軸拡縮
			scale.x += 0.1f;
		}
		else if (pInputKeyboard->GetRepeat(DIK_S) == true)
		{// X軸拡縮
			scale.x -= 0.1f;
		}
		if (pInputKeyboard->GetRepeat(DIK_A) == true)
		{// Y軸拡縮
			scale.y += 0.1f;
		}
		else if (pInputKeyboard->GetRepeat(DIK_D) == true)
		{// Y軸拡縮
			scale.y -= 0.1f;
		}
		if (pInputKeyboard->GetRepeat(DIK_Z) == true)
		{// Z軸拡縮
			scale.z += 0.1f;
		}
		else if (pInputKeyboard->GetRepeat(DIK_C) == true)
		{// Z軸拡縮
			scale.z -= 0.1f;
		}

		// スケールを設定
		pMesh->SetScale(scale);

		break;
	}

	// 位置、回転、スケールに関するコメント
	pDebugProc->Print("現在の位置 ( %f %f %f )\n", pos.x, pos.y, pos.z);
	pDebugProc->Print("現在の向き ( %f %f %f )\n", rot.x, rot.y, rot.z);
	pDebugProc->Print("現在のスケール ( %f %f %f )\n", scale.x, scale.y, scale.z);
}

//========================================================================
// ボーン処理
//========================================================================
void CConsole::Bone(void)
{
	CInputKeyboard* pInputKeyboard = CManager::GetInputKeyboard();		// キーボード入力の取得
	CDebugProc* pDebugProc = CManager::GetDebugProc();					// デバッグ表示の取得
	D3DXVECTOR3 posTarget;
	static int nTarget = 0;

	if (pInputKeyboard->GetTrigger(DIK_TAB) == true)
	{// 操作対象を切り替え
		nTarget = (nTarget + 1) % (m_nNumBone + 1);
	}

	if (nTarget == m_nNumBone)
	{// 模型を対象としている
		posTarget = m_pSample->GetPosition();

		// 模型の移動
		m_pSample->SetPosition(Movement(&posTarget));
		m_pSample->SetColor(D3DXCOLOR(1.0f, 0.0f, 0.0f, 0.5f));
	}
	else
	{// ボーンを対象としている
		posTarget = m_vpBone.at(nTarget)->GetPosition();

		m_vpBone.at(nTarget)->SetPosition(Movement(&posTarget));
		m_vpModel.at(nTarget)->SetPosition(Movement(&posTarget));

		m_pSample->SetColor(COLOR_WHITE_ALPHA);
	}

	if (pInputKeyboard->GetTrigger(DIK_RETURN) == true)
	{// エンター
		m_vpBone.push_back(CBone::Create(posTarget, DEFAULT_VECTOR3));
		m_vnParent.push_back(-1);
		m_vpModel.push_back(CObjectX::Create(posTarget, CObject::TYPE_NONE, "data\\MODEL\\bone.x"));

		m_nNumBone++;
		nTarget++;
	}

	// 模型操作に関するコメント
	pDebugProc->Print("\n****** 模型操作 ******\n");
	pDebugProc->Print("現在の位置 : ( %f %f %f )\n", m_pSample->GetPosition().x, m_pSample->GetPosition().y, m_pSample->GetPosition().z);
	pDebugProc->Print("移動 [ W/A/S/D/Z/C ]\n");
	pDebugProc->Print("ボーン配置 [ ENTER ]\n");

	// ボーンに関するコメント
	pDebugProc->Print("\n****** ボーン ******\n");
	pDebugProc->Print("ボーンの総数 : ( %d )\n", m_nNumBone);

	// ボーンの処理
	for (int nCntBone = 0; nCntBone < m_nNumBone; nCntBone++)
	{
		if (nTarget == nCntBone)
		{// 操作されているボーン
			m_vpModel.at(nCntBone)->SetColor(COLOR_RED);
			pDebugProc->Print("● ");

			if (pInputKeyboard->GetTrigger(DIK_F) == true)
			{// 親を設定する
				m_vnParent.at(nCntBone) = (m_vnParent.at(nCntBone) + 1) % m_nNumBone;

				if (m_vnParent.at(nCntBone) != nCntBone)
				{// 親がいる場合
					m_vpBone.at(nCntBone)->SetParent(m_vpBone.at(m_vnParent.at(nCntBone)));
				}
				else
				{// 親がいない場合
					m_vpBone.at(nCntBone)->SetParent(nullptr);
				}
			}
		}
		else
		{// 操作されていないボーン
			m_vpModel.at(nCntBone)->SetColor(COLOR_WHITE);
		}

		pDebugProc->Print("[ %d ] ", nCntBone);
		pDebugProc->Print("現在の位置 : ( %f %f %f )\n",
			m_vpBone.at(nCntBone)->GetPosition().x, m_vpBone.at(nCntBone)->GetPosition().y, m_vpBone.at(nCntBone)->GetPosition().z);
		pDebugProc->Print("親 [ F ] : ( %d )\n", m_vnParent.at(nCntBone));
	}
}

//========================================================================
// 移動処理
//========================================================================
D3DXVECTOR3 CConsole::Movement(D3DXVECTOR3* pOut)
{
	CInputKeyboard* pInputKeyboard = CManager::GetInputKeyboard();		// キーボード入力の取得
	CDebugProc* pDebugProc = CManager::GetDebugProc();					// デバッグ表示の取得

	if (pInputKeyboard->GetRepeat(DIK_W) == true)		// キーボード移動
	{// 奥に移動
		if (pInputKeyboard->GetRepeat(DIK_A) == true)
		{// 左奥に移動
			pOut->x += sinf(-D3DX_PI * 0.75f) * MOVEMENT.x;
			pOut->z += cosf(-D3DX_PI * 0.25f) * MOVEMENT.z;
		}
		else if (pInputKeyboard->GetRepeat(DIK_D) == true)
		{// 右奥に移動
			pOut->x += sinf(D3DX_PI * 0.75f) * MOVEMENT.x;
			pOut->z += cosf(D3DX_PI * 0.25f) * MOVEMENT.z;
		}
		else if (pInputKeyboard->GetRepeat(DIK_W) == true)
		{// 奥に移動
			pOut->x += sinf(D3DX_PI * 0.0f) * MOVEMENT.x;
			pOut->z += cosf(D3DX_PI * 0.0f) * MOVEMENT.z;
		}
	}
	else if (pInputKeyboard->GetRepeat(DIK_S) == true)
	{// 手前に移動
		if (pInputKeyboard->GetRepeat(DIK_A) == true)
		{// 左手前に移動
			pOut->x += sinf(-D3DX_PI * 0.25f) * MOVEMENT.x;
			pOut->z += cosf(-D3DX_PI * 0.75f) * MOVEMENT.z;
		}
		else if (pInputKeyboard->GetRepeat(DIK_D) == true)
		{// 右手前に移動
			pOut->x += sinf(D3DX_PI * 0.25f) * MOVEMENT.x;
			pOut->z += cosf(D3DX_PI * 0.75f) * MOVEMENT.z;
		}
		else if (pInputKeyboard->GetRepeat(DIK_S) == true)
		{// 手前に移動
			pOut->x += sinf(D3DX_PI * 1.0f) * MOVEMENT.x;
			pOut->z += cosf(D3DX_PI * 1.0f) * MOVEMENT.z;
		}
	}
	else if (pInputKeyboard->GetRepeat(DIK_A) == true)
	{// 左に移動
		pOut->x += sinf(-D3DX_PI * 0.5f) * MOVEMENT.x;
		pOut->z += cosf(-D3DX_PI * 0.5f) * MOVEMENT.z;
	}
	else if (pInputKeyboard->GetRepeat(DIK_D) == true)
	{// 右に移動
		pOut->x += sinf(D3DX_PI * 0.5f) * MOVEMENT.x;
		pOut->z += cosf(D3DX_PI * 0.5f) * MOVEMENT.z;
	}

	if (pInputKeyboard->GetRepeat(DIK_C) == true)
	{// 上に移動
		pOut->y += MOVEMENT.y;
	}
	else if (pInputKeyboard->GetRepeat(DIK_Z) == true)
	{// 下に移動
		pOut->y += -MOVEMENT.y;
	}

	return *pOut;
}

//========================================================================
// ボーンの書き込み処理
//========================================================================
HRESULT CConsole::WriteData(const char* pFilename)
{
	// ファイルオープン
	std::ofstream filetxt(pFilename);

	if (filetxt.is_open() == true)
	{// ファイルが開け
		std::string str;		// 情報書き込み用
		str.clear();

		str = "NUM_BONE = ";
		str += std::to_string(m_nNumBone);
		str += "\n";
		filetxt.write(str.c_str(), str.size());

		for (int nCntBone = 0; nCntBone < m_nNumBone; nCntBone++)
		{
			str.clear();

			str = "\tBONE\n";
			str += "\t\tPARENT = ";
			str += std::to_string((m_vnParent.at(nCntBone) == nCntBone) ? -1 : m_vnParent.at(nCntBone));
			str += "\n";
			str += "\t\tPOS = ";
			str += std::to_string(m_vpBone.at(nCntBone)->GetPosition().x);
			str += " ";
			str += std::to_string(m_vpBone.at(nCntBone)->GetPosition().y);
			str += " ";
			str += std::to_string(m_vpBone.at(nCntBone)->GetPosition().z);
			str += "\n";
			str += "\t\tROT = ";
			str += std::to_string(m_vpBone.at(nCntBone)->GetRotation().x);
			str += " ";
			str += std::to_string(m_vpBone.at(nCntBone)->GetRotation().y);
			str += " ";
			str += std::to_string(m_vpBone.at(nCntBone)->GetRotation().z);
			str += "\n";
			str += "\tEND_BONE\n\n";

			filetxt.write(str.c_str(), str.size());
		}

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
// ボーンの読み込み処理
//========================================================================
HRESULT CConsole::ReadData(const char* pFilename)
{
	// ファイルオープン
	std::ifstream file(pFilename);

	if (file.is_open() == true)
	{// ファイルが開けた
		// 現在のボーンを破棄
		for (int nCntBone = 0; nCntBone < m_nNumBone; nCntBone++)
		{
			if (m_vpBone.at(nCntBone) != nullptr)
			{// NULLチェック
				// 終了処理
				m_vpBone.at(nCntBone)->Uninit();

				delete[] m_vpBone.at(nCntBone);
				m_vpBone.at(nCntBone) = nullptr;
			}

			if (m_vpModel.at(nCntBone) != nullptr)
			{// NULLチェック
				// 終了処理
				m_vpModel.at(nCntBone)->Uninit();
			}
		}

		// リセット
		m_vpBone.clear();
		m_vpModel.clear();
		m_vnParent.clear();

		// ローカル変数
		int nNumBone;
		int nParent;
		D3DXVECTOR3 pos, rot;
		std::string line = {};				// テキストを読み込む

		while (file >> line)
		{
			if (line.find("NUM_BONE") != std::string::npos)
			{// モデル数
				file >> line;				// =
				file >> nNumBone;		// 数字

				m_nNumBone = nNumBone;

				continue;
			}

			// ボーン読み込み
			for (int nCntBone = 0; nCntBone < nNumBone; nCntBone++)
			{
				if (line.find("BONE") != std::string::npos)
				{// BONE
					file >> line;				// ボーン設定
				}

				if (line.find("PARENT") != std::string::npos)
				{// 親のボーン番号
					file >> line;				// =
					file >> nParent;			// 親のボーン番号

					file >> line;				// 次の文字列

					if (line.find("#") != std::string::npos)
					{// コメント
						std::getline(file, line);

						// 次の文字列を読み込む
						file >> line;			// 次の文字列
					}
				}

				if (line.find("POS") != std::string::npos)
				{// ボーンの位置(オフセット)
					file >> line;				// =
					file >> pos.x;				// 位置(オフセット)x
					file >> pos.y;				// 位置(オフセット)y
					file >> pos.z;				// 位置(オフセット)y

					file >> line;				// 次の文字列

					if (line.find("#") != std::string::npos)
					{// コメント
						std::getline(file, line);

						// 次の文字列を読み込む
						file >> line;			// 次の文字列
					}
				}

				if (line.find("ROT") != std::string::npos)
				{// ボーンの向き
					file >> line;				// =
					file >> rot.x;				// 向き(オフセット)x
					file >> rot.y;				// 向き(オフセット)y
					file >> rot.z;				// 向き(オフセット)y

					file >> line;				// 次の文字列

					if (line.find("#") != std::string::npos)
					{// コメント
						std::getline(file, line);

						// 次の文字列を読み込む
						file >> line;			// 次の文字列
					}
				}

				if (line.find("END_BONE") != std::string::npos)
				{// END_BONE
					file >> line;				// 次の文字列
				}

				// ボーンを生成
				m_vpBone.push_back(CBone::Create(pos, rot));
				m_vpModel.push_back(CObjectX::Create(pos, CObject::TYPE_NONE, "data\\MODEL\\bone.x"));
				m_vnParent.push_back(nParent);

				// 親の設定
				if (m_vpBone.at(nCntBone) != nullptr)
				{// NULLチェック
					if (nParent < 0 || nParent >= nNumBone)
					{// 親がいない場合
						m_vpBone.at(nCntBone)->SetParent(nullptr);
					}
					else
					{// 親がいる場合
						m_vpBone.at(nCntBone)->SetParent(m_vpBone.at(nCntBone));
					}
				}

				if (nCntBone == nNumBone - 1)
				{// 設定する数分生成した
					break;
				}
			}
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