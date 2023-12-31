//******************************************************************************
//* COPYRIGHT (c) 2003 by K�rber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObjectAttributeClass
//* .File Name       : $Workfile:   SVObjectAttributeClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 16:38:26  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVObjectAttributeClass.h"
#include "Definitions/TextDefinesSvDef.h"
#include "SVUtilityLibrary/SafeArrayHelper.h"
#include "Definitions/GlobalConst.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SVObjectAttributeClass::SVObjectAttributeClass()
{
	m_Type = SV_UNKNOWN_Type;
}

SVObjectAttributeClass::SVObjectAttributeClass( const SVObjectAttributeClass& rRhs )
{
	(*this) = rRhs;
}

SVObjectAttributeClass::~SVObjectAttributeClass()
{
}

SVObjectAttributeClass& SVObjectAttributeClass::operator=(const SVObjectAttributeClass& rRhs)
{
	if ( this != &rRhs )
	{
		SetName( rRhs.GetName() );
		SetType( rRhs.GetType() );

		m_BoolArray = rRhs.m_BoolArray;
		m_ByteArray = rRhs.m_ByteArray;
		m_StringArray = rRhs.m_StringArray;
		m_DoubleArray = rRhs.m_DoubleArray;
		m_DWordArray = rRhs.m_DWordArray;
		m_LongArray = rRhs.m_LongArray;
		m_PointArray = rRhs.m_PointArray;
		m_DPointArray = rRhs.m_DPointArray;
		m_VariantArray = rRhs.m_VariantArray;
		m_Int64Array = rRhs.m_Int64Array;
	}	
	return *this;
}

LPCTSTR SVObjectAttributeClass::GetName() const
{
	return m_Name.c_str();
}

SVObjectScriptDataObjectTypeEnum SVObjectAttributeClass::GetType() const
{
	return m_Type;
}

bool SVObjectAttributeClass::GetData(SvCl::SVObjectBoolArrayClass& svData)
{
	bool bOk = false;

	switch ( GetType() )
	{
		case SV_BOOL_Type:
		{
			svData = m_BoolArray;

			bOk = true;

			break;
		}
		case SV_BYTE_Type:
		{
			break;
		}
		case SV_STRING_Type:
		{
			break;
		}
		case SV_DOUBLE_Type:
		{
			break;
		}
		case SV_DWORD_Type:
		{
			break;
		}
		case SV_LONG_Type:
		{
			break;
		}
		case SV_POINT_Type:
		{
			break;
		}
		case SV_DPOINT_Type:
		{
			break;
		}
		case SV_VARIANT_Type:
		{
			break;
		}
		case SV_INT64_Type:
		{
			break;
		}
		default:
		{
			break;
		}
	}

	return bOk;
}// end GetData(SVObjectBoolArrayClass& svData)

bool SVObjectAttributeClass::GetData(SvCl::SVObjectByteArrayClass& rData)
{
	bool bOk = false;

	rData.clear();
	switch ( GetType() )
	{
		case SV_BOOL_Type:
		{
			break;
		}
		case SV_BYTE_Type:
		{
			rData = m_ByteArray;

			bOk = true;
			break;
		}
		case SV_STRING_Type:
		{
			for ( int i = 0; i < static_cast<int> (m_StringArray.size()); i++ )
			{
				BYTE bTemp = (BYTE)atoi( m_StringArray[i].c_str() );
				rData.push_back( bTemp );
			}

			bOk = true;
			break;
		}
		case SV_DOUBLE_Type:
		{
			break;
		}
		case SV_DWORD_Type:
		{
			break;
		}
		case SV_LONG_Type:
		{
			break;
		}
		case SV_POINT_Type:
		{
			break;
		}
		case SV_DPOINT_Type:
		{
			break;
		}
		case SV_VARIANT_Type:
		{
			break;
		}
		case SV_INT64_Type:
		{
			break;
		}
		default:
		{
			break;
		}
	}

	return bOk;
}

