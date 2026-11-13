//========================================================================
// 
// メッシュマネージャーヘッダー [ mesh_manager.h ]
// Author : Nakazawa Yuna
// 
//========================================================================
#ifndef _MESH_MANAGER_H_		// このマクロ定義がされていなかったら
#define _MESH_MANAGER_H_		// 2重インクルード防止のマクロを定義する

#include "main.h"

//************************************************************************
// マクロ定義
//************************************************************************

//************************************************************************
// 前方宣言
//************************************************************************
class CObject;
class CMesh;

//************************************************************************
// メッシュマネージャークラス
//************************************************************************
class CMeshManager
{
public:
	// 生成可能なメッシュ
	enum MESH
	{
		MESHWALL = 0,		// メッシュウォール
		MESHFIELD,			// メッシュフィールド
		MESHRING,			// メッシュリング
		MESHCYLINDER,		// メッシュシリンダー
		MESHDOME,			// メッシュドーム
		MESH_MAX
	};

	CMeshManager();
	virtual ~CMeshManager();

	static void Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot, const D3DXVECTOR2 block, const D3DXVECTOR2 size, const int type);
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);

	void ResetData(void);
	HRESULT WriteData(const char* pFilenameTxt, const char* pFilenameBin);
	HRESULT ReadData(const char* pFilename);

	CMesh* GetMesh(void) { return m_apMesh; }

private:
	static CMesh* m_apMesh;			// 配置したメッシュの情報
	static int m_nNumObject;		// 現在オブジェクトの総数
};

#endif