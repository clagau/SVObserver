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

	HRESULT RangeValidator::IsFieldValid(SVString& rMsg, const SVString& rFieldName, const SVString& rValue)
	{
		HRESULT hr = S_OK;
	
		size_t len = rValue.size();

		if (!len)
		{
			rMsg = SvUl_SF::Format(SvOg::RangeValue_EmptyString, rFieldName.c_str());
			hr = E_POINTER;
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
					rMsg = SvUl_SF::Format(SvOg::RangeValue_WrongRange, rFieldName.c_str(), static_cast<int>(s_RangeMin), static_cast<int>(s_RangeMax));
					hr = E_INVALIDARG;
				}		
			}
		}
		return hr;	
	}

	HRESULT RangeValidator::Validate(SVString& msg,
									HINSTANCE resHandle,
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
		HRESULT retVal = S_OK;
	
		SVString sFailHigh(SvU::SVStringLoader(resHandle, static_cast<UINT>(IDS_FAIL_HIGH)));
		SVString sWarnHigh(SvU::SVStringLoader(resHandle, static_cast<UINT>(IDS_WARN_HIGH)));
		SVString sFailLow(SvU::SVStringLoader(resHandle, static_cast<UINT>(IDS_FAIL_LOW)));
		SVString sWarnLow(SvU::SVStringLoader(resHandle, static_cast<UINT>(IDS_WARN_LOW)));

		SVString InvalidRef(SvU::SVStringLoader(resHandle, static_cast<UINT>(IDS_ISANINVALID_REFERENCE)));
		SVString ToolSetName(SvU::SVStringLoader(resHandle, static_cast<UINT>(IDS_CLASSNAME_SVTOOLSET)));
	
		if (!FailHighIndirectValue.empty())
		{
			if (!isValidReference(rInspectionID, InspectionName, ToolSetName, FailHighIndirectValue))
			{
				//IDS_ISANINVALID_REFERENCE  "ERROR:\n%1: %2\nis an invalid reference."
				const std::vector<DWORD_PTR> args = boost::assign::list_of<DWORD_PTR>((DWORD_PTR)sFailHigh.c_str())((DWORD_PTR)FailHighIndirectValue.c_str());
				msg = FormatErrorMessage(InvalidRef, args);
				retVal =  -SvOi::Err_16018;
			}
		}
		if (S_OK == retVal && !WarnHighIndirectValue.empty())
		{
			if( !isValidReference(rInspectionID, InspectionName, ToolSetName, WarnHighIndirectValue))
			{
				//IDS_ISANINVALID_REFERENCE  "ERROR:\n%1: %2\nis an invalid reference."
				const std::vector<DWORD_PTR> args = boost::assign::list_of<DWORD_PTR>((DWORD_PTR)sWarnHigh.c_str())((DWORD_PTR)WarnHighIndirectValue.c_str());
				msg = FormatErrorMessage(InvalidRef, args);
				retVal = -SvOi::Err_16019;
			}
		}
		if (S_OK == retVal && !WarnLowIndirectValue.empty())
		{
			if (!isValidReference(rInspectionID, InspectionName, ToolSetName, WarnLowIndirectValue))
			{
				//IDS_ISANINVALID_REFERENCE  "ERROR:\n%1: %2\nis an invalid reference."
				const std::vector<DWORD_PTR> args = boost::assign::list_of<DWORD_PTR>((DWORD_PTR)sWarnLow.c_str())((DWORD_PTR)WarnLowIndirectValue.c_str());
				msg = FormatErrorMessage(InvalidRef, args);
				retVal = -SvOi::Err_16020;
			}
		}
		if (S_OK == retVal && !FailLowIndirectValue.empty())
		{
			if (!isValidReference(rInspectionID, InspectionName, ToolSetName, FailLowIndirectValue))
			{
				//IDS_ISANINVALID_REFERENCE  "ERROR:\n%1: %2\nis an invalid reference."
				const std::vector<DWORD_PTR> args = boost::assign::list_of<DWORD_PTR>((DWORD_PTR)sFailLow.c_str())((DWORD_PTR)FailLowIndirectValue.c_str());
				msg = FormatErrorMessage(InvalidRef, args);
				retVal = -SvOi::Err_16021;
			}
		}

		if (S_OK == retVal)
		{
			SVString LessThanErr(SvU::SVStringLoader(resHandle, static_cast<UINT>(IDS_IS_LESS_THAN)));

			if (FailHighIndirectValue.empty())
			{
				if (WarnHighIndirectValue.empty() && FailHighValue < WarnHighValue)
				{
					//IDS_IS_LESS_THAN			"ERROR:\n%1\nis less than\n%2"
					const std::vector<DWORD_PTR> args = boost::assign::list_of<DWORD_PTR>((DWORD_PTR)sFailHigh.c_str())((DWORD_PTR)sWarnHigh.c_str());
					msg = FormatErrorMessage(LessThanErr, args);
					retVal = -SvOi::Err_16012;
				}
				if (S_OK == retVal && WarnLowIndirectValue.empty() && FailHighValue < WarnLowValue)
				{
					//IDS_IS_LESS_THAN			"ERROR:\n%1\nis less than\n%2"
					const std::vector<DWORD_PTR> args = boost::assign::list_of<DWORD_PTR>((DWORD_PTR)sFailHigh.c_str())((DWORD_PTR)sWarnLow.c_str());
					msg = FormatErrorMessage(LessThanErr, args);
					retVal = -SvOi::Err_16013;
				}
				if (S_OK == retVal && FailLowIndirectValue.empty() && FailHighValue < FailLowValue)
				{
					//IDS_IS_LESS_THAN			"ERROR:\n%1\nis less than\n%2"
					const std::vector<DWORD_PTR> args = boost::assign::list_of<DWORD_PTR>((DWORD_PTR)sFailHigh.c_str())((DWORD_PTR)sFailLow.c_str());
					msg = FormatErrorMessage(LessThanErr, args);
					retVal = -SvOi::Err_16014;
				}
			}
			if (S_OK == retVal && WarnHighIndirectValue.empty())
			{
				if (WarnLowIndirectValue.empty() && WarnHighValue < WarnLowValue)
				{
					//IDS_IS_LESS_THAN			"ERROR:\n%1\nis less than\n%2"
					const std::vector<DWORD_PTR> args = boost::assign::list_of<DWORD_PTR>((DWORD_PTR)sWarnHigh.c_str())((DWORD_PTR)sWarnLow.c_str());
					msg = FormatErrorMessage(LessThanErr, args);
					retVal = -SvOi::Err_16015;
				}
				if (S_OK == retVal && FailLowIndirectValue.empty() && WarnHighValue < FailLowValue)
				{
					//IDS_IS_LESS_THAN			"ERROR:\n%1\nis less than\n%2"
					const std::vector<DWORD_PTR> args = boost::assign::list_of<DWORD_PTR>((DWORD_PTR)sWarnHigh.c_str())((DWORD_PTR)sFailLow.c_str());
					msg = FormatErrorMessage(LessThanErr, args);
					retVal = -SvOi::Err_16016;
				}
				if (S_OK == retVal && FailLowIndirectValue.empty() && WarnLowValue < FailLowValue)
				{
					//IDS_IS_LESS_THAN			"ERROR:\n%1\nis less than\n%2"
					const std::vector<DWORD_PTR> args = boost::assign::list_of<DWORD_PTR>((DWORD_PTR)sWarnLow.c_str())((DWORD_PTR)sFailLow.c_str());
					msg = FormatErrorMessage(LessThanErr, args);
					retVal = -SvOi::Err_16017;
				}
			}
		}
		return retVal;
	}
} /* namespace SVOGui */ } /* namespace Seidenader */