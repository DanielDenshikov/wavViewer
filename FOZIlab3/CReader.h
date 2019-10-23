
// CReader.h : ���� ���������
//

#pragma once
#include "afxwin.h"

struct tagHeader
{
	unsigned int chunkID;
	unsigned int chunkSize;
	unsigned int format;
	unsigned int subchunk1Id;
	unsigned int subchunk1Size;
	unsigned short int audioFormat;
	unsigned short int numChannels;
	unsigned int sampleRate;
	unsigned int byteRate;
	unsigned short int FrameByteSize;
	unsigned short int SampleBitSize;
	unsigned int subchunk2Id;
	unsigned int DataSize;
};

union unHeader
{
	tagHeader Header;
	unsigned int Array[11];
};


// ���������� ���� CReader
class CReader : public CDialogEx
{
// ��������
public:
	CReader(CWnd* pParent = NULL);	// ����������� �����������

// ������ ����������� ����
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FOZILAB3_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// ��������� DDX/DDV


// ����������
protected:
	HICON m_hIcon;

	// ��������� ������� ����� ���������
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
protected:
	CButton m_bFileInProject;
	CString m_sFileName;
protected:
	void ReadHeader();
	bool PreDrawing(int XMouse, int Mode);
	void Drawing();
protected:
	//���������� ��� ����
	int X1, X2, Y1, Y2;
	int DPI = 4;
	float Zoom = 2;

	//���������� ��� �����
	float width;
	int ZeroLevel = 0;
	int Max;
	int Step = 0;

	//���������� ��� ������ ������
	int *WAV = NULL;
	FILE *wavfile = NULL;
	unHeader HEADER;
	int ActiveChannel = 0;

	//�����
	bool F1 = FALSE;
	bool F2 = FALSE;
	bool R = FALSE;
	bool L = FALSE;
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedOk();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton5();
};
