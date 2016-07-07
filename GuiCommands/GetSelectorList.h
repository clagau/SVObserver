//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the Command for getting the Task Object's Output list (for the object selector).
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <boost/noncopyable.hpp>
//Moved to precompiled header: #include <Guiddef.h>
#include "ObjectInterfaces/IObjectManager.h"
#include "SVUtilityLibrary/SVString.h"
#include "ObjectInterfaces/IObjectClass.h"
#include "ObjectInterfaces/ITaskObject.h"
#include "ObjectInterfaces/IInspectionProcess.h"
#include "AttributesAllowedFilter.h"
#include "AttributesSetFilter.h"
#include "RangeSelectorFilter.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace GuiCommand
	{
		enum SelectorFilterTypeEnum
		{
			AttributesAllowedFilterType = 1,
			AttributesSetFilterType,
			RangeSelectorFilterType
		};

		template <typename Results>
		struct GetSelectorList : public boost::noncopyable
		{
			GetSelectorList(const GUID& rInstanceID, SelectorFilterTypeEnum filterType, UINT Attribute, bool WholeArray) 
			: m_InstanceID(rInstanceID)
			, m_filterType(filterType)
			, m_Attribute(Attribute)
			, m_WholeArray(WholeArray) 
			{}

			// This method is where the real separation would occur by using sockets/named pipes/shared memory
			// The logic contained within this method would be moved to the "Server" side of a Client/Server architecture
			// and replaced with the building and sending of the command
			HRESULT Execute()
			{
				HRESULT hr = E_POINTER;

				SvOi::IObjectClass* pObject = SvOi::getObject(m_InstanceID);
				if (pObject)
				{
					SvOi::ITaskObject* pTaskObject = dynamic_cast<SvOi::ITaskObject *>(pObject);

					// When using the RangeSelectorFilter, the InstanceId is for the Range or Tool owning the Range
					// which is needed to get the name for exclusion in filtering, so get the Toolset as well 
					if (RangeSelectorFilterType == m_filterType)
					{
						pTaskObject = dynamic_cast<SvOi::ITaskObject *>(pObject->GetAncestorInterface(SVToolSetObjectType));
					}

					if (nullptr == pTaskObject)
					{
						SvOi::IInspectionProcess* pInspection = dynamic_cast<SvOi::IInspectionProcess *>(pObject);
						if (pInspection)
						{
							pTaskObject = pInspection->GetToolSetInterface();
						}
					}

					if (nullptr != pTaskObject)
					{
						switch (m_filterType)
						{
						case RangeSelectorFilterType:
							{
								SVString name;
								hr = pObject->GetCompleteNameToType(SVToolObjectType, name);
								if (S_OK == hr)
								{
									RangeSelectorFilter filter(name);
									SvOi::IsObjectInfoAllowed func = boost::bind(&RangeSelectorFilter::operator(), &filter, _1, _2, _3);
									m_SelectedList = pTaskObject->GetSelectorList(func, m_Attribute, m_WholeArray);
									hr = S_OK;
								}
							}
							break;

						case AttributesAllowedFilterType:
							{
								AttributesAllowedFilter filter;
								SvOi::IsObjectInfoAllowed func = boost::bind(&AttributesAllowedFilter::operator(), &filter, _1, _2, _3);
								m_SelectedList = pTaskObject->GetSelectorList(func, m_Attribute, m_WholeArray);
								hr = S_OK;
							}
							break;

						case AttributesSetFilterType:
							{
								AttributesSetFilter filter;
								SvOi::IsObjectInfoAllowed func = boost::bind(&AttributesSetFilter::operator(), &filter, _1, _2, _3);
								m_SelectedList = pTaskObject->GetSelectorList(func, m_Attribute, m_WholeArray);
								hr = S_OK;
							}
							break;

						default:
							hr = E_INVALIDARG;
							break;
						}
					}
				}
				return hr;
			}
			bool empty() const { return false; }
			const Results& GetResults() const { return m_SelectedList; }

		private:
			GUID m_InstanceID;
			SelectorFilterTypeEnum m_filterType;
			UINT m_Attribute;
			bool m_WholeArray;
			Results m_SelectedList;
		};
	}
}

namespace GuiCmd = Seidenader::GuiCommand;
