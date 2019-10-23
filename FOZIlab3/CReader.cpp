
// CReader.cpp : файл реализации
//

#include "stdafx.h"
#include "FOZIlab3.h"
#include "CReader.h"
#include "afxdialogex.h"
#include <locale.h>
#include <stdio.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// диалоговое окно CReader
CReader::CReader(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_FOZILAB3_DIALOG, pParent)
	, m_sFileName(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CReader::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_sFileName);
}

BEGIN_MESSAGE_MAP(CReader, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CReader::OnBnClickedButton1)
	ON_BN_CLICKED(IDOK, &CReader::OnBnClickedOk)
	ON_WM_MOUSEWHEEL()
	ON_BN_CLICKED(IDC_BUTTON2, &CReader::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CReader::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CReader::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &CReader::OnBnClickedButton5)
END_MESSAGE_MAP()

// обработчики сообщений CReader
BOOL CReader::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Задает значок для этого диалогового окна.  Среда делает это автоматически,
	//  если главное окно приложения не является диалоговым
	SetIcon(m_hIcon, TRUE);			// Крупный значок
	SetIcon(m_hIcon, FALSE);		// Мелкий значок

	return TRUE;  // возврат значения TRUE, если фокус не передан элементу управления
}

void CReader::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // контекст устройства для рисования

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Выравнивание значка по центру клиентского прямоугольника
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Нарисуйте значок
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

HCURSOR CReader::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//------------------------------------------------------------------------------

bool CReader::PreDrawing(int XMouse, int Mode)
{
	bool Result = TRUE;
	int Displacement;

	fpos_t Pos;
	fgetpos(wavfile, &Pos);

	if (Mode)
	{
		if (XMouse < X1) XMouse = X1;
		if (XMouse > X2) XMouse = X2;
		Displacement = Step * (X2 - XMouse) * DPI * HEADER.Header.FrameByteSize;
		Pos -= Displacement;
		
		if (Mode == 1) Step /= Zoom;
		else if (Mode == 2) Step *= Zoom;

		if (!Step)
		{
			Step = 1;
			return FALSE;
		}

		Displacement = Step * (XMouse - X1) * DPI * HEADER.Header.FrameByteSize;
		Pos -= Displacement;
	}
	else
	{
		Pos += Step * DPI * XMouse * HEADER.Header.FrameByteSize;
	}

	
	if (Pos < 44) Pos = 44;
	if (Pos + (X2 - X1) * Step * DPI * HEADER.Header.FrameByteSize > HEADER.Header.DataSize + 44)
	{
		int Buffer = HEADER.Header.DataSize + 44 - (X2 - X1) * Step * DPI * HEADER.Header.FrameByteSize;
		if (Buffer < 44)
		{
			Pos = 44;
			Step = HEADER.Header.DataSize / HEADER.Header.FrameByteSize / (X2 - X1) / DPI;
			Result = FALSE;
		}
		else Pos = Buffer;
	}

	fsetpos(wavfile, &Pos);

	for (int i = 0; i <	(X2 - X1) * DPI; i++)
	{
		WAV[i] = 0;
		fseek(wavfile, (ActiveChannel - 1) * HEADER.Header.SampleBitSize / 8, SEEK_CUR);
		fread(&WAV[i], 1, HEADER.Header.SampleBitSize / 8, wavfile);
		fseek(wavfile, (HEADER.Header.numChannels - ActiveChannel) * HEADER.Header.SampleBitSize / 8, SEEK_CUR);
		fseek(wavfile, (Step - 1) * HEADER.Header.FrameByteSize, SEEK_CUR);
	}
	Drawing();
	return Result;
}

void CReader::Drawing()
{
	//Очистка экрана
	RedrawWindow();

	CClientDC dc(this);
	
	dc.FillSolidRect(X1, Y1, X2 - 10, Y2, BLACK_BRUSH);
	CPen Axis(PS_SOLID, 1, RGB(255, 0, 0));
	CPen *pOldPen;
	pOldPen = dc.SelectObject(&Axis);
	int ZeroLine = Y1 + (Y2 - Y1) / 2;
	dc.MoveTo(X1, ZeroLine);
	dc.LineTo(X2, ZeroLine);

	CPen WAVSignal(PS_SOLID, 1, RGB(125, 255, 193));
	dc.SelectObject(&WAVSignal);
	Axis.DeleteObject();

	dc.MoveTo(X1, ZeroLine);
	for (int i = 0; i < (X2 - X1) * DPI; i++)
	{
		double x = X1 + (double)i / DPI;
		double y = ZeroLine - ((double)(WAV[i] - ZeroLevel) / Max) * ((Y2 - Y1) / 2);
		dc.LineTo(x, y);
	}
	dc.LineTo(X2, ZeroLine);
	
	dc.SelectObject(pOldPen);
	WAVSignal.DeleteObject();
}

