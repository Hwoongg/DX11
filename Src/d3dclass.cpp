////////////////////////////////////////////////////////////////////////////////
// Filename: d3dclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "d3dclass.h"


D3DClass::D3DClass()
{
	m_swapChain = 0;
	m_device = 0;
	m_deviceContext = 0;
	m_renderTargetView = 0;
	m_depthStencilBuffer = 0;
	m_depthStencilState = 0;
	m_depthStencilView = 0;
	m_rasterState = 0;
}


D3DClass::D3DClass(const D3DClass& other)
{
}


D3DClass::~D3DClass()
{
}


bool D3DClass::Initialize(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen,
	float screenDepth, float screenNear)
{
	HRESULT result;
	IDXGIFactory* factory;
	IDXGIAdapter* adapter;
	IDXGIOutput* adapterOutput;
	unsigned int numModes, i, numerator, denominator, stringLength;
	DXGI_MODE_DESC* displayModeList;
	DXGI_ADAPTER_DESC adapterDesc;
	int error;
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	D3D_FEATURE_LEVEL featureLevel;
	ID3D11Texture2D* backBufferPtr;
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	D3D11_RASTERIZER_DESC rasterDesc;
	D3D11_VIEWPORT viewport;
	float fieldOfView, screenAspect;


	// Store the vsync setting. 수직동기화
	m_vsync_enabled = vsync;

	// Create a DirectX graphics interface factory.
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if (FAILED(result))
	{
		return false;
	}


	// ///////////////////////////////////////////////////////////
	// 그래픽 카드와 모니터의 주사율(새로고침 비율)이 필요함.
	// 기본값으로 대충 맞춰놓으면 버퍼플립을 사용하지 않고 blit를 사용하게됨.
	// 이는 성능 저하, 디버그에 에러 출력.
	// ///////////////////////////////////////////////////////////

	// 팩토리 객체로 첫번째에 있는 그래픽 카드에 대한 아답터 생성
	result = factory->EnumAdapters(0, &adapter);
	if (FAILED(result))
	{
		return false;
	}

	// 출력(모니터)에 대한 첫번째 아답터.
	result = adapter->EnumOutputs(0, &adapterOutput);
	if (FAILED(result))
	{
		return false;
	}

	// 첫번째 매개변수의 포맷에 맞는 모드의 개수 구하기.
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	if (FAILED(result))
	{
		return false;
	}

	// 가능한 모든 모니터와 그래픽카드 조합을 저장할 리스트 생성.
	displayModeList = new DXGI_MODE_DESC[numModes];
	if (!displayModeList)
	{
		return false;
	}

	// 디스플레이 모드에 대한 리스트 구조를 채워넣는다.
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
	if (FAILED(result))
	{
		return false;
	}

	// 모든 디스플레이 모드에 대해 화면 너비/높이에 맞는 디스플레이 모드 찾기.
	// 적절한 것을 찾으면 모니터 새로고침 비율의 분자, 분모값 저장
	for (i = 0; i < numModes; i++)
	{
		if (displayModeList[i].Width == (unsigned int)screenWidth)
		{
			if (displayModeList[i].Height == (unsigned int)screenHeight)
			{
				numerator = displayModeList[i].RefreshRate.Numerator; // 분자
				denominator = displayModeList[i].RefreshRate.Denominator; // 분모
			}
		}
	}

	// 그래픽카드 정보 획득.
	result = adapter->GetDesc(&adapterDesc);
	if (FAILED(result))
	{
		return false;
	}

	// 그래픽 카드 메모리 용량 측정.
	m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	// 그래픽 카드의 이름을 문자열로 저장.
	error = wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128);
	if (error != 0)
	{
		return false;
	}


	// ////////////////////////////////////
	// 위에서 썼던 초기화용 변수들 할당 해제
	// ////////////////////////////////////

	// Release the display mode list.
	delete[] displayModeList;
	displayModeList = 0;

	// Release the adapter output.
	adapterOutput->Release();
	adapterOutput = 0;

	// Release the adapter.
	adapter->Release();
	adapter = 0;

	// Release the factory.
	factory->Release();
	factory = 0;

	


	// /////////////////////////////////////////////////////
	//
	// 스왑체인 & 버퍼 구성. 백버퍼, 깊이, 스텐실버퍼.
	//
	// /////////////////////////////////////////////////////

	// Initialize the swap chain description.
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	// 백버퍼 개수
	swapChainDesc.BufferCount = 1;

	// 백버퍼 크기
	swapChainDesc.BufferDesc.Width = screenWidth;
	swapChainDesc.BufferDesc.Height = screenHeight;

	// 버퍼 픽셀 포멧 설정
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	// 백버퍼 새로고침 비율
	if (m_vsync_enabled) // 수직동기화 활성 여부에 맞춰 설정.
	{
		// 위에서 계산했던 새로고침 비율값
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	// Set the usage of the back buffer.
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// Set the handle for the window to render to.
	swapChainDesc.OutputWindow = hwnd;

	// Turn multisampling off.
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	// Set to full screen or windowed mode.
	if (fullscreen)
	{
		swapChainDesc.Windowed = false;
	}
	else
	{
		swapChainDesc.Windowed = true;
	}

	// Set the scan line ordering and scaling to unspecified.
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// Discard the back buffer contents after presenting.
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// Don't set the advanced flags.
	swapChainDesc.Flags = 0;

	// Set the feature level to DirectX 11.
	featureLevel = D3D_FEATURE_LEVEL_11_0;

	// Create the swap chain, Direct3D device, and Direct3D device context.
	result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1,
		D3D11_SDK_VERSION, &swapChainDesc, &m_swapChain, &m_device, NULL, &m_deviceContext);
	if (FAILED(result))
	{
		return false;
	}

	// Get the pointer to the back buffer.
	result = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
	if (FAILED(result))
	{
		return false;
	}

	// Create the render target view with the back buffer pointer.
	result = m_device->CreateRenderTargetView(backBufferPtr, NULL, &m_renderTargetView);
	if (FAILED(result))
	{
		return false;
	}

	// 백버퍼 포인터 자원 회수
	backBufferPtr->Release();
	backBufferPtr = 0;

	// 깊이버퍼 기술용 구조체 초기화
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	// 깊이버퍼 정보 기술
	depthBufferDesc.Width = screenWidth;
	depthBufferDesc.Height = screenHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	// 깊이 버퍼 텍스쳐 생성
	result = m_device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// 깊이&스텐실 상태 기술 구조체 초기화
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	// 깊이값 처리 옵션
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// 픽셀의 앞면 스텐실 옵션
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// 픽셀의 뒷면 스텐실 옵션
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// 깊이&스텐실 상태 객체 생성
	result = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
	if (FAILED(result))
	{
		return false;
	}

	// 깊이&스텐실 상태 세트.
	m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);

	// 깊이&스텐실 뷰 초기화
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	// 깊이&스텐실 뷰 옵션 기술.
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// 깊이&스텐실 뷰 생성
	result = m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
	if (FAILED(result))
	{
		return false;
	}

	// 렌더타겟, 깊이&스텐실 뷰 세트
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

	// 래스터 상태 옵션 설정. 다각형을 어떻게 채울지에 대한 설정.
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	// 래스터 상태 객체 생성.
	result = m_device->CreateRasterizerState(&rasterDesc, &m_rasterState);
	if (FAILED(result))
	{
		return false;
	}

	// 래스터 상태 세트.
	m_deviceContext->RSSetState(m_rasterState);

	// 뷰포트 옵션 설정.
	viewport.Width = (float)screenWidth;
	viewport.Height = (float)screenHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	// 뷰포트 세트.
	m_deviceContext->RSSetViewports(1, &viewport);

	// 투영 행렬 생성을 위한 필요값 계산.
	fieldOfView = (float)D3DX_PI / 4.0f;
	screenAspect = (float)screenWidth / (float)screenHeight;

	// FOV값과 해상도 비율, 근평면과 원평면의 거리값을 받아 원근투영 행렬 생성.
	D3DXMatrixPerspectiveFovLH(&m_projectionMatrix, fieldOfView, screenAspect, screenNear, screenDepth);

	// 월드행렬 초기화.
	D3DXMatrixIdentity(&m_worldMatrix);

	// 직교 투영 행렬 생성. 현재 사용하지 않음.
	D3DXMatrixOrthoLH(&m_orthoMatrix, (float)screenWidth, (float)screenHeight, screenNear, screenDepth);

	return true;
}


