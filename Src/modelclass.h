#pragma once

#ifndef _MODELCLASS_H_
#define _MODELCLASS_H_

#include <d3d11.h>
#include <D3DX10math.h>
#include "textureclass.h"

class ModelClass
{
private:
	struct VertexType
	{
		D3DXVECTOR3 position; // ���� ��ǥ
		D3DXVECTOR4 color; // ���� ����
		D3DXVECTOR2 texUV; // ������ �ؽ��� ��ǥ(uv)
		D3DXVECTOR3 normal; // ������ �븻(Face Normal)
	};

public:
	ModelClass();
	ModelClass(const ModelClass&);
	~ModelClass();

	bool Initialize(ID3D11Device*);
	bool Initialize(ID3D11Device*,const WCHAR*); // �ؽ��� �ε� ����� �߰��� �����ε�
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();

	ID3D11ShaderResourceView* GetTexture();

	bool Update();
	void GetWorldMatrix(MATRIX& pOut);

private:
	bool InitializeBuffers(ID3D11Device*);
	void ShotdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

	bool LoadTexture(ID3D11Device*, const WCHAR*);
	void ReleaseTexture();

private:
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	int m_vertexCount, m_indexCount;

	TextureClass* m_Texture;

	MATRIX m_worldMatrix;
};


#endif // !_MODELCLASS_H_
