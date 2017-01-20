//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
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
#include "ObjectInterfaces/TextDefineSvOi.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SVObjectAttributeClass::SVObjectAttributeClass()
{
	miType = SV_OBJECT_TYPE_UNKNOWN;
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

int SVObjectAttributeClass::GetType() const
{
	return miType;
}

SVObjectScriptDataObjectTypeEnum SVObjectAttributeClass::GetSVObjectScriptDataObjectTypeEnum()
{
	SVObjectScriptDataObjectTypeEnum eType = SV_UNKNOWN_Type;

	switch ( GetType() )
	{
		case SV_OBJECT_TYPE_BOOL:
		{
			eType = SV_BOOL_Type;

			break;
		}
		case SV_OBJECT_TYPE_BYTE:
		{
			eType = SV_BYTE_Type;

			break;
		}
		case SV_OBJECT_TYPE_STRING:
		{
			eType = SV_STRING_Type;

			break;
		}
		case SV_OBJECT_TYPE_DOUBLE:
		{
			eType = SV_DOUBLE_Type;

			break;
		}
		case SV_OBJECT_TYPE_DWORD:
		{
			eType = SV_DWORD_Type;

			break;
		}
		case SV_OBJECT_TYPE_LONG:
		{
			eType = SV_LONG_Type;

			break;
		}
		case SV_OBJECT_TYPE_POINT:
		{
			eType = SV_POINT_Type;

			break;
		}
		case SV_OBJECT_TYPE_DPOINT:
		{
			eType = SV_DPOINT_Type;

			break;
		}
		case SV_OBJECT_TYPE_VARIANT:
		{
			eType = SV_VARIANT_Type;

			break;
		}
		case SV_OBJECT_TYPE_INT64:
		{
			eType = SV_INT64_Type;

			break;
		}
		default:
		{
			break;
		}
	}

	return eType;
}// end GetSVObjectScriptDataObjectTypeEnum()

BOOL SVObjectAttributeClass::GetData(SvCl::SVObjectBoolArrayClass& svData)
{
	BOOL bOk = FALSE;

	switch ( GetType() )
	{
		case SV_OBJECT_TYPE_BOOL:
		{
			svData = m_BoolArray;

			bOk = TRUE;

			break;
		}
		case SV_OBJECT_TYPE_BYTE:
		{
			break;
		}
		case SV_OBJECT_TYPE_STRING:
		{
			break;
		}
		case SV_OBJECT_TYPE_DOUBLE:
		{
			break;
		}
		case SV_OBJECT_TYPE_DWORD:
		{
			break;
		}
		case SV_OBJECT_TYPE_LONG:
		{
			break;
		}
		case SV_OBJECT_TYPE_POINT:
		{
			break;
		}
		case SV_OBJECT_TYPE_DPOINT:
		{
			break;
		}
		case SV_OBJECT_TYPE_VARIANT:
		{
			break;
		}
		case SV_OBJECT_TYPE_INT64:
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

BOOL SVObjectAttributeClass::GetData(SvCl::SVObjectByteArrayClass& svData)
{
	BOOL bOk = FALSE;

	switch ( GetType() )
	{
		case SV_OBJECT_TYPE_BOOL:
		{
			break;
		}
		case SV_OBJECT_TYPE_BYTE:
		{
			svData = m_ByteArray;

			bOk = TRUE;

			break;
		}
		case SV_OBJECT_TYPE_STRING:
		{
			for ( int i = 0; i < m_StringArray.GetSize(); i++ )
			{
				BYTE bTemp = (BYTE)atoi( m_StringArray[i].c_str() );

				svData.InsertAt( i, bTemp );
			}

			bOk = TRUE;

			break;
		}
		case SV_OBJECT_TYPE_DOUBLE:
		{
			break;
		}
		case SV_OBJECT_TYPE_DWORD:
		{
			break;
		}
		case SV_OBJECT_TYPE_LONG:
		{
			break;
		}
		case SV_OBJECT_TYPE_POINT:
		{
			break;
		}
		case SV_OBJECT_TYPE_DPOINT:
		{
			break;
		}
		case SV_OBJECT_TYPE_VARIANT:
		{
			break;
		}
		case SV_OBJECT_TYPE_INT64:
		{
			break;
		}
		default:
		{
			break;
		}
	}

	return bOk;
}// end GetData(SVObjectByteArrayClass& svData)

BOOL SVObjectAttributeClass::GetData(SvCl::SVObjectArrayClassTemplate<char>& svData)
{
	BOOL bOk = FALSE;

	switch ( GetType() )
	{
		case SV_OBJECT_TYPE_BOOL:
		{
			break;
		}
		case SV_OBJECT_TYPE_BYTE:
		{
			svData.RemoveAll();
			for (int i=0; i < m_ByteArray.GetSize(); i++)
			{
				char c = (char)m_ByteArray[i];
				svData.Add(c);
			}

			bOk = TRUE;

			break;
		}
		case SV_OBJECT_TYPE_STRING:
		{
			for ( int i = 0; i < m_StringArray.GetSize(); i++ )
			{
				char cTemp = (char) atoi( m_StringArray[i].c_str() );

				svData.InsertAt( i, cTemp );
			}

			bOk = TRUE;

			break;
		}
		case SV_OBJECT_TYPE_DOUBLE:
		{
			break;
		}
		case SV_OBJECT_TYPE_DWORD:
		{
			break;
		}
		case SV_OBJECT_TYPE_LONG:
		{
			break;
		}
		case SV_OBJECT_TYPE_POINT:
		{
			break;
		}
		case SV_OBJECT_TYPE_DPOINT:
		{
			break;
		}
		case SV_OBJECT_TYPE_VARIANT:
		{
			break;
		}
		case SV_OBJECT_TYPE_INT64:
		{
			break;
		}
		default:
		{
			break;
		}
	}

	return bOk;
}// end GetData(SVObjectArrayClassTemplate<char>& svData)

BOOL SVObjectAttributeClass::GetData(SvCl::SVObjectSVStringArrayClass& svData)
{
	BOOL bOk = FALSE;

	switch ( GetType() )
	{
		case SV_OBJECT_TYPE_BOOL:
		{
			for ( int i = 0; i < m_BoolArray.GetSize(); i++ )
			{
				SVString Temp;
				
				if ( m_BoolArray[i] )
				{
					Temp = SvOi::cTrue;
				}
				else
				{
					Temp = SvOi::cFalse;
				}

				svData.InsertAt( i, Temp );
			}

			bOk = TRUE;

			break;
		}
		case SV_OBJECT_TYPE_BYTE:
		{
			for ( int i = 0; i < m_ByteArray.GetSize(); i++ )
			{
				SVString Temp = SvUl_SF::Format( "%x", m_ByteArray[i] );

				svData.InsertAt( i, Temp );
			}

			bOk = TRUE;

			break;
		}
		case SV_OBJECT_TYPE_STRING:
		{
			svData = m_StringArray;

			bOk = TRUE;

			break;
		}
		case SV_OBJECT_TYPE_DOUBLE:
		{
			for ( int i = 0; i < m_DoubleArray.GetSize(); i++ )
			{
				SVString Temp = SvUl_SF::Format( "%f", m_DoubleArray[i] );

				svData.InsertAt( i, Temp );
			}

			bOk = TRUE;

			break;
		}
		case SV_OBJECT_TYPE_DWORD:
		{
			for ( int i = 0; i < m_DWordArray.GetSize(); i++)
			{
				SVString Temp = SvUl_SF::Format( _T("%lx"), m_DWordArray[i] );

				svData.InsertAt( i, Temp );
			}

			bOk = TRUE;

			break;
		}
		case SV_OBJECT_TYPE_LONG:
		{
			for ( int i = 0; i < m_LongArray.GetSize(); i++)
			{
				SVString Temp = SvUl_SF::Format( _T("%ld"), m_LongArray[i] );

				svData.InsertAt( i, Temp );
			}

			bOk = TRUE;

			break;
		}
		case SV_OBJECT_TYPE_POINT:
		{
			for ( int i = 0; i < m_PointArray.GetSize(); i++)
			{
				svData.InsertAt( i, m_PointArray[i].ToString() );
			}

			bOk = TRUE;

			break;
		}
		case SV_OBJECT_TYPE_DPOINT:
		{
			for ( int i = 0; i < m_DPointArray.GetSize(); i++)
			{
				SVString Temp = SvUl_SF::Format( _T("(%lf,%lf)"), m_DPointArray[i].x, m_DPointArray[i].y );

				svData.InsertAt( i, Temp );
			}

			bOk = TRUE;

			break;
		}
		case SV_OBJECT_TYPE_VARIANT:
		{
			for ( int i = 0; i < m_DPointArray.GetSize(); i++)
			{
				SVString Temp;

				VARIANT vtTemp = m_VariantArray[i];
				HRESULT hr = ::VariantChangeType( &vtTemp, &vtTemp, VARIANT_ALPHABOOL, VT_BSTR);
				if( S_OK == hr )
				{
					Temp = SvUl_SF::createSVString( _bstr_t( vtTemp.bstrVal ) );
				}
				else
				{
					Temp = _T("");
				}
				svData.InsertAt( i, Temp );
			}

			bOk = TRUE;

			break;
		}
		case SV_OBJECT_TYPE_INT64:
		{
			for ( int i = 0; i < m_Int64Array.GetSize(); i++)
			{
				SVString Temp = SvUl_SF::Format( _T("%I64d"), m_Int64Array[i] );

				svData.InsertAt( i, Temp );
			}

			bOk = TRUE;

			break;
		}
		default:
		{
			break;
		}
	}

	return bOk;
}

BOOL SVObjectAttributeClass::GetData(SvCl::SVObjectDoubleArrayClass& svData)
{
	BOOL bOk = FALSE;

	switch ( GetType() )
	{
		case SV_OBJECT_TYPE_BOOL:
		{
			break;
		}
		case SV_OBJECT_TYPE_BYTE:
		{
			for ( int i = 0; i < m_ByteArray.GetSize(); i++ )
			{
				double dTemp = (double)m_ByteArray[i];

				svData.InsertAt( i, dTemp );
			}

			bOk = TRUE;

			break;
		}
		case SV_OBJECT_TYPE_STRING:
		{
			for ( int i = 0; i < m_StringArray.GetSize(); i++ )
			{
				double dTemp = atof( m_StringArray[i].c_str() );

				svData.InsertAt( i, dTemp );
			}

			bOk = TRUE;

			break;
		}
		case SV_OBJECT_TYPE_DOUBLE:
		{
			svData = m_DoubleArray;

			bOk = TRUE;

			break;
		}
		case SV_OBJECT_TYPE_DWORD:
		{
			for ( int i = 0; i < m_DWordArray.GetSize(); i++)
			{
				double dTemp = (double)m_DWordArray[i];

				svData.InsertAt( i, dTemp );
			}

			bOk = TRUE;

			break;
		}
		case SV_OBJECT_TYPE_LONG:
		{
			for ( int i = 0; i < m_LongArray.GetSize(); i++)
			{
				double dTemp = (double)m_LongArray[i];

				svData.InsertAt( i, dTemp );
			}

			bOk = TRUE;

			break;
		}
		case SV_OBJECT_TYPE_POINT:
		{
			break;
		}
		case SV_OBJECT_TYPE_DPOINT:
		{
			break;
		}
		case SV_OBJECT_TYPE_VARIANT:
		{

			for ( int i = 0; i < m_VariantArray.GetSize(); i++)
			{
				VARIANT vtTemp = m_VariantArray[i];
				if(vtTemp.vt == VT_R8)
				{
					svData.InsertAt(i, vtTemp.dblVal);
				}
			}
			break;
		}
		case SV_OBJECT_TYPE_INT64:
		{
			break;
		}
		default:
		{
			break;
		}
	}

	return bOk;
}// end GetData(SVObjectDoubleArrayClass& svData)

BOOL SVObjectAttributeClass::GetData(SvCl::SVObjectDWordArrayClass& svData)
{
	BOOL bOk = FALSE;

	switch ( GetType() )
	{
		case SV_OBJECT_TYPE_BOOL:
		{
			break;
		}
		case SV_OBJECT_TYPE_BYTE:
		{
			for ( int i = 0; i < m_ByteArray.GetSize(); i++ )
			{
				DWORD dwTemp = (DWORD)m_ByteArray[i];

				svData.InsertAt( i, dwTemp );
			}

			bOk = TRUE;

			break;
		}
		case SV_OBJECT_TYPE_STRING:
		{
			for ( int i = 0; i < m_StringArray.GetSize(); i++ )
			{
				DWORD dwTemp = (DWORD)atol( m_StringArray[i].c_str() );

				svData.InsertAt( i, dwTemp );
			}

			bOk = TRUE;

			break;
		}
		case SV_OBJECT_TYPE_DOUBLE:
		{
			for ( int i = 0; i < m_DoubleArray.GetSize(); i++)
			{
				DWORD dwTemp = (DWORD)m_DoubleArray[i];

				svData.InsertAt( i, dwTemp );
			}

			bOk = TRUE;

			break;
		}
		case SV_OBJECT_TYPE_DWORD:
		{
			svData = m_DWordArray;

			bOk = TRUE;

			break;
		}
		case SV_OBJECT_TYPE_LONG:
		{
			for ( int i = 0; i < m_LongArray.GetSize(); i++)
			{
				DWORD dwTemp = (DWORD)m_LongArray[i];

				svData.InsertAt( i, dwTemp );
			}

			bOk = TRUE;

			break;
		}
		case SV_OBJECT_TYPE_POINT:
		{
			break;
		}
		case SV_OBJECT_TYPE_DPOINT:
		{
			break;
		}
		case SV_OBJECT_TYPE_VARIANT:
		{

			for ( int i = 0; i < m_VariantArray.GetSize(); i++)
			{
				VARIANT vtTemp = m_VariantArray[i];
				if(vtTemp.vt == VT_UI4)
				{
					svData.InsertAt(i, vtTemp.ulVal);
				}
			}
			break;
		}
		case SV_OBJECT_TYPE_INT64:
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

BOOL SVObjectAttributeClass::GetData(SvCl::SVObjectLongArrayClass& svData)
{
	BOOL bOk = FALSE;

	switch ( GetType() )
	{
		case SV_OBJECT_TYPE_BOOL:
		{
			break;
		}
		case SV_OBJECT_TYPE_BYTE:
		{
			for ( int i = 0; i < m_ByteArray.GetSize(); i++ )
			{
				long lTemp = (long)m_ByteArray[i];

				svData.InsertAt( i, lTemp );
			}

			bOk = TRUE;

			break;
		}
		case SV_OBJECT_TYPE_STRING:
		{
			for ( int i = 0; i < m_StringArray.GetSize(); i++ )
			{
				long lTemp = atol( m_StringArray[i].c_str() );

				svData.InsertAt( i, lTemp );
			}

			bOk = TRUE;

			break;
		}
		case SV_OBJECT_TYPE_DOUBLE:
		{
			for ( int i = 0; i < m_DoubleArray.GetSize(); i++)
			{
				long lTemp = (long)m_DoubleArray[i];

				svData.InsertAt( i, lTemp );
			}

			bOk = TRUE;

			break;
		}
		case SV_OBJECT_TYPE_DWORD:
		{
			for ( int i = 0; i < m_DWordArray.GetSize(); i++)
			{
				long lTemp = (long)m_DWordArray[i];

				svData.InsertAt( i, lTemp );
			}

			bOk = TRUE;

			break;
		}
		case SV_OBJECT_TYPE_LONG:
		{
			svData = m_LongArray;

			bOk = TRUE;

			break;
		}
		case SV_OBJECT_TYPE_POINT:
		{
			break;
		}
		case SV_OBJECT_TYPE_DPOINT:
		{
			break;
		}
		case SV_OBJECT_TYPE_VARIANT:
		{

			for ( int i = 0; i < m_VariantArray.GetSize(); i++)
			{
				VARIANT vtTemp = m_VariantArray[i];
				if(vtTemp.vt == VT_I4)
				{
					svData.InsertAt(i, vtTemp.lVal);
				}
			}
			break;
		}
		case SV_OBJECT_TYPE_INT64:
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

BOOL SVObjectAttributeClass::GetData(SVObjectSVPointArrayClass& svData)
{
	BOOL bOk = FALSE;

	switch ( GetType() )
	{
		case SV_OBJECT_TYPE_BOOL:
		{
			break;
		}
		case SV_OBJECT_TYPE_BYTE:
		{
			break;
		}
		case SV_OBJECT_TYPE_STRING:
		{
			break;
		}
		case SV_OBJECT_TYPE_DOUBLE:
		{
			break;
		}
		case SV_OBJECT_TYPE_DWORD:
		{
			break;
		}
		case SV_OBJECT_TYPE_LONG:
		{
			break;
		}
		case SV_OBJECT_TYPE_POINT:
		{
			bOk = TRUE;

			for ( int i = 0; bOk && i < m_PointArray.GetSize(); i++)
			{
				POINT Point;
				
				m_PointArray[i].GetValue( Point );

				svData.InsertAt( i, SVPOINT( Point ) );
			}

			break;
		}
		case SV_OBJECT_TYPE_DPOINT:
		{
			bOk = TRUE;

			for ( int i = 0; bOk && i < m_DPointArray.GetSize(); i++)
			{
				POINT Point;
				
				m_DPointArray[i].GetValue( Point );

				svData.InsertAt( i, SVPOINT( Point ) );
			}

			break;
		}
		case SV_OBJECT_TYPE_VARIANT:
		{
			break;
		}
		case SV_OBJECT_TYPE_INT64:
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

BOOL SVObjectAttributeClass::GetData(SVObjectDPointArrayClass& svData)
{
	BOOL bOk = FALSE;

	switch ( GetType() )
	{
		case SV_OBJECT_TYPE_BOOL:
		{
			break;
		}
		case SV_OBJECT_TYPE_BYTE:
		{
			break;
		}
		case SV_OBJECT_TYPE_STRING:
		{
			break;
		}
		case SV_OBJECT_TYPE_DOUBLE:
		{
			break;
		}
		case SV_OBJECT_TYPE_DWORD:
		{
			break;
		}
		case SV_OBJECT_TYPE_LONG:
		{
			break;
		}
		case SV_OBJECT_TYPE_POINT:
		{
			bOk = TRUE;

			for ( int i = 0; bOk && i < m_PointArray.GetSize(); i++)
			{
				SVDPointClass Point;
				
				Point = m_PointArray[i];

				svData.InsertAt( i, Point );
			}

			break;
		}
		case SV_OBJECT_TYPE_DPOINT:
		{
			svData = m_DPointArray;

			bOk = TRUE;

			break;
		}
		case SV_OBJECT_TYPE_VARIANT:
		{
			break;
		}
		case SV_OBJECT_TYPE_INT64:
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

BOOL SVObjectAttributeClass::GetData(SvCl::SVObjectVariantArrayClass& svData)
{
	BOOL bOk = FALSE;

	switch ( GetType() )
	{
		case SV_OBJECT_TYPE_BOOL:
		{
			for ( int i = 0; i < m_ByteArray.GetSize(); i++ )
			{
				_variant_t vt;
				vt.vt = VT_BOOL;
				vt.boolVal = m_BoolArray[i];

				svData.InsertAt( i, vt );
			}

			bOk = TRUE;

			break;
		}
		case SV_OBJECT_TYPE_BYTE:
		{
			for ( int i = 0; i < m_ByteArray.GetSize(); i++ )
			{
				_variant_t vt;
				vt.vt = VT_UI1;
				vt.bVal = m_ByteArray[i];

				svData.InsertAt( i, vt );
			}

			bOk = TRUE;

			break;
		}
		case SV_OBJECT_TYPE_STRING:
		{
			for ( int i = 0; i < m_ByteArray.GetSize(); i++ )
			{
				_variant_t vt;
				vt.SetString( m_StringArray[i].c_str() );

				svData.InsertAt( i, vt );
			}

			bOk = TRUE;

			break;
		}
		case SV_OBJECT_TYPE_DOUBLE:
		{
			for ( int i = 0; i < m_ByteArray.GetSize(); i++ )
			{
				_variant_t vt;
				vt.vt = VT_R8;
				vt.dblVal = m_DoubleArray[i];

				svData.InsertAt( i, vt );
			}

			bOk = TRUE;

			break;
		}
		case SV_OBJECT_TYPE_DWORD:
		{
			for ( int i = 0; i < m_ByteArray.GetSize(); i++ )
			{
				_variant_t vt;
				vt.vt = VT_UI4;
				//vt.bVal = msvDWordArray[i];
				vt.ulVal = m_DWordArray[i];

				svData.InsertAt( i, vt );
			}

			bOk = TRUE;

			break;
		}
		case SV_OBJECT_TYPE_LONG:
		{
			for ( int i = 0; i < m_ByteArray.GetSize(); i++ )
			{
				_variant_t vt;
				vt.vt = VT_I4;
				//vt.bVal = msvLongArray[i];
				vt.lVal = m_LongArray[i];

				svData.InsertAt( i, vt );
			}

			bOk = TRUE;

			break;
		}
		case SV_OBJECT_TYPE_POINT:
		{
			break;
		}
		case SV_OBJECT_TYPE_DPOINT:
		{
			break;
		}
		case SV_OBJECT_TYPE_VARIANT:
		{

			for ( int i = 0; i < m_VariantArray.GetSize(); i++)
			{
				_variant_t vtTemp;
				HRESULT hr = ::VariantCopy(&vtTemp, &m_VariantArray[i]);
				svData.InsertAt(i, vtTemp);
			}

			bOk = TRUE;
			break;
		}
		case SV_OBJECT_TYPE_INT64:
		{
			for ( int i = 0; i < m_Int64Array.GetSize(); i++ )
			{
				_variant_t vt;
				vt.vt = VT_I8;
				vt.llVal = m_Int64Array[i];

				svData.InsertAt( i, vt );
			}

			bOk = TRUE;

			break;

		}
		default:
		{
			break;
		}
	}

	return bOk;
}// end GetData(SVObjectVariantArrayClass& svData)

BOOL SVObjectAttributeClass::GetData(SvCl::SVObjectInt64ArrayClass& svData)
{
	BOOL bOk = FALSE;

	switch ( GetType() )
	{
		case SV_OBJECT_TYPE_BOOL:
		{
			break;
		}
		case SV_OBJECT_TYPE_BYTE:
		{
			break;
		}
		case SV_OBJECT_TYPE_STRING:
		{
			break;
		}
		case SV_OBJECT_TYPE_DOUBLE:
		{
			break;
		}
		case SV_OBJECT_TYPE_DWORD:
		{
			break;
		}
		case SV_OBJECT_TYPE_LONG:
		{
			break;
		}
		case SV_OBJECT_TYPE_POINT:
		{
			break;
		}
		case SV_OBJECT_TYPE_DPOINT:
		{
			break;
		}
		case SV_OBJECT_TYPE_VARIANT:
		{
			break;
		}
		case SV_OBJECT_TYPE_INT64:
		{
			svData = m_Int64Array;
			bOk = TRUE;
			break;
		}
		default:
		{
			break;
		}
	}
	return bOk;
}// end GetData(SVObjectInt64ArrayClass& svData)

BOOL SVObjectAttributeClass::SetName(LPCTSTR szName)
{
	m_Name = szName;

	SetType( SV_OBJECT_TYPE_UNKNOWN );

	return TRUE;
}

BOOL SVObjectAttributeClass::AddData(BOOL Value)
{
	BOOL bOk = TRUE;

	if ( GetType() != SV_OBJECT_TYPE_BOOL )
	{
		bOk = SetType( SV_OBJECT_TYPE_BOOL );
	}

	if ( bOk )
	{
		m_BoolArray.Add( Value );
	}

	return bOk;
}

BOOL SVObjectAttributeClass::AddData(VARIANT Value)
{
	BOOL bOk = TRUE;

	if ( GetType() != SV_OBJECT_TYPE_VARIANT )
	{
		bOk = SetType( SV_OBJECT_TYPE_VARIANT );
	}

	if ( bOk )
	{
		_variant_t l_Variant( Value );

		m_VariantArray.Add( l_Variant );
	}

	return bOk;
}

BOOL SVObjectAttributeClass::AddData(BYTE Value)
{
	BOOL bOk = TRUE;

	if ( GetType() != SV_OBJECT_TYPE_BYTE )
	{
		bOk = SetType( SV_OBJECT_TYPE_BYTE );
	}

	if ( bOk )
	{
		m_ByteArray.Add( Value );
	}

	return bOk;
}

BOOL SVObjectAttributeClass::AddData(LPCTSTR Value)
{
	BOOL bOk = TRUE;

	if ( GetType() != SV_OBJECT_TYPE_STRING )
	{
		bOk = SetType( SV_OBJECT_TYPE_STRING );
	}

	if ( bOk )
	{
		m_StringArray.Add( SVString( Value ) );
	}

	return bOk;
}

BOOL SVObjectAttributeClass::AddData(double Value)
{
	BOOL bOk = TRUE;

	if ( GetType() != SV_OBJECT_TYPE_DOUBLE )
	{
		bOk = SetType( SV_OBJECT_TYPE_DOUBLE );
	}

	if ( bOk )
	{
		m_DoubleArray.Add( Value );
	}

	return bOk;
}

BOOL SVObjectAttributeClass::AddData(DWORD Value)
{
	BOOL bOk = TRUE;

	if ( GetType() != SV_OBJECT_TYPE_DWORD )
	{
		bOk = SetType( SV_OBJECT_TYPE_DWORD );
	}

	if ( bOk )
	{
		m_DWordArray.Add( Value );
	}

	return bOk;
}

BOOL SVObjectAttributeClass::AddData(long Value)
{
	BOOL bOk = TRUE;

	if ( GetType() != SV_OBJECT_TYPE_LONG )
	{
		bOk = SetType( SV_OBJECT_TYPE_LONG );
	}

	if ( bOk )
	{
		m_LongArray.Add( Value );
	}

	return bOk;
}

BOOL SVObjectAttributeClass::AddData(POINT Value)
{
	BOOL bOk = TRUE;

	if ( GetType() != SV_OBJECT_TYPE_POINT )
	{
		bOk = SetType( SV_OBJECT_TYPE_POINT );
	}

	if ( bOk )
	{
		m_PointArray.Add( Value );
	}

	return bOk;
}

BOOL SVObjectAttributeClass::AddData(SVDPointClass Value)
{
	BOOL bOk = TRUE;

	if ( GetType() != SV_OBJECT_TYPE_DPOINT )
	{
		bOk = SetType( SV_OBJECT_TYPE_DPOINT );
	}

	if ( bOk )
	{
		m_DPointArray.Add( Value );
	}

	return bOk;
}

BOOL SVObjectAttributeClass::AddData(__int64 Value)
{
	BOOL bOk = TRUE;

	if ( GetType() != SV_OBJECT_TYPE_INT64 )
	{
		bOk = SetType( SV_OBJECT_TYPE_INT64 );
	}

	if ( bOk )
	{
		m_Int64Array.Add( Value );
	}

	return bOk;
}


// ***** Private Operators *****



BOOL SVObjectAttributeClass::SetType(int iType)
{
	miType = iType;

	m_BoolArray.RemoveAll();
	m_ByteArray.RemoveAll();
	m_StringArray.RemoveAll();
	m_DoubleArray.RemoveAll();
	m_DWordArray.RemoveAll();
	m_LongArray.RemoveAll();
	m_PointArray.RemoveAll();
	m_DPointArray.RemoveAll();
	m_Int64Array.RemoveAll();

	return TRUE;
}

BOOL SVObjectAttributeClass::SetType(SVObjectScriptDataObjectTypeEnum eType)
{
	BOOL bOk = FALSE;

	switch ( eType )
	{
		case SV_UNKNOWN_Type:
		{
			bOk = SetType( SV_OBJECT_TYPE_UNKNOWN );
			break;
		}
		case SV_BOOL_Type:
		{
			bOk = SetType( SV_OBJECT_TYPE_BOOL );
			break;
		}
		case SV_BYTE_Type:
		{
			bOk = SetType( SV_OBJECT_TYPE_BYTE );
			break;
		}
		case SV_DWORD_Type:
		{
			bOk = SetType( SV_OBJECT_TYPE_DWORD );
			break;
		}
		case SV_LONG_Type:
		{
			bOk = SetType( SV_OBJECT_TYPE_LONG );
			break;
		}
		case SV_DOUBLE_Type:
		{
			bOk = SetType( SV_OBJECT_TYPE_DOUBLE );
			break;
		}
		case SV_POINT_Type:
		{
			bOk = SetType( SV_OBJECT_TYPE_POINT );
			break;
		}
		case SV_STRING_Type:
		{
			bOk = SetType( SV_OBJECT_TYPE_STRING );
			break;
		}
		case SV_DPOINT_Type:
		{
			bOk = SetType( SV_OBJECT_TYPE_DPOINT );
			break;
		}
		case SV_VARIANT_Type:
		{
			bOk = SetType( SV_OBJECT_TYPE_VARIANT );
			break;
		}
		case SV_INT64_Type:
		{
			bOk = SetType( SV_OBJECT_TYPE_INT64 );
			break;
		}
		default:
		{
			break;
		}
	}

	return bOk;
}

