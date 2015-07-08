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
		SVString LhsValue;
		SVString RhsValue;
		switch( m_Value.vt )
		{
		case VT_R8:
			LhsValue.Format(_T("%.06f"), m_Value.dblVal );
			break;
		case VT_BSTR:
			LhsValue = m_Value;
			break;
		default:
			break;
		}
		switch( Rhs.m_Value.vt )
		{
		case VT_R8:
			RhsValue.Format(_T("%.06f"), Rhs.m_Value.dblVal );
			break;
		case VT_BSTR:
			RhsValue = Rhs.m_Value;
			break;
		default:
			break;
		}
		Result = std::tie(m_Guid, m_DottedName, m_Value.vt, LhsValue, m_AttributesAllowed, m_Description, m_Selected) < 
				 std::tie(Rhs.m_Guid, Rhs.m_DottedName, Rhs.m_Value.vt, RhsValue, Rhs.m_AttributesAllowed, Rhs.m_Description, Rhs.m_Selected);
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


