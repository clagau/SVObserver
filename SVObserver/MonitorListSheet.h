//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidendader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : MonitorListSheet
//* .File Name       : $Workfile:   MonitorListSheet.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   17 Apr 2014 16:24:06  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVPropertySheetCancelImpl.h"
#include "RemoteMonitorNamedList.h"
#include "SVUtilityLibrary/NameSelectionList.h"
#pragma endregion Includes

#pragma region Enum
enum MonitorListType
{
	PRODUCT_OBJECT_LIST   = 0,
	FAIL_STATUS_LIST	  = 1,
	REJECT_CONDITION_LIST = 2,
};
#pragma endregion Enum

class MonitorListSheet : public CPropertySheet, public ISVCancel//, public SVPropertySheetCancelImpl
{
	DECLARE_DYNAMIC(MonitorListSheet)
#pragma region Public
// Construction
public:
	MonitorListSheet( LPCTSTR pszCaption, MonitorListType eListType, bool bImageTab, const RemoteMonitorNamedList& rList, CWnd* pParentWnd = nullptr, UINT iSelectPage = 0 );
	virtual ~MonitorListSheet();

	const RemoteMonitorNamedList& GetMonitorList() const;
	HRESULT CreatePages(bool bImageTab);

	// ISVCancel
	virtual bool CanCancel();
	virtual HRESULT GetCancelData(SVCancelData*& rpData);
	virtual HRESULT SetCancelData(SVCancelData* pData);

// Attributes
	CString    m_sListName;
	MonitorListType m_eListType;
// Operations

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(MonitorListSheet)
public:
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL
	void OnOK();
#pragma endregion Public

#pragma region Protected
	RemoteMonitorNamedList m_MonitorList;
	// Generated message map functions
protected:
	//{{AFX_MSG(MonitorListSheet)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	MonitoredObjectList GetMonitoredObjectList(const SvUl::NameSelectionList& rList);
	void SetMonitoredValues(MonitorListType eListType, const MonitoredObjectList& rList);
	void SetMonitoredImages(MonitorListType eListType, const MonitoredObjectList& rList);

	void DestroyPages();
#pragma endregion Protected
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

