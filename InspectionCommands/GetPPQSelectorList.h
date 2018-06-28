//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the Command for getting the PPQ input names.
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <algorithm>
#include "ObjectInterfaces/IInspectionProcess.h"
#include "ObjectInterfaces/IObjectManager.h"
#include "SVContainerLibrary/SelectorItem.h"
#pragma endregion Includes

namespace SvCmd
{
	class GetPPQSelectorList
	{
	public:
		GetPPQSelectorList(const GetPPQSelectorList&) = delete;
		GetPPQSelectorList& operator=(const GetPPQSelectorList&) = delete;

		GetPPQSelectorList(SvCl::SelectorItemInserter Inserter, const GUID& rInstanceID, UINT Attribute) 
			: m_selectorInserter{Inserter}
			, m_instanceID(rInstanceID)
			, m_attribute(Attribute) {};

		virtual ~GetPPQSelectorList() {};

		// This method is where the real separation would occur by using sockets/named pipes/shared memory
		// The logic contained within this method would be moved to the "Server" side of a Client/Server architecture
		// and replaced with the building and sending of the command
		HRESULT Execute()
		{
			HRESULT hr = S_OK;

			SvOi::IInspectionProcess* pInspection = dynamic_cast<SvOi::IInspectionProcess*>(SvOi::getObject(m_instanceID));
			if (pInspection)
			{
				pInspection->GetPPQSelectorList(m_selectorInserter,  m_attribute);
			}
			return hr;
		}
		bool empty() const { return false; }

	private:
		GUID m_instanceID;
		UINT m_attribute;
		SvCl::SelectorItemInserter m_selectorInserter;
	};
} //namespace SvCmd
