//*****************************************************************************
/// \copyright (c) 2017,2017 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file MonitorListSelector.cpp
/// All Rights Reserved 
//*****************************************************************************
/// this class replaces the class  MonitorListSheet
/// to use the ObjectSelector to  create Monitorlist 
/// it uses the old interface from MonitorlistSheet which is not suited for big Monitorlist
/// in a later version RemoteMonitorNamedList and RemoteMonitorList should be replaced by MLMoniotorListcpy
//******************************************************************************

#pragma region Includes
#include "StdAfx.h"
#include "MonitorListSelector.h"
#include "SVConfigurationObject.h"
#include "SVObjectLibrary\SVObjectManagerClass.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "ObjectSelectorLibrary/ObjectTreeGenerator.h"
#include "RemoteMonitorListHelper.h"
#include "InspectionCommands/CommandExternalHelper.h"
#include "SVOResource/ConstGlobalSvOr.h"
#pragma endregion Includes


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

	SvOsl::ObjectTreeGenerator::Instance().setSelectorType(SvOsl::ObjectTreeGenerator::SelectorTypeEnum::TypeMultipleObject, IDD_MONITOR_LIST_SELECTOR + SvOr::HELPFILE_DLG_IDD_OFFSET);
	SvPb::InspectionCmdResponse responseCmd;

	long numInspections {pPPQ->GetInspectionCount()};

	for (int inspIndex = 0; inspIndex < numInspections; inspIndex++)
	{
		SVInspectionProcess* pInspection(nullptr);
		pPPQ->GetInspection(inspIndex, pInspection);
		if (nullptr != pInspection)
		{
			SvPb::ObjectSelectorType type {SvPb::allNumberValueObjects};
			if (m_bImage == TRUE && m_eListType == PRODUCT_OBJECT_LIST)
			{
				type = SvPb::allImageObjects;
			}
			else if(m_eListType == PRODUCT_OBJECT_LIST)
			{
				type = SvPb::allValueObjects;
			}

			SvPb::InspectionCmdRequest requestCmd;
			*requestCmd.mutable_getobjectselectoritemsrequest() = SvCmd::createObjectSelectorRequest(
				{SvPb::SearchArea::toolsetItems}, pInspection->getObjectId(), SvPb::viewable, pInspection->getObjectId(), false, type, SvPb::GetObjectSelectorItemsRequest::kAttributesAllowed, pInspection->getObjectId());
			SvCmd::InspectionCommands(pInspection->getObjectId(), requestCmd, &responseCmd);
		}
		if (responseCmd.has_getobjectselectoritemsresponse())
		{
			SvOsl::ObjectTreeGenerator::Instance().insertTreeObjects(responseCmd.getobjectselectoritemsresponse().tree());
		}
	}
	SvOsl::ObjectTreeGenerator::Instance().setCheckItems(BuildCheckItems());
	
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

SvDef::StringSet MonitorlistSelector::BuildCheckItems()
{
	SvDef::StringSet Result;

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
		return Result;
	}
	for (auto& it : *pMonitorObjectList)
	{
		std::string name = RemoteMonitorListHelper::GetNameFromMonitoredObject(it, false);
		Result.insert(name);
	}
	return Result;
}

MonitoredObjectList MonitorlistSelector::GetMonitoredObjectList(const SvDef::StringVector& rList)
{
	MonitoredObjectList monitoredObjectList;
	for (const auto& rEntry : rList)
	{
		SVObjectReference objectRef{rEntry};
		MonitoredObject monitoredObj;
		monitoredObj.m_objectId = (nullptr != objectRef.getObject()) ? objectRef.getObject()->getObjectId() : SvDef::InvalidObjectId;
		if(nullptr != objectRef.getValueObject())
		{
			monitoredObj.isArray = objectRef.isArray();
			monitoredObj.wholeArray = objectRef.isEntireArray();
			if (monitoredObj.isArray)
			{
				monitoredObj.arrayIndex = objectRef.ArrayIndex();
			}
		}
		if (SvDef::InvalidObjectId != monitoredObj.m_objectId)
		{
			monitoredObjectList.push_back(monitoredObj);
		}
	}
	return monitoredObjectList;
}