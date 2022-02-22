//******************************************************************************
/// COPYRIGHT (c) 2015 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved
//******************************************************************************
/// Interface class for ExternalToolTask
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVProtoBuf/InspectionCommands.h"
#include <Windows.h>
#include <winerror.h>
#include <functional>
#pragma endregion Includes

namespace SvOi
{
	class IExternalToolTaskDataAdmin;
	class IObjectClass;
}

namespace SvOi
{
	class IExternalToolTask 
	{

	public:
		virtual ~IExternalToolTask() {}

		/// data admin access
		virtual IExternalToolTaskDataAdmin& getExternalToolDataAdmin() = 0;

		virtual HRESULT triggerInitialize(std::vector<std::string>& status, bool inCreationProcess = false, bool initializeAll = false) = 0;
		
		virtual void SetAllAttributes() = 0;
		virtual HRESULT ClearData() = 0;

		virtual HRESULT validateValueParameter(uint32_t taskObjectId, long index, _variant_t newVal, SvPb::ExDllInterfaceType ediType) = 0;
		virtual std::string getDllMessageString(long hResultError) const = 0;

		virtual SvOi::IObjectClass* getResultRangeObjectAtIndex(int index) = 0;

		virtual SvPb::GetImageInfoExternalToolResponse getImageInfoList() const = 0;
	};

}