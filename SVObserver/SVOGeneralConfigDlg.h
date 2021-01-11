//*****************************************************************************
// \copyright COPYRIGHT (c) 2020,2020 by Seidenader Maschinenbau GmbH. All Rights Reserved
/// \file SVOGeneralConfigDlg.h
/// This is the class for the General Tab in the edit config dialog
//******************************************************************************

#pragma once
class SVOConfigAssistantDlg;

class SVOGeneralConfigDlg : public CPropertyPage
{
public:
	SVOGeneralConfigDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~SVOGeneralConfigDlg();
    void SetupList();

	enum { IDD = IDD_DLG_PROPPAGE_AA_GENERAL};
	CButton	m_btnAddPre;
	CButton m_btnAddPost;
	CButton m_btnRemovePre;
	CButton m_btnRemovePost;

	CString m_edtPrePath;
	CString m_edtPostPath;
	CString m_messageField;

	enum ProcessPosition
	{
		PreBatch = 0,
		PostBatch = 1,
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
	void copyAndAddToConfig(std::string path, int preOrPost);
	void clearTextfieldAndRemoveFromConfig(int preOrPost);

	DECLARE_MESSAGE_MAP()
private:
    SVOConfigAssistantDlg *m_pParent;
};

