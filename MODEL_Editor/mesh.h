//========================================================================
// 
// メッシュヘッダー [ mesh.h ]
// Author : Nakazawa Yuna
// 
//========================================================================
#ifndef _MESH_H_		// このマクロ定義がされていなかったら
#define _MESH_H_		// 2重インクルード防止のマクロを定義する

#include "main.h"
#include "object.h"

//************************************************************************
// マクロ定義
//************************************************************************

//************************************************************************
// メッシュクラス
//************************************************************************
class CMesh : public CObject
{
public:
	// 種類
	enum MESH
	{
		MESHWALL = 0,		// メッシュウォール
		MESHFIELD,			// メッシュフィールド
		MESHRING,			// メッシュリング
		MESHCYLINDER,		// メッシュシリンダー
		MESHDOME,			// メッシュドーム
		MESH_MAX
	};

	CMesh(const int nPriority = PRIORITY_3);
	virtual ~CMesh();

	static CMesh* Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot, const D3DXVECTOR2 block, const D3DXVECTOR2 size, const int type);
	virtual HRESULT Init(void);
	virtual void Uninit(void);
	virtual void Update(void);
	virtual void Draw(void);

	HRESULT CreateVtxBuff(const int nValue);
	HRESULT CreateVtxBuffMulti(const int nValue);
	HRESULT CreateIdxBuff(const int nValue);
	LPDIRECT3DVERTEXBUFFER9 GetVtxBuff(void) { return m_pVtxBuff; }
	LPDIRECT3DINDEXBUFFER9 GetIdxBuff(void) { return m_pIdxBuff; }

	int GetVtxIdx(void) { return m_nVtxIdx; }
	D3DXVECTOR3 GetVtxPos(void);

	HRESULT WriteData(const char* pFilename);
	HRESULT ReadData(const char* pFilename);

private:
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;				// 頂点バッファへのポインタ
	LPDIRECT3DINDEXBUFFER9 m_pIdxBuff;				// インデックスバッファへのポインタ
	int m_nNumVtxAll;	// 頂点の総数
	int m_nNumIdxAll;	// インデックスの総数
	int m_nVtxIdx;		// 選択されている頂点
	int m_type;			// 種類
};

#endif