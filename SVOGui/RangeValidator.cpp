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
#include "ObjectInterfaces\ErrorNumbers.h"
#include "GuiCommands\GetInstanceIDByDottedName.h"
#include "GuiCommands\GetAttributesAllowed.h"
#include "SVObjectLibrary\SVObjectSynchronousCommandTemplate.h"
#include "SVUtilityLibrary\SVStringLoader.h"
#include "TextDefinesSvOg.h"
#include "ObjectInterfaces\MessageTextEnum.h"
#include "SVStatusLibrary\MessageContainer.h"
#include "SVMessage\SVMessage.h"
#pragma endregion Includes

namespace Seidenader { namespace SVOGui
{
	static SVString FormatErrorMessage(const SVString& rMsg, const std::vector<DWORD_PTR>& rArgList)
	{
		char buf[512];
		FormatMessageA(FORMAT_MESSAGE_FROM_STRING | FORMAT_MESSAGE_ARGUMENT_ARRAY, rMsg.c_str(), 0, 0, buf, sizeof(buf), (va_list*)&rArgList[0]);
		return SVString(buf);
	}

	static bool isValidReference(const GUID& rInspectionID, const SVString& rInspectionName, const SVString& ToolSetName, const SVString& rIndirectString)
	{
		bool bRetVal = false;
		// Build name (prepend inspectionname and tool set name if not present)
		// Send Command to do lookup
		SVString dottedName;
	
		//If the tool set name is not at the start then add the inspection name and ".Tool Set" at the beginning
		if (SVString::npos != rIndirectString.find(ToolSetName.c_str()))
		{
			dottedName = rInspectionName + "." + rIndirectString;
		}
		else
		{
			dottedName = rIndirectString;
		}
	
		typedef GuiCmd::GetInstanceIDFromDottedName Command;
		typedef SVSharedPtr<Command> CommandPtr;
	
		CommandPtr commandPtr(new Command(dottedName));
		SVObjectSynchronousCommandTemplate<CommandPtr> cmd(rInspectionID, commandPtr);
		HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
		if (S_OK == hr)
		{
			const GUID& rGuid = commandPtr->GetInstanceID();
			if (GUID_NULL != rGuid)
			{
				typedef GuiCmd::GetAttributesAllowed Command;
				typedef SVSharedPtr<Command> CommandPtr;
				CommandPtr commandPtr(new Command(rGuid));
				SVObjectSynchronousCommandTemplate<CommandPtr> cmd(rInspectionID, commandPtr);
				HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
				if (S_OK == hr)
				{
					if (commandPtr->AttributesAllowed() & SV_SELECTABLE_FOR_EQUATION)
					{
						bRetVal = true;
					}
				}
			}
		}
		return bRetVal;
	}

	void RangeValidator::IsFieldValid(SvOi::MessageTextEnum fieldName, const SVString& rValue)
	{
		size_t len = rValue.size();

		if (!len)
		{
			SVStringArray msgList;
			msgList.push_back(SvStl::MessageData::convertId2AddtionalText(fieldName));
			SvStl::MessageContainer message(SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_RangeValue_EmptyString, msgList, StdMessageParams, SvOi::Err_10229);
			throw message;
		}
		else
		{
			double val = 0.0;
			SVString tmp = rValue;
			bool isNumber = SvUl_SF::Convert2Number<double>(tmp, val, true);
			if (isNumber)
			{
				const double s_RangeMax = 17000000;
				const double s_RangeMin = -s_RangeMax;
				if (val > s_RangeMax || val < s_RangeMin)
				{
					SVStringArray msgList;
					msgList.push_back(SvStl::MessageData::convertId2AddtionalText(fieldName));
					msgList.push_back(SvUl_SF::Format(_T("%d"), static_cast<int>(s_RangeMin)));
					msgList.push_back(SvUl_SF::Format(_T("%d"), static_cast<int>(s_RangeMax)));
					SvStl::MessageContainer message(SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_RangeValue_WrongRange, msgList, StdMessageParams, SvOi::Err_10230);
					throw message;
				}		
			}
		}
	}

