//*****************************************************************************
/// \copyright (c) 2015 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file TADlgLinearResultRangesPage.cpp
/// All Rights Reserved 
//*****************************************************************************
/// Property page for the tool adjustment sheet for setting Result Ranges for Linear Tool.
//*****************************************************************************


#pragma region Includes
#include "stdafx.h"
#include "TADlgLinearResultRangesPage.h"
#include "SVObserver/SVSetupDialogManager.h"
#include "SVOGuiUtility/DataController.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

	// TADlgLinearResultRangesPage


TADlgLinearResultRangesPage::TADlgLinearResultRangesPage(uint32_t inspectionId, uint32_t taskObjectId)
	: CPropertyPage(TADlgLinearResultRangesPage::IDD)
	, m_InspectionID{ inspectionId }
	, m_TaskObjectID{ taskObjectId }
	, m_pValueController (nullptr)
{

}

TADlgLinearResultRangesPage::~TADlgLinearResultRangesPage()
{
}


BEGIN_MESSAGE_MAP(TADlgLinearResultRangesPage, CPropertyPage)
	//{{AFX_MSG_MAP(TADlgLinearResultRangesPage)
	ON_BN_CLICKED(IDC_BTN_RANGE_MIN, &TADlgLinearResultRangesPage::OnBnClickedBtnRangeMin)
	ON_BN_CLICKED(IDC_BTN_RANGE_MAX, &TADlgLinearResultRangesPage::OnBnClickedBtnRangeMax)
	ON_BN_CLICKED(IDC_BTN_RANGE_DELTA, &TADlgLinearResultRangesPage::OnBnClickedBtnRangeDelta)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// TADlgLinearResultRangesPage message handlers
BOOL TADlgLinearResultRangesPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	auto operatorListObjectId = getOperatorListObjectId();
	if (0 != operatorListObjectId)
	{
		m_pValueController = std::make_unique<SvOgu::ValueController>(SvOgu::BoundValues{ m_InspectionID, operatorListObjectId, true });
		m_pValueController->Init();
	}
	std::map<uint32_t, SvPb::EmbeddedIdEnum> valueObjectIdEmbeddedIdMap {
		{m_pValueController->GetObjectID(SvPb::EmbeddedIdEnum::LinearThresholdMinEId), SvPb::EmbeddedIdEnum::LinearThresholdMinEId},
		{m_pValueController->GetObjectID(SvPb::EmbeddedIdEnum::LinearThresholdMaxEId), SvPb::EmbeddedIdEnum::LinearThresholdMaxEId},
		{m_pValueController->GetObjectID(SvPb::EmbeddedIdEnum::LinearThresholdDeltaEId), SvPb::EmbeddedIdEnum::LinearThresholdDeltaEId}
	};

	auto results = getResults(operatorListObjectId);
	m_ValueEmbeddedIdResultMap.clear();
	for (const auto& result : results)
	{
		auto valueObject = getMatchedInputsObjectsForResult(result.taskobjectid(), valueObjectIdEmbeddedIdMap);
		if (valueObject.size() == 1) 
		{
			m_ValueEmbeddedIdResultMap.insert_or_assign(valueObjectIdEmbeddedIdMap[valueObject[0].connected_objectid()], result.taskobjectid());
		}
	}
	

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void TADlgLinearResultRangesPage::OnBnClickedBtnRangeMin()
{
	if (false == setRanges(SvPb::EmbeddedIdEnum::LinearThresholdMinEId))
	{
		SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
		Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_Error_NoResultObject, SvStl::SourceFileParams(StdMessageParams));
	}
}


void TADlgLinearResultRangesPage::OnBnClickedBtnRangeMax()
{
	if (false == setRanges(SvPb::EmbeddedIdEnum::LinearThresholdMaxEId))
	{
		SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
		Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_Error_NoResultObject, SvStl::SourceFileParams(StdMessageParams));
	}
}


void TADlgLinearResultRangesPage::OnBnClickedBtnRangeDelta()
{
	if (false == setRanges(SvPb::EmbeddedIdEnum::LinearThresholdDeltaEId))
	{
		SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
		Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_Error_NoResultObject, SvStl::SourceFileParams(StdMessageParams));
	}
}

bool TADlgLinearResultRangesPage::setRanges(SvPb::EmbeddedIdEnum embeddedId)
{
	auto resultSetup = SVSetupDialogManager::Instance().SetupDialog(SvPb::DoubleResultClassId, m_InspectionID, m_ValueEmbeddedIdResultMap[embeddedId], this);
	// SetupDialog() returns S_FALSE if the user press Cancel, which is NOT an error! Therefore the test below. 
	if (S_OK != resultSetup && S_FALSE != resultSetup)
	{
		return false;
	}

	return true;
}


uint32_t TADlgLinearResultRangesPage::getOperatorListObjectId()
{
	uint32_t operatorListObjectId = 0U;
	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	auto* pRequest = requestCmd.mutable_taskobjectlistrequest();
	pRequest->set_taskobjectid(m_TaskObjectID);

	SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
	if (responseCmd.has_taskobjectlistresponse())
	{		
		auto objectInfoIter = std::find_if(responseCmd.taskobjectlistresponse().taskobjectinfos().begin(), responseCmd.taskobjectlistresponse().taskobjectinfos().end(), 
			[](SvPb::TaskObjectInfo objectInfo)
			{
				return (objectInfo.objectsubtype() == SvPb::SVObjectSubTypeEnum::SVLinearImageOperatorListObjectType);
			});
		if (objectInfoIter != responseCmd.taskobjectlistresponse().taskobjectinfos().end())
		{
			operatorListObjectId = objectInfoIter->taskobjectid();
		}
	}

	return operatorListObjectId;
}

std::vector<SvPb::TaskObjectInfo> TADlgLinearResultRangesPage::getResults(uint32_t operatorListObjectId)
{
	std::vector<SvPb::TaskObjectInfo> results;
	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	auto* pRequest = requestCmd.mutable_taskobjectlistrequest();
	pRequest->set_taskobjectid(operatorListObjectId);

	SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
	if (responseCmd.has_taskobjectlistresponse())
	{
		responseCmd.taskobjectlistresponse().taskobjectinfos();
		
		std::copy_if(responseCmd.taskobjectlistresponse().taskobjectinfos().begin(), 
			responseCmd.taskobjectlistresponse().taskobjectinfos().end(), 
			std::back_inserter(results), [](SvPb::TaskObjectInfo objectInfo)
			{
				return (objectInfo.objectsubtype() == SvPb::SVObjectSubTypeEnum::SVResultDoubleObjectType);
			});

	}
	
	return results;
}

SvPb::InputDataList TADlgLinearResultRangesPage::getMatchedInputsObjectsForResult(uint32_t resultId, 
	const std::map<uint32_t, SvPb::EmbeddedIdEnum>& valueObjects)
{
	SvPb::InputDataList results;
	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	auto* pRequest = requestCmd.mutable_getinputsrequest();
	pRequest->set_objectid(resultId);
	pRequest->mutable_typeinfo()->set_objecttype(SvPb::SVValueObjectType);
	pRequest->mutable_typeinfo()->set_subtype(SvPb::SVObjectSubTypeEnum::SVDoubleValueObjectType);
	
	auto hr = SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
	if (S_OK == hr && responseCmd.has_getinputsresponse())
	{
		for (const auto& rData : responseCmd.getinputsresponse().list())
		{
			if (valueObjects.end() != valueObjects.find(rData.connected_objectid()))
			{
				auto* pData = results.Add();
				pData->CopyFrom(rData);
			}
		}
	}

	return results;
}


