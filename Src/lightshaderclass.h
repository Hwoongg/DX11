////////////////////////////////////////////////////////////////////////////////
// Filename: lightshaderclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _LIGHTSHADERCLASS_H_
#define _LIGHTSHADERCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11async.h>
#include <fstream>
using namespace std;


////////////////////////////////////////////////////////////////////////////////
// Class name: LightShaderClass
////////////////////////////////////////////////////////////////////////////////
class LightShaderClass
{
private:
	struct MatrixBufferType
	{
		D3DXMATRIX world;
		D3DXMATRIX view;
		D3DXMATRIX projection;
	};

	struct LightBufferType
	{
		// CreateBuffer()가 16의 배수 바이트를 요구하기 때문에 패딩 필드를 추가하였음.
		// 16*4096의 크기까지로 생성할 수 있다.
		D3DXVECTOR4 diffuseColor;
		D3DXVECTOR3 lightDirection;
		float padding; 
	};

public:
	LightShaderClass();
	LightShaderClass(const LightShaderClass&);
	~LightShaderClass();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, 
		ID3D11ShaderResourceView*, D3DXVECTOR3, D3DXVECTOR4);

private:
	bool InitializeShader(ID3D11Device*, HWND, const WCHAR*, const WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, const WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, 
		ID3D11ShaderResourceView*, D3DXVECTOR3, D3DXVECTOR4);
	void RenderShader(ID3D11DeviceContext*, int);

private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11SamplerState* m_sampleState;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_lightBuffer;
};

#endif