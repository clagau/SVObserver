//*****************************************************************************
/// \copyright (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// General data structures which are shared across projects
//******************************************************************************
#pragma once

#pragma region Includes
#include <set>
#include <vector>
#include <tuple> 
#include "SVUtilityLibrary\SVString.h"
#include "SVUtilityLibrary\SVGUID.h"
#pragma endregion Includes


namespace Seidenader { namespace ObjectInterfaces
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
		inline GlobalConstantData();

	#pragma region Public Methods
	public:
		inline bool operator<( const GlobalConstantData& Rhs ) const;
		inline const GlobalConstantData& operator=( const GlobalConstantData& Rhs );
		inline bool operator==( const GlobalConstantData& Rhs ) const;
		inline bool operator!=( const GlobalConstantData& Rhs ) const { return !( Rhs == *this ); };
	#pragma endregion Public Methods

	#pragma region Member variables
	public:
		SVGUID		m_Guid;						//The object unique ID or GUID_NULL
		SVString	m_DottedName;				//The dotted name of the constant
		_variant_t	m_Value;					//The value of the constant
		SVString	m_Description;				//The description of the constant
		bool		m_Selected;					//To determine if this has been selected
	#pragma endregion Member variables
	};

	typedef std::set<GlobalConstantData> GlobalConstantDataSet;
	typedef std::pair< GlobalConstantData, GlobalConstantData > GlobalConflictPair;
	typedef std::vector< GlobalConflictPair > GlobalConflictPairVector;

} /* namespace ObjectInterfaces */ } /* namespace Seidenader */


#pragma region Inline
#include "DataStructures.inl"
#pragma endregion Inline

namespace SvOi = Seidenader::ObjectInterfaces;