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
#include "SVGlobal.h"
#pragma endregion

class SVArchiveHeaderEditDlg : public CDialog
{
	DECLARE_DYNAMIC(SVArchiveHeaderEditDlg)
#pragma region Public
public:
	SVArchiveHeaderEditDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~SVArchiveHeaderEditDlg();

// Dialog Data
	enum { IDD = IDD_HEADER_DIALOG };
	bool SetValues( const StringPairVect& p_astrValues);
	bool GetValues( StringPairVect& p_astrValues) const;
#pragma endregion

#pragma region Protected
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLvnItemchangedStringListEdit(NMHDR *pNMHDR, LRESULT *pResult);

	DECLARE_MESSAGE_MAP()

	virtual BOOL OnInitDialog();
	virtual void OnOK();
	void HeaderListChangeSize(int cx, int cy);
	void DisplaySelectedText( );
#pragma endregion

#pragma region Private
private:
	StringPairVect m_Strings;
	CEdit* m_pEdit;
	CRect m_PreviousClient;
	CRect m_OriginalRec;
	SvMc::SVListCtrl m_HeaderListCtrl;
#pragma endregion

};

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVArchiveHeaderEditDlg.h_v  $
 * 
 *    Rev 1.0   11 Nov 2013 09:25:08   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  872
 * SCR Title:  Add Archive Tool Headers to Archive File
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added dialog to edit archive tool headers
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
