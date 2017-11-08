//*****************************************************************************
/// \copyright (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file MonitorListSelector.cpp
/// All Rights Reserved 
//*****************************************************************************
/// this class replaces the class  MonitorListSheet
/// to use the ObjectSelector to  create Monitorlist 
/// it uses the old interface from MonitorlistSheet which is not suited for big Monitorlist
/// in a later version RemoteMonitorNamedList and RemoteMonitorList should be replaced by MLMoniotorListcpy
//******************************************************************************
#include "StdAfx.h"
#include "MonitorListSelector.h"
#include "SVConfigurationObject.h"
#include "SVObjectLibrary\SVObjectManagerClass.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "ObjectSelectorLibrary\SelectorItem.h"
#include "ObjectSelectorLibrary\ObjectTreeGenerator.h"
#include "SVOGui\TADialogTableParameterPage.h"
#include "SVOGui\NoSelector.h"
#include "SVOGui\ToolSetItemSelector.h"
#include "RemoteMonitorListHelper.h"
#include "GuiCommands\GetSelectorList.h"



MonitorlistSelector::MonitorlistSelector(MonitorListType eListType, bool bImage, const RemoteMonitorNamedList& rList, CWnd* pParentWnd)
	:m_bImage(bImage),
	m_eListType(eListType)
	, m_MonitorList(rList)
	, m_ParentWindow(pParentWnd)
{

}
MonitorlistSelector::~MonitorlistSelector()
{
}

void MonitorlistSelector::BuildCaption(std::string &rCaption)
{
	switch (m_eListType)
	{
	case	PRODUCT_OBJECT_LIST:
		if (m_bImage)
		{
			rCaption = _T("Edit ProductItemImageList");
		}
		else
		{
			rCaption = _T("Edit ProductItemValueList");
		}
		break;
	case	FAIL_STATUS_LIST:
	{
		rCaption = _T("Edit FailStatusList");
	}
	break;

	case REJECT_CONDITION_LIST:
	{
		rCaption = _T("Edit RejectconditionList");
	}
	break;
	}

	rCaption +=  _T(" for ");
	rCaption +=  m_MonitorList.GetName();
}

int  MonitorlistSelector::DisplayDialog()
{
	SVConfigurationObject* pConfig(nullptr);
	SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);
	if (!pConfig)
	{
		return -1;
	}
	SVPPQObject* pPPQ(nullptr);
	std::string ppqName = m_MonitorList.GetPPQName();
	pConfig->GetPPQByName(ppqName.c_str(), &pPPQ);
	if(!pPPQ)
	{ 
		return -1;
	}

	SvOsl::ObjectTreeGenerator::Instance().setSelectorType(SvOsl::ObjectTreeGenerator::SelectorTypeEnum::TypeMultipleObject);
	long numInspections(0);
	pPPQ->GetInspectionCount(numInspections);
	for (int inspIndex = 0; inspIndex < numInspections; inspIndex++)
	{
		SVInspectionProcess* pInspection(nullptr);
		pPPQ->GetInspection(inspIndex, pInspection);
		if (nullptr != pInspection)
		{
			if (m_bImage == TRUE && m_eListType == PRODUCT_OBJECT_LIST)
			{
				SvOsl::SelectorOptions BuildOptions(pInspection->GetUniqueObjectID(), SvDef::SV_ARCHIVABLE_IMAGE);
				SvOsl::ObjectTreeGenerator::Instance().BuildSelectableItems<SvOg::NoSelector, SvOg::NoSelector, SvOg::ToolSetItemSelector<>>(BuildOptions);
			}
			else if(m_eListType == PRODUCT_OBJECT_LIST)
			{
				SvOsl::SelectorOptions BuildOptions(pInspection->GetUniqueObjectID(), SvDef::SV_VIEWABLE);
				SvOsl::ObjectTreeGenerator::Instance().BuildSelectableItems<SvOg::NoSelector, SvOg::NoSelector, SvOg::ToolSetItemSelector<>>(BuildOptions);
			}
			else
			{
				SvOsl::SelectorOptions BuildOptions(pInspection->GetUniqueObjectID(), SvDef::SV_VIEWABLE);
				SvOsl::ObjectTreeGenerator::Instance().BuildSelectableItems<SvOg::NoSelector, SvOg::NoSelector, SvOg::ToolSetItemSelector<SvCmd::MLRejectValueFilterType>>(BuildOptions);
			}


			
		}
	}
	BuildCheckItems();
	SvOsl::ObjectTreeGenerator::Instance().setCheckItems(m_CheckItems);
	
	std::string Caption;
	BuildCaption(Caption);
	std::string Caption2 = (m_bImage == TRUE && m_eListType == PRODUCT_OBJECT_LIST) ? "Images" : "Value";
	std::string Filter = SvUl::LoadStdString(IDS_FILTER);
	INT_PTR Result = SvOsl::ObjectTreeGenerator::Instance().showDialog(Caption.c_str(), Caption2.c_str(), Filter.c_str());
	if (Result == IDOK)
	{
		m_List = SvOsl::ObjectTreeGenerator::Instance().getSelectedObjects();
		InsertSelectedListToMoniterList();
	}
	return static_cast<int>(Result);
}

