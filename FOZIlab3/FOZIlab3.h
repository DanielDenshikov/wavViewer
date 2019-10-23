
// FOZIlab3.h : главный файл заголовка для приложения PROJECT_NAME
//

#pragma once

#ifndef __AFXWIN_H__
	#error "включить stdafx.h до включения этого файла в PCH"
#endif

#include "resource.h"		// основные символы


// CFOZIlab3App:
// О реализации данного класса см. FOZIlab3.cpp
//

class CFOZIlab3App : public CWinApp
{
public:
	CFOZIlab3App();

// Переопределение
public:
	virtual BOOL InitInstance();

// Реализация

	DECLARE_MESSAGE_MAP()
};

extern CFOZIlab3App theApp;