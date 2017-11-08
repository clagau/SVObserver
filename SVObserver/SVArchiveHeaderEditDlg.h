//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVArchiveHeaderEditDlg
//* .File Name       : $Workfile:   SVArchiveHeaderEditDlg.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   11 Nov 2013 09:25:08  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "afxcmn.h"
#include "SVMFCControls\SVListCtrl.h"
#include "Definitions/StringTypeDef.h"
#include "SVGlobal.h"
#pragma endregion

class SVArchiveHeaderEditDlg : public CDialog
{
	DECLARE_DYNAMIC(SVArchiveHeaderEditDlg)
#pragma region Public
public:
	SVArchiveHeaderEditDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~SVArchiveHeaderEditDlg();

// Dialog Data
	enum { IDD = IDD_HEADER_DIALOG };
	bool SetValues( const SvDef::StringPairVector& p_astrValues);
	bool GetValues(SvDef::StringPairVector& p_astrValues) const;
#pragma endregion

#pragma region Protected
protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLvnItemchangedStringListEdit(NMHDR *pNMHDR, LRESULT *pResult);

	DECLARE_MESSAGE_MAP()

	virtual BOOL OnInitDialog() override;
	virtual void OnOK() override;
	void HeaderListChangeSize(int cx, int cy);
	void DisplaySelectedText( );
#pragma endregion

#pragma region Private
private:
	SvDef::StringPairVector m_Strings;
	CEdit* m_pEdit;
	CRect m_PreviousClient;
	CRect m_OriginalRec;
	SvMc::SVListCtrl m_HeaderListCtrl;
#pragma endregion

};

