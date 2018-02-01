
// AutocropDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Autocrop.h"
#include "AutocropDlg.h"
#include "resource.h"
#include "afxdialogex.h"
#include "opencv2\core.hpp"
#include "opencv2\imgproc.hpp"
#include "opencv2\highgui.hpp"
#include <iostream>
#include <fstream>
#include "stdafx.h"
#include <io.h>
#include "PickColor.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif
using namespace cv;
using namespace std;

// CAboutDlg dialog used for App About
CString selected_dir_path;
int p_B = 0;
int p_G = 0;
int p_R = 0;

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
//	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
//	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CAutocropDlg dialog



CAutocropDlg::CAutocropDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_AUTOCROP_DIALOG, pParent)
	, input_file_dir(_T(""))
	, m_progress_count(_T(""))
	, m_file_display_path(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CAutocropDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_INPUT_DIR, input_file_dir);
	DDX_Text(pDX, IDC_STATIC_count, m_progress_count);
	DDX_Text(pDX, IDC_STATIC_FILE_PATH, m_file_display_path);
}

BEGIN_MESSAGE_MAP(CAutocropDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_BROWSER, &CAutocropDlg::OnBnClickedButtonBrowser)
	ON_BN_CLICKED(IDC_RADIO2, &CAutocropDlg::OnBnClickedRadio2)
	ON_BN_CLICKED(IDC_RADIO3, &CAutocropDlg::OnBnClickedRadio3)
	ON_BN_CLICKED(IDC_RADIO1, &CAutocropDlg::OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_BUTTON2, &CAutocropDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON_PICKER, &CAutocropDlg::OnBnClickedButtonPicker)
	ON_BN_CLICKED(IDC_CHECK1, &CAutocropDlg::OnBnClickedCheck1)
END_MESSAGE_MAP()


// CAutocropDlg message handlers

BOOL CAutocropDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	CRect rect_p_r;
	GetDlgItem(IDC_STATIC_PREVIEW_COLOR)->GetWindowRect(&rect_p_r);
	prev_mat_w = rect_p_r.right - rect_p_r.left;
	prev_mat_h = rect_p_r.bottom - rect_p_r.top;
	displayPreivewColor();
	cvNamedWindow("IDC_STATIC_PREVIEW", CV_WINDOW_AUTOSIZE);
	HWND hWnd_p = (HWND)cvGetWindowHandle("IDC_STATIC_PREVIEW");
	HWND hParent_p = ::GetParent(hWnd_p);
	::SetParent(hWnd_p, GetDlgItem(IDC_STATIC_PREVIEW_COLOR)->m_hWnd);
	::ShowWindow(hParent_p, SW_HIDE);


	CheckRadioButton(IDC_RADIO1, IDC_RADIO3, IDC_RADIO1);

	n_selected = GetCheckedRadioButton(IDC_RADIO1, IDC_RADIO3) - IDC_RADIO1;
	m_check_btn = (CButton*)GetDlgItem(IDC_CHECK1);
	white_balance_enable = m_check_btn->GetCheck();
	GetDlgItem(IDC_BUTTON_PICKER)->EnableWindow(false);
	return TRUE;  // return TRUE  unless you set the focus to a control
}