bool SVObjectAttributeClass::GetData(SvCl::SVObjectCharArrayClass& rData)
{
	bool bOk = false;

	rData.clear();
	switch ( GetType() )
	{
		case SV_BOOL_Type:
		{
			break;
		}
		case SV_BYTE_Type:
		{
			for (int i=0; i < static_cast<int> (m_ByteArray.size()); i++)
			{
				rData.push_back(static_cast<char> (m_ByteArray[i]));
			}

			bOk = true;
			break;
		}
		case SV_STRING_Type:
		{
			for ( int i = 0; i < static_cast<int> (m_StringArray.size()); i++ )
			{
				char cTemp = static_cast<char> (atoi(m_StringArray[i].c_str()));

				rData.push_back(cTemp);
			}

			bOk = true;

			break;
		}
		case SV_DOUBLE_Type:
		{
			break;
		}
		case SV_DWORD_Type:
		{
			break;
		}
		case SV_LONG_Type:
		{
			break;
		}
		case SV_POINT_Type:
		{
			break;
		}
		case SV_DPOINT_Type:
		{
			break;
		}
		case SV_VARIANT_Type:
		{
			break;
		}
		case SV_INT64_Type:
		{
			break;
		}
		default:
		{
			break;
		}
	}

	return bOk;
}

bool SVObjectAttributeClass::GetData(SvCl::SVObjectStdStringArrayClass& rData)
{
	bool bOk = false;

	rData.clear();
	switch ( GetType() )
	{
		case SV_BOOL_Type:
		{
			for ( int i = 0; i < static_cast<int> (m_BoolArray.size()); i++ )
			{
				rData.push_back(std::string(m_BoolArray[i] ? SvDef::cTrue : SvDef::cFalse));
			}

			bOk = true;
			break;
		}
		case SV_BYTE_Type:
		{
			for ( int i = 0; i < static_cast<int> (m_ByteArray.size()); i++ )
			{
				std::string Temp = std::format( "{:x}", m_ByteArray[i] );
				rData.push_back( Temp );
			}

			bOk = true;

			break;
		}
		case SV_STRING_Type:
		{
			rData = m_StringArray;

			bOk = true;
			break;
		}
		case SV_DOUBLE_Type:
		{
			for ( int i = 0; i < static_cast<int> (m_DoubleArray.size()); i++ )
			{
				std::string Temp = std::format( "{}", m_DoubleArray[i] );
				rData.push_back(Temp);
			}

			bOk = true;

			break;
		}
		case SV_DWORD_Type:
		{
			for ( int i = 0; i < static_cast<int> (m_DWordArray.size()); i++)
			{
				std::string Temp = std::format( _T("{:x}"), m_DWordArray[i] );
				rData.push_back(Temp);
			}

			bOk = true;
			break;
		}
		case SV_LONG_Type:
		{
			for ( int i = 0; i < static_cast<int> (m_LongArray.size()); i++)
			{
				std::string Temp = std::format( _T("{:d}"), m_LongArray[i] );
				rData.push_back(Temp);
			}

			bOk = true;

			break;
		}
		case SV_POINT_Type:
		{
			for ( int i = 0; i < static_cast<int> (m_PointArray.size()); i++)
			{
				rData.push_back(m_PointArray[i].toString());
			}

			bOk = true;

			break;
		}
		case SV_DPOINT_Type:
		{
			for ( int i = 0; i < static_cast<int> (m_DPointArray.size()); i++)
			{
				rData.push_back(  m_DPointArray[i].toString() );
			}

			bOk = true;

			break;
		}
		case SV_VARIANT_Type:
		{
			for ( int i = 0; i < static_cast<int> (m_DPointArray.size()); i++)
			{
				std::string Temp;

				VARIANT vtTemp = m_VariantArray[i];
				HRESULT hr = ::VariantChangeTypeEx( &vtTemp, &vtTemp, SvDef::LCID_USA, VARIANT_ALPHABOOL, VT_BSTR);
				if( S_OK == hr )
				{
					Temp = SvUl::createStdString( _bstr_t( vtTemp.bstrVal ) );
				}
				else
				{
					Temp = _T("");
				}
				rData.push_back(Temp);
			}

			bOk = true;

			break;
		}
		case SV_INT64_Type:
		{
			for ( int i = 0; i < static_cast<int> (m_Int64Array.size()); i++)
			{
				std::string Temp = std::format( _T("{:d}"), m_Int64Array[i] );

				rData.push_back( Temp );
			}

			bOk = true;

			break;
		}
		default:
		{
			break;
		}
	}

	return bOk;
}

