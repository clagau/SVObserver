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
#include "SVUtilityLibrary\SVGUID.h"

#include "SVContainerLibrary/SelectorItem.h"
#pragma endregion Includes

class SelectedObjectsPage;
class SVIPDoc;

class SVDataDefinitionSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(SVDataDefinitionSheet)

#pragma region Constructor
public:
	SVDataDefinitionSheet(SVIPDoc* pDoc, LPCTSTR pszCaption, const std::string& rInspectionName, const SVGUID& rInspectionID, CWnd* pParentWnd = nullptr, UINT iSelectPage = 0 );
	virtual ~SVDataDefinitionSheet();
#pragma endregion Constructor

#pragma region Public Methods
public:
#pragma endregion Public Methods

#pragma region Private Methods
private:
	DECLARE_MESSAGE_MAP()

	HRESULT CreatePages();
	void DestroyPages();
	void OnOK();

	void initSelectedList( SvCl::SelectorItemVector* pList, UINT Attribute );
	bool setChangedData( SelectedObjectsPage* const pPage );
	void setAttributes( const SvCl::SelectorItemVector& rList, UINT Attribute, bool Clear ) const;
#pragma endregion Private Methods

#pragma region Member variables
private:
	SVIPDoc* m_pDoc;
	std::string m_InspectionName;				//Name of the Inspection
	SVGUID m_InspectionID;					//UniqueID of the Inspection
	SvCl::SelectorItemVector m_ValueList;	//The selected value list 
	SvCl::SelectorItemVector m_ImageList;	//The selected image list
#pragma endregion Member variables
};

