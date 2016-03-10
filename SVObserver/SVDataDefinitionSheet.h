//******************************************************************************
//* COPYRIGHT (c) 2008 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDataDefinitionSheet
//* .File Name       : $Workfile:   SVDataDefinitionSheet.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:07:34  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "ISVCancel.h"
#include "SVUtilityLibrary\SVGUID.h"
#include "SVUtilityLibrary\SVString.h"
#include "ObjectSelectorLibrary\SelectorItemVector.h"
#pragma endregion Includes

class SelectedObjectsPage;

class SVDataDefinitionSheet : public CPropertySheet, public ISVCancel
{
	DECLARE_DYNAMIC(SVDataDefinitionSheet)

#pragma region Constructor
public:
	SVDataDefinitionSheet( LPCTSTR pszCaption, const SVString& rInspectionName, const SVGUID& rInspectionID, CWnd* pParentWnd = nullptr, UINT iSelectPage = 0 );
	virtual ~SVDataDefinitionSheet();
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
	virtual BOOL OnInitDialog();
	void OnOK();

	bool setChangedData( const SelectedObjectsPage* const pPage );
#pragma endregion Private Methods

#pragma region Member variables
private:
	SVString m_InspectionName;				//Name of the Inspection
	SVGUID m_InspectionID;					//UniqueID of the Inspection
	SvOsl::SelectorItemVector m_ValueList;	//The selected value list 
	SvOsl::SelectorItemVector m_ImageList;	//The selected image list
#pragma endregion Member variables
};

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVDataDefinitionSheet.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 10:07:34   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   07 Mar 2008 09:22:34   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  616
 * SCR Title:  Add Server-side Configuration Data Def Lists to the SVObserver program
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   New Property Sheet for Data and image Definition lists
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
