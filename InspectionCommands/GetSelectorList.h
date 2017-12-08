//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the Command for getting the Task Object's Output list (for the object selector).
//******************************************************************************
#pragma once

#pragma region Includes
#include "ObjectInterfaces/IObjectManager.h"
#include "ObjectInterfaces/IObjectClass.h"
#include "ObjectInterfaces/ITaskObject.h"
#include "ObjectInterfaces/IInspectionProcess.h"
#include "SVContainerLibrary/SelectorItem.h"
#include "AttributesAllowedFilter.h"
#include "AttributesSetFilter.h"
#include "RangeSelectorFilter.h"
#include "MLRejectValueFilter.h"
#pragma endregion Includes

#pragma region Declarations
//! Declaration is in SVUtilityLibrary\SVGUID.h
class SVGUID;
#pragma endregion Declarations

namespace SvCmd
{
	enum SelectorFilterTypeEnum
	{
		AttributesAllowedFilterType = 1,
		AttributesSetFilterType,
		RangeSelectorFilterType,
		MLRejectValueFilterType

	};
	
	class GetSelectorList
	{
	public:
		GetSelectorList(const GetSelectorList&) = delete;
		GetSelectorList& operator=(const GetSelectorList&) = delete;

		GetSelectorList(const SVGUID& rInstanceID, SelectorFilterTypeEnum filterType, UINT Attribute, bool WholeArray)
			: m_InstanceID(rInstanceID)
			, m_filterType(filterType)
			, m_Attribute(Attribute)
			, m_WholeArray(WholeArray)
		{};

		virtual ~GetSelectorList() {};

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
					pTaskObject = dynamic_cast<SvOi::ITaskObject *>(pObject->GetAncestorInterface(SvDef::SVToolSetObjectType));
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
						std::string name;
						hr = pObject->GetCompleteNameToType(SvDef::SVToolObjectType, name);
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
					case  MLRejectValueFilterType:
					{
						MLRejectValueFilter filter;
						SvOi::IsObjectInfoAllowed func = boost::bind(&MLRejectValueFilter::operator(), &filter, _1, _2, _3);
						m_SelectedList = pTaskObject->GetSelectorList(func, m_Attribute, m_WholeArray);
						hr = S_OK;
					}
					break;
					default:
					{
						hr = E_INVALIDARG;
					}
					break;
					}
				}
			}
			return hr;
		}
		bool empty() const { return false; }
		const SvCl::SelectorItemVectorPtr& GetResults() const { return m_SelectedList; }

	private:
		SVGUID m_InstanceID;
		SelectorFilterTypeEnum m_filterType;
		UINT m_Attribute;
		bool m_WholeArray;
		SvCl::SelectorItemVectorPtr m_SelectedList;
	};
} //namespace SvCmd

