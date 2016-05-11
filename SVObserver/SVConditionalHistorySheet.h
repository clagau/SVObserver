//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVConditionalHistorySheet
//* .File Name       : $Workfile:   SVConditionalHistorySheet.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:02:12  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "ISVCancel.h"
#include "ObjectSelectorLibrary\SelectorItemVector.h"
#pragma endregion Includes

class SVInspectionProcess;
class SelectedObjectsPage;
class SVIPDoc;

class SVConditionalHistorySheet : public CPropertySheet, public ISVCancel
{
	DECLARE_DYNAMIC(SVConditionalHistorySheet)
#pragma region Constructor
public:
	SVConditionalHistorySheet( SVIPDoc* pDoc, LPCTSTR pszCaption, SVInspectionProcess& rInspection, CWnd* pParentWnd = nullptr, UINT iSelectPage = 0 );
	virtual ~SVConditionalHistorySheet();
#pragma endregion Constructor

#pragma region Public Methods
public:
	// ISVCancel
	virtual bool CanCancel();
	virtual HRESULT GetCancelData(SVCancelData*& rpData);
	virtual HRESULT SetCancelData(SVCancelData* pData);
#pragma endregion Public Methods

#pragma region Private Methods
private:
	DECLARE_MESSAGE_MAP()

	HRESULT CreatePages();
	void DestroyPages();
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	void OnOK();

	bool setChangedData( SelectedObjectsPage* const pPage );
	SvOsl::SelectorItemVector ConvertList( const SVScalarValueVector& rSelectedList ) const;
	SVScalarValueVector ConvertList( const SvOsl::SelectorItemVector& rSelectedList ) const;
#pragma endregion Private Methods

#pragma region Member variables
private:
	SVIPDoc* m_pDoc;
	SVInspectionProcess& m_rInspection;		//Reference to the respective inspection object
	SvOsl::SelectorItemVector m_ValueList;	//List of value objects
	SvOsl::SelectorItemVector m_ImageList;	//List of image objects
	SvOsl::SelectorItemVector m_Conditionals; //List of conditional objects
	CEdit m_MaxHistoryCtrl;					//Maximum history control
	CStatic m_MaxHistoryLabel;				//Maximum history label
	CButton m_OverwriteCtrl;				//Overwrite control
	int  m_MaxHistory;						//Maximum history number
	BOOL m_Overwrite;						//Flag to overwrite
	int  m_MaxHistoryOriginal;				//Maximum history number original
	BOOL m_OverwriteOriginal;				//Flag to overwrite original 
#pragma endregion Member variables
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