bool SVObjectAttributeClass::GetData(SvCl::SVObjectDoubleArrayClass& rData)
{
	bool Result = false;

	rData.clear();
	switch ( GetType() )
	{
		case SV_BOOL_Type:
		{
			break;
		}
		case SV_BYTE_Type:
		{
			for ( int i = 0; i < static_cast<int> (m_ByteArray.size()); i++ )
			{
				rData.push_back(static_cast<double> (m_ByteArray[i]));
			}

			Result = true;
			break;
		}
		case SV_STRING_Type:
		{
			for ( int i = 0; i < static_cast<int> (m_StringArray.size()); i++ )
			{
				double dTemp = atof( m_StringArray[i].c_str() );
				rData.push_back(dTemp);
			}

			Result = true;
			break;
		}
		case SV_DOUBLE_Type:
		{
			rData = m_DoubleArray;

			Result = true;

			break;
		}
		case SV_DWORD_Type:
		{
			for ( int i = 0; i < static_cast<int> (m_DWordArray.size()); i++)
			{
				rData.push_back(static_cast<double> (m_DWordArray[i]));
			}

			Result = true;

			break;
		}
		case SV_LONG_Type:
		{
			for ( int i = 0; i < static_cast<int> (m_LongArray.size()); i++)
			{
				rData.push_back(static_cast<double> (m_LongArray[i]));
			}

			Result = true;

			break;
		}
		case SV_POINT_Type:
		{
			break;
		}
		case SV_DPOINT_Type:
		{
			break;
		}
		case SV_VARIANT_Type:
		{

			for ( int i = 0; i < static_cast<int> (m_VariantArray.size()); i++)
			{
				if(m_VariantArray[i].vt == VT_R8)
				{
					rData.push_back(m_VariantArray[i].dblVal);
				}
			}
			break;
		}
		case SV_INT64_Type:
		{
			break;
		}
		default:
		{
			break;
		}
	}

	return Result;
}// end GetData(SVObjectDoubleArrayClass& svData)

bool SVObjectAttributeClass::GetData(SvCl::SVObjectDWordArrayClass& rData)
{
	bool bOk = false;

	rData.clear();
	switch ( GetType() )
	{
		case SV_BOOL_Type:
		{
			break;
		}
		case SV_BYTE_Type:
		{
			for ( int i = 0; i < static_cast<int> (m_ByteArray.size()); i++ )
			{
				rData.push_back(static_cast<DWORD> (m_ByteArray[i]));
			}

			bOk = true;
			break;
		}
		case SV_STRING_Type:
		{
			for ( int i = 0; i < static_cast<int> (m_StringArray.size()); i++ )
			{
				DWORD dwTemp = static_cast<DWORD> (atol(m_StringArray[i].c_str()));
				rData.push_back(dwTemp);
			}

			bOk = true;
			break;
		}
		case SV_DOUBLE_Type:
		{
			for ( int i = 0; i < static_cast<int> (m_DoubleArray.size()); i++)
			{
				rData.push_back(static_cast<DWORD> (m_DoubleArray[i]));
			}

			bOk = true;
			break;
		}
		case SV_DWORD_Type:
		{
			rData = m_DWordArray;

			bOk = true;
			break;
		}
		case SV_LONG_Type:
		{
			for ( int i = 0; i < static_cast<int> (m_LongArray.size()); i++)
			{
				rData.push_back(static_cast<DWORD> (m_LongArray[i]));
			}

			bOk = true;

			break;
		}
		case SV_POINT_Type:
		{
			break;
		}
		case SV_DPOINT_Type:
		{
			break;
		}
		case SV_VARIANT_Type:
		{

			for ( int i = 0; i < static_cast<int> (m_VariantArray.size()); i++)
			{
				if(m_VariantArray[i].vt == VT_UI4)
				{
					rData.push_back(static_cast<DWORD> (m_VariantArray[i].ulVal));
				}
			}
			break;
		}
		case SV_INT64_Type:
		{
			break;
		}
		default:
		{
			break;
		}
	}

	return bOk;
}// end GetData(SVObjectDWordArrayClass& svData)

