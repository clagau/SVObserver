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
#include "ObjectInterfaces/IObjectClass.h"
#include "ObjectInterfaces/IObjectManager.h"
#pragma endregion Includes

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
		//! \param id [in] reference to the Graph vertex id
		//! \returns the name of the vertex
		virtual LPCTSTR getVertexName( uint32_t id ) const override
		{
			LPCTSTR pResult( nullptr );

			SvOi::IObjectClass* pObject(SvOi::getObject(id));
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
		mutable std::string m_LabelName;
	#pragma endregion Member Variables
	};
} //namespace SvOl
