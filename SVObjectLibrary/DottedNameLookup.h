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
	class DottedNameLookup : public SvCl::IGraphNameLookup
	{
	#pragma region Constructor
	public:
		explicit DottedNameLookup() {};
		virtual ~DottedNameLookup() {};
	#pragma endregion Constructor

	#pragma region Public Methods
	public:
		//! Gets the dotted name of the graph vertex
		//! \param rGuid [in] reference to the Graph vertex Guid
		//! \returns the name of the vertex
		virtual LPCTSTR getVertexName( const SVGUID& rGuid ) const override
		{
			LPCTSTR pResult( nullptr );

			SVObjectClass* pObject( SVObjectManagerClass::Instance().GetObject(rGuid) );
			if( nullptr != pObject )
			{
				m_LabelName = pObject->GetCompleteName();
				pResult = m_LabelName.c_str();
			}
			return pResult;
		}
	#pragma endregion Public Methods

	#pragma region Member Variables
	private:
		mutable SVString m_LabelName;
	#pragma endregion Member Variables
	};
} //namespace SvOl
