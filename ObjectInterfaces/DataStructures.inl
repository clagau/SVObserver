//*****************************************************************************
/// \copyright (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// General data structures which are shared across projects
//******************************************************************************
#pragma once

namespace Seidenader { namespace ObjectInterfaces
{
	#pragma region Constructor
	GlobalConstantData::GlobalConstantData():
		m_AttributesAllowed(0)
		,m_Guid( SV_GUID_NULL )
		,m_Selected( false )
	{
	}
	#pragma endregion Constructor

	#pragma region Public Methods
	bool GlobalConstantData::operator<( const GlobalConstantData& Rhs ) const
	{
		bool Result( false );

		if( Rhs != *this )
		{
			//Need some constant value so use the address as these should not change during a compare
			Result = this < &Rhs;
		}
		return Result;
	}

	const GlobalConstantData& GlobalConstantData::operator=( const GlobalConstantData& Rhs )
	{
		m_Guid = Rhs.m_Guid;
		m_DottedName = Rhs.m_DottedName;
		m_Value = Rhs.m_Value;
		m_AttributesAllowed = Rhs.m_AttributesAllowed;
		m_Description = Rhs.m_Description;
		m_Selected = Rhs.m_Selected;
		return *this;
	}

	bool GlobalConstantData::operator==( const GlobalConstantData& Rhs ) const
	{
		bool Result( false );

		if( m_Guid == Rhs.m_Guid &&
			m_DottedName == Rhs.m_DottedName &&
			m_Value == Rhs.m_Value &&
			m_AttributesAllowed == Rhs.m_AttributesAllowed &&
			m_Description == Rhs.m_Description &&
			m_Selected == Rhs.m_Selected )
		{
			Result = true;
		}

		return Result;
	}
	#pragma endregion Public Methods

} /* namespace ObjectInterfaces */ } /* namespace Seidenader */