	void RangeValidator::Validate(HINSTANCE resHandle,
									const SVString& InspectionName, 
									const SVString& FailHighIndirectValue, 
									const SVString& FailLowIndirectValue, 
									const SVString& WarnHighIndirectValue, 
									const SVString& WarnLowIndirectValue, 
									double FailHighValue, 
									double FailLowValue, 
									double WarnHighValue, 
									double WarnLowValue,
									const GUID& rInspectionID)
	{
		SVString ToolSetName(SvU::SVStringLoader(resHandle, static_cast<UINT>(IDS_CLASSNAME_SVTOOLSET)));
	
		if (!FailHighIndirectValue.empty())
		{
			if (!isValidReference(rInspectionID, InspectionName, ToolSetName, FailHighIndirectValue))
			{
				SVStringArray messageList;
				messageList.push_back(SvStl::MessageData::convertId2AddtionalText(SvOi::Tid_FailHigh));
				messageList.push_back(FailHighIndirectValue);
				SvStl::MessageContainer message(SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_IsInvalidRef, messageList, StdMessageParams, SvOi::Err_16018);
				throw message;
			}
		}
		if (!WarnHighIndirectValue.empty())
		{
			if( !isValidReference(rInspectionID, InspectionName, ToolSetName, WarnHighIndirectValue))
			{
				SVStringArray messageList;
				messageList.push_back(SvStl::MessageData::convertId2AddtionalText(SvOi::Tid_WarnHigh));
				messageList.push_back(WarnHighIndirectValue);
				SvStl::MessageContainer message(SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_IsInvalidRef, messageList, StdMessageParams, SvOi::Err_16019);
				throw message;
			}
		}
		if (!WarnLowIndirectValue.empty())
		{
			if (!isValidReference(rInspectionID, InspectionName, ToolSetName, WarnLowIndirectValue))
			{
				SVStringArray messageList;
				messageList.push_back(SvStl::MessageData::convertId2AddtionalText(SvOi::Tid_WarnLow));
				messageList.push_back(WarnLowIndirectValue);
				SvStl::MessageContainer message(SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_IsInvalidRef, messageList, StdMessageParams, SvOi::Err_16020);
				throw message;
			}
		}
		if (!FailLowIndirectValue.empty())
		{
			if (!isValidReference(rInspectionID, InspectionName, ToolSetName, FailLowIndirectValue))
			{
				SVStringArray messageList;
				messageList.push_back(SvStl::MessageData::convertId2AddtionalText(SvOi::Tid_FailLow));
				messageList.push_back(FailLowIndirectValue);
				SvStl::MessageContainer message(SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_IsInvalidRef, messageList, StdMessageParams, SvOi::Err_16021);
				throw message;
			}
		}

		if (FailHighIndirectValue.empty())
		{
			if (WarnHighIndirectValue.empty() && FailHighValue < WarnHighValue)
			{
				SVStringArray messageList;
				messageList.push_back(SvStl::MessageData::convertId2AddtionalText(SvOi::Tid_FailHigh));
				messageList.push_back(SvStl::MessageData::convertId2AddtionalText(SvOi::Tid_WarnHigh));
				SvStl::MessageContainer message(SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_IsLessThan, messageList, StdMessageParams, SvOi::Err_16012);
				throw message;
			}
			if (WarnLowIndirectValue.empty() && FailHighValue < WarnLowValue)
			{
				SVStringArray messageList;
				messageList.push_back(SvStl::MessageData::convertId2AddtionalText(SvOi::Tid_FailHigh));
				messageList.push_back(SvStl::MessageData::convertId2AddtionalText(SvOi::Tid_WarnLow));
				SvStl::MessageContainer message(SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_IsLessThan, messageList, StdMessageParams, SvOi::Err_16013);
				throw message;
			}
			if (FailLowIndirectValue.empty() && FailHighValue < FailLowValue)
			{
				SVStringArray messageList;
				messageList.push_back(SvStl::MessageData::convertId2AddtionalText(SvOi::Tid_FailHigh));
				messageList.push_back(SvStl::MessageData::convertId2AddtionalText(SvOi::Tid_FailLow));
				SvStl::MessageContainer message(SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_IsLessThan, messageList, StdMessageParams, SvOi::Err_16014);
				throw message;
			}
		}
		if (WarnHighIndirectValue.empty())
		{
			if (WarnLowIndirectValue.empty() && WarnHighValue < WarnLowValue)
			{
				SVStringArray messageList;
				messageList.push_back(SvStl::MessageData::convertId2AddtionalText(SvOi::Tid_WarnHigh));
				messageList.push_back(SvStl::MessageData::convertId2AddtionalText(SvOi::Tid_WarnLow));
				SvStl::MessageContainer message(SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_IsLessThan, messageList, StdMessageParams, SvOi::Err_16015);
				throw message;
			}
			if (FailLowIndirectValue.empty() && WarnHighValue < FailLowValue)
			{
				SVStringArray messageList;
				messageList.push_back(SvStl::MessageData::convertId2AddtionalText(SvOi::Tid_WarnHigh));
				messageList.push_back(SvStl::MessageData::convertId2AddtionalText(SvOi::Tid_FailLow));
				SvStl::MessageContainer message(SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_IsLessThan, messageList, StdMessageParams, SvOi::Err_16016);
				throw message;
			}
			if (FailLowIndirectValue.empty() && WarnLowValue < FailLowValue)
			{
				SVStringArray messageList;
				messageList.push_back(SvStl::MessageData::convertId2AddtionalText(SvOi::Tid_WarnLow));
				messageList.push_back(SvStl::MessageData::convertId2AddtionalText(SvOi::Tid_FailLow));
				SvStl::MessageContainer message(SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_IsLessThan, messageList, StdMessageParams, SvOi::Err_16017);
				throw message;
			}
		}
	}
} /* namespace SVOGui */ } /* namespace Seidenader */