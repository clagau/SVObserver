//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDisplayImageSelect
//* .File Name       : $Workfile:   SVDisplayImageSelect.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:14:22  $
//******************************************************************************

#pragma once

class SVDisplayImageSelect : public CDialog
{
public:
	SVDisplayImageSelect(uint32_t inspectionId, uint32_t imageId, CWnd* pParent = nullptr );   // standard constructor
	virtual ~SVDisplayImageSelect();


	//{{AFX_DATA(SVDisplayImageSelect)
	enum { IDD = IDD_SELECT_DISPLAY_IMAGE };
	CListBox	m_ImageSelectList;
	//}}AFX_DATA

	uint32_t getImageId() const { return m_imageId; };

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVDisplayImageSelect)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL


protected:

	// Generated message map functions
	//{{AFX_MSG(SVDisplayImageSelect)
	virtual void OnOK() override;
	virtual void OnCancel() override;
	virtual BOOL OnInitDialog() override;
	//}}AFX_MSG
	afx_msg void OnReloadImageList();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* pMinMaxInfo);
	DECLARE_MESSAGE_MAP()
private:
	/// Add the current inspection's name to the default window's title.
	/// Title format: "Select Display Image - [Inspection name]"
	/// \returns void
	void SetTitle();

	void LoadImageList();

	CString m_WildcardPattern;
	uint32_t m_inspectionId;
	uint32_t m_imageId;
	CSize m_minSize {0,0};
};