void MonitorlistSelector::InsertSelectedListToMoniterList()
{
	switch (m_eListType)
	{
	case	PRODUCT_OBJECT_LIST:
	{
		if (m_bImage)
		{
			m_MonitorList.SetProductImagesList(GetMonitoredObjectList(m_List));
		}
		else
		{
			m_MonitorList.SetProductValuesList(GetMonitoredObjectList(m_List));
		}
		break;
	}
	case	FAIL_STATUS_LIST:
	{
		m_MonitorList.SetFailStatusList(GetMonitoredObjectList(m_List));
	}
	break;

	case REJECT_CONDITION_LIST:
	{
		m_MonitorList.SetRejectConditionList(GetMonitoredObjectList(m_List));
	}
	break;
	}
}

const RemoteMonitorNamedList& MonitorlistSelector::GetMonitorList() const
{
	return m_MonitorList;
}

void MonitorlistSelector::BuildCheckItems()
{
	m_CheckItems.clear();
	const MonitoredObjectList* pMonitorObjectList(nullptr);
	switch (m_eListType)
	{
	case	PRODUCT_OBJECT_LIST:
	{
		if (m_bImage)
		{
			pMonitorObjectList = &(m_MonitorList.GetProductImagesList());
		}
		else
		{
			pMonitorObjectList = &(m_MonitorList.GetProductValuesList());
		}
		break;
	}
	case	FAIL_STATUS_LIST:
	{
		pMonitorObjectList = &(m_MonitorList.GetFailStatusList());
	}
	break;

	case REJECT_CONDITION_LIST:
	{
		pMonitorObjectList = &(m_MonitorList.GetRejectConditionList());
	}
	break;
	}
	if (!pMonitorObjectList)
	{
		return;
	}
	for (auto& it : *pMonitorObjectList)
	{
		const std::string& name = RemoteMonitorListHelper::GetNameFromMonitoredObject(it);
		m_CheckItems.insert(name);
	}


}

MonitoredObjectList MonitorlistSelector::GetMonitoredObjectList(const SvOsl::SelectorItemVector& rList)
{
	MonitoredObjectList monitoredObjectList;
	for (SvOsl::SelectorItemVector::const_iterator it = rList.begin(); it != rList.end(); ++it)
	{
			const MonitoredObject& monitoredObj = RemoteMonitorListHelper::GetMonitoredObjectFromName(it->getLocation());
			if (!monitoredObj.guid.empty())
			{
				monitoredObjectList.push_back(monitoredObj);
			}
	}
	return monitoredObjectList;
}