//******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// ToolSetItemSelector is the function object to get the ToolSet Items for inclusion
/// in the object selector
//******************************************************************************

#pragma once

#pragma region Includes
#include <boost\bind.hpp>
#include "ObjectInterfaces\IOutputInfoListClass.h"
#include "SVUtilityLibrary\SVSharedPtr.h"
#include "GuiCommands\GetOutputList.h"
#include "SVObjectLibrary\SVObjectSynchronousCommandTemplate.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace SVOGui
	{
		struct AllowAll
		{
			AllowAll(const GUID& rInspectionID, const GUID& rInstanceID){}
			bool operator()(const SvOi::IObjectInfoStruct& rInfo) const { return true; }
		};

		template<bool bInstance=true, typename Filter=AllowAll>
		class ToolSetItemSelector
		{
			typedef Filter FilterImpl;
		public:
			SvOi::IOutputInfoListClassPtr operator()(const GUID& rInspectionID, const GUID& rInstanceID)
			{
				FilterImpl filter(rInspectionID, rInstanceID);
				SvOi::IOutputInfoListClassPtr outputInfoList;

				typedef GuiCmd::GetOutputList<SvOi::IsObjectInfoAllowed, SvOi::IOutputInfoListClassPtr> Command;
				typedef SVSharedPtr<Command> CommandPtr;
		
				SvOi::IsObjectInfoAllowed func = boost::bind(&FilterImpl::operator(), &filter, _1);

				const GUID& rGuid = (bInstance) ? rInstanceID : rInspectionID;
				CommandPtr commandPtr(new Command(rGuid, func));
				SVObjectSynchronousCommandTemplate<CommandPtr> cmd(rInspectionID, commandPtr);
				HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
				if (S_OK == hr)
				{
					outputInfoList = commandPtr->GetResults();
				}
				return outputInfoList;
			}
		};
	}
}

namespace SvOg = Seidenader::SVOGui;