//*****************************************************************************
/// \copyright (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// This is the data items to be used for the object selector
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVUtilityLibrary/SVGUID.h"
#pragma endregion Includes

namespace Seidenader { namespace ObjectSelectorLibrary
{
	struct SelectorOptions
	{
	#pragma region Constructor
	public:
		SelectorOptions( const SVGUID InspectionID, UINT AttributesFilter, const SVGUID InstanceID=GUID_NULL, bool WholeArray=false) :
		  m_InspectionID( InspectionID )
		, m_AttributesFilter( AttributesFilter )
		, m_InstanceID( InstanceID )
		, m_WholeArray( WholeArray )
		{};

		  virtual ~SelectorOptions() {};
	#pragma endregion Constructor

	#pragma region Public Methods
	public:
		//************************************
		//! The method gets the inspection GUID
		//! \return the GUID
		//************************************
		const SVGUID& getInspectionID() const { return m_InspectionID; };

		//************************************
		//! The method gets the Attributes filter
		//! \return filter
		//************************************
		UINT getAttributesFilter() const { return m_AttributesFilter; };

		//************************************
		//! The method gets the instance GUID
		//! \return the GUID
		//************************************
		const SVGUID& getInstanceID() const { return m_InstanceID; };

		//************************************
		//! The method gets the Whole array flag
		//! \return the flag
		//************************************
		bool getWholeArray() const { return m_WholeArray; };

	#pragma endregion Public Methods

	#pragma region Member Variables
	private:
		SVGUID				m_InspectionID;				//The inspection GUID
		SVGUID				m_InstanceID;				//The instance GUID
		UINT				m_AttributesFilter;			//The attributes filter
		bool				m_WholeArray;				//Insert whole arrays
	#pragma endregion Member Variables
	};

} /* namespace ObjectSelectorLibrary */ } /* namespace Seidenader */

namespace SvOsl = Seidenader::ObjectSelectorLibrary;
