
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// RangeSelectorFilter is the function object used to filter the output list 
/// for the object selector
//******************************************************************************
#pragma once
#include "stdafx.h"
#include <algorithm>
#include <boost\assign\list_of.hpp>
#include "RangeSelectorFilter.h"
#include "ObjectInterfaces\ObjectDefines.h"
#include "GuiCommands\GetCompleteObjectName.h"
#include "SVObjectLibrary\SVObjectSynchronousCommandTemplate.h"

namespace Seidenader { namespace SVOGui
{
	static SVString GetToolName( const SvOsl::SelectorOptions& rOptions )
	{
		SVString name;
		typedef GuiCmd::GetCompleteObjectName Command;
		typedef SVSharedPtr<Command> CommandPtr;

		CommandPtr commandPtr(new Command(rOptions.getInstanceID(), SVToolObjectType));
		SVObjectSynchronousCommandTemplate<CommandPtr> cmd(rOptions.getInspectionID(), commandPtr);
		HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
		if (S_OK == hr)
		{
			name = commandPtr->GetName();
		}
		return name;
	}

	RangeSelectorFilter::RangeSelectorFilter( const SvOsl::SelectorOptions& rOptions ) :
	  m_rOptions( rOptions )
	, m_filter(boost::assign::list_of
			(SVDWordValueObjectType)
			(SVLongValueObjectType)
			(SVDoubleValueObjectType)
			(SVBoolValueObjectType)
			(SVPointValueObjectType)
			(SVByteValueObjectType))
	{
		m_excludePath = GetToolName(m_rOptions);
	}

	static bool IsExcluded(SVObjectTypeEnum type, const std::set<SVObjectTypeEnum>& rFilter)
	{
		return (rFilter.size() > 0 && rFilter.end() == rFilter.find(type));
	}

	static bool IsViewable( UINT attributesFilter, UINT attributesAllowed)
	{
		return 0 != (attributesFilter & attributesAllowed ) ;
	}

	static bool IsSameLinage(const SVString& name, const SVString& excludedPath)
	{
		bool bSame = false; 
		size_t len = excludedPath.size();
		if (len > 0)
		{
			bSame = (0 ==  name.Left(len).Compare(excludedPath));
		}
		return bSame;
	}

	static bool IsAllowed(SVObjectTypeEnum type, UINT attributesFilter, UINT attributesAllowed, const SVString& name, const std::set<SVObjectTypeEnum>& filter, const SVString& excludePath)
	{
		return (IsViewable(attributesFilter, attributesAllowed) && !IsSameLinage(name, excludePath) && !IsExcluded(type, filter));
	}

	bool RangeSelectorFilter::operator()(const SvOi::IObjectInfoStruct& rInfo, int ArrayIndex) const
	{
		bool bRetVal = false;
		SvOi::IObjectClass* pObject = rInfo.getObject();
		if (pObject)
		{
			const SVObjectTypeEnum& type = pObject->GetObjectType();
			const UINT attributesAllowed = pObject->ObjectAttributesAllowed();
			const SVString& name = pObject->GetCompleteName();
			bRetVal = IsAllowed(type, m_rOptions.getAttributesFilter(), attributesAllowed, name, m_filter, m_excludePath);
		}
		return bRetVal;
	}
} /* namespace SVOGui */ } /* namespace Seidenader */