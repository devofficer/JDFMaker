
// JDFMakerDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "JDFMaker.h"
#include "JDFMakerDlg.h"
#include "afxdialogex.h"
#include "ReadDirectoryChanges.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CJDFMakerDlg dialog

#define _CRT_SECURE_NO_WARNINGS

CJDFMakerDlg::CJDFMakerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_JDFMAKER_DIALOG, pParent)
	, m_szTarPath(_T(""))
	, m_strTDDPath(_T(""))
	, m_szSrcPath(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

LPCWSTR ExplainAction(DWORD dwAction)
{
	switch (dwAction)
	{
	case FILE_ACTION_ADDED:
		return L"Added";
	case FILE_ACTION_REMOVED:
		return L"Deleted";
	case FILE_ACTION_MODIFIED:
		return L"Modified";
	case FILE_ACTION_RENAMED_OLD_NAME:
		return L"Renamed From";
	case FILE_ACTION_RENAMED_NEW_NAME:
		return L"Renamed To";
	default:
		return L"BAD DATA";
	}
}

UINT WatchDirThread(LPVOID pParam)
{
	CJDFMakerDlg* pDlg = (CJDFMakerDlg*)pParam;

	if (pDlg == NULL ||
		!pDlg->IsKindOf(RUNTIME_CLASS(CJDFMakerDlg)))
		return 1;   // if pObject is not valid

	const DWORD dwNotificationFlags = FILE_NOTIFY_CHANGE_DIR_NAME;

	// Create the monitor and add two directories.
	CReadDirectoryChanges changes;
	changes.AddDirectory(pDlg->GetSourceDir(), false, dwNotificationFlags);

	bool bTerminate = false;

	while (!bTerminate)
	{
		DWORD rc = ::WaitForSingleObject(changes.GetWaitHandle(), INFINITE);

		// We've received a notification in the queue.
		if (rc == WAIT_OBJECT_0)
		{
			DWORD dwAction;
			CStringW wstrFilename;
			if (changes.CheckOverflow())
				AfxMessageBox(L"Queue overflowed.\n");
			else
			{
				changes.Pop(dwAction, wstrFilename);

				if (dwAction == FILE_ACTION_ADDED) {
					Sleep(2000);
					pDlg->GenerateJDF(wstrFilename);
				}
			}
		}
	}

	// Just for sample purposes. The destructor will
	// call Terminate() automatically.
	changes.Terminate();

	return EXIT_SUCCESS;
}

void CJDFMakerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_CD_EDIT, m_szTarPath);
	DDX_Text(pDX, IDC_DATA_EDIT, m_szSrcPath);
}

BEGIN_MESSAGE_MAP(CJDFMakerDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_CD_BTN, &CJDFMakerDlg::OnBnClickedCdBtn)
	ON_BN_CLICKED(IDC_DATA_BTN, &CJDFMakerDlg::OnBnClickedDataBtn)
	ON_BN_CLICKED(IDR_MAINFRAME, &CJDFMakerDlg::OnBnClickedMainframe)
END_MESSAGE_MAP()


// CJDFMakerDlg message handlers

BOOL CJDFMakerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	CRegKey registryKey;
	registryKey.Open(HKEY_CURRENT_USER, _T("Software\\JDFAutomator"), KEY_READ);

	TCHAR szSrcPath[MAX_PATH] = _T("");
	TCHAR szTarPath[MAX_PATH] = _T("");
	ULONG nSrcPath = _countof(szSrcPath);
	ULONG nTarPath = _countof(szTarPath);
	registryKey.QueryStringValue(_T("SourcePath"), szSrcPath, &nSrcPath);
	m_szSrcPath = CString(szSrcPath);

	registryKey.QueryStringValue(_T("TargetPath"), szTarPath, &nTarPath);
	m_szTarPath = CString(szTarPath);
	
	registryKey.Close();
	UpdateData(false);

	AfxBeginThread(WatchDirThread, this);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CJDFMakerDlg::OnPaint()
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
HCURSOR CJDFMakerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CJDFMakerDlg::OnBnClickedCdBtn()
{
	// TODO: Add your control notification handler code here
	CFolderPickerDialog dlg;
	
	if (dlg.DoModal() == IDOK) {
		m_szTarPath = dlg.GetPathName();
		CRegKey registryKey;
		registryKey.Open(HKEY_CURRENT_USER, _T("Software\\JDFAutomator"), KEY_WRITE);
		registryKey.SetStringValue(_T("TargetPath"), m_szTarPath);
		registryKey.Close();
		UpdateData(false);
	}
}

