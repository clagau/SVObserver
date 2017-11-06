//*****************************************************************************
/// \copyright (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// Global constant data structure for comparing
//******************************************************************************

namespace SvDef
{
#pragma region Constructor
	GlobalConstantData::GlobalConstantData():
		m_Guid( SV_GUID_NULL )
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
			LhsValue = SvUl_SF::Format(_T("%.06f"), m_Value.dblVal );
			break;
		case VT_BSTR:
			LhsValue = SvUl_SF::createSVString(m_Value);
			break;
		default:
			break;
		}
		switch( Rhs.m_Value.vt )
		{
		case VT_R8:
			RhsValue = SvUl_SF::Format(_T("%.06f"), Rhs.m_Value.dblVal );
			break;
		case VT_BSTR:
			RhsValue = SvUl_SF::createSVString(Rhs.m_Value);
			break;
		default:
			break;
		}
		Result = std::tie(m_Guid, m_DottedName, m_Value.vt, LhsValue, m_Description, m_Selected) < 
					std::tie(Rhs.m_Guid, Rhs.m_DottedName, Rhs.m_Value.vt, RhsValue, Rhs.m_Description, Rhs.m_Selected);
		return Result;
	}

	const GlobalConstantData& GlobalConstantData::operator=( const GlobalConstantData& Rhs )
	{
		m_Guid = Rhs.m_Guid;
		m_DottedName = Rhs.m_DottedName;
		m_Value = Rhs.m_Value;
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
			m_Description == Rhs.m_Description &&
			m_Selected == Rhs.m_Selected )
		{
			Result = true;
		}

		return Result;
	}
#pragma endregion Public Methods
} //namespace SvOi
