////////////////////////////////////////////////////////////////////////////////
// Filename: dxinputclass.h
////////////////////////////////////////////////////////////////////////////////

//
// Direct Input �������̽��� �̿��� �Է� ��ġ ��� ���� Ŭ����.
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
	IDirectInputDevice8* m_keyboard; // Ű���� ��ġ
	IDirectInputDevice8* m_mouse; // ���콺 ��ġ

	unsigned char m_keyboardState[256]; // ���� Ű���� ����
	DIMOUSESTATE m_mouseState; // ���� ���콺 ����

	int m_screenWidth, m_screenHeight;
	int m_mouseX, m_mouseY;
};

#endif