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

#pragma region Declarations
//! Declaration is in SVUtilityLibrary\SVGUID.h
class SVGUID;
#pragma endregion Declarations

namespace SvCmd
{
	class GetPPQSelectorList
	{
	public:
		GetPPQSelectorList(const GetPPQSelectorList&) = delete;
		GetPPQSelectorList& operator=(const GetPPQSelectorList&) = delete;

		GetPPQSelectorList(const SVGUID& rInstanceID, UINT Attribute) : m_InstanceID(rInstanceID), m_Attribute(Attribute) {};

		virtual ~GetPPQSelectorList() {};

		// This method is where the real separation would occur by using sockets/named pipes/shared memory
		// The logic contained within this method would be moved to the "Server" side of a Client/Server architecture
		// and replaced with the building and sending of the command
		HRESULT Execute()
		{
			HRESULT hr = S_OK;

			SvOi::IInspectionProcess* pInspection = dynamic_cast<SvOi::IInspectionProcess*>(SvOi::getObject(m_InstanceID));
			if (pInspection)
			{
				m_SelectorList = pInspection->GetPPQSelectorList( m_Attribute );
			}
			return hr;
		}
		bool empty() const { return false; }
		SvCl::SelectorItemVectorPtr GetResults() const { return m_SelectorList; }

	private:
		SVGUID m_InstanceID;
		UINT m_Attribute;
		SvCl::SelectorItemVectorPtr m_SelectorList;
	};
} //namespace SvCmd
