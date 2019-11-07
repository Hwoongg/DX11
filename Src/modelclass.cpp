#include "modelclass.h"

ModelClass::ModelClass()
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;

	m_Texture = 0;
}

ModelClass::ModelClass(const ModelClass& other)
{

}

ModelClass::~ModelClass()
{

}

bool ModelClass::Initialize(ID3D11Device* device)
{
	bool result;

	// 해당 모델의 정점, 색인버퍼 생성.
	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}

	D3DXMatrixIdentity(&m_worldMatrix);

	return true;
}

bool ModelClass::Initialize(ID3D11Device* device, const WCHAR* texFilename)
{
	bool result;

	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}

	result = LoadTexture(device, texFilename);
	if (!result)
	{
		return false;
	}

	D3DXMatrixIdentity(&m_worldMatrix);

	return true;
}

void ModelClass::Shutdown()
{
	ReleaseTexture();

	ShotdownBuffers();

	return;
}

void ModelClass::Render(ID3D11DeviceContext* deviceContext)
{
	RenderBuffers(deviceContext);

	return;
}

int ModelClass::GetIndexCount()
{
	return m_indexCount;
}

ID3D11ShaderResourceView* ModelClass::GetTexture()
{
	return m_Texture->GetTexture();
}

bool ModelClass::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	m_vertexCount = 3;
	m_indexCount = 3;

	// 정점 데이터 배열 생성
	vertices = new VertexType[m_vertexCount];
	if (!vertices)
	{
		return false;
	}

	// 인덱스 데이터 배열 생성
	indices = new unsigned long[m_indexCount];
	if (!indices)
	{
		return false;
	}


	// 정점 데이터 입력
	vertices[0].position = D3DXVECTOR3(-1, -1, 0);
	vertices[0].color = D3DXVECTOR4(1, 0, 0, 1);
	vertices[0].texUV = D3DXVECTOR2(0.0f, 1.0f);
	vertices[0].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
	
	vertices[1].position = D3DXVECTOR3(0, 1, 0);
	vertices[1].color = D3DXVECTOR4(0, 1, 0, 1);
	vertices[1].texUV = D3DXVECTOR2(0.5f, 0.0f);
	vertices[1].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

	vertices[2].position = D3DXVECTOR3(1, -1, 0);
	vertices[2].color = D3DXVECTOR4(0, 0, 1, 1);
	vertices[2].texUV = D3DXVECTOR2(1.0f, 1.0f);
	vertices[2].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
	
	// 인덱스 데이터 입력
	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;

	// 정점 버퍼 내용 작성
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// 정점 데이터를 가리키는 보조 리소스 작성
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// 정점 버퍼 생성
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// 인덱스 버퍼 내용 작성
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// 인덱스 데이터를 가리키는 보조 리소스 작성.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// 인덱스 버퍼 생성
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}
	
	// 메모리 회수
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	return true;
}

void ModelClass::ShotdownBuffers()
{
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	return;
}

void ModelClass::RenderBuffers(ID3D11DeviceContext* dc)
{
	unsigned int stride;
	unsigned int offset;

	stride = sizeof(VertexType);
	offset = 0;

	dc->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	dc->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;

}

bool ModelClass::LoadTexture(ID3D11Device* device, const WCHAR* filename)
{
	bool result;

	m_Texture = new TextureClass;
	if (!m_Texture)
	{
		return false;
	}

	result = m_Texture->Initialize(device, filename);
	if (!result)
	{
		return false;
	}

	return true;
}

void ModelClass::ReleaseTexture()
{
	if (m_Texture)
	{
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = 0;
	}

	return;
}

bool ModelClass::Update()
{
	// 모델 회전.
	MATRIX mRot;

	D3DXMatrixRotationY(&mRot, 0.01f); // 10도 회전 행렬 생성.

	m_worldMatrix *= mRot;

	return true;
}

void ModelClass::GetWorldMatrix(MATRIX& pOut)
{
	pOut = m_worldMatrix;

	return;
}