cv::Mat CAutocropDlg::colorBalance(cv::Mat image)
{
	Mat new_image = image.clone();
	float rSmpl = 0, gSmpl = 0, bSmpl = 0, greySmpl = 0;
	for (int x = 0; x<6; x++)
	{
		for (int y = 0; y<6; y++)
		{
			if (rSmpl == 0)
			{
				bSmpl = image.at<Vec3b>(Point(x, y))[0];
				gSmpl = image.at<Vec3b>(Point(x, y))[1];
				rSmpl = image.at<Vec3b>(Point(x, y))[2];
			}
			else
			{
				bSmpl = (bSmpl + image.at<Vec3b>(Point(x, y))[0]) / 2;
				gSmpl = (gSmpl + image.at<Vec3b>(Point(x, y))[1]) / 2;
				rSmpl = (rSmpl + image.at<Vec3b>(Point(x, y))[2]) / 2;
			}
		}
	}
	//rSmpl = images[0](disp.mouse_x,disp.mouse_y,0,0);
	//gSmpl = images[0](disp.mouse_x,disp.mouse_y,0,1);
	//bSmpl = images[0](disp.mouse_x,disp.mouse_y,0,2);
	greySmpl = (rSmpl + gSmpl + bSmpl) / 3;
	//cout << "SAMPLE PIXEL:" << rSmpl << "," << gSmpl << "," << bSmpl << "," << (int)greySmpl << endl;
	float rCoeff = 0, gCoeff = 0, bCoeff = 0, skinGrey = 0;
	skinGrey = (p_R + p_G + p_B) / 3;
	rCoeff = (p_R / skinGrey) / (rSmpl / greySmpl);
	gCoeff = (p_G / skinGrey) / (gSmpl / greySmpl);
	bCoeff = (p_B / skinGrey) / (bSmpl / greySmpl);
	//cout << "COEFFICIENTS:" << rCoeff << "," << gCoeff << "," << bCoeff << endl;
	for (unsigned int x = 0; x<image.size().width; x++)
	{
		for (unsigned int y = 0; y<image.size().height; y++)
		{
			float bPix = image.at<Vec3b>(Point(x, y))[0];
			float gPix = image.at<Vec3b>(Point(x, y))[1];
			float rPix = image.at<Vec3b>(Point(x, y))[2];
			new_image.at<Vec3b>(Point(x, y))[2] = rPix*rCoeff;
			if (rPix*rCoeff > 255)
			{
				new_image.at<Vec3b>(Point(x, y))[2] = 255;
			}
			new_image.at<Vec3b>(Point(x, y))[1] = gPix*gCoeff;
			if (gPix*gCoeff > 255)
			{
				new_image.at<Vec3b>(Point(x, y))[1] = 255;
			}
			new_image.at<Vec3b>(Point(x, y))[0] = bPix*bCoeff;
			if (bPix*bCoeff > 255)
			{
				new_image.at<Vec3b>(Point(x, y))[0] = 255;
			}
		}
	}
	
	return new_image;
}


void CAutocropDlg::displayPreivewColor()
{
	std::vector<int> newVector;
	ifstream input_file("balance.dat");
	double tempVar;
	while (input_file >> tempVar)
	{
		newVector.push_back(tempVar);
	}

	p_B = newVector.at(0);
	p_G = newVector.at(1);
	p_R = newVector.at(2);

	Mat prevMat(prev_mat_w, prev_mat_h, CV_8UC3, Scalar(p_B, p_G, p_R));
	imshow("IDC_STATIC_PREVIEW", prevMat);
}

void CAutocropDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CAutocropDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CAutocropDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CAutocropDlg::OnBnClickedButtonBrowser()
{
	BROWSEINFO bi = { 0 };
	bi.lpszTitle = _T("Select Folder");
	LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
	if (pidl != 0)
	{
		// get the name of the folder
		TCHAR path[MAX_PATH];
		SHGetPathFromIDList(pidl, path);
		CString str(path);
		input_file_dir = path;
		// free memory used
		IMalloc * imalloc = 0;
		if (SUCCEEDED(SHGetMalloc(&imalloc)))
		{
			imalloc->Free(pidl);
			imalloc->Release();
		}
	}
	UpdateData(false);
}


void CAutocropDlg::OnBnClickedRadio2()
{
	CheckRadioButton(IDC_RADIO1, IDC_RADIO3, IDC_RADIO2);

	n_selected = GetCheckedRadioButton(IDC_RADIO1, IDC_RADIO3) - IDC_RADIO1;
}


void CAutocropDlg::OnBnClickedRadio3()
{
	CheckRadioButton(IDC_RADIO1, IDC_RADIO3, IDC_RADIO3);

	n_selected = GetCheckedRadioButton(IDC_RADIO1, IDC_RADIO3) - IDC_RADIO1;
}


void CAutocropDlg::OnBnClickedRadio1()
{
	CheckRadioButton(IDC_RADIO1, IDC_RADIO3, IDC_RADIO1);

	n_selected = GetCheckedRadioButton(IDC_RADIO1, IDC_RADIO3) - IDC_RADIO1;
}


void CAutocropDlg::OnBnClickedButton2()
{
	UpdateData(true);
	if (input_file_dir.IsEmpty())
	{
		MessageBoxA(NULL, "Please select image directory path", "Input Invaild", MB_OK | MB_ICONERROR);
		return;
	}
	selected_dir_path = input_file_dir;
	GetDlgItem(IDC_BUTTON2)->EnableWindow(false);
	::AfxBeginThread(MyThreadProc, (LPVOID)this);
	
}

vector<CString> CAutocropDlg::ListFilesInDirectory(LPCTSTR dirName)
{

	// Check input parameters

	ASSERT(dirName != NULL);

	// Clear filename list
	vector<CString> filepaths;
	filepaths.clear();

	// Object to enumerate files

	CFileFind finder;

	// Build a string using wildcards *.*,

	// to enumerate content of a directory

	CString wildcard(dirName);

	wildcard += _T("\\*.*");

	// Init the file finding job

	BOOL working = finder.FindFile(wildcard);

	// For each file that is found:

	while (working)

	{

		// Update finder status with new file

		working = finder.FindNextFile();

		// Skip '.' and '..'

		if (finder.IsDots())

		{

			continue;

		}

		// Skip sub-directories

		if (finder.IsDirectory())

		{

			continue;

		}

		// Add file path to container

		filepaths.push_back(finder.GetFilePath());
		

	}

	// Cleanup file finder
	
	finder.Close();
	return filepaths;

}


UINT CAutocropDlg::MyThreadProc(LPVOID param)
{
	CAutocropDlg* cad = (CAutocropDlg*)param;
	cad->processImage();

	return 0;   // thread completed successfully  
}

void CAutocropDlg::processImage()
{
	std::vector<CString> filePaths;

	CString dir = selected_dir_path;
	
	filePaths = ListFilesInDirectory(dir);
	//filePaths.push_back(_T("C:\\Users\\ELSA\\Desktop\\photos grey\\DSC_1905.JPG"));
	//filePaths.push_back(_T("C:\\Users\\ELSA\\Desktop\\photos grey\\DSC_1906.JPG"));
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	CProgressCtrl * cpr;
	cpr = (CProgressCtrl*)GetDlgItem(IDC_PROGRESS2);
	cpr->SetStep(1);
	cpr->SetRange(0, filePaths.size());
	CString count_str;
	count_str.Format(_T("%d"), 0);
	CString total_str;
	total_str.Format(_T("%d"), filePaths.size());
	CString display_str = count_str + "/" + total_str;
	
	for (int i = 0; i < filePaths.size(); i++)
	{
		m_file_display_path = filePaths[i];
		UpdateData(false);
	
		CString buff_str = filePaths[i];
		std::string file_path_str(CW2A(buff_str.GetString(), CP_UTF8));

		try {
			cv::Mat input = cv::imread(file_path_str);

			cv::Mat src = input.clone();
			if (n_selected == 1)
			{
				cv::rotate(input, src, ROTATE_90_CLOCKWISE);
			}
			if (n_selected == 2)
			{
				cv::rotate(input, src, ROTATE_90_COUNTERCLOCKWISE);
			}
			//cv::resize(src, src, cv::Size(input.cols * image_scale, input.rows * image_scale), 0, 0, CV_INTER_NN);
			Mat gray, edge, draw;
			cvtColor(src, gray, CV_BGR2GRAY);

			Canny(gray, edge, 10, 30);
			Mat binary_img;
			Mat kernel = Mat::ones(Size(31, 31), CV_8U);
			dilate(edge, binary_img, kernel);
			//edge.convertTo(draw, CV_8U);

			findContours(binary_img, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE); // Find the contours in the image
			int largest_area = 0;
			int largest_contour_index = 0;
			Rect bounding_rect;
			Rect final_bounding_rect;
			for (int i = 0; i< contours.size(); i++) // iterate through each contour.
			{
				double a = contourArea(contours[i], false);  //  Find the area of contour
				if (a>largest_area) {
					largest_area = a;
					largest_contour_index = i;                //Store the index of largest contour
					bounding_rect = boundingRect(contours[i]); // Find the bounding rectangle for biggest contour
				}

			}
			/*rectangle(src, bounding_rect, Scalar(0, 255, 0), 1, 8, 0);
			cv::namedWindow("image_src", CV_WINDOW_AUTOSIZE);
			cv::imshow("image_src", src);

			cv::namedWindow("image_b", CV_WINDOW_AUTOSIZE);
			cv::imshow("image_b", binary_img);*/

			Mat src_clone = src.clone();
			int height_ext = bounding_rect.size().width * 1.4;
			float extend_size_ratio = 0.0;
			int h_diff = 0;
			int w_diff = 0;
			if (bounding_rect.size().height <= height_ext)
			{
				final_bounding_rect.height = height_ext;
				final_bounding_rect.width = bounding_rect.size().width;
				final_bounding_rect.x = bounding_rect.x;
				h_diff = height_ext - bounding_rect.size().height;
				final_bounding_rect.y = bounding_rect.y - h_diff / 2;
			}
			if (bounding_rect.size().height > height_ext) {

				extend_size_ratio = bounding_rect.size().height*1.0 / height_ext;
				final_bounding_rect.width = bounding_rect.size().width * extend_size_ratio;
				final_bounding_rect.height = bounding_rect.size().height;
				h_diff = bounding_rect.size().height - final_bounding_rect.height;
				w_diff = final_bounding_rect.width - bounding_rect.size().width;
				final_bounding_rect.x = bounding_rect.x - w_diff / 2;
				final_bounding_rect.y = bounding_rect.y - h_diff / 2;

			}
			if (final_bounding_rect.height > src_clone.size().height || final_bounding_rect.width > src_clone.size().width)
			{
				if (src_clone.size().width < src_clone.size().height)
				{
					final_bounding_rect.width = src_clone.size().width;
					final_bounding_rect.height = bounding_rect.height;
					final_bounding_rect.x = 0;
				}
				if (src_clone.size().height < src_clone.size().width)
				{
					final_bounding_rect.height = src_clone.size().height;
					final_bounding_rect.y = 0;
					final_bounding_rect.width = bounding_rect.width;
				}
				//final_bounding_rect = bounding_rect;
			}
			
			CString cropped_fileName = buff_str.Mid(buff_str.ReverseFind('\\') + 1);
			CString directory_path = buff_str;
			directory_path.Truncate(directory_path.ReverseFind('\\'));
			directory_path = directory_path + _T("\\CROP\\");
			if (CreateDirectory(directory_path, NULL) ||
				ERROR_ALREADY_EXISTS == GetLastError())
			{
				// CopyFile(...)
			}
			else
			{
				// Failed to create directory.
			}
			CString cropped_file_path = directory_path + cropped_fileName;
			try {
				Mat croppedImage = src_clone(final_bounding_rect);
				std::string cropped_file_path_str(CW2A(cropped_file_path.GetString(), CP_UTF8));
				if (white_balance_enable == 1)
				{
					croppedImage = colorBalance(croppedImage);
				}
				cv::imwrite(cropped_file_path_str, croppedImage);
			}
			catch (exception e)
			{
				//printf("error");

			}

			//rectangle(src_clone, bounding_rect, Scalar(0, 255, 0), 1, 8, 0);
			/*rectangle(src_clone, final_bounding_rect, Scalar(0, 255, 0), 1, 8, 0);
			cv::namedWindow("image", CV_WINDOW_AUTOSIZE);
			cv::imshow("image", src_clone);*/

			
		}
		catch (exception e)
		{

		}
		cpr->SetPos(i + 1);
		count_str.Format(_T("%d"), i + 1);
		display_str = count_str + "/" + total_str;
		this->m_progress_count = display_str;
		UpdateData(false);
		
	}
	
	MessageBoxA(NULL,"Completed", "Success information", MB_OK | MB_ICONINFORMATION);
	GetDlgItem(IDC_BUTTON2)->EnableWindow(true);
}