bool SVObjectAttributeClass::GetData(SvCl::SVObjectLongArrayClass& rData)
{
	bool bOk = false;

	rData.clear();
	switch ( GetType() )
	{
		case SV_BOOL_Type:
		{
			break;
		}
		case SV_BYTE_Type:
		{
			for ( int i = 0; i < static_cast<int> (m_ByteArray.size()); i++ )
			{
				rData.push_back(static_cast<long> (m_ByteArray[i]));
			}

			bOk = true;
			break;
		}
		case SV_STRING_Type:
		{
			for ( int i = 0; i < static_cast<int> (m_StringArray.size()); i++ )
			{
				long lTemp = atol( m_StringArray[i].c_str() );
				rData.push_back(lTemp);
			}

			bOk = true;

			break;
		}
		case SV_DOUBLE_Type:
		{
			for ( int i = 0; i < static_cast<int> (m_DoubleArray.size()); i++)
			{
				rData.push_back(static_cast<long> (m_DoubleArray[i]));
			}

			bOk = true;

			break;
		}
		case SV_DWORD_Type:
		{
			for ( int i = 0; i < static_cast<int> (m_DWordArray.size()); i++)
			{
				rData.push_back(static_cast<long> (m_DWordArray[i]));
			}

			bOk = true;

			break;
		}
		case SV_LONG_Type:
		{
			rData = m_LongArray;

			bOk = true;

			break;
		}
		case SV_POINT_Type:
		{
			break;
		}
		case SV_DPOINT_Type:
		{
			break;
		}
		case SV_VARIANT_Type:
		{

			for ( int i = 0; i < static_cast<int> (m_VariantArray.size()); i++)
			{
				if(m_VariantArray[i].vt == VT_I4)
				{
					rData.push_back(m_VariantArray[i].lVal);
				}
			}
			break;
		}
		case SV_INT64_Type:
		{
			break;
		}
		default:
		{
			break;
		}
	}

	return bOk;
}// end GetData(SVObjectLongArrayClass& svData)

bool SVObjectAttributeClass::GetData(SVObjectSVPointArrayClass& rData)
{
	bool bOk = false;

	rData.clear();
	switch (GetType())
	{
	case SV_BOOL_Type:
	{
		break;
	}
	case SV_BYTE_Type:
	{
		break;
	}
	case SV_STRING_Type:
	{
		break;
	}
	case SV_DOUBLE_Type:
	{
		break;
	}
	case SV_DWORD_Type:
	{
		break;
	}
	case SV_LONG_Type:
	{
		break;
	}
	case SV_POINT_Type:
	{
		rData = m_PointArray;
		bOk = true;
		break;
	}
	case SV_DPOINT_Type:
	{
		rData.resize(m_DPointArray.size());
		for (int i = 0; bOk && i < static_cast<int> (m_DPointArray.size()); i++)
		{
			rData.emplace_back(SVPoint<long>(static_cast<long> (m_DPointArray[i].m_x), static_cast<long> (m_DPointArray[i].m_y)));
		}
		bOk = true;
		break;
	}
	case SV_VARIANT_Type:
	{
		break;
	}
	case SV_INT64_Type:
	{
		break;
	}
	default:
	{
		break;
	}
	}

	return bOk;
}// end GetData(SVObjectSVPointArrayClass& svData)

bool SVObjectAttributeClass::GetData(SVObjectDPointArrayClass& rData)
{
	bool bOk = false;

	rData.clear();
	switch ( GetType() )
	{
		case SV_BOOL_Type:
		{
			break;
		}
		case SV_BYTE_Type:
		{
			break;
		}
		case SV_STRING_Type:
		{
			break;
		}
		case SV_DOUBLE_Type:
		{
			break;
		}
		case SV_DWORD_Type:
		{
			break;
		}
		case SV_LONG_Type:
		{
			break;
		}
		case SV_POINT_Type:
		{
			bOk = true;
			rData.resize(m_PointArray.size());
			for ( int i = 0; bOk && i < static_cast<int> (m_PointArray.size()); i++)
			{
				rData.emplace_back(SVPoint<double>{static_cast<double> (m_PointArray[i].m_x), static_cast<double> (m_PointArray[i].m_y)});
			}

			break;
		}
		case SV_DPOINT_Type:
		{
			rData = m_DPointArray;
			bOk = true;

			break;
		}
		case SV_VARIANT_Type:
		{
			break;
		}
		case SV_INT64_Type:
		{
			break;
		}
		default:
		{
			break;
		}
	}

	return bOk;
}// end GetData(SVObjectDPointArrayClass& svData)

