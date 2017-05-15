//*****************************************************************************
/// \copyright (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
/// \file ObjectNameLookup.h
//*****************************************************************************
/// Class to get the name of the object corresponding to the graph vertex
//******************************************************************************
#pragma once

#include "SVContainerLibrary/IGraphNameLookup.h"
#include "SVUtilityLibrary/SVString.h"
#include "SVObjectManagerClass.h"
#include "SVObjectClass.h"

namespace SvOl
{
	class ObjectNameLookup : public SvCl::IGraphNameLookup
	{
	#pragma region Constructor
	public:
		explicit ObjectNameLookup() {};
		virtual ~ObjectNameLookup() {};
	#pragma endregion Constructor

	#pragma region Public Methods
	public:
		//! Gets the name of the graph vertex
		//! \param rGuid [in] reference to the Graph vertex Guid
		//! \returns the name of the vertex
		virtual LPCTSTR getVertexName( const SVGUID& rGuid ) const override
		{
			LPCTSTR pResult( nullptr );

			SVObjectClass* pObject( SVObjectManagerClass::Instance().GetObject(rGuid) );
			if( nullptr != pObject )
			{
				pResult = pObject->GetName();
				//If name is empty then get the object name
				if( nullptr == pResult || _T('\0') == *pResult )
				{
					pResult = pObject->GetObjectName();
				}
			}
			return pResult;
		}
	#pragma endregion Public Methods
	};
} //namespace SvOl
