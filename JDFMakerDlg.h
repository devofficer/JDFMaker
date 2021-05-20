
// JDFMakerDlg.h : header file
//

#pragma once


// CJDFMakerDlg dialog
class CJDFMakerDlg : public CDialogEx
{
// Construction
public:
	CJDFMakerDlg(CWnd* pParent = nullptr);
	void GenerateJDF(LPCTSTR sourcePath);
	// standard constructor
	CString GetSourceDir();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_JDFMAKER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CString m_szTarPath;
	CString m_strTDDPath;
	CString m_szSrcPath;
	afx_msg void OnBnClickedCdBtn();
	afx_msg void OnBnClickedTddBtn();
	afx_msg void OnBnClickedDataBtn();
	afx_msg void OnBnClickedMainframe();
};
