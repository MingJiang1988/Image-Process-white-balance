// PickColor.cpp : implementation file
//

#include "stdafx.h"
#include "Autocrop.h"
#include "PickColor.h"
#include "afxdialogex.h"
#include "opencv2\highgui.hpp"
#include "opencv2\imgproc.hpp"
#include <fstream>
#include <iostream>
#include <io.h>

using namespace std;
using namespace cv;
// CPickColor dialog
Mat selected_mat;
int color_w = 0;
int color_h = 0;
int B;
int G;
int R;
IMPLEMENT_DYNAMIC(CPickColor, CDialogEx)

CPickColor::CPickColor(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG1, pParent)
{
	
}

CPickColor::~CPickColor()
{
}

void CPickColor::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_PICTURE, c_picture);
}


BEGIN_MESSAGE_MAP(CPickColor, CDialogEx)
	ON_STN_CLICKED(IDC_STATIC_PICTURE, &CPickColor::OnStnClickedStaticPicture)
	ON_WM_CTLCOLOR()
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDOK, &CPickColor::OnBnClickedOk)
END_MESSAGE_MAP()


// CPickColor message handlers
static void onMouse(int event, int x, int y, int f, void*) {

	if (event == CV_EVENT_LBUTTONDOWN)
	{
		Vec3b pix = selected_mat.at<Vec3b>(y, x);
		B = pix.val[0];
		G = pix.val[1];
		R = pix.val[2];

		/*int R = 255; 
		int G = 0;
		int B = 0;*/
		Mat grHistogram(color_w, color_h, CV_8UC3, Scalar(B, G, R));
		imshow("IDC_STATIC_OUTPUT_COLOR", grHistogram);
		
	}

	
}
//void getSkin(int r, int g, int b)
//{
//	int skinR = r;
//	int skinG = g;
//	int skinB = b;
//
//}




BOOL CPickColor::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CRect rect;
	GetDlgItem(IDC_STATIC_PICTURE)->GetWindowRect(&rect);
	//CImage image;
	//image.Load(strFileName); // just change extension to load jpg
	//CBitmap bitmap;
	//bitmap.Attach(image.Detach());
	//c_picture.SetBitmap(bitmap)

	int wd = rect.right - rect.left;
	int ht = rect.bottom - rect.top;

	CRect rect_c;
	GetDlgItem(IDC_STATIC_SELECT_COLOR)->GetWindowRect(&rect_c);
	color_w = rect_c.right - rect.left;
	color_h = rect_c.bottom - rect_c.top;

	CT2CA pszConvertedAnsiString(strFileName);
	std::string img_file(pszConvertedAnsiString);
	cv::Mat simg = cv::imread(img_file);
	cv::resize(simg, simg, cv::Size(wd, ht));
	cvNamedWindow("IDC_STATIC_OUTPUT", CV_WINDOW_AUTOSIZE);
	HWND hWnd = (HWND)cvGetWindowHandle("IDC_STATIC_OUTPUT");
	HWND hParent = ::GetParent(hWnd);
	::SetParent(hWnd, GetDlgItem(IDC_STATIC_PICTURE)->m_hWnd);
	::ShowWindow(hParent, SW_HIDE);

	cvNamedWindow("IDC_STATIC_OUTPUT_COLOR", CV_WINDOW_AUTOSIZE);
	HWND hWnd_c = (HWND)cvGetWindowHandle("IDC_STATIC_OUTPUT_COLOR");
	HWND hParent_C = ::GetParent(hWnd_c);
	::SetParent(hWnd_c, GetDlgItem(IDC_STATIC_SELECT_COLOR)->m_hWnd);
	::ShowWindow(hParent_C, SW_HIDE);

	cv::imshow("IDC_STATIC_OUTPUT", simg);

	selected_mat = simg.clone();
	setMouseCallback("IDC_STATIC_OUTPUT", onMouse, 0);
	std::vector<int> colorVector;
	ifstream input_file("balance.dat");
	double tempVar;
	while (input_file >> tempVar)
	{
		colorVector.push_back(tempVar);
	}

	Mat c_Mat(color_w, color_h, CV_8UC3, Scalar(colorVector.at(0), colorVector.at(1), colorVector.at(2)));
	cv::imshow("IDC_STATIC_OUTPUT_COLOR", c_Mat);

	std::vector<int> c_pic_Vector;
	ifstream in_file("balance.dat");
	double tVar;
	while (in_file >> tVar)
	{
		c_pic_Vector.push_back(tVar);
	}

	B = c_pic_Vector.at(0);
	G = c_pic_Vector.at(1);
	R = c_pic_Vector.at(2);

		
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}



void CPickColor::OnStnClickedStaticPicture()
{
	setMouseCallback("IDC_STATIC_OUTPUT", onMouse, 0);
	AfxMessageBox(_T("ssss"));
}




void CPickColor::OnBnClickedOk()
{
	string str_save;
	string Result;
	stringstream convert; // stringstream used for the conversion
	for (int i = 0; i < 3; i++)
	{
		convert.str("");
		if (i == 0)
		{
			convert << B;//add the value of Number to the characters in the stream
		}
		if (i == 1)
		{
			convert << G;//add the value of Number to the characters in the stream
		}
		if (i == 2)
		{
			convert << R;
		}

		Result = convert.str();//set Result to the content of the stream
		str_save.append(Result);
		if (i != 2)
		{
			str_save.append("\n");
		}

	}

	ofstream ff;
	ff.open("balance.dat");
	ff << str_save;
	ff.close();
	CDialogEx::OnOK();
}
