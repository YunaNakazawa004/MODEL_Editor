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

//************************************************************************
// コンソールクラス
//************************************************************************
class CConsole : public CObject
{
public:
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

	CConsole(const int nPriority = PRIORITY_3);
	virtual ~CConsole();

	static CConsole* Create(const D3DXVECTOR3 pos);
	HRESULT Init(const D3DXVECTOR3 pos);
	HRESULT Init(void) { return S_OK; }
	void Uninit(void);
	void Update(void);
	void Draw(void);

	D3DXVECTOR3 GetPosition(void) { return DEFAULT_VECTOR3; }
	D3DXVECTOR3 GetRotation(void) { return DEFAULT_VECTOR3; }

private:
	CObject* pObject;		// 生成したオブジェクト
	int m_type;				// 配置できるオブジェクトの種類
	D3DXVECTOR2 m_block;	// 分割数
	D3DXVECTOR2 m_size;		// サイズ
};	

#endif