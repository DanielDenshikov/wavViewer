
// FOZIlab3.h : ������� ���� ��������� ��� ���������� PROJECT_NAME
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�������� stdafx.h �� ��������� ����� ����� � PCH"
#endif

#include "resource.h"		// �������� �������


// CFOZIlab3App:
// � ���������� ������� ������ ��. FOZIlab3.cpp
//

class CFOZIlab3App : public CWinApp
{
public:
	CFOZIlab3App();

// ���������������
public:
	virtual BOOL InitInstance();

// ����������

	DECLARE_MESSAGE_MAP()
};

extern CFOZIlab3App theApp;