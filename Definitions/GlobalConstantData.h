//*****************************************************************************
/// \copyright (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// Global constant data structure for comparing
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <set>
//Moved to precompiled header: #include <vector>
//Moved to precompiled header: #include <tuple>
#include "SVUtilityLibrary\SVGUID.h"
#pragma endregion Includes


namespace SvDef
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
		inline bool operator<(const GlobalConstantData& Rhs) const;
		inline const GlobalConstantData& operator=(const GlobalConstantData& Rhs);
		inline bool operator==(const GlobalConstantData& Rhs) const;
		inline bool operator!=(const GlobalConstantData& Rhs) const { return !(Rhs == *this); };
#pragma endregion Public Methods

#pragma region Member variables
	public:
		SVGUID		m_Guid{GUID_NULL};			//The object unique ID or GUID_NULL
		std::string	m_DottedName;				//The dotted name of the constant
		_variant_t	m_Value;					//The value of the constant
		std::string	m_Description;				//The description of the constant
		bool		m_Selected{false};			//To determine if this has been selected
#pragma endregion Member variables
	};

	typedef std::set<GlobalConstantData> GlobalConstantDataSet;
	typedef std::pair< GlobalConstantData, GlobalConstantData > GlobalConflictPair;
	typedef std::vector< GlobalConflictPair > GlobalConflictPairVector;

} //namespace SvDef

#pragma region Inline
#include "GlobalConstantData.inl"
#pragma endregion Inline