void D3DClass::Shutdown()
{
	// Before shutting down set to windowed mode or when you release the swap chain it will throw an exception.
	if (m_swapChain)
	{
		m_swapChain->SetFullscreenState(false, NULL);
	}

	if (m_rasterState)
	{
		m_rasterState->Release();
		m_rasterState = 0;
	}

	if (m_depthStencilView)
	{
		m_depthStencilView->Release();
		m_depthStencilView = 0;
	}

	if (m_depthStencilState)
	{
		m_depthStencilState->Release();
		m_depthStencilState = 0;
	}

	if (m_depthStencilBuffer)
	{
		m_depthStencilBuffer->Release();
		m_depthStencilBuffer = 0;
	}

	if (m_renderTargetView)
	{
		m_renderTargetView->Release();
		m_renderTargetView = 0;
	}

	if (m_deviceContext)
	{
		m_deviceContext->Release();
		m_deviceContext = 0;
	}

	if (m_device)
	{
		m_device->Release();
		m_device = 0;
	}

	if (m_swapChain)
	{
		m_swapChain->Release();
		m_swapChain = 0;
	}

	return;
}

// 렌더타겟 초기화
void D3DClass::BeginScene(float red, float green, float blue, float alpha)
{
	float color[4];


	// Setup the color to clear the buffer to.
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	// Clear the back buffer.
	m_deviceContext->ClearRenderTargetView(m_renderTargetView, color);

	// Clear the depth buffer.
	m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	return;
}


void D3DClass::EndScene()
{
	// Present the back buffer to the screen since rendering is complete.
	if (m_vsync_enabled)
	{
		// Lock to screen refresh rate.
		m_swapChain->Present(1, 0);
	}
	else
	{
		// Present as fast as possible.
		m_swapChain->Present(0, 0);
	}

	return;
}


ID3D11Device* D3DClass::GetDevice()
{
	return m_device;
}


ID3D11DeviceContext* D3DClass::GetDeviceContext()
{
	return m_deviceContext;
}


void D3DClass::GetProjectionMatrix(MATRIX& projectionMatrix)
{
	projectionMatrix = m_projectionMatrix;
	return;
}


void D3DClass::GetWorldMatrix(MATRIX& worldMatrix)
{
	worldMatrix = m_worldMatrix;
	return;
}


void D3DClass::GetOrthoMatrix(MATRIX& orthoMatrix)
{
	orthoMatrix = m_orthoMatrix;
	return;
}


void D3DClass::GetVideoCardInfo(char* cardName, int& memory)
{
	strcpy_s(cardName, 128, m_videoCardDescription);
	memory = m_videoCardMemory;
	return;
}