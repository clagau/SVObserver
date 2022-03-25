//*****************************************************************************
// \copyright COPYRIGHT (c) 2020,2020 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file SVOFileExecutionConfigDlg.h
/// This is the class for the General Tab in the edit config dialog
//******************************************************************************

#pragma once
class SVOConfigAssistantDlg;

class SVOFileExecutionConfigDlg : public CPropertyPage
{
public:
	SVOFileExecutionConfigDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~SVOFileExecutionConfigDlg();
    void SetupList();

	enum { IDD = IDD_DLG_PROPPAGE_FILE_EXEC};
	CButton	m_btnAddPre;
	CButton m_btnAddPost;
	CButton m_btnRemovePre;
	CButton m_btnRemovePost;

	CString m_edtPrePath;
	CString m_edtPostPath;

	enum ProcessPosition
	{
		PreRun = 0,
		PostRun = 1,
	};

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support


protected:
	
	virtual BOOL OnInitDialog() override;
	afx_msg void OnBtnAddPre();
	afx_msg void OnBtnAddPost();
	afx_msg void OnBtnRemovePre();
	afx_msg void OnBtnRemovePost();
	afx_msg void OnEditPreTextfield();
	afx_msg void OnEditPostTextfield();
	void addExecutionFileToConfig(std::string path, ProcessPosition preOrPost);
	void clearTextfieldAndRemoveFromConfig(ProcessPosition preOrPost);

	DECLARE_MESSAGE_MAP()
private:
    SVOConfigAssistantDlg *m_pParent;
};

