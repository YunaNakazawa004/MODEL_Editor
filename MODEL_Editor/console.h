//========================================================================
// 
// コンソールヘッダー [console.h]
// Author : Nakazawa Yuna
// 
//========================================================================
#ifndef _CONSOLE_H_		// このマクロ定義がされていなかったら
#define _CONSOLE_H_		// 2重インクルード防止のマクロを定義する

#include "main.h"
#include "object.h"

#include <vector>

//************************************************************************
// 前方宣言
//************************************************************************
class CObjectX;
class CBone;

//************************************************************************
// コンソールクラス
//************************************************************************
class CConsole : public CObject
{
public:
	// モード
	enum MODE
	{
		MODE_SKIN = 0,		// スキンメッシュ
		MODE_BONE,			// ボーン
		MODE_MAX
	};

	// 配置できるオブジェクト
	enum MESH
	{
		MESHWALL = 0,		// メッシュウォール
		MESHFIELD,			// メッシュフィールド
		MESHRING,			// メッシュリング
		MESHCYLINDER,		// メッシュシリンダー
		MESHDOME,			// メッシュドーム
		MESH_MAX
	};

	// 全体操作方法
	enum PRS
	{
		PRS_POS = 0,		// 移動
		PRS_ROT,			// 回転
		PRS_SCALE,			// 拡縮
		PRS_MAX
	};

	CConsole(const int nPriority = PRIORITY_3);
	virtual ~CConsole();

	static CConsole* Create(const D3DXVECTOR3 pos);
	HRESULT Init(const D3DXVECTOR3 pos);
	HRESULT Init(void) { return S_OK; }
	void Uninit(void);
	void Update(void);
	void Draw(void);

	void Skin(void);
	void ObjectSetting(void);
	void PosRotScaleControll(void);

	void Bone(void);
	D3DXVECTOR3 Movement(D3DXVECTOR3* pOut);

	HRESULT WriteData(const char* pFilename);
	HRESULT ReadData(const char* pFilename);

	D3DXVECTOR3 GetPosition(void) { return DEFAULT_VECTOR3; }
	D3DXVECTOR3 GetRotation(void) { return DEFAULT_VECTOR3; }

private:
	CObjectX* m_pSample;				// 模型
	std::vector<CBone*> m_vpBone;		// ボーン
	std::vector<int> m_vnParent;		// ボーンの親
	std::vector<CObjectX*> m_vpModel;	// ボーンのモデル
	int m_nNumBone;			// ボーンの総数

	int m_mode;				// モード
	int m_type;				// 配置できるオブジェクトの種類
	int m_prs;				// 全体操作方法
	D3DXVECTOR2 m_block;	// 分割数
	D3DXVECTOR2 m_size;		// サイズ
};	

#endif