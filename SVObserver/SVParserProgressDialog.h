//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVParserProgressDialog
//* .File Name       : $Workfile:   SVParserProgressDialog.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   25 Jun 2014 11:57:24  $
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <map>
#include "SVContainerLibrary/SVVector.h"
#include "SVProgressDialog.h"
#include "SVSystemLibrary/SVLockableClass.h"
#include "SVUtilityLibrary\SVSharedPtr.h"

#pragma endregion Includes

class SVObjectScriptParserClass;

typedef SVSharedPtr<CProgressCtrl> ProgressCtrlSharedPtr;
typedef SVSharedPtr<CStatic> StaticSharedPtr;

struct SVParserProgressControlStruct
{
	BOOL bValidate;
	GUID OwnerGuid;
	SVObjectScriptParserClass* pParser;
	ProgressCtrlSharedPtr pProgressCtrl;
	StaticSharedPtr pStaticTextCtrl;
	std::string Text;
	int TotalSize;

	SVParserProgressControlStruct()
	{
		bValidate = FALSE;
		OwnerGuid = GUID_NULL;
		pParser = nullptr;
		pProgressCtrl = nullptr;
		pStaticTextCtrl = nullptr;
		TotalSize = 0;
	}
};

typedef std::map< int, SVParserProgressControlStruct > SVParserControlList;

class SVParserProgressDialog : public SVProgressDialog
{
// Construction
public:
	SVParserProgressDialog(LPCTSTR title, CWnd* pParent = nullptr);   // standard constructor
	virtual ~SVParserProgressDialog();

	BOOL AddParser( unsigned long parserHandle, SVObjectScriptParserClass* pParser );
	
	CProgressCtrl* GetProgressControl( unsigned long parserHandle );
	CStatic* GetTextControl( unsigned long parserHandle );
	bool CheckComplete();

protected:
	SVParserProgressControlStruct& GetParserControl(unsigned long parserHandle);
	SVParserProgressControlStruct& GetFirstAvailableParserControl();

	void resizeAndMoveDialog( CRect& RRect, int cnt );
	void addScrollBar( int visibleWidth, int maxWidth );

protected:
	SVParserControlList m_parserControlList;

// msvEndDialogLock - When the parsing threads end, they all call
// OnEndProgressDialog (). OnEndProgressDialog () checks to see whether or
// not all other threads are terminated, if so then the call does "final"
// clean up.  More than one thread can call this function, and a race 
// condition can exist where multiple threads are doing final clean up.
// This lock variable is used by OnEndProgressDialog () to ensure that only 
// one thread at a time may use it.
	SvSyl::SVLockableClass      msvEndDialogLock;

	// Implementation
protected:
	void SetProgressLocations();

	// Generated message map functions
	//{{AFX_MSG(SVParserProgressDialog)
	virtual BOOL OnInitDialog() override;
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	afx_msg LRESULT OnUpdateProgress( WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEndProgressDialog( WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

private:
	int m_originalWidth;
	BOOL m_scrollBarShown;
};