void CJDFMakerDlg::OnBnClickedTddBtn()
{
	// TODO: Add your control notification handler code here
	CFileDialog dlg(true);

	if (dlg.DoModal() == IDOK) {
		m_strTDDPath = dlg.GetPathName();
		UpdateData(false);
	}
}


void CJDFMakerDlg::OnBnClickedDataBtn()
{
	// TODO: Add your control notification handler code here
	CFolderPickerDialog dlg;

	if (dlg.DoModal() == IDOK) {
		m_szSrcPath = dlg.GetPathName();
		CRegKey registryKey;
		registryKey.Open(HKEY_CURRENT_USER, _T("Software\\JDFAutomator"), KEY_WRITE);
		registryKey.SetStringValue(_T("SourcePath"), m_szSrcPath);
		registryKey.Close();

		UpdateData(false);
	}
}

void CJDFMakerDlg::OnBnClickedMainframe()
{
	GenerateJDF(m_szSrcPath);
}


void CJDFMakerDlg::GenerateJDF(LPCTSTR sourcePath)
{
	CString szSourcePath(sourcePath);
	int nTokenPos = 0, nTokenCnt = 0;
	int nSlashPos = szSourcePath.ReverseFind('\\');
	CString srcFilename = szSourcePath.Mid(nSlashPos + 1);
	CString year, month, date;
	CString name;

	std::regex rx("([A-Z_]*)_(\\d{4}_\\d{2}_\\d{2})");
	std::smatch mr;
	std::string str = CT2A(srcFilename.GetString());
	
	if (std::regex_search(str, mr, rx) == FALSE) {
		MessageBox(L"Source folder name does not match the pattern.");
		return;
	}
	
	std::regex dateRegx("(\\d{4})_(\\d{2})_(\\d{2})");
	std::smatch dateMr;
	std::string dateToken = mr[2].str();

	std::regex_search(dateToken, dateMr, dateRegx);

	year = CString(dateMr[1].str().c_str());
	month = CString(dateMr[2].str().c_str());
	date = CString(dateMr[3].str().c_str());
	name = CString(mr[1].str().c_str());
	name.Replace(_T("_"), _T(" "));

	int ts = CTime::GetCurrentTime().GetTime();
	char printFile[MAX_PATH];
	sprintf_s(printFile, "print_data\\%d.dat", ts);

	FILE* fpPrintData = 0;
	fopen_s(&fpPrintData, printFile, "w");
	fprintf(fpPrintData, "Name=%S\n", (LPCTSTR)name);
	fprintf(fpPrintData, "Date=%S/%S/%S", (LPCTSTR)date, (LPCTSTR)month, (LPCTSTR)year);
	fclose(fpPrintData);

	const char JDFTemplate[] = "JOB_ID=%d\n\
PUBLISHER=PP-100II 1\n\
COPIES=1\n\
OUT_STACKER=2\n\
DISC_TYPE=DVD\n\
FORMAT=UDF102\n\
DATA=%S\n\
LABEL=%S\\label.tdd\n\
REPLACE_FIELD=%S\\print_data\\%d.dat";

	TCHAR currentDir[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, currentDir);

	CString srcFilePath, tarPath;
	srcFilePath.Format(L"%s\\temp.jdf", currentDir);
	tarPath.Format(L"%s\\%s_%S_%d.jdf", (LPCTSTR)m_szTarPath, (LPCTSTR)name, dateToken.c_str(), ts);

	FILE* fpJDFTemp = 0;
	fopen_s(&fpJDFTemp, "temp.jdf", "w");
	fprintf(fpJDFTemp, JDFTemplate, ts, sourcePath, currentDir, currentDir, ts);
	fclose(fpJDFTemp);


	if (MoveFile(srcFilePath, tarPath))
		return;
}

CString CJDFMakerDlg::GetSourceDir()
{
	return m_szSrcPath;
}