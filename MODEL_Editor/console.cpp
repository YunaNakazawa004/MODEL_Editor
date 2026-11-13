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

#include "camera.h"
#include "mesh_manager.h"
#include "mesh.h"
#include "meshwall.h"
#include "meshfield.h"
#include "meshring.h"

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
	pObject = nullptr;
	m_type = 0;
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
	// 初期化処理

	return S_OK;
}

//========================================================================
// コンソールクラスの終了処理
//========================================================================
void CConsole::Uninit(void)
{
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
	CMesh* pMesh = pMeshManager->GetMesh();			// メッシュの取得
	D3DXVECTOR3 pos = CConsole::GetPosition();
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

	if (pInputKeyboard->GetTrigger(DIK_F9) == true)
	{// ファイル書き出し
		pMeshManager->WriteData("data\\mesh.txt", "data\\mesh.bin");
	}
	
	if (pInputKeyboard->GetTrigger(DIK_F10) == true)
	{// ファイル書き出し
		pMeshManager->ReadData("data\\mesh.bin");
	}

	// 配置に関するコメント
	pDebugProc->Print("\n*** 配置 ***\n");
	pDebugProc->Print("オブジェクト配置 [ ENTER ]\n");
	pDebugProc->Print("オブジェクトの種類 [ 1 ←→ 2 ] : %s\n",
		(m_type == MESHWALL) ? "メッシュウォール" :
		(m_type == MESHFIELD) ? "メッシュフィールド" :
		(m_type == MESHRING) ? "メッシュリング" :
		(m_type == MESHCYLINDER) ? "メッシュシリンダー" : "メッシュドーム");
	pDebugProc->Print("X/Y切り替え [ 右SHIFT ] : ( %s )\n", (bXY) ? "X" : "Y");
	pDebugProc->Print("分割数 [ ↑/↓ ] : ( %f %f )\n", m_block.x, m_block.y);
	pDebugProc->Print("サイズ [ ←/→ ] : ( %f %f )\n", m_size.x, m_size.y);
	pDebugProc->Print("リセット [ F6 ]\n");
	pDebugProc->Print("書き出し [ F9 ]\n");
	pDebugProc->Print("読み込み [ F10 ]\n");

	if (pMesh != nullptr)
	{// NULLチェック
		// 頂点操作に関するコメント
		pDebugProc->Print("\n****** 頂点操作 ******\n");
		pDebugProc->Print("頂点の選択 [ Q ←→ E ]\n");
		pDebugProc->Print("現在の頂点番号 ( %d )\n", pMesh->GetVtxIdx());
		pDebugProc->Print("現在の位置 ( %f %f %f )\n", pMesh->GetPosition().x, pMesh->GetPosition().y, pMesh->GetPosition().z);
		pDebugProc->Print("現在の向き ( %f %f %f )\n", pMesh->GetRotation().x, pMesh->GetRotation().y, pMesh->GetRotation().z);
		pDebugProc->Print("全体X軸移動 [ 4 > 6 ]\n");
		pDebugProc->Print("全体Y軸移動 [ 8 > 5 ]\n");
		pDebugProc->Print("全体Z軸移動 [ 1 > 3 ]\n");
		pDebugProc->Print("X軸回転 [ W > S ]\n");
		pDebugProc->Print("Y軸回転 [ A > D ]\n");
		pDebugProc->Print("Z軸回転 [ Z > C ]\n");
		pDebugProc->Print("X軸移動 [ T > G ] : ( %f )\n", pMesh->GetVtxPos().x);
		pDebugProc->Print("Y軸移動 [ Y > H ] : ( %f )\n", pMesh->GetVtxPos().y);
		pDebugProc->Print("Z軸移動 [ U > J ] : ( %f )\n", pMesh->GetVtxPos().z);
	}

	if (pInputKeyboard->GetTrigger(DIK_F6) == true)
	{// 配置リセット
		pMeshManager->ResetData();
	}
}

//========================================================================
// コンソールクラスの描画処理
//========================================================================
void CConsole::Draw(void)
{
}