void CAutocropDlg::OnBnClickedButtonPicker()
{
	
	char szFilters[] = "Image Files (*.jpg)|*.jpg|All Files (*.*)|*.*||";
	// Create an Open dialog; the default file name extension is ".my".
	CFileDialog fileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("Image Files (*.jpg;*.jpeg;*.png)|*.jpg;*.jpeg;*.png||"), this);

	// Display the file dialog. When user clicks OK, fileDlg.DoModal()
	// returns IDOK.
	if (fileDlg.DoModal() == IDOK)
	{
		CString m_strPathname = fileDlg.GetPathName();
		CPickColor dlg;
		dlg.strFileName = m_strPathname;
		
		if (dlg.DoModal() == IDOK) {
			displayPreivewColor();
				
		}

	}
}


//HBRUSH CAboutDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
//{
//	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);
//
//	// TODO:  Change any attributes of the DC here
//
//	
//
//	// TODO:  Return a different brush if the default is not desired
//	return hbr;
//}


void CAutocropDlg::OnBnClickedCheck1()
{
	white_balance_enable = m_check_btn->GetCheck();
	if (white_balance_enable == 0)
	{
		GetDlgItem(IDC_BUTTON_PICKER)->EnableWindow(false);
	}
	if (white_balance_enable == 1)
	{
		GetDlgItem(IDC_BUTTON_PICKER)->EnableWindow(true);
	}
	
}
