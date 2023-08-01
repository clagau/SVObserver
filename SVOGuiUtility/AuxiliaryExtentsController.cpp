//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Körber Pharma Inspection GmbH. All Rights Reserved
// All Rights Reserved
//*****************************************************************************

// This is the AuxiliaryExtentsController class for use by the GUI.
//******************************************************************************
#pragma region Includes
#include "Stdafx.h"
#include "AuxiliaryExtentsController.h"
#include "InspectionCommands\CommandExternalHelper.h"
#include "ExternalToolTaskController.h"
#pragma endregion Includes

namespace SvOgu
{
static LPCSTR UpdateAuxiliaryExtentsTag = "UpdateAuxiliaryExtents";

AuxiliaryExtentsController::AuxiliaryExtentsController(uint32_t inspectionID, uint32_t taskObjectID)
	: m_InspectionID {inspectionID}
	, m_TaskObjectID {taskObjectID}
	, m_ImageController {inspectionID, taskObjectID}
	, m_values {BoundValues{ inspectionID, taskObjectID }}
{
	std::unique_ptr<ExternalToolTaskController> pExternalToolTaskControler = std::make_unique<ExternalToolTaskController>(inspectionID, taskObjectID);
	m_ExternalToolTaskObjectId = pExternalToolTaskControler->getExternalToolTaskObjectId();
	
	if (SvDef::InvalidObjectId != m_ExternalToolTaskObjectId)
	{
		m_pExternalToolTaskValues = std::make_unique<ValueController>(BoundValues {inspectionID, m_ExternalToolTaskObjectId});
	}
}

HRESULT AuxiliaryExtentsController::Init()
{
	
	if (m_pExternalToolTaskValues)
	{
		m_pExternalToolTaskValues->Init();
	}
	
	HRESULT hr = m_values.Init();
	if (S_OK == hr)
	{
		hr = findAuxSourceImages();
	}
	return hr;
}

bool AuxiliaryExtentsController::areAuxiliaryExtentsAvailable() const
{
	bool bRetVal = false;

	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	auto* pRequest = requestCmd.mutable_areauxiliaryextentsavailablerequest();
	pRequest->set_objectid(m_TaskObjectID);

	HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
	if (S_OK == hr && responseCmd.has_areauxiliaryextentsavailableresponse())
	{
		bRetVal = responseCmd.areauxiliaryextentsavailableresponse().areavailable();
	}
	return bRetVal;
}

HRESULT AuxiliaryExtentsController::Commit()
{
	if (m_pExternalToolTaskValues)
	{
		m_pExternalToolTaskValues->Commit();
	}
	return m_values.Commit();
}

bool AuxiliaryExtentsController::isUpdateAuxExtentsEnabled() const
{
	return m_values.Get<bool>(SvPb::UpdateAuxiliaryExtentsEId);
}

bool AuxiliaryExtentsController::isShowOverlayInAncestorEnabled() const
{
	return m_values.Get<bool>(SvPb::IsShowOverlayInAncestorEnabledEId);
}

void AuxiliaryExtentsController::enableAuxExtents(bool bEnable)
{
	m_values.Set<bool>(SvPb::UpdateAuxiliaryExtentsEId, bEnable);
}

void AuxiliaryExtentsController::enableShowOverlayInAncestor(bool bEnable)
{
	m_values.Set<bool>(SvPb::IsShowOverlayInAncestorEnabledEId, bEnable);
}

const SvUl::NameObjectIdList& AuxiliaryExtentsController::getAvailableImageList() const
{
	return m_auxSourceImages;
}

const SvUl::NameObjectIdList& AuxiliaryExtentsController::getAvailableAncestorForOverlayImageList() const
{
	return m_ancestorImageList;
}

HRESULT AuxiliaryExtentsController::findAuxSourceImages()
{
	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	auto* pRequest = requestCmd.mutable_getavailableancestorimagesrequest();
	pRequest->set_objectid(m_TaskObjectID);

	HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
	if (S_OK == hr && responseCmd.has_getavailableancestorimagesresponse())
	{
		m_auxSourceImages.clear();
		for (auto item : responseCmd.getavailableancestorimagesresponse().list())
		{
			SvUl::NameObjectIdPair tmp {item.objectname(), item.objectid()};
			m_auxSourceImages.push_back(tmp);
			if (SvPb::SVExtentTranslationShift == item.translationtype() || SvPb::SVExtentTranslationProfileShift == item.translationtype() || SvPb::SVExtentTranslationFigureShift == item.translationtype())
			{
				m_ancestorImageList.push_back(tmp);
			}
			else
			{
				m_ancestorImageList.clear();
			}
		}
	}

	return hr;
}

std::string AuxiliaryExtentsController::getAuxSourceImageName() const
{
	SvUl::NameObjectIdPair source = getAuxSourceImage();
	return source.first;
}

class ByName
{
	std::string m_name;
public:
	explicit ByName(const std::string& rName) : m_name(rName) {}
	bool operator()(const SvUl::NameObjectIdPair& rVal) const { return rVal.first == m_name; }
};

HRESULT AuxiliaryExtentsController::setAuxSourceImage(const std::string& rName)
{
	HRESULT hr = E_INVALIDARG;
	uint32_t imageID = SvDef::InvalidObjectId;
	SvUl::NameObjectIdList::const_iterator it = std::find_if(m_auxSourceImages.begin(), m_auxSourceImages.end(), ByName(rName));
	if (it != m_auxSourceImages.end())
	{
		imageID = it->second;
	}
	if (SvDef::InvalidObjectId != imageID)
	{	
		SvPb::InspectionCmdRequest requestCmd;
		SvPb::InspectionCmdResponse responseCmd;
		auto* pRequest = requestCmd.mutable_connecttoobjectrequest();
		pRequest->set_objectid(m_TaskObjectID);
		pRequest->set_newconnectedid(imageID);
		pRequest->set_embeddedid(SvPb::AuxImageInputEId);
		pRequest->set_objecttype(SvPb::SVImageObjectType);
		hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
		if (SUCCEEDED(hr))
		{
			hr = RunOnce();
		}
	}
	return hr;
}

bool AuxiliaryExtentsController::setAncestorforOverlay(const std::string& rName)
{
	uint32_t imageID = SvDef::InvalidObjectId;
	SvUl::NameObjectIdList::const_iterator it = std::find_if(m_ancestorImageList.begin(), m_ancestorImageList.end(), ByName(rName));
	if (it != m_ancestorImageList.end())
	{
		imageID = it->second;
	}

	if (SvDef::InvalidObjectId == imageID)
	{
		return false;
	}
	
	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	auto* pRequest = requestCmd.mutable_connecttoobjectrequest();
	pRequest->set_objectid(m_TaskObjectID);
	pRequest->set_newconnectedid(imageID);
	pRequest->set_embeddedid(SvPb::AncestorForOverlayInputEId);
	pRequest->set_objecttype(SvPb::SVImageObjectType);
	HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
	if (SUCCEEDED(hr))
	{
		hr = RunOnce();
	}
	return S_OK == hr;
}

SvUl::NameObjectIdPair AuxiliaryExtentsController::getAuxSourceImage() const
{
	SvUl::NameObjectIdPair retValue;
	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	auto* pDataRequest = requestCmd.mutable_getinputdatarequest();
	pDataRequest->set_objectid(m_TaskObjectID);
	pDataRequest->set_embeddedid(SvPb::AuxImageInputEId);
	pDataRequest->set_desired_first_object_type_for_connected_name(SvPb::SVToolSetObjectType);
	pDataRequest->set_exclude_first_object_name_in_conntected_name(false);
	responseCmd.Clear();
	HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
	if (S_OK == hr && responseCmd.has_getinputdataresponse())
	{
		retValue.first = responseCmd.getinputdataresponse().data().connected_objectdottedname();
		retValue.second = responseCmd.getinputdataresponse().data().connected_objectid();
	}
	else
	{
		Log_Assert(false);
	}
	return retValue;
}

SvUl::NameObjectIdPair AuxiliaryExtentsController::getAncestorForOverlay() const
{
	SvUl::NameObjectIdPair retValue;
	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	auto* pDataRequest = requestCmd.mutable_getinputdatarequest();
	pDataRequest->set_objectid(m_TaskObjectID);
	pDataRequest->set_embeddedid(SvPb::AncestorForOverlayInputEId);
	pDataRequest->set_desired_first_object_type_for_connected_name(SvPb::SVToolSetObjectType);
	pDataRequest->set_exclude_first_object_name_in_conntected_name(false);
	responseCmd.Clear();
	HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
	if (S_OK == hr && responseCmd.has_getinputdataresponse())
	{
		retValue.first = responseCmd.getinputdataresponse().data().connected_objectdottedname();
		retValue.second = responseCmd.getinputdataresponse().data().connected_objectid();
	}
	else
	{
		Log_Assert(false);
	}

	return retValue;
}

HRESULT AuxiliaryExtentsController::RunOnce()
{
	return SvCmd::RunOnceSynchronous(m_InspectionID);
}
bool AuxiliaryExtentsController::isExternalTool() const
{
	return m_ExternalToolTaskObjectId != SvDef::InvalidObjectId;
}

bool AuxiliaryExtentsController::hasUnitMappeing() const
{
	if (m_pExternalToolTaskValues)
	{
		return m_pExternalToolTaskValues->Get<bool>(SvPb::UseUnitMappingEId);
	}
	return false;
}

void AuxiliaryExtentsController::setUseUnitMapping(bool val)
{
	if (m_pExternalToolTaskValues)
	{
		m_pExternalToolTaskValues->Set<bool>(SvPb::UseUnitMappingEId, val);
	}
}
} //namespace SvOgu