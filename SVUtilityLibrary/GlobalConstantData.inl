//*****************************************************************************
/// \copyright (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// Global constant data structure for comparing
//******************************************************************************

#pragma region Includes
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

namespace SvUl
{
#pragma region Public Methods
	bool GlobalConstantData::operator<( const GlobalConstantData& Rhs ) const
	{
		bool Result( false );
		std::string LhsValue;
		std::string RhsValue;
		switch( m_Value.vt )
		{
		case VT_R8:
			LhsValue = SvUl::Format(_T("%.06f"), m_Value.dblVal );
			break;
		case VT_BSTR:
			LhsValue = SvUl::createStdString(m_Value);
			break;
		default:
			break;
		}
		switch( Rhs.m_Value.vt )
		{
		case VT_R8:
			RhsValue = SvUl::Format(_T("%.06f"), Rhs.m_Value.dblVal );
			break;
		case VT_BSTR:
			RhsValue = SvUl::createStdString(Rhs.m_Value);
			break;
		default:
			break;
		}
		Result = std::tie(m_objectId, m_DottedName, m_Value.vt, LhsValue, m_Description, m_Selected) < 
					std::tie(Rhs.m_objectId, Rhs.m_DottedName, Rhs.m_Value.vt, RhsValue, Rhs.m_Description, Rhs.m_Selected);
		return Result;
	}

	const GlobalConstantData& GlobalConstantData::operator=( const GlobalConstantData& Rhs )
	{
		m_objectId = Rhs.m_objectId;
		m_DottedName = Rhs.m_DottedName;
		m_Value = Rhs.m_Value;
		m_Description = Rhs.m_Description;
		m_Selected = Rhs.m_Selected;
		return *this;
	}

	bool GlobalConstantData::operator==( const GlobalConstantData& Rhs ) const
	{
		bool Result( false );

		if( m_objectId == Rhs.m_objectId &&
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
} //namespace SvUl
