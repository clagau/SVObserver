//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDisplayImageSelect
//* .File Name       : $Workfile:   SVDisplayImageSelect.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:14:22  $
//******************************************************************************

#pragma once

#pragma region Includes
#pragma endregion Includes

namespace SvIe
{
class SVImageClass;
}
class SVIPDoc;

class SVDisplayImageSelect : public CDialog
{
public:
	SvIe::SVImageClass *m_pCurrentImage;
	SVIPDoc			 *m_pDoc;
	SVDisplayImageSelect(CWnd* pParent = nullptr);   // standard constructor
	virtual ~SVDisplayImageSelect();


	//{{AFX_DATA(SVDisplayImageSelect)
	enum { IDD = IDD_SELECT_DISPLAY_IMAGE };
	CListBox	m_ImageSelectList;
	//}}AFX_DATA

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
	DECLARE_MESSAGE_MAP()
private:
	/// Add the current inspection's name to the default window's title.
	/// Title format: "Select Display Image - [Inspection name]"
	/// \returns void
	void SetTitle();
};

