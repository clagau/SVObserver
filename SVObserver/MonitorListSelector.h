//*****************************************************************************
/// \copyright (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file MonitorListSelector.h
/// All Rights Reserved 
//*****************************************************************************
/// this class replaces the class  MonitorListSheet
/// to use the ObjectSelector to  create Monitorlist 
/// it uses the old interface from MonitorlistSheet which is not suited for big Monitorlist
/// in a later version RemoteMonitorNamedList and RemoteMonitorList should be replaced by MLMoniotorListcpy
//******************************************************************************
#pragma once
#pragma region Includes
#include "RemoteMonitorNamedList.h"
#include "SVUtilityLibrary/NameSelectionList.h"
#pragma endregion Includes
#include "ObjectSelectorLibrary/ObjectTreeGenerator.h"


#pragma region Enum
enum MonitorListType
{
	PRODUCT_OBJECT_LIST = 0,
	FAIL_STATUS_LIST = 1,
	REJECT_CONDITION_LIST = 2,
};
#pragma endregion Enum

class MonitorlistSelector
{
public:
	MonitorlistSelector(MonitorListType eListType, bool bImageTab, const RemoteMonitorNamedList& rList, CWnd* pParentWnd = nullptr);
	virtual ~MonitorlistSelector();
	
	///Display the object Selector for the Monitorlist
	int  DisplayDialog();
	const RemoteMonitorNamedList& GetMonitorList() const;

private:
	/// Builds the main caption for the object selector
	void BuildCaption(SVString &rCaption);
	
	///fills m_CheckItems with the objects in the monitorobjectlist from m_MonitorList
	void BuildCheckItems();
	
	///Gets a Monitorobjectlist for the selected items 
	MonitoredObjectList  GetMonitoredObjectList(const SvOsl::SelectorItemVector& rList);

	/// insert the selected Item to the monitorlist
	void InsertSelectedListToMoniterList();
	

private:	
	SvOsl::SelectorItemVector m_List;			//The selected list
	bool m_bImage;
	MonitorListType m_eListType;
	RemoteMonitorNamedList m_MonitorList;
	CWnd*  m_ParentWindow;
	SVStringSet m_CheckItems; 
};

