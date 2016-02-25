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
#include "ObjectInterfaces\IObjectInfoStruct.h"
#include "ObjectInterfaces\ISelectorItemVector.h"
#include "SVUtilityLibrary\SVSharedPtr.h"
#include "GuiCommands\GetSelectorList.h"
#include "ObjectSelectorLibrary\SelectorOptions.h"
#include "SVObjectLibrary\SVObjectSynchronousCommandTemplate.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace SVOGui
	{
		class AttributeAllowedFilter
		{
		private:
			UINT m_Attribute;

		public:
			AttributeAllowedFilter( const SvOsl::SelectorOptions& rOptions ): m_Attribute(rOptions.getAttributesFilter()) {}
			bool operator()(const SvOi::IObjectInfoStruct& rInfo, int ArrayIndex) const
			{
				bool Result(false);

				if( m_Attribute == 0 )
				{
					Result = true;
				}
				else
				{
					SvOi::IObjectClass* pObject = rInfo.getObject();
					if( nullptr != pObject)
					{
						if( 0 != (pObject->ObjectAttributesAllowed() & m_Attribute) )
						{
							Result = true;
						}
					}
				}

				return Result;
			}
		};

		class AttributeSetFilter
		{
		private:
			UINT m_Attribute;

		public:
			AttributeSetFilter( const SvOsl::SelectorOptions& rOptions ): m_Attribute(rOptions.getAttributesFilter()) {}
			bool operator()(const SvOi::IObjectInfoStruct& rInfo, int ArrayIndex) const
			{
				bool Result(false);

				if( m_Attribute == 0 )
				{
					Result = true;
				}
				else
				{
					SvOi::IObjectClass* pObject = rInfo.getObject();
					if( nullptr != pObject)
					{
						if( pObject->IsArray() )
						{
							if( 0 != (pObject->ObjectAttributesSet( ArrayIndex ) & m_Attribute) )
							{
								Result = true;
							}
						}
						else
						{
							if( 0 != (pObject->ObjectAttributesSet() & m_Attribute) )
							{
								Result = true;
							}
						}
					}
				}

				return Result;
			}
		};

		//Note the default filter uses the object attribute if in rOptions if it is 0 then all objects are listed
		template<typename Filter=AttributeAllowedFilter>
		class ToolSetItemSelector
		{
			typedef Filter FilterImpl;
		public:
			SvOi::ISelectorItemVectorPtr operator()( const SvOsl::SelectorOptions& rOptions )
			{
				FilterImpl filter( rOptions );
				SvOi::ISelectorItemVectorPtr SelectorList;

				typedef GuiCmd::GetSelectorList<SvOi::IsObjectInfoAllowed, SvOi::ISelectorItemVectorPtr> Command;
				typedef SVSharedPtr<Command> CommandPtr;
		
				SvOi::IsObjectInfoAllowed func = boost::bind(&FilterImpl::operator(), &filter, _1, _2);

				const SVGUID& rGuid = (rOptions.getInstanceID() != GUID_NULL) ? rOptions.getInstanceID() : rOptions.getInspectionID();
				CommandPtr commandPtr(new Command(rGuid, func, rOptions.getAttributesFilter(), rOptions.getWholeArray()));
				SVObjectSynchronousCommandTemplate<CommandPtr> cmd(rOptions.getInspectionID(), commandPtr);
				HRESULT hr = cmd.Execute(TWO_MINUTE_CMD_TIMEOUT);
				if (S_OK == hr)
				{
					SelectorList = commandPtr->GetResults( );
				}
				return SelectorList;
			}
		};
	}
}

namespace SvOg = Seidenader::SVOGui;