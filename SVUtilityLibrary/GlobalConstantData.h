//*****************************************************************************
/// \copyright (c) 2015,2015 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved 
//*****************************************************************************
/// Global constant data structure for comparing
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <set>
//Moved to precompiled header: #include <vector>
//Moved to precompiled header: #include <tuple>
#include "Definitions/ObjectDefines.h"
#pragma endregion Includes


namespace SvUl
{
	struct GlobalConstantData
	{
#pragma region Declarations
	public:
		enum DataTypeEnum
		{
			DecimalType,
			TextType,
			DataTypeMax
		};

#pragma endregion Declarations

#pragma region Constructor
	public:
		GlobalConstantData() = default;
		virtual ~GlobalConstantData() = default;

#pragma region Public Methods
	public:
		bool operator<(const GlobalConstantData& Rhs) const;
		const GlobalConstantData& operator=(const GlobalConstantData& Rhs);
		bool operator==(const GlobalConstantData& Rhs) const;
		bool operator!=(const GlobalConstantData& Rhs) const { return !(Rhs == *this); };
#pragma endregion Public Methods

#pragma region Member variables
	public:
		uint32_t	m_objectId{ SvDef::InvalidObjectId };		//The object unique ID or SvDef::InvalidObjectId
		std::string	m_DottedName;				//The dotted name of the constant
		_variant_t	m_Value;					//The value of the constant
		std::string	m_Description;				//The description of the constant
		bool		m_Selected{false};			//To determine if this has been selected
#pragma endregion Member variables
	};

	typedef std::pair< GlobalConstantData, GlobalConstantData > GlobalConflictPair;
	typedef std::vector< GlobalConflictPair > GlobalConflictPairVector;

} //namespace SvUl

