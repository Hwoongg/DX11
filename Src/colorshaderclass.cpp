#include "colorshaderclass.h"

ColorShaderClass::ColorShaderClass()
{
	m_vs = 0;
	m_ps = 0;
	m_layout = 0;
	m_matrixBuffer = 0;
}

ColorShaderClass::ColorShaderClass(const ColorShaderClass& other)
{

}

ColorShaderClass::~ColorShaderClass()
{

}

bool ColorShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;

	// 셰이더 컴파일&객체 생성. + 상수버퍼 생성
	result = InitializeShader(device, hwnd, L"./Shaders/color.vs",
		L"./Shaders/color.ps");
	if (!result)
	{
		return false;
	}

	return true;
}

void ColorShaderClass::Shutdown()
{
	ShutdownShader();

	return;
}

bool ColorShaderClass::Render(ID3D11DeviceContext* dc,
	int indexCount, D3DXMATRIX wm, D3DXMATRIX vm, D3DXMATRIX pm)
{
	bool result;

	result = SetShaderParameters(dc, wm, vm, pm);
	if (!result)
	{
		return false;
	}
	RenderShader(dc, indexCount);

	return true;
}

bool ColorShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, const WCHAR* vsFilename,
	const WCHAR* psFilename)
{
	HRESULT result;
	ID3D10Blob* errorMsg;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;

	errorMsg = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	// vsFilename의 경로(파일)에서 ColorVertexShader라는 키워드를
	// 메인함수 진입접으로 찾아 셰이더 5.0버전으로 컴파일한다.
	// 컴파일하여 vertexShaderBuffer에 할당. 에러가 있다면 errorMsg에 할당됨.
	result = D3DX11CompileFromFile(vsFilename, NULL, NULL, "ColorVertexShader",
		"vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS,
		0, NULL, &vertexShaderBuffer, &errorMsg, NULL);
	if (FAILED(result))
	{
		if (errorMsg)
		{
			OutputShaderErrorMessage(errorMsg, hwnd, vsFilename);
		}
		else
		{
			// 에러 메세지가 없다면 파일 없는 것.
			MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	// 바로 윗단과 동일한 과정.
	result = D3DX11CompileFromFile(psFilename, NULL, NULL, "ColorPixelShader",
		"ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &pixelShaderBuffer, &errorMsg, NULL);
	if (FAILED(result))
	{
		if (errorMsg)
		{
			OutputShaderErrorMessage(errorMsg, hwnd, psFilename);
		}
		else
		{
			// 에러 메세지가 없다면 파일 없는 것.
			MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}


	// 버퍼를 통해 셰이더 인스턴스화. (객체 생성)
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), NULL, &m_vs);
	if (FAILED(result))
	{
		return false;
	}
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(),
		pixelShaderBuffer->GetBufferSize(), NULL, &m_ps);
	if (FAILED(result))
	{
		return false;
	}

	// 입력 속성 내용 작성.
	// 어떤 정보들이 버텍스 셰이더에 입력되는지에 대한 기술.
	// 첫 12바이트는 POSITION에 대해 사용될 것.
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0; // 버퍼에 데이터가 어떻게 배열될 지 알려주는 변수
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	// 다음 16바이트는 COLOR에 대해 사용될 것
	polygonLayout[1].SemanticName = "COLOR";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT; // 이렇게 지정하면 자동?
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	// 레이아웃이 몇 개인지 측정.
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// 위 입력 속성을 기반으로 정점 셰이더 입력 레이아웃 생성
	result = device->CreateInputLayout(polygonLayout, numElements,
		vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), &m_layout);

	if (FAILED(result))
	{
		return false;
	}

	// 셰이더 버퍼 메모리 회수
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;
	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	// 상수 버퍼 내용 기술.
	// 프레임 마다 사용될 버퍼기 때문에 동적사용이 가능해야 한다.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC; // 동적으로 사용됨을 의미. (USAGE=사용법)
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType); // 해당 버퍼의 크기.
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER; // 상수버퍼라고 알림.
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // CPU 접근 권한.
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// 위 내용을 기반으로 상수버퍼 개체 생성.
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// 혼합 객체 생성
	// ...

	return true;
}

void ColorShaderClass::ShutdownShader()
{
	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}
	if (m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}
	if (m_ps)
	{
		m_ps->Release();
		m_ps = 0;
	}
	if (m_vs)
	{
		m_vs->Release();
		m_vs = 0;
	}

	return;
}

// 셰이더 에러 메시지 출력
void ColorShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMsg, HWND hwnd,
	const WCHAR* shaderFilename)
{
	char* compileErrors;
	unsigned long bufferSize, i;
	ofstream fout;

	compileErrors = (char*)(errorMsg->GetBufferPointer());

	bufferSize = errorMsg->GetBufferSize();

	// 에러 로그 기록용 텍스트파일
	fout.open("shader-error.txt");

	for (i = 0; i < bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	fout.close();

	errorMsg->Release();
	errorMsg = 0;

	MessageBox(hwnd, L"Error compiling shader. Check shader-error.txt for message.",
		shaderFilename, MB_OK);

	return;
}

bool ColorShaderClass::SetShaderParameters(ID3D11DeviceContext* dc,
	D3DXMATRIX mWM, D3DXMATRIX mVM, D3DXMATRIX mPM)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	unsigned int bufferNumber;

	// 행렬 전치 전에 한번 wvp를 곱해서 넘겨보려고 하는 중
	D3DXMATRIX WVP = mWM * mVM * mPM;
	//D3DXMatrixTranspose(&WVP, &WVP);

	// 행렬 전치. 셰이더 내부에서 열벡터 기준 연산시 속도가 빠르기 때문.
	// HLSL 셰이더는 기본적으로 열벡터 기준으로 연산한다.
	// DX9의 고정함수 파이프라인처럼 행벡터 기준 연산하려면
	// 셰이더 컴파일 시 컴파일 옵션에 
	// D3DCOMPILE_PACK_MATRIX_ROW_MAJOR 키워드를 추가해야 한다.
	D3DXMatrixTranspose(&mWM, &mWM);
	D3DXMatrixTranspose(&mVM, &mVM);
	D3DXMatrixTranspose(&mPM, &mPM);


	result = dc->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	dataPtr = (MatrixBufferType*)mappedResource.pData;

	dataPtr->world = mWM;
	dataPtr->view = mVM;
	dataPtr->projection = mPM;
	dataPtr->wvp = WVP;

	dc->Unmap(m_matrixBuffer, 0);

	bufferNumber = 0;

	dc->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	return true;
}

void ColorShaderClass::RenderShader(ID3D11DeviceContext* dc, int indexCount)
{
	dc->IASetInputLayout(m_layout);

	dc->VSSetShader(m_vs, NULL, 0);
	dc->PSSetShader(m_ps, NULL, 0);

	dc->DrawIndexed(indexCount, 0, 0);

	return;
}