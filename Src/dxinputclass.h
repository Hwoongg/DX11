////////////////////////////////////////////////////////////////////////////////
// Filename: dxinputclass.h
////////////////////////////////////////////////////////////////////////////////

//
// Direct Input 인터페이스를 이용한 입력 장치 고속 접근 클래스.
//

#ifndef _DXINPUTCLASS_H_
#define _DXINPUTCLASS_H_


///////////////////////////////
// PRE-PROCESSING DIRECTIVES //
///////////////////////////////
#define DIRECTINPUT_VERSION 0x0800


/////////////
// LINKING //
/////////////
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")


//////////////
// INCLUDES //
//////////////
#include <dinput.h>


////////////////////////////////////////////////////////////////////////////////
// Class name: DXInputClass
////////////////////////////////////////////////////////////////////////////////
class DXInputClass
{
public:
	DXInputClass();
	DXInputClass(const DXInputClass&);
	~DXInputClass();

	bool Initialize(HINSTANCE, HWND, int, int);
	void Shutdown();
	bool Frame();

	bool IsEscapePressed();
	void GetMouseLocation(int&, int&);

private:
	bool ReadKeyboard();
	bool ReadMouse();
	void ProcessInput();

private:
	IDirectInput8* m_directInput; // Direct Input
	IDirectInputDevice8* m_keyboard; // 키보드 장치
	IDirectInputDevice8* m_mouse; // 마우스 장치

	unsigned char m_keyboardState[256]; // 현재 키보드 상태
	DIMOUSESTATE m_mouseState; // 현재 마우스 상태

	int m_screenWidth, m_screenHeight;
	int m_mouseX, m_mouseY;
};

#endif