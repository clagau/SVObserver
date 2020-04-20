//*****************************************************************************
/// \copyright (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
/// \file ObjectNameLookup.h
//*****************************************************************************
/// Class to get the name of the object corresponding to the graph vertex
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVContainerLibrary/IGraphNameLookup.h"
#include "SVObjectClass.h"
#include "SVObjectManagerClass.h"
#pragma endregion Includes

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
		//! \param id [in] reference to the Graph vertex id
		//! \returns the name of the vertex
		virtual LPCTSTR getVertexName(uint32_t id) const override
		{
			LPCTSTR pResult( nullptr );

			SVObjectClass* pObject( SVObjectManagerClass::Instance().GetObject(id) );
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