void CReader::ReadHeader()
{
	//Чтение заголовка
	fread(&HEADER, 4, 11, wavfile);
	fpos_t X;
	fgetpos(wavfile, &X);


	//Находим нулевой уровень и масштаб для вывода
	switch (HEADER.Header.SampleBitSize)
	{
	case 8:
		Max = 127;
		ZeroLevel = 128;
		break;
	case 16:
		Max = 32767;
		ZeroLevel = 0;
		break;
	case 24:
		Max = 8388607;
		ZeroLevel = 0;
		break;
	case 32:
		Max = 2147483647;
		ZeroLevel = 0;
		break;
	}

	//Находим количество каналов
	CButton *pBtn;
	pBtn = (CButton*)GetDlgItem(IDC_BUTTON5);
	pBtn->EnableWindow(FALSE);
	pBtn = (CButton*)GetDlgItem(IDC_BUTTON4);
	pBtn->EnableWindow(FALSE);
	pBtn = (CButton*)GetDlgItem(IDC_BUTTON3);
	pBtn->EnableWindow(FALSE);
	pBtn = (CButton*)GetDlgItem(IDC_BUTTON2);
	pBtn->EnableWindow(FALSE);
	switch (HEADER.Header.numChannels)
	{
	case 4:
		pBtn = (CButton*)GetDlgItem(IDC_BUTTON5);
		pBtn->EnableWindow(TRUE);
	case 3:
		pBtn = (CButton*)GetDlgItem(IDC_BUTTON4);
		pBtn->EnableWindow(TRUE);
	case 2:
		pBtn = (CButton*)GetDlgItem(IDC_BUTTON3);
		pBtn->EnableWindow(TRUE);
	case 1:
		pBtn = (CButton*)GetDlgItem(IDC_BUTTON2);
		pBtn->EnableWindow(TRUE);
		pBtn->SetFocus();
	}
}

//------------------------------------------------------------------------------

//Чтение
void CReader::OnBnClickedButton1()
{
	RedrawWindow();
	F1 = FALSE;
	F2 = FALSE;
	ActiveChannel = 0;
	//Проверка
	if (wavfile) fclose(wavfile);
	WAV = 0;
	wavfile = 0;

	setlocale(LC_ALL, "RUS");

	UpdateData(TRUE);
	wavfile = fopen(m_sFileName, "rb");
	if (wavfile)
	{
		RECT rect;
		GetClientRect(&rect);
		X1 = rect.left + 10;
		X2 = rect.right - 10;
		Y1 = rect.top + 100;
		Y2 = rect.bottom - 10;
		WAV = new int[(X2 - X1) * DPI];

		ReadHeader();
		F1 = TRUE;
	}
	else
	{
		m_sFileName.Format("Ошибка чтения!");
		UpdateData(FALSE);
	}
}

//Выход
void CReader::OnBnClickedOk()
{
	if(WAV) delete[] WAV;
	if(wavfile) fclose(wavfile);
	CDialogEx::OnOK();
}

//1 канал
void CReader::OnBnClickedButton2()
{
	F1 = TRUE;
	F2 = FALSE;
	fpos_t X = 44;
	fsetpos(wavfile, &X);
	Step = HEADER.Header.DataSize / HEADER.Header.FrameByteSize / (X2 - X1) / DPI;
	ActiveChannel = 1;
	PreDrawing(0, 0);
}

//2 канал
void CReader::OnBnClickedButton3()
{
	F1 = TRUE;
	F2 = FALSE;
	fpos_t X = 44;
	fsetpos(wavfile, &X);
	Step = HEADER.Header.DataSize / HEADER.Header.FrameByteSize / (X2 - X1) / DPI;
	ActiveChannel = 2;
	PreDrawing(0, 0);
}

//3 канал
void CReader::OnBnClickedButton4()
{
	F1 = TRUE;
	F2 = FALSE;
	fpos_t X = 44;
	fsetpos(wavfile, &X);
	Step = HEADER.Header.DataSize / HEADER.Header.FrameByteSize / (X2 - X1) / DPI;
	ActiveChannel = 3;
	PreDrawing(0, 0);
}

//4 канал
void CReader::OnBnClickedButton5()
{
	F1 = TRUE;
	F2 = FALSE;
	fpos_t X = 44;
	fsetpos(wavfile, &X);
	Step = HEADER.Header.DataSize / HEADER.Header.FrameByteSize / (X2 - X1) / DPI;
	ActiveChannel = 4;
	PreDrawing(0, 0);
}

//------------------------------------------------------------------------------

//Колесико мыши
BOOL CReader::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (zDelta > 0)
	{
		if (F1 && ActiveChannel)
		{
			CRect ClientRect;
			GetWindowRect(ClientRect);
			F1 = PreDrawing(pt.x - ClientRect.left - 8, 1);
			F2 = 1;
		}
	}
	else
	{
		if (F2 && ActiveChannel)
		{
			CRect ClientRect;
			GetWindowRect(ClientRect);
			F2 = PreDrawing(pt.x - ClientRect.left - 8, 2);
			F1 = 1;
		}
	}

	return CDialogEx::OnMouseWheel(nFlags, zDelta, pt);
}
