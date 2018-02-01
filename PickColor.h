#pragma once
#include "afxwin.h"
#include "opencv2\highgui.hpp"


// CPickColor dialog

class CPickColor : public CDialogEx
{
	DECLARE_DYNAMIC(CPickColor)

public:
	
	CPickColor(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPickColor();

	CString strFileName;
// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CStatic c_picture;
	afx_msg void OnStnClickedStaticPicture();
	afx_msg void OnBnClickedOk();
};
