
// MFCApplication1Dlg.cpp: 구현 파일
//

#include "stdafx.h"
#include "MFCApplication1.h"
#include "MFCApplication1Dlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

														// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMFCApplication1Dlg 대화 상자



CMFCApplication1Dlg::CMFCApplication1Dlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFCAPPLICATION1_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCApplication1Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMFCApplication1Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()


// CMFCApplication1Dlg 메시지 처리기

BOOL CMFCApplication1Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

									// 32 비트 색상으로 출력하기 위해서 폭 512, 높이 512의 32 비트 비트맵을 생성
	m_image.Create(m_Re_height, m_Re_width, 32);
	histogram.Create(m_Re_height, m_Re_width, 32);

	unsigned char *m_OutputImage = new unsigned char[m_Re_height*m_Re_width];
	// 이미지가 폭 512, 높이 512 크기인 8비트 이미지이기 때문에 512*512 크기의 메모리 할당
	unsigned char *p_src_image = new unsigned char[m_Re_height*m_Re_width];
	// 출력할 때 32비트 이미지로 출력하기 위해서 8비트 이미지를 32비트로 바꾸는 메모리 할당
	// 512*512*4 크기 메모리 할당
	unsigned char *p_dest_image = new unsigned char[m_Re_height*m_Re_width * 4];
	// p_dest_image, p_scr_image는 동적 할당된 메모리의 시작 주소를 기억하고 있어야지 작업이 끝난 후 동적 할당된 메모리의 주소 해제 가능
	unsigned char *p_dest_pos = p_dest_image;
	unsigned char *p_src_pos = p_src_image;
	unsigned char *m_OutputImage_pos = m_OutputImage;

	unsigned char *m_output_dest = new unsigned char[m_Re_height*m_Re_width * 4];
	unsigned char *m_OutputImage_dest = m_output_dest;
	// 히스토그램 출력용

	FILE *p_file;
	// 폭 512, 높이 512 크기의 8비트 이미지 파일을 읽음
	if (0 == fopen_s(&p_file, "512_512.raw", "rb"))
	{
		fread(p_src_image, m_Re_height*m_Re_width, 1, p_file);
		// 이미지 파일을 모두 읽음

		double hist[256];
		unsigned char scaleHist[256];
		// 히스토그램 생성 및 정규화 변수들
		
		double sumHist[256];
		double sum = 0;
		unsigned char low = 0, high = 255, temp;
		// 히스토그램 평활화 변수들

		// 히스토그램의 생성
		// hist[256] : 0 ~ 255
		// hist[i] : value(0~255), hist[i] = 빈도수
		// hist[pixel[i]]++;	-> 밝기값

		for (int i = 0; i < 256; i++)
		{
			hist[i] = 0;	// 초기화
		}

		for (int i = 0; i <= m_Re_height * m_Re_width; i++)
		{
			hist[p_src_pos[i]]++;	// 밝기값
		}	// 빈도수 조사(누적빈도 수)

		double max = hist[0], min = hist[0], dif = 0;	
		// 히스토그램 정규화 변수

		for (int i = 0; i < 256; i++)
		{
			if (hist[i] > max)
				max = hist[i];
		}

		for (int i = 0; i < 256; i++)
		{
			if (hist[i] < min)
				min = hist[i];
		}
		dif = max - min;

		for (int i = 0; i < 256; i++)
		{
			scaleHist[i] = (unsigned char)((hist[i] - min) * 255 / dif);
		}
		// 여기까지 정규화 과정!
		
		for (int i = 0; i < 256; i++)
		{
			sum += hist[i];
			sumHist[i] = sum;
		}	// 누적 히스토그램 생성

		int imgSize = m_Re_height * m_Re_width;
		for (int i = 0; i < imgSize; i++)
		{
			temp = p_src_image[i];
			p_src_pos[i] = (unsigned char)(sumHist[temp] * high / imgSize);
		}
		// 히스토그램 평활화

		for (int i = 0; i < 256; i++)
		{
			hist[i] = 0;	// 초기화
		}

		for (int i = 0; i <= m_Re_height * m_Re_width; i++)
		{
			hist[p_src_pos[i]]++;	// 밝기값
		}	// 빈도수 조사(누적빈도 수)

		for (int i = 0; i < 256; i++)
		{
			if (hist[i] > max)
				max = hist[i];
		}

		for (int i = 0; i < 256; i++)
		{
			if (hist[i] < min)
				min = hist[i];
		}
		dif = max - min;

		for (int i = 0; i < 256; i++)
		{
			scaleHist[i] = (unsigned char)((hist[i] - min) * 255 / dif);
		}
		
		for (int i = 0; i < 256; i++)	// 밝기 값 max = 256
		{
			for (int j = 0; j < scaleHist[i]; j++)	// 누적빈도 수 보다 작을 때
			{
				m_OutputImage[m_Re_width*(m_Re_height - j - 1) + i] = 0;
				// 해당 위치에 검은 점 입력
			}
		}
		// 히스토그램의 값을 출력 배열에 검은 점(0)으로 표현
		for (int i = 0; i < m_Re_height*m_Re_width; i++)
		{
			*p_dest_pos++ = *p_src_pos;	// Blue
			*p_dest_pos++ = *p_src_pos; // Green
			*p_dest_pos++ = *p_src_pos++;	// Red
			*p_dest_pos++ = 0xFF;	// Alpha
									// 사진 출력

			*m_OutputImage_dest++ = *m_OutputImage_pos;	// Blue
			*m_OutputImage_dest++ = *m_OutputImage_pos; // Green
			*m_OutputImage_dest++ = *m_OutputImage_pos++;	// Red
			*m_OutputImage_dest++ = 0xFF;	// Alpha
											// 히스토그램 출력
		}
		fclose(p_file);
		::SetBitmapBits(m_image, m_Re_height*m_Re_width * 4, p_dest_image);	// 사진
		::SetBitmapBits(histogram, m_Re_height*m_Re_width * 4, m_output_dest);	// 히스토그램
																				// 32비트로 변환된 이미지 값을 CImage 객체에 적용 시킴
	}
	delete[] p_dest_image;
	// 이미지 변환에 사용된 메모리 해제
	delete[]p_src_image;
	// 8비트 이미지를 읽기 위해 사용한 메모리 해제

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CMFCApplication1Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CMFCApplication1Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		// CDialogEx::OnPaint();
		CPaintDC dc(this);
		// (10, 10) 위치에 32비트로 변환한 이미지를 출력
		m_image.Draw(dc, 10, 10);	// 사진 출력 위치
		histogram.Draw(dc, 512, 10);	// 히스토그램 출력 위치
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CMFCApplication1Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

