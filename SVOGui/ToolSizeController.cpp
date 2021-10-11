//*****************************************************************************
// \copyright COPYRIGHT (c) 2021/09/14,2021/09/14 by Seidenader Maschinenbau GmbH. All Rights Reserved 
/// \file ToolsizeHelper.cpp
/// DESCRIPTION
//******************************************************************************

#pragma region Includes 
#include "stdafx.h"
#include "ToolSizeController.h"
#include "Definitions/SVObjectTypeInfoStruct.h"
#include "FormulaController.h"
#include "InspectionCommands/CommandExternalHelper.h"
#pragma endregion Includes



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


using SizeModes = std::map<SvDef::ToolSizeAdjustEnum, SvDef::ToolSizeModes>;
using SizeValues = std::map<SvDef::ToolSizeAdjustEnum, std::string>;


namespace SvOg
{
ToolSizeController::ToolSizeController(uint32_t inspectionId, uint32_t toolId) :
	m_toolId(toolId),
	m_ipId(inspectionId)
{
	InitHelper();
	StoreExtents(true);
}

ToolSizeController::~ToolSizeController()
{

}

void  ToolSizeController::InitHelper()
{

	for (auto ad : SvDef::AllToolSizeAdjustEnum)
	{
		SvDef::SVObjectTypeInfoStruct evaluateObjectInfo;
		evaluateObjectInfo.m_ObjectType = SvPb::SVEquationObjectType;
		evaluateObjectInfo.m_SubType = m_EQAdjustStruct[ad].m_subType;
		SvPb::InspectionCmdRequest requestCmd;
		SvPb::InspectionCmdResponse responseCmd;
		auto* pRequest = requestCmd.mutable_getobjectidrequest()->mutable_info();
		pRequest->set_ownerid(m_toolId);
		SvCmd::setTypeInfos(evaluateObjectInfo, *pRequest->mutable_infostruct());

		HRESULT hr = SvCmd::InspectionCommands(m_ipId, requestCmd, &responseCmd);
		if (S_OK == hr && responseCmd.has_getobjectidresponse())
		{
			m_EQAdjustStruct[ad].m_Id = responseCmd.getobjectidresponse().objectid();
			m_EQAdjustStruct[ad].m_name = SvUl::LoadStdString(m_EQAdjustStruct[ad].m_IDSname);
		}
		
	}

	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	SvDef::SVObjectTypeInfoStruct ToolSizeAdjustTaskInfo;
	ToolSizeAdjustTaskInfo.m_ObjectType = SvPb::SVToolSizeAdjustTaskType;
	auto* pRequest = requestCmd.mutable_getobjectidrequest()->mutable_info();
	pRequest->set_ownerid(m_toolId);
	SvCmd::setTypeInfos(ToolSizeAdjustTaskInfo, *pRequest->mutable_infostruct());

	HRESULT hr = SvCmd::InspectionCommands(m_ipId, requestCmd, &responseCmd);
	if (S_OK == hr && responseCmd.has_getobjectidresponse())
	{
		m_SizeAdjustTaskId = responseCmd.getobjectidresponse().objectid();
		m_pTaskValueController = std::make_unique<ValueController>(BoundValues {m_ipId, m_SizeAdjustTaskId});
	}
	InitValues();
}

bool  ToolSizeController::InitToolList()
{
	bool ret {false};
	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;

	auto* pAvailableRequest = requestCmd.mutable_getavailableobjectsrequest();
	pAvailableRequest->set_objectid(m_ipId);
	pAvailableRequest->mutable_typeinfo()->set_objecttype(SvPb::SVToolObjectType);
	pAvailableRequest->set_objecttypetoinclude(SvPb::SVToolSetObjectType);
	pAvailableRequest->set_importantobjectforstopatborder(m_toolId);
	pAvailableRequest->mutable_hasextension();

	HRESULT hr = SvCmd::InspectionCommands(m_ipId, requestCmd, &responseCmd);
	if (S_OK == hr && responseCmd.has_getavailableobjectsresponse())
	{
		m_availableToolList = SvCmd::convertNameObjectIdList(responseCmd.getavailableobjectsresponse().list());

		auto con = [=](const SvUl::NameObjectIdPair& rpair){ return (rpair.second == m_toolId); };
		std::erase_if(m_availableToolList, con);
		ret = true;

	}
	return ret;
}

const SvUl::NameObjectIdList& ToolSizeController::GetAvailableToolList() const
{
	return m_availableToolList;
}

void  ToolSizeController::InitValues()
{

	
	if (nullptr != m_pTaskValueController)
	{
		m_pTaskValueController->Init();
	}
	

	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;

	requestCmd.Clear();
	responseCmd.Clear();
	auto* pParamRequest = requestCmd.mutable_gettoolsizeadjustparameterrequest();
	pParamRequest->set_objectid(m_SizeAdjustTaskId);

	HRESULT hr = SvCmd::InspectionCommands(m_ipId, requestCmd, &responseCmd);
	if (S_OK == hr && responseCmd.has_gettoolsizeadjustparameterresponse())
	{
		m_hasAdjustToolSizeParameter = true;
		auto responseData = responseCmd.gettoolsizeadjustparameterresponse();
		m_isAdjustSizeAllowed = responseData.isadjustsizeallowed();
		m_isAdjustPositionAllowed = responseData.isadjustpositionallowed();
		m_autoSizeEnabled = responseData.enableautosize();
		m_canResizetoparent = responseData.canresizetoparent();
		m_isAutofitAllowedFlag = responseData.isfullsizeallowed();
	}
	

	requestCmd.Clear();
	responseCmd.Clear();
	auto* pRequest = requestCmd.mutable_getextentparameterrequest();
	pRequest->set_objectid(m_toolId);

	hr = SvCmd::InspectionCommands(m_ipId, requestCmd, &responseCmd);
	if (S_OK == hr && responseCmd.has_getextentparameterresponse())
	{


		m_Extents = responseCmd.getextentparameterresponse().parameters();
	}
	else
	{
		m_Extents = Extents();

	}


	responseCmd.Clear();
	pRequest->set_shouldfromparent(true);

	hr = SvCmd::InspectionCommands(m_ipId, requestCmd, &responseCmd);
	if (S_OK == hr &&  responseCmd.has_getextentparameterresponse())
	{
		m_ParentExtents = responseCmd.getextentparameterresponse().parameters();
	}
	else 
	{
		m_ParentExtents = Extents();

	}
	
	requestCmd.Clear();
	responseCmd.Clear();
	auto* pReq = requestCmd.mutable_getobjectparametersrequest();
	pReq->set_objectid(m_toolId);

	hr = SvCmd::InspectionCommands(m_ipId, requestCmd, &responseCmd);
	if (S_OK == hr && responseCmd.has_getobjectparametersresponse())
	{
		m_isToolValid = responseCmd.getobjectparametersresponse().isvalid();
	}
	
}

const SvOg::ToolSizeController::EQAdjustStruct ToolSizeController::GetEquationStruct(SvDef::ToolSizeAdjustEnum adjustenum) const
{
	return m_EQAdjustStruct[adjustenum];
}

bool ToolSizeController::IsFullSizeAllowed(bool init)
{
	if (init)
	{
		InitValues();
	}

	bool bAllowFullsize(true);
	if (m_autoSizeEnabled == SvPb::EnableNone)
	{
		bAllowFullsize = false;
	}

	std::array<SvPb::SVExtentPropertyEnum, 4> PropArray = {SvPb::SVExtentPropertyWidth, SvPb::SVExtentPropertyHeight, SvPb::SVExtentPropertyPositionPointX, SvPb::SVExtentPropertyPositionPointY};
	std::for_each(PropArray.begin(), PropArray.end(), [&](SvPb::SVExtentPropertyEnum p)
	{
		if (bAllowFullsize)
		{
			auto iter = std::find_if(m_Extents.begin(), m_Extents.end(), [p](auto item) { return p == item.type(); });
			if (m_Extents.end() != iter && iter->issetbyreset())
			{
				bAllowFullsize = false;
			}
		}

	}
	);

	return bAllowFullsize;

}

bool ToolSizeController::IsFullSize(bool init)
{
	if (init)
	{
		InitValues();
			
	}

	bool bFull {false};
	long width = SvCmd::getValueForProperties<long>(m_Extents, SvPb::SVExtentPropertyOutputWidth);
	long height = SvCmd::getValueForProperties<long>(m_Extents, SvPb::SVExtentPropertyOutputHeight);
	long top = SvCmd::getValueForProperties<long>(m_Extents, SvPb::SVExtentPropertyPositionPointY);
	long left = SvCmd::getValueForProperties<long>(m_Extents, SvPb::SVExtentPropertyPositionPointX);

	if (m_ParentExtents.size())
	{

		long parentWidth = SvCmd::getValueForProperties<long>(m_ParentExtents, SvPb::SVExtentPropertyOutputWidth);
		long parentHeight = SvCmd::getValueForProperties<long>(m_ParentExtents, SvPb::SVExtentPropertyOutputHeight);
		long parentTop = SvCmd::getValueForProperties<long>(m_ParentExtents, SvPb::SVExtentPropertyPositionPointY);
		long parentLeft = SvCmd::getValueForProperties<long>(m_ParentExtents, SvPb::SVExtentPropertyPositionPointX);
		if (parentWidth == width
			&& parentHeight == height
			&& parentTop == top
			&& parentLeft == left)
		{
			bFull = true;
		}
	}
	return bFull;
}


bool   ToolSizeController::GetValuesFromExtents(const Extents& rExtents, SizeValues& rValues)
{


	for (auto vType : SvDef::AllToolSizeAdjustEnum)
	{
		std::string Value;
		auto propEnum = m_EQAdjustStruct[vType].m_extentProp;
		auto valuePair = std::find_if(rExtents.begin(), rExtents.end(), [propEnum](const auto value) { return value.type() == propEnum; });
		if (rExtents.end() != valuePair)
		{
			Value = std::format("{}", static_cast<long>(valuePair->value()));

		}
		rValues[vType] = Value;
	}
	return true;
}


bool   ToolSizeController::GetToolSizeMode(bool init, SizeModes& rModes, SizeValues& rValues)
{
	if (init)
	{
		InitValues();
	}
	
	rModes.clear();
	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	auto* pRequest = requestCmd.mutable_getextentparameterrequest();
	pRequest->set_objectid(m_toolId);

	for (auto vType : SvDef::AllToolSizeAdjustEnum)
	{
		std::string Value;
		long SelMode = m_pTaskValueController->Get<long>(m_EQAdjustStruct[vType].m_inputModeEmbeddedId);
		if (SvDef::ToolSizeAdjustEnum::TSPositionX == vType || SvDef::ToolSizeAdjustEnum::TSPositionY == vType)
		{
			if (0 == (m_autoSizeEnabled & SvPb::EnablePosition))
			{
				SelMode = SvDef::TSNone;
			}
		}
		else if (0 == (m_autoSizeEnabled & SvPb::EnableSize))
		{
			SelMode = SvDef::TSNone;
		}
		rModes[vType] = (SvDef::ToolSizeModes)SelMode;

		if (SvDef::TSFormula == SelMode)
		{
			FormulaController formula(m_ipId, m_toolId, m_EQAdjustStruct[vType].m_Id);
			bool isOwnerEnabled, isEquationEnabled = false;
			formula.IsOwnerAndEquationEnabled(isOwnerEnabled, isEquationEnabled);
			std::string text = formula.GetEquationText();
			if (isEquationEnabled && !text.empty())
			{
				double result = 0;
				SvStl::MessageContainerVector errorMessage;
				int errorPos = formula.ValidateEquation(text, result, false, errorMessage);
				if (FormulaController::validateSuccessful == errorPos)
				{
					Value = std::format("{}", static_cast<long>(result));
				}
			}
		}
		else if (SvDef::TSNone == SelMode)
		{
			auto propEnum = m_EQAdjustStruct[vType].m_extentProp;
			auto valuePair = std::find_if(m_Extents.begin(), m_Extents.end(), [propEnum](const auto value) { return value.type() == propEnum; });
			if (m_Extents.end() != valuePair)
			{
				Value = std::format("{}", static_cast<long>(valuePair->value()));

			}
		}
		else if (SelMode == SvDef::TSAutoFit)
		{
			if (SvDef::ToolSizeAdjustEnum::TSHeight == SvDef::ToolSizeAdjustEnum(vType) || SvDef::ToolSizeAdjustEnum::TSWidth == SvDef::ToolSizeAdjustEnum(vType))
			{
				auto propEnum = m_EQAdjustStruct[vType].m_extentProp;
				auto valuePair = std::find_if(m_ParentExtents.begin(), m_ParentExtents.end(), [propEnum](const auto value) { return value.type() == propEnum; });
				if (m_ParentExtents.end() != valuePair)
				{
					Value = std::format("{}", static_cast<long>(valuePair->value()));
				}
			}
			else
			{
				Value = std::format("{}", 0l);
			}
		}
		rValues[vType] = Value;
	}
	return true;
}
bool   ToolSizeController::GetToolSizeMode(bool init, SizeModes& rModes)
{
	if (init)
	{
		InitValues();
	}


	rModes.clear();
	if (!hasAdjustToolSizeParameter())
		return false;

	for (auto vType : SvDef::AllToolSizeAdjustEnum)
	{
		long SelMode = m_pTaskValueController->Get<long>(m_EQAdjustStruct[vType].m_inputModeEmbeddedId);
		if (SvDef::ToolSizeAdjustEnum::TSPositionX == vType || SvDef::ToolSizeAdjustEnum::TSPositionY == vType)
		{
			if (0 == (m_autoSizeEnabled & SvPb::EnablePosition))
			{
				SelMode = SvDef::TSNone;
			}
		}
		else if (0 == (m_autoSizeEnabled & SvPb::EnableSize))
		{
			SelMode = SvDef::TSNone;
		}
		rModes[vType] = (SvDef::ToolSizeModes)SelMode;

	}
	return true;
}

SvOi::NameValueVector ToolSizeController::GetEnumTypes(SvDef::ToolSizeAdjustEnum adjustenum) const
{
	SvOi::NameValueVector result(m_pTaskValueController->GetEnumTypes(m_EQAdjustStruct[adjustenum].m_inputModeEmbeddedId));
	if (!m_canResizetoparent || !m_isAutofitAllowedFlag) //if (!m_isFullSizeAllowed)
	{
		///Remove Auto fit from vector
		auto cond = [](const SvOi::NameValuePair& pair){return (pair.first == SvDef::SizeAdjustTextAutoFitSize); };
		auto newend = std::remove_if(result.begin(), result.end(), cond);
		result.erase(newend, result.end());
	}

	return result;

}


bool ToolSizeController::SetToolSizeMode(SizeModes modes, bool reset)
{
	for (auto Size_Mode : modes)
	{
		auto mode = Size_Mode.second;
		auto AdEnum = Size_Mode.first;

		if (AdEnum >= SvDef::ToolSizeAdjustEnum::TSValuesCount)
		{
			return false;
		}

		if ((AdEnum == SvDef::ToolSizeAdjustEnum::TSPositionX || AdEnum == SvDef::ToolSizeAdjustEnum::TSPositionY))
		{
			if ((m_autoSizeEnabled & SvPb::EnablePosition) == 0)
			{
				mode = SvDef::TSNone;
			}
		}
		else if ((m_autoSizeEnabled & SvPb::EnableSize) == 0)
		{
			mode = SvDef::TSNone;
		}
		m_pTaskValueController->Set<long>(m_EQAdjustStruct[AdEnum].m_inputModeEmbeddedId, mode);
		bool bEnabled = (mode == SvDef::TSFormula);
		ValueController EquationValues {BoundValues{ m_ipId, m_EQAdjustStruct[AdEnum].m_Id }};
		EquationValues.Init();
		EquationValues.Set<bool>(SvPb::EquationEnabledEId, bEnabled);
		EquationValues.Commit(PostAction::doNothing);
	}
	m_pTaskValueController->Commit();
	if (reset)
	{
		ResetObject();
	}
	return true;
}


bool ToolSizeController::SetAllToolSizeMode(SvDef::ToolSizeModes mode, bool reset)
{
	SizeModes temp
	{
	{SvDef::TSPositionX,mode},
	{SvDef::TSPositionY,mode },
	{SvDef::TSHeight,mode},
	{SvDef::TSWidth,mode}
	};
	bool ret = SetToolSizeMode(temp, reset);
	if (ret)
	{

	}
	return ret;
}

bool ToolSizeController::ResetObject()
{
	SvPb::InspectionCmdRequest requestCmd;
	auto* pRequest = requestCmd.mutable_resetobjectrequest();
	pRequest->set_objectid(m_toolId);

	return (S_OK == SvCmd::InspectionCommands(m_ipId, requestCmd, nullptr));
}


bool  ToolSizeController::StoreExtents(bool init)
{
	if (init)
	{
		InitValues();
		
	}
	m_OriginalExtents = m_Extents;
	return true;
};
const Extents& ToolSizeController::GetExtents(bool init)
{
	if (init)
	{
		InitValues();
	}

	return m_Extents;
}
double  ToolSizeController::getValueForProperties(bool init, SvPb::SVExtentPropertyEnum  eProperty)
{
	if (init)
	{
		InitValues();
	}
	return  SvCmd::getValueForProperties<double>(m_Extents, eProperty);
}

// cppcheck-suppress unusedFunction
bool   ToolSizeController::setValueForProperties(double value, SvPb::SVExtentPropertyEnum  eProperty)
{
	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	auto* pRequest = requestCmd.mutable_setextentparameterrequest();
	pRequest->set_objectid(m_toolId);
	pRequest->mutable_setproperty()->set_propertyflag(eProperty);
	pRequest->mutable_setproperty()->set_value(value);

	HRESULT hr = SvCmd::InspectionCommands(m_ipId, requestCmd, nullptr);
	return (hr == S_OK);
}

bool  ToolSizeController::HasRotation(bool init)
{
	if (init)
	{
		InitValues();
	}

	auto iterFindRot = std::find_if(m_Extents.begin(), m_Extents.end(), [](const auto& rItem) { return SvPb::SVExtentPropertyRotationAngle == rItem.type(); });
	if (m_Extents.end() == iterFindRot || iterFindRot->filteredoutflag())
	{
		return false;
	}
	else
	{
		return true;
	}

}

bool ToolSizeController::HasChanged(bool init)
{
	if (init)
	{
		InitValues();
	}

	bool hasChanged(false);
	if (m_OriginalExtents.size() == m_Extents.size())
	{
		for (int i = 0; i < m_Extents.size(); i++)
		{
			if (m_Extents[i].value() != m_OriginalExtents[i].value() || m_Extents[i].type() != m_OriginalExtents[i].type())
			{
				hasChanged = true;
				break;
			}
		}
	}
	else
	{
		hasChanged = true;
	}
	return hasChanged;
}
bool ToolSizeController::RestoreExtents()
{
	
	HRESULT hr = SetExtents(m_OriginalExtents);
	return (hr == S_OK);
}

HRESULT  ToolSizeController::SetExtents(const Extents&  extents)
{
	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	auto* pRequest = requestCmd.mutable_setextentparameterrequest();
	pRequest->set_objectid(m_toolId);
	pRequest->mutable_extentlist()->mutable_extentlist()->MergeFrom(extents);
	HRESULT hr = SvCmd::InspectionCommands(m_ipId, requestCmd, &responseCmd);
	if (S_OK == hr && responseCmd.has_setextentparameterresponse())
	{
		hr = responseCmd.hresult();
	}
	return hr;
}

bool ToolSizeController::SetExtentToParent()
{
	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	auto* pRequest = requestCmd.mutable_setextentparameterrequest();
	pRequest->set_objectid(m_toolId);
	pRequest->set_settoparent(true);

	HRESULT hr = SvCmd::InspectionCommands(m_ipId, requestCmd, &responseCmd);
	if (S_OK == hr && responseCmd.has_setextentparameterresponse())
	{
		m_Extents = responseCmd.setextentparameterresponse().parameters();
		m_translationType = responseCmd.getextentparameterresponse().translationtype();
		return true;
	}
	return false;
}


bool ToolSizeController::Propagate()
{
	SvPb::InspectionCmdRequest requestCmd;
	SvPb::PropagateSizeAndPositionRequest* pRequest = requestCmd.mutable_propagatesizeandpositionrequest();
	pRequest->set_objectid(m_ipId);
	HRESULT hr = SvCmd::InspectionCommands(m_ipId, requestCmd, nullptr);
	return hr == S_OK;

}
HRESULT  ToolSizeController::SetExtentsMap(const std::map< SvPb::SVExtentPropertyEnum, double>& extents)
{
	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	auto* pRequest = requestCmd.mutable_setextentparameterrequest();
	pRequest->set_objectid(m_toolId);

	for (auto& element : extents)
	{
		auto pParameter = pRequest->mutable_extentlist()->mutable_extentlist()->Add();
		pParameter->set_type(element.first);
		pParameter->set_value(element.second);
		pParameter->set_filteredoutflag(false);
		pParameter->set_issetbyreset(false);
	}
	HRESULT hr = SvCmd::InspectionCommands(m_ipId, requestCmd, &responseCmd);
	if (S_OK == hr && responseCmd.has_setextentparameterresponse())
	{
		m_Extents = responseCmd.setextentparameterresponse().parameters();
		m_translationType = responseCmd.getextentparameterresponse().translationtype();
		hr = responseCmd.hresult();
	}

	return hr;

}
// cppcheck-suppress unusedFunction
std::map< SvPb::SVExtentPropertyEnum, double> ToolSizeController::GetExtentValues()
{
	std::map< SvPb::SVExtentPropertyEnum, double> ret;
	for (auto parameter : m_Extents)
	{
		ret[parameter.type()] = parameter.value();
	}
	return ret;
}


//call updatefrom output space
bool  ToolSizeController::AdjustTool(SvPb::SVExtentLocationPropertyEnum eAction, int dx, int dy)
{
	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	auto* pRequest = requestCmd.mutable_setextentparameterrequest();
	pRequest->set_objectid(m_toolId);
	auto* pUpdateParam = pRequest->mutable_updatefromoutputspace();
	pUpdateParam->set_action(eAction);
	pUpdateParam->set_dx(dx);
	pUpdateParam->set_dy(dy);

	HRESULT hr = SvCmd::InspectionCommands(m_ipId, requestCmd, &responseCmd);
	if (S_OK == hr && responseCmd.has_setextentparameterresponse())
	{
		m_Extents = responseCmd.setextentparameterresponse().parameters();
		m_translationType = responseCmd.getextentparameterresponse().translationtype();
	}

	return (hr == S_OK);

}




bool ToolSizeController::AdjustToolAngle(bool init, double dDAngle)
{

	if (init)
	{
		InitValues();
	}
	double dCurrentAngle = SvCmd::getValueForProperties<double>(m_Extents, SvPb::SVExtentPropertyRotationAngle);
	if (SvPb::SVExtentTranslationPolarUnwrap == m_translationType)
	{
		dCurrentAngle += dDAngle;
	}
	else
	{
		dCurrentAngle -= dDAngle;
	}

	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	auto* pRequest = requestCmd.mutable_setextentparameterrequest();
	pRequest->set_objectid(m_toolId);
	pRequest->mutable_setproperty()->set_propertyflag(SvPb::SVExtentPropertyRotationAngle);
	pRequest->mutable_setproperty()->set_value(dCurrentAngle);

	HRESULT result = SvCmd::InspectionCommands(m_ipId, requestCmd, &responseCmd);
	if (S_OK == result && responseCmd.has_setextentparameterresponse())
	{
		m_Extents = responseCmd.setextentparameterresponse().parameters();
		m_translationType = responseCmd.getextentparameterresponse().translationtype();
	}

	return (result == S_OK);

}

bool ToolSizeController::SetFormulas(bool init, bool overwrite)
{
	SizeModes Modes;
	bool   ret = GetToolSizeMode(init, Modes);
	if (ret)
	{
		SizeValues Values;
		GetValuesFromExtents(GetExtents(false), Values);
		return SetFormulas(false, overwrite, Values);
	}
	else
	{
		return false;
	}
}
//Insert string from rvalues to  formula if mode is TSFormula
bool ToolSizeController::SetFormulas(bool init, bool overwrite, const SizeValues& rValues)
{
	SizeModes Modes;

	bool   ret = GetToolSizeMode(init, Modes);
	if (false == ret)
	{
		return false;
	}
	if (rValues.size() < SvDef::TSValuesCount )
	{
		return false;
	}
	for (auto adEnum : SvDef::AllToolSizeAdjustEnum)
	{
		if (SvDef::TSFormula == Modes[adEnum])
		{
			DWORD eqId = GetEquationStruct(adEnum).m_Id;
			FormulaController formula(m_ipId, m_toolId, eqId);
			if (overwrite || formula.GetEquationText().empty())
			{
				double val;
				SvStl::MessageContainerVector  dummy;
				formula.ValidateEquation(rValues.at(adEnum), val, true, dummy);
			}
		}
	}
	return true;
}

bool ToolSizeController::Isallowedlocation(SvPb::SVExtentLocationPropertyEnum eAction, SvPb::SVExtentDirectionsEnum Direction)
{

	bool bAllowedAction = false;
	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	auto* pRequest = requestCmd.mutable_isallowedlocationrequest();
	pRequest->set_objectid(m_toolId);
	pRequest->set_location(eAction);
	pRequest->set_direction(Direction);

	HRESULT hr = SvCmd::InspectionCommands(m_ipId, requestCmd, &responseCmd);
	if (S_OK == hr && responseCmd.has_isallowedlocationresponse())
	{
		bAllowedAction = responseCmd.isallowedlocationresponse().isallowed();
	}
	return bAllowedAction;

}
}