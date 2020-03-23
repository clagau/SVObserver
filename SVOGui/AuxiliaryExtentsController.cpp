//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the AuxiliaryExtentsController class for use by the GUI.
//******************************************************************************
#pragma region Includes
#include "Stdafx.h"
#include "AuxiliaryExtentsController.h"
#include "InspectionCommands\CommandExternalHelper.h"
#include "SVProtoBuf\ConverterHelper.h"
#pragma endregion Includes

namespace SvOg
{
static LPCSTR UpdateAuxiliaryExtentsTag = "UpdateAuxiliaryExtents";

AuxiliaryExtentsController::AuxiliaryExtentsController(const SVGUID& rInspectionID, const SVGUID& rTaskObjectID)
	: m_InspectionID {rInspectionID}
	, m_TaskObjectID {rTaskObjectID}
	, m_ImageController {rInspectionID, rTaskObjectID}
	, m_Values {BoundValues{ rInspectionID, rTaskObjectID }}
{
}

HRESULT AuxiliaryExtentsController::Init()
{
	HRESULT hr = m_Values.Init();
	if (S_OK == hr)
	{
		hr = FindAuxSourceImages();
	}
	return hr;
}

bool AuxiliaryExtentsController::AreAuxiliaryExtentsAvailable() const
{
	bool bRetVal = false;

	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	auto* pRequest = requestCmd.mutable_areauxiliaryextentsavailablerequest();
	SvPb::SetGuidInProtoBytes(pRequest->mutable_objectid(), m_TaskObjectID);

	HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
	if (S_OK == hr && responseCmd.has_areauxiliaryextentsavailableresponse())
	{
		bRetVal = responseCmd.areauxiliaryextentsavailableresponse().areavailable();
	}
	return bRetVal;
}

HRESULT AuxiliaryExtentsController::Commit()
{
	return m_Values.Commit();
}

bool AuxiliaryExtentsController::IsUpdateAuxExtentsEnabled() const
{
	return m_Values.Get<bool>(SvPb::UpdateAuxiliaryExtentsEId);
}

void AuxiliaryExtentsController::EnableAuxExtents(bool bEnable)
{
	m_Values.Set<bool>(SvPb::UpdateAuxiliaryExtentsEId, bEnable);
}

const SvUl::NameGuidList& AuxiliaryExtentsController::GetAvailableImageList() const
{
	return m_auxSourceImages;
}

HRESULT AuxiliaryExtentsController::FindAuxSourceImages()
{
	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	auto* pRequest = requestCmd.mutable_getavailableauximagesrequest();
	SvPb::SetGuidInProtoBytes(pRequest->mutable_objectid(), m_TaskObjectID);

	HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
	if (S_OK == hr && responseCmd.has_getavailableauximagesresponse())
	{
		m_auxSourceImages.clear();
		for (auto item : responseCmd.getavailableauximagesresponse().list())
		{
			SvUl::NameGuidPair tmp {item.objectname(), SvPb::GetGuidFromProtoBytes(item.objectid())};
			m_auxSourceImages.push_back(tmp);
		}
	}
	return hr;
}

std::string AuxiliaryExtentsController::GetAuxSourceImageName() const
{
	SvUl::NameGuidPair source = GetAuxSourceImage();
	return source.first;
}

class ByName
{
	std::string m_name;
public:
	explicit ByName(const std::string& rName) : m_name(rName) {}
	bool operator()(const SvUl::NameGuidPair& rVal) const { return rVal.first == m_name; }
};

HRESULT AuxiliaryExtentsController::SetAuxSourceImage(const std::string& rName)
{
	HRESULT hr = E_INVALIDARG;
	SVGUID imageID;
	SvUl::NameGuidList::const_iterator it = std::find_if(m_auxSourceImages.begin(), m_auxSourceImages.end(), ByName(rName));
	if (it != m_auxSourceImages.end())
	{
		imageID = it->second;
	}
	if (!imageID.empty())
	{
		SvPb::InspectionCmdRequest requestCmd;
		SvPb::InspectionCmdResponse responseCmd;
		auto* pRequest = requestCmd.mutable_setauximageobjectrequest();
		SvPb::SetGuidInProtoBytes(pRequest->mutable_objectid(), m_TaskObjectID);
		SvPb::SetGuidInProtoBytes(pRequest->mutable_sourceimageid(), imageID);

		hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
		if (SUCCEEDED(hr) && responseCmd.has_standardresponse())
		{
			hr = RunOnce();
		}
	}
	return hr;
}

SvUl::NameGuidPair AuxiliaryExtentsController::GetAuxSourceImage() const
{
	SvUl::NameGuidPair retValue;
	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	auto* pRequest = requestCmd.mutable_getauximageobjectrequest();
	SvPb::SetGuidInProtoBytes(pRequest->mutable_objectid(), m_TaskObjectID);

	HRESULT hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
	if (S_OK == hr && responseCmd.has_getauximageobjectresponse())
	{
		retValue.first = responseCmd.getauximageobjectresponse().auxobject().objectname();
		retValue.second = SvPb::GetGuidFromProtoBytes(responseCmd.getauximageobjectresponse().auxobject().objectid());
	}
	return retValue;
}

HRESULT AuxiliaryExtentsController::RunOnce()
{
	return SvCmd::RunOnceSynchronous(m_InspectionID);
}
} //namespace SvOg