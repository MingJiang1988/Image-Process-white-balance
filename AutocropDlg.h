
// AutocropDlg.h : header file
//
#include "stdafx.h"
#include <string>
#include <vector>
#include "opencv2\highgui.hpp"
#pragma once


// CAutocropDlg dialog
class CAutocropDlg : public CDialogEx
{
// Construction
public:

	
	CAutocropDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_AUTOCROP_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonBrowser();
	afx_msg void OnBnClickedRadio2();
	afx_msg void OnBnClickedRadio3();
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButtonPicker();
	afx_msg void OnBnClickedCheck1();

	void processImage();
	std::vector<CString> ListFilesInDirectory(LPCTSTR dirName);
	static UINT MyThreadProc(LPVOID param);
	void displayPreivewColor();
	cv::Mat colorBalance(cv::Mat image);

	CString folder_path;
	CString input_file_dir;
	int n_selected;
	float image_scale = 0.5;
	
	CString m_progress_count;
	CString m_file_display_path;
	CButton *m_check_btn;
	int white_balance_enable = 0;
	

	int prev_mat_w = 0;
	int prev_mat_h = 0;

	
};