bool SVObjectAttributeClass::GetData(SvCl::SVObjectVariantArrayClass& rData)
{
	bool bOk = false;

	rData.clear();
	switch ( GetType() )
	{
		case SV_BOOL_Type:
		{
			for ( int i = 0; i < static_cast<int> (m_BoolArray.size()); i++ )
			{
				_variant_t vt;
				vt.vt = VT_BOOL;
				vt.boolVal = m_BoolArray[i] ? VARIANT_TRUE : VARIANT_FALSE;
				rData.push_back(vt);
			}

			bOk = true;

			break;
		}
		case SV_BYTE_Type:
		{
			for ( int i = 0; i < static_cast<int> (m_ByteArray.size()); i++ )
			{
				_variant_t vt;
				vt.vt = VT_UI1;
				vt.bVal = m_ByteArray[i];
				rData.push_back(vt);
			}

			bOk = true;

			break;
		}
		case SV_STRING_Type:
		{
			for ( int i = 0; i < static_cast<int> (m_StringArray.size()); i++ )
			{
				_variant_t vt;
				vt.SetString( m_StringArray[i].c_str() );
				rData.push_back(vt);
			}

			bOk = true;

			break;
		}
		case SV_DOUBLE_Type:
		{
			for ( int i = 0; i < static_cast<int> (m_DoubleArray.size()); i++ )
			{
				_variant_t vt;
				vt.vt = VT_R8;
				vt.dblVal = m_DoubleArray[i];
				rData.push_back(vt);
			}

			bOk = true;

			break;
		}
		case SV_DWORD_Type:
		{
			for ( int i = 0; i < static_cast<int> (m_DWordArray.size()); i++ )
			{
				_variant_t vt;
				vt.vt = VT_UI4;
				vt.ulVal = m_DWordArray[i];
				rData.push_back(vt);
			}

			bOk = true;

			break;
		}
		case SV_LONG_Type:
		{
			for ( int i = 0; i < static_cast<int> (m_LongArray.size()); i++ )
			{
				_variant_t vt;
				vt.vt = VT_I4;
				vt.lVal = m_LongArray[i];
				rData.push_back(vt);
			}

			bOk = true;

			break;
		}
		case SV_POINT_Type:
		{
			break;
		}
		case SV_DPOINT_Type:
		{
			break;
		}
		case SV_VARIANT_Type:
		{

			for ( int i = 0; i < static_cast<int> (m_VariantArray.size()); i++)
			{
				rData.push_back(m_VariantArray[i]);
			}

			bOk = true;
			break;
		}
		case SV_INT64_Type:
		{
			for ( int i = 0; i < static_cast<int>(m_Int64Array.size()); i++ )
			{
				_variant_t vt;
				vt.vt = VT_I8;
				vt.llVal = m_Int64Array[i];
				rData.push_back(vt);
			}

			bOk = true;

			break;

		}
		default:
		{
			break;
		}
	}

	return bOk;
}// end GetData(SVObjectVariantArrayClass& svData)

bool SVObjectAttributeClass::GetData(SvCl::SVObjectInt64ArrayClass& rData)
{
	bool bOk = false;

	rData.clear();
	switch ( GetType() )
	{
		case SV_BOOL_Type:
		{
			break;
		}
		case SV_BYTE_Type:
		{
			break;
		}
		case SV_STRING_Type:
		{
			break;
		}
		case SV_DOUBLE_Type:
		{
			break;
		}
		case SV_DWORD_Type:
		{
			break;
		}
		case SV_LONG_Type:
		{
			break;
		}
		case SV_POINT_Type:
		{
			break;
		}
		case SV_DPOINT_Type:
		{
			break;
		}
		case SV_VARIANT_Type:
		{
			break;
		}
		case SV_INT64_Type:
		{
			rData = m_Int64Array;
			bOk = true;
			break;
		}
		default:
		{
			break;
		}
	}
	return bOk;
}// end GetData(SVObjectInt64ArrayClass& svData)

variant_t SVObjectAttributeClass::getData()
{
	//rData.clear();
	switch (GetType())
	{
	case SV_BOOL_Type:
	{
		if (0 < m_BoolArray.size())
		{
			return m_BoolArray[0];
		}
		break;
	}
	case SV_BYTE_Type:
	{
		if (0 < m_ByteArray.size())
		{
			return m_ByteArray[0];
		}
		break;
	}
	case SV_STRING_Type:
	{
		if (0 < m_StringArray.size())
		{
			return m_StringArray[0].c_str();
		}
		break;
	}
	case SV_DOUBLE_Type:
	{
		if (1 == m_DoubleArray.size())
		{
			return m_DoubleArray[0];
		}
		else if (1 < m_DoubleArray.size())
		{
			return SvUl::vectorToSafeArray(m_DoubleArray);
		}
		break;
	}
	case SV_DWORD_Type:
	{
		if (0 < m_DWordArray.size())
		{
			return m_DWordArray[0];
		}
		break;
	}
	case SV_LONG_Type:
	{
		if (1 == m_LongArray.size())
		{
			return m_LongArray[0];
		}
		else if (1 < m_LongArray.size())
		{
			return SvUl::vectorToSafeArray(m_LongArray);
		}
		break;
	}
	case SV_VARIANT_Type:
	{
		if (0 < m_VariantArray.size())
		{
			return m_VariantArray[0];
		}
		break;
	}
	case SV_INT64_Type:
	{
		if (0 < m_Int64Array.size())
		{
			return m_Int64Array[0];
		}
		break;
	}
	case SV_POINT_Type:
	case SV_DPOINT_Type:
	default:
	{
		break;
	}
	}
	return {};
}

