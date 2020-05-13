
// RemoveCommentaryDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "RemoveCommentary.h"
#include "RemoveCommentaryDlg.h"
#include "afxdialogex.h"
#include "XRemoveCommentary.h"
#include <shlwapi.h>
#pragma comment(lib,"Shlwapi.lib")

#pragma warning(disable:4996)

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx {
public:
    CAboutDlg();

    // Dialog Data
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_ABOUTBOX };
#endif

protected:
    virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support

    // Implementation
protected:
    DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX) {
}

void CAboutDlg::DoDataExchange(CDataExchange *pDX) {
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CRemoveCommentaryDlg dialog



CRemoveCommentaryDlg::CRemoveCommentaryDlg(CWnd *pParent /*=nullptr*/)
    : CDialogEx(IDD_REMOVECOMMENTARY_DIALOG, pParent) {
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CRemoveCommentaryDlg::DoDataExchange(CDataExchange *pDX) {
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CRemoveCommentaryDlg, CDialogEx)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_BUTTON1, &CRemoveCommentaryDlg::OnBnClickedButton1)
    ON_EN_CHANGE(IDC_EDIT1, &CRemoveCommentaryDlg::OnEnChangeEdit1)
    ON_BN_CLICKED(IDC_BUTTON2, &CRemoveCommentaryDlg::OnBnClickedButton2)
END_MESSAGE_MAP()

CProgressCtrl* myProCtrl2;

// CRemoveCommentaryDlg message handlers

BOOL CRemoveCommentaryDlg::OnInitDialog() {
    CDialogEx::OnInitDialog();

    // Add "About..." menu item to system menu.

    // IDM_ABOUTBOX must be in the system command range.
    ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
    ASSERT(IDM_ABOUTBOX < 0xF000);

    CMenu *pSysMenu = GetSystemMenu(FALSE);
    if (pSysMenu != nullptr) {
        BOOL bNameValid;
        CString strAboutMenu;
        bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
        ASSERT(bNameValid);
        if (!strAboutMenu.IsEmpty()) {
            pSysMenu->AppendMenu(MF_SEPARATOR);
            pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
        }
    }

    // Set the icon for this dialog.  The framework does this automatically
    //  when the application's main window is not a dialog
    SetIcon(m_hIcon, TRUE);			// Set big icon
    SetIcon(m_hIcon, FALSE);		// Set small icon

    // TODO: Add extra initialization here
    CButton *pBtn = (CButton *)GetDlgItem(IDC_CHECK1);
    pBtn->SetCheck(1);
    myProCtrl2 = (CProgressCtrl*)GetDlgItem(IDC_PROGRESS1);
    myProCtrl2->SetRange(0, 100);
    return TRUE;  // return TRUE  unless you set the focus to a control
}

void CRemoveCommentaryDlg::OnSysCommand(UINT nID, LPARAM lParam) {
    if ((nID & 0xFFF0) == IDM_ABOUTBOX) {
        CAboutDlg dlgAbout;
        dlgAbout.DoModal();
    } else {
        CDialogEx::OnSysCommand(nID, lParam);
    }
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CRemoveCommentaryDlg::OnPaint() {
    if (IsIconic()) {
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
    } else {
        CDialogEx::OnPaint();
    }
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CRemoveCommentaryDlg::OnQueryDragIcon() {
    return static_cast<HCURSOR>(m_hIcon);
}

CString dir;

void CRemoveCommentaryDlg::OnBnClickedButton1() {
    // TODO: Add your control notification handler code here
    CFolderPickerDialog fd(NULL, 0, this, 0);
    if (fd.DoModal() == IDOK) {
        dir = fd.GetPathName();
        GetDlgItem(IDC_EDIT1)->SetWindowText(dir);
    }
}


void CRemoveCommentaryDlg::OnEnChangeEdit1() {
    // TODO:  If this is a RICHEDIT control, the control will not
    // send this notification unless you override the CDialogEx::OnInitDialog()
    // function and call CRichEditCtrl().SetEventMask()
    // with the ENM_CHANGE flag ORed into the mask.

    // TODO:  Add your control notification handler code here
    GetDlgItem(IDC_EDIT1)->GetWindowText(dir);
}

CString GetFEName(CString csFileFullName) {
    int nPos = csFileFullName.ReverseFind('.');
    CString  csFileExtName = csFileFullName.Right(csFileFullName.GetLength() - nPos - 1);
    return csFileExtName;
}

char *wcharTochar(const wchar_t *wchar) {
    size_t length = wcslen(wchar);
    char *chr = new char[length + 1];
    WideCharToMultiByte(CP_ACP, 0, wchar, -1, chr, length, NULL, NULL);
    return chr;
}

void CRemoveCommentaryDlg::recurse(vector<CString> &vFilePathList, CString strDir) {
    CFileFind finder;
    BOOL isNotEmpty = finder.FindFile(strDir + _T("*.*"));
    while (isNotEmpty) {
        isNotEmpty = finder.FindNextFile();
        CString filename = finder.GetFilePath();
        if (!(finder.IsDirectory())) {
            CString ext = GetFEName(filename);
            if ((ext == "cpp" || ext == "c" || ext == "cs" || ext == "java" || ext == "h" || ext == "hpp" || ext == "inl")) vFilePathList.push_back(filename);
        } else {
            if ( !(finder.IsDots() || finder.IsHidden() || finder.IsSystem() || finder.IsTemporary() || finder.IsReadOnly()) ) {
                recurse(vFilePathList, filename + _T("/"));
            }
        }
    }
}

BOOL IsDirExist(const CString &csDir) {
    DWORD dwAttrib = GetFileAttributes(csDir);
    return INVALID_FILE_ATTRIBUTES != dwAttrib && 0 != (dwAttrib & FILE_ATTRIBUTE_DIRECTORY);
}

void CRemoveCommentaryDlg::OnBnClickedButton2() {
    // TODO: Add your control notification handler code here
    if (dir.IsEmpty() || !IsDirExist(dir)) {
        int nButtonPressed = 0;
        TaskDialog(m_hWnd, NULL,
                   L"Warning",
                   L"Please choose a valid folder before recursing!",
                   NULL,
                   TDCBF_OK_BUTTON,
                   TD_WARNING_ICON,
                   &nButtonPressed);
    } else {
        vector<CString> v;
        recurse(v, dir);
        CString msg;
        msg.Format(L"Total %d file", v.size());
        for (int i = 0; i < v.size(); i++) {
            CString yourpath = v[i];
            XRemoveCommentary temp;
            temp.removeCommentary(wcharTochar(yourpath));
            CButton *pBtn = (CButton *)GetDlgItem(IDC_CHECK1);
            int state = pBtn->GetCheck();
            if (state == 0) {
                CString cotr = yourpath + L".tmp";
                CString cotr1 = yourpath + L".tnp";
                CString cotr2 = yourpath + L".top";
                if (PathFileExists(cotr)) DeleteFile(cotr);
                if (PathFileExists(cotr1)) DeleteFile(cotr1);
                if (PathFileExists(cotr2)) DeleteFile(cotr2);
            }
            int percent = (int)(((double)(i + 1.0f) / (double)v.size()) * 100.0f);
            myProCtrl2->SetPos(percent);
            GetDlgItem(IDC_SHOWF)->SetWindowText(L"Processed file : " + yourpath);
        }
        int nButtonPressed = 0;
        TaskDialog(m_hWnd, NULL,
            L"Succeed",
            L"All comments have been removed from the source code",
            NULL,
            TDCBF_OK_BUTTON,
            TD_INFORMATION_ICON,
            &nButtonPressed);
        GetDlgItem(IDC_SHOWF)->SetWindowText(L"Process finished!");
    }
}
