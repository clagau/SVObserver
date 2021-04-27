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
#include "Definitions/StringTypeDef.h"
#include "SVProtoBuf/SVO-Enum.h"
#pragma endregion Includes

class SelectedObjectsPage;
class SVIPDoc;

namespace SvPb
{
class TreeItem;
}

class SVDataDefinitionSheet : public CPropertySheet
{
#pragma region Constructor
public:
	SVDataDefinitionSheet(SVIPDoc* pDoc, LPCTSTR pszCaption, const std::string& rInspectionName, uint32_t inspectionID, CWnd* pParentWnd = nullptr, UINT iSelectPage = 0);
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

	void initSelectedList(SvDef::StringVector* pList, UINT Attribute, SvPb::ObjectSelectorType type );
	bool setChangedData( SelectedObjectsPage* const pPage );
	void setAttributes( const SvDef::StringVector& rList, UINT Attribute, bool Clear ) const;
	void insertObjectsToList(const SvPb::TreeItem& rTree, SvDef::StringVector* pList);
#pragma endregion Private Methods

#pragma region Member variables
private:
	SVIPDoc* m_pDoc;
	std::string m_InspectionName;		//Name of the Inspection
	uint32_t m_InspectionID;				//UniqueID of the Inspection
	SvDef::StringVector m_ValueList;	//The selected value list 
	SvDef::StringVector m_ImageList;	//The selected image list
#pragma endregion Member variables
};