void SVObjectAttributeClass::SetName(LPCTSTR szName)
{
	m_Name = szName;

	SetType( SV_UNKNOWN_Type );
}

bool SVObjectAttributeClass::AddData(BOOL Value)
{
	bool bOk = true;

	if ( GetType() != SV_BOOL_Type )
	{
		bOk = SetType( SV_BOOL_Type );
	}

	if ( bOk )
	{
		m_BoolArray.push_back( Value );
	}

	return bOk;
}

bool SVObjectAttributeClass::AddData(VARIANT Value)
{
	bool bOk = true;

	if ( GetType() != SV_VARIANT_Type )
	{
		bOk = SetType( SV_VARIANT_Type );
	}

	if ( bOk )
	{
		_variant_t vt( Value );

		m_VariantArray.push_back( vt );
	}

	return bOk;
}

bool SVObjectAttributeClass::AddData(BYTE Value)
{
	bool bOk = true;

	if ( GetType() != SV_BYTE_Type )
	{
		bOk = SetType( SV_BYTE_Type );
	}

	if ( bOk )
	{
		m_ByteArray.push_back( Value );
	}

	return bOk;
}

bool SVObjectAttributeClass::AddData(LPCTSTR Value)
{
	bool bOk = true;

	if ( GetType() != SV_STRING_Type )
	{
		bOk = SetType( SV_STRING_Type );
	}

	if ( bOk )
	{
		m_StringArray.push_back( std::string( Value ) );
	}

	return bOk;
}

bool SVObjectAttributeClass::AddData(double Value)
{
	bool bOk = true;

	if ( GetType() != SV_DOUBLE_Type )
	{
		bOk = SetType( SV_DOUBLE_Type );
	}

	if ( bOk )
	{
		m_DoubleArray.push_back( Value );
	}

	return bOk;
}

bool SVObjectAttributeClass::AddData(DWORD Value)
{
	bool bOk = true;

	if ( GetType() != SV_DWORD_Type )
	{
		bOk = SetType( SV_DWORD_Type );
	}

	if ( bOk )
	{
		m_DWordArray.push_back( Value );
	}

	return bOk;
}

bool SVObjectAttributeClass::AddData(long Value)
{
	bool bOk = true;

	if ( GetType() != SV_LONG_Type )
	{
		bOk = SetType( SV_LONG_Type );
	}

	if ( bOk )
	{
		m_LongArray.push_back( Value );
	}

	return bOk;
}

bool SVObjectAttributeClass::AddData(POINT Value)
{
	bool bOk = true;

	if ( GetType() != SV_POINT_Type )
	{
		bOk = SetType( SV_POINT_Type );
	}

	if ( bOk )
	{
		m_PointArray.push_back(SVPoint<long>(Value));
	}

	return bOk;
}

bool SVObjectAttributeClass::AddData(SVPoint<double> Value)
{
	bool bOk = true;

	if ( GetType() != SV_DPOINT_Type )
	{
		bOk = SetType( SV_DPOINT_Type );
	}

	if ( bOk )
	{
		m_DPointArray.push_back( Value );
	}

	return bOk;
}

bool SVObjectAttributeClass::AddData(__int64 Value)
{
	bool bOk = true;

	if ( GetType() != SV_INT64_Type )
	{
		bOk = SetType( SV_INT64_Type );
	}

	if ( bOk )
	{
		m_Int64Array.push_back( Value );
	}

	return bOk;
}


bool SVObjectAttributeClass::SetType(SVObjectScriptDataObjectTypeEnum eType)
{
	m_BoolArray.clear();
	m_ByteArray.clear();
	m_StringArray.clear();
	m_DoubleArray.clear();
	m_DWordArray.clear();
	m_LongArray.clear();
	m_PointArray.clear();
	m_DPointArray.clear();
	m_Int64Array.clear();
	m_Type = eType;
	return true;
}

