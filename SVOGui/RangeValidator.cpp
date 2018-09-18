//******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// RangeValidator is the function object used to Validate the range variable(s)
//******************************************************************************
#pragma region Includes
#include "Stdafx.h"
//Moved to precompiled header: #include <boost\assign\list_of.hpp>
#include "SVOResource/resource.h"
#include "RangeValidator.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "InspectionCommands\GetInstanceIDByDottedName.h"
#include "InspectionCommands\GetAttributesAllowed.h"
#include "SVCommandLibrary\SVObjectSynchronousCommandTemplate.h"
#include "TextDefinesSvOg.h"
#include "SVStatusLibrary/MessageTextEnum.h"
#include "SVStatusLibrary\MessageContainer.h"
#include "SVMessage\SVMessage.h"
#include "Definitions/ObjectDefines.h"
#include "Definitions/StringTypeDef.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

namespace SvOg
{
	static std::string FormatErrorMessage(const std::string& rMsg, const std::vector<DWORD_PTR>& rArgList)
	{
		char buf[512];
		FormatMessageA(FORMAT_MESSAGE_FROM_STRING | FORMAT_MESSAGE_ARGUMENT_ARRAY, rMsg.c_str(), 0, 0, buf, sizeof(buf), (va_list*)&rArgList[0]);
		return std::string(buf);
	}

	static bool isValidReference(const GUID& rInspectionID, const std::string& rInspectionName, const std::string& ToolSetName, const std::string& rIndirectString)
	{
		bool bRetVal = false;
		// Build name (prepend inspectionname and tool set name if not present)
		// Send Command to do lookup
		std::string dottedName;
	
		//If the tool set name is not at the start then add the inspection name and ".Tool Set" at the beginning
		if (std::string::npos != rIndirectString.find(ToolSetName.c_str()))
		{
			dottedName = rInspectionName + "." + rIndirectString;
		}
		else
		{
			dottedName = rIndirectString;
		}
	
		typedef SvCmd::GetInstanceIDFromDottedName Command;
		typedef std::shared_ptr<Command> CommandPtr;
	
		CommandPtr commandPtr(new Command(dottedName));
		SVObjectSynchronousCommandTemplate<CommandPtr> cmd(rInspectionID, commandPtr);
		HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
		if (S_OK == hr)
		{
			const GUID& rGuid = commandPtr->GetInstanceID();
			if (GUID_NULL != rGuid)
			{
				typedef SvCmd::GetAttributesAllowed Command;
				typedef std::shared_ptr<Command> CommandPtr;
				CommandPtr commandPtr(new Command(rGuid));
				SVObjectSynchronousCommandTemplate<CommandPtr> cmd(rInspectionID, commandPtr);
				HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
				if (S_OK == hr)
				{
					if (commandPtr->AttributesAllowed() & SvDef::SV_SELECTABLE_FOR_EQUATION)
					{
						bRetVal = true;
					}
				}
			}
		}
		return bRetVal;
	}

	void RangeValidator::IsFieldValid(SvStl::MessageTextEnum fieldName, const std::string& rValue)
	{
		size_t len = rValue.size();

		if (!len)
		{
			SvDef::StringVector msgList;
			msgList.push_back(SvStl::MessageData::convertId2AddtionalText(fieldName));
			SvStl::MessageContainer message(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_RangeValue_EmptyString, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10229);
			throw message;
		}
		else
		{
			double val = 0.0;
			std::string tmp = rValue;
			bool isNumber = SvUl::Convert2Number<double>(tmp, val, true);
			if (isNumber)
			{
				const double s_RangeMax = 17000000;
				const double s_RangeMin = -s_RangeMax;
				if (val > s_RangeMax || val < s_RangeMin)
				{
					SvDef::StringVector msgList;
					msgList.push_back(SvStl::MessageData::convertId2AddtionalText(fieldName));
					msgList.push_back(SvUl::Format(_T("%d"), static_cast<int>(s_RangeMin)));
					msgList.push_back(SvUl::Format(_T("%d"), static_cast<int>(s_RangeMax)));
					SvStl::MessageContainer message(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_RangeValue_WrongRange, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10230);
					throw message;
				}		
			}
		}
	}
} //namespace SvOg
