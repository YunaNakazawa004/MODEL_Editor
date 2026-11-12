//=============================================================================
// 
// メッシュドームヘッダー [ meshdome.h ]
// Author : Nakazawa Yuna
// 
//=============================================================================
#ifndef _MESHDOME_H_		// このマクロ定義がされていなかったら
#define _MESHDOME_H_		// 2重インクルード防止のマクロを定義する

#include "main.h"
#include "mesh.h"

//************************************************************************
// メッシュドームクラス
//************************************************************************
class CMeshDome : public CMesh
{
public:
	CMeshDome(const int nPriority = PRIORITY_2);
	virtual ~CMeshDome();

	static CMeshDome* Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot, const D3DXVECTOR2 block,
		const float fRadius, const char* pFilename, const CObject::TYPE type, const int nPriority = PRIORITY_2);
	HRESULT Init(void) { return S_OK; }
	HRESULT Init(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot,
		const D3DXVECTOR2 block, const float fRadius);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	void BindTexture(const int nTexture){ m_nIdxTexture = nTexture; }

	void SetPosition(const D3DXVECTOR3 pos);
	D3DXVECTOR3 GetPosition(void) { return m_pos; }
	void SetPosVtx(const int nCntVtx, const D3DXVECTOR3 pos);
	D3DXVECTOR3 GetPosVtx(const int nCntVtx);
	void SetRotation(const D3DXVECTOR3 rot);
	D3DXVECTOR3 GetRotation(void) { return m_rot; }
	void SetScale(const D3DXVECTOR3 scale);
	D3DXVECTOR3 GetScale(void) { return m_scale; }
	void SetColor(const D3DXCOLOR col);
	void SetColor(const int nCntVtx, const D3DXCOLOR col);
	D3DXCOLOR GetColor(void) { return m_col; }
	void SetTexUV(const int nCntVtx, const float ftexU, const float ftexV);
	void SetTexScroll(const int nCntVtx, const float fScrollU, const float fScrollV);
	float GetRadius(void) { return m_fRadius; }
	D3DXVECTOR2 GetBlock(void) { return m_block; }
	int GetNumVtx(void) { return m_nNumVtx; }
	int GetNumIdx(void) { return m_nNumIdx; }
	void SetDisp(const bool bDisp) { m_bDisp = bDisp; }
	void SwapDisp(void) { m_bDisp = m_bDisp ? false : true; }

private:
	int m_nIdxTexture;			// テクスチャのインデックス
	D3DXMATRIX m_mtxWorld;		// ワールドマトリックス
	D3DXVECTOR3 m_pos;			// 位置
	D3DXVECTOR3 m_rot;			// 向き
	float m_fRadius;			// 半径
	D3DXVECTOR3 m_scale;		// スケール
	D3DXVECTOR2 m_block;		// 分割数
	D3DXCOLOR m_col;			// 色
	int m_nNumVtx;				// 頂点の総数
	int m_nNumIdx;				// インデックスの総数
	bool m_bDisp;				// 表示するかどうか
};

#endif