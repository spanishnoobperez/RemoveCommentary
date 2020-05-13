
// RemoveCommentaryDlg.h : header file
//

#pragma once

#include <vector>
using namespace std;


// CRemoveCommentaryDlg dialog
class CRemoveCommentaryDlg : public CDialogEx {
    // Construction
public:
    CRemoveCommentaryDlg(CWnd *pParent = nullptr);	// standard constructor

    // Dialog Data
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_REMOVECOMMENTARY_DIALOG };
#endif

protected:
    virtual void DoDataExchange(CDataExchange *pDX);	// DDX/DDV support


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
    afx_msg void OnBnClickedButton1();
    afx_msg void OnEnChangeEdit1();
    afx_msg void OnBnClickedButton2();
    void recurse(vector<CString> &vFilePathList, CString strDir);
};
