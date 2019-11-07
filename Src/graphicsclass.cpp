////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "graphicsclass.h"


GraphicsClass::GraphicsClass()
{
	m_D3D = 0;

	m_Camera = 0;
	m_Model = 0;
	m_ColorShader = 0;

	m_TexShader = 0;

	m_LightShader = 0;
	m_Light = 0;
}


GraphicsClass::GraphicsClass(const GraphicsClass& other)
{
}


GraphicsClass::~GraphicsClass()
{
}


bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;

	// D3D 클래스 초기화
	m_D3D = new D3DClass();
	if (!m_D3D)
	{
		return false;
	}
	result = m_D3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED,
		hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		return false;
	}

	// 카메라 클래스 초기화
	m_Camera = new CameraClass;
	if (!m_Camera)
	{
		return false;
	}
	m_Camera->SetPosition(0, 0, -10.0f);

	// 삼각형 모델 초기화
	m_Model = new ModelClass;
	if (!m_Model)
		return false;
	//result = m_Model->Initialize(m_D3D->GetDevice());
	result = m_Model->Initialize(m_D3D->GetDevice(), L"./Textures/seafloor.dds");
	if (!result)
	{
		MessageBox(hwnd, L"Coult not initialize the model.", L"Error", MB_OK);
		return false;
	}

	// Color 셰이더 클래스 초기화
	m_ColorShader = new ColorShaderClass;
	if (!m_ColorShader)
		return false;
	result = m_ColorShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the color shader object.", L"Error", MB_OK);
		return false;
	}	

	// Texture 셰이더 클래스 초기화
	m_TexShader = new TextureShaderClass;
	if (!m_TexShader)
		return false;
	result = m_TexShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the texture shader object.",
			L"Error", MB_OK);
	}

	m_LightShader = new LightShaderClass;
	if (!m_LightShader)
		return false;

	result = m_LightShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the light shader object.",
			L"Error", MB_OK);
	}

	m_Light = new LightClass;
	if (!m_Light)
		return false;
	m_Light->SetDiffuseColor(1.0f, 0.0f, 1.0f, 1.0f); // 빛의 색상 설정.
	m_Light->SetDirection(0.0f, 0.0f, 1.0f); // 빛의 방향 설정.

	return true;
}


void GraphicsClass::Shutdown()
{
	if (m_Light)
	{
		delete m_Light;
		m_Light = 0;
	}

	if (m_LightShader)
	{
		m_LightShader->Shutdown();
		delete m_LightShader;
		m_LightShader = 0;
	}

	if (m_ColorShader)
	{
		m_ColorShader->Shutdown();
		delete m_ColorShader;
		m_ColorShader = 0;
	}

	if (m_Model)
	{
		m_Model->Shutdown();
		delete m_Model;
		m_Model = 0;
	}

	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	if (m_D3D)
	{
		m_D3D->Shutdown();
		delete m_D3D;
		m_D3D = 0;

	}

	
	return;
}


bool GraphicsClass::Frame()
{
	bool result;

	result = m_Model->Update();
	if (!result)
	{
		return false;
	}

	result = Render();
	if (!result)
	{
		return false;
	}
	return true;
}


bool GraphicsClass::Render()
{
	D3DXMATRIX mWM, mVM, mPM;
	bool result;


	m_D3D->BeginScene(0, 0, 0, 1.0f);

	// 카메라 준비.
	m_Camera->Render();

	// 셰이더에 세팅할 행렬 정보 수집.
	m_Camera->GetViewMatrix(mVM);
	//m_D3D->GetWorldMatrix(mWM);
	m_Model->GetWorldMatrix(mWM);
	m_D3D->GetProjectionMatrix(mPM);
	
	// 모델 정보 세팅
	m_Model->Render(m_D3D->GetDeviceContext());

	// Draw
	/*result = m_ColorShader->Render(m_D3D->GetDeviceContext(), m_Model->GetIndexCount()
		, mWM, mVM, mPM);
	if (!result)
		return false;*/

	/*result = m_TexShader->Render(m_D3D->GetDeviceContext(), m_Model->GetIndexCount(),
		mWM, mVM, mPM, m_Model->GetTexture());
	if (!result)
		return false;*/

	result = m_LightShader->Render(m_D3D->GetDeviceContext(), m_Model->GetIndexCount(),
		mWM, mVM, mPM, m_Model->GetTexture(), m_Light->GetDirection(), m_Light->GetDiffuseColor());
	if (!result)
		return false;

	m_D3D->EndScene();

	return true;
}