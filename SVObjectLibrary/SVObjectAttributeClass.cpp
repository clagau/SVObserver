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

#include "stdafx.h"
#include "SVObjectAttributeClass.h"

SVObjectAttributeClass::SVObjectAttributeClass()
{
	miType = SV_OBJECT_TYPE_UNKNOWN;
}


SVObjectAttributeClass::SVObjectAttributeClass(SVObjectAttributeClass& orig)
{
	(*this) = orig;
}

SVObjectAttributeClass::~SVObjectAttributeClass()
{
}

SVObjectAttributeClass& SVObjectAttributeClass::operator=(SVObjectAttributeClass& rhs)
{
	if ( this != &rhs )
	{
		SetName( rhs.GetName() );
		SetType( rhs.GetType() );

		msvBoolArray = rhs.msvBoolArray;
		msvByteArray = rhs.msvByteArray;
		msvCStringArray = rhs.msvCStringArray;
		msvDoubleArray = rhs.msvDoubleArray;
		msvDWordArray = rhs.msvDWordArray;
		msvLongArray = rhs.msvLongArray;
		msvPointArray = rhs.msvPointArray;
		msvDPointArray = rhs.msvDPointArray;
		msvVariantArray = rhs.msvVariantArray;
		msvInt64Array = rhs.msvInt64Array;
	}	
	return rhs;
}


LPCTSTR SVObjectAttributeClass::GetName()
{
	return mcsName;
}


int SVObjectAttributeClass::GetType()
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
		case SV_OBJECT_TYPE_CSTRING:
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
			svData = msvBoolArray;

			bOk = TRUE;

			break;
		}
		case SV_OBJECT_TYPE_BYTE:
		{
			break;
		}
		case SV_OBJECT_TYPE_CSTRING:
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
			svData = msvByteArray;

			bOk = TRUE;

			break;
		}
		case SV_OBJECT_TYPE_CSTRING:
		{
			for ( int i = 0; i < msvCStringArray.GetSize(); i++ )
			{
				BYTE bTemp = (BYTE)atoi( msvCStringArray[i] );

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
			for (int i=0; i < msvByteArray.GetSize(); i++)
			{
				char c = (char)msvByteArray[i];
				svData.Add(c);
			}

			bOk = TRUE;

			break;
		}
		case SV_OBJECT_TYPE_CSTRING:
		{
			for ( int i = 0; i < msvCStringArray.GetSize(); i++ )
			{
				char cTemp = (char) atoi( msvCStringArray[i] );

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

BOOL SVObjectAttributeClass::GetData(SvCl::SVObjectCStringArrayClass& svData)
{
	BOOL bOk = FALSE;

	switch ( GetType() )
	{
		case SV_OBJECT_TYPE_BOOL:
		{
			for ( int i = 0; i < msvBoolArray.GetSize(); i++ )
			{
				CString csTemp;
				
				if ( msvBoolArray[i] )
				{
					csTemp = "TRUE";
				}
				else
				{
					csTemp = "FALSE";
				}

				svData.InsertAt( i, csTemp );
			}

			bOk = TRUE;

			break;
		}
		case SV_OBJECT_TYPE_BYTE:
		{
			for ( int i = 0; i < msvByteArray.GetSize(); i++ )
			{
				CString csTemp;

				csTemp.Format( "%x", msvByteArray[i] );

				svData.InsertAt( i, csTemp );
			}

			bOk = TRUE;

			break;
		}
		case SV_OBJECT_TYPE_CSTRING:
		{
			svData = msvCStringArray;

			bOk = TRUE;

			break;
		}
		case SV_OBJECT_TYPE_DOUBLE:
		{
			for ( int i = 0; i < msvDoubleArray.GetSize(); i++ )
			{
				CString csTemp;

				csTemp.Format( "%f", msvDoubleArray[i] );

				svData.InsertAt( i, csTemp );
			}

			bOk = TRUE;

			break;
		}
		case SV_OBJECT_TYPE_DWORD:
		{
			for ( int i = 0; i < msvDWordArray.GetSize(); i++)
			{
				CString csTemp;

				csTemp.Format( "%lx", msvDWordArray[i] );

				svData.InsertAt( i, csTemp );
			}

			bOk = TRUE;

			break;
		}
		case SV_OBJECT_TYPE_LONG:
		{
			for ( int i = 0; i < msvLongArray.GetSize(); i++)
			{
				CString csTemp;

				csTemp.Format( "%ld", msvLongArray[i] );

				svData.InsertAt( i, csTemp );
			}

			bOk = TRUE;

			break;
		}
		case SV_OBJECT_TYPE_POINT:
		{
			for ( int i = 0; i < msvPointArray.GetSize(); i++)
			{
				CString csTemp = msvPointArray[i].ToString().c_str();

				svData.InsertAt( i, csTemp );
			}

			bOk = TRUE;

			break;
		}
		case SV_OBJECT_TYPE_DPOINT:
		{
			for ( int i = 0; i < msvDPointArray.GetSize(); i++)
			{
				CString csTemp;

				csTemp.Format( "(%lf,%lf)", msvDPointArray[i].x, msvDPointArray[i].y );

				svData.InsertAt( i, csTemp );
			}

			bOk = TRUE;

			break;
		}
		case SV_OBJECT_TYPE_VARIANT:
		{
			for ( int i = 0; i < msvDPointArray.GetSize(); i++)
			{
				CString csTemp;

				VARIANT vtTemp = msvVariantArray[i];
				HRESULT hr = ::VariantChangeType( &vtTemp, &vtTemp, VARIANT_ALPHABOOL, VT_BSTR);
				if(hr == S_OK)
				{
					csTemp = vtTemp.bstrVal;
				}
				else
				{
					csTemp = "";
				}
				svData.InsertAt( i, csTemp );
			}

			bOk = TRUE;

			break;
		}
		case SV_OBJECT_TYPE_INT64:
		{
			for ( int i = 0; i < msvInt64Array.GetSize(); i++)
			{
				CString csTemp;

				csTemp.Format( "%I64d", msvInt64Array[i] );

				svData.InsertAt( i, csTemp );
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
}// end GetData(SVObjectArrayClassTemplate<CString>& svData)

BOOL SVObjectAttributeClass::GetData(SvCl::SVObjectArrayClassTemplate<SVString>& svData)
{
	BOOL bOk = FALSE;

	switch ( GetType() )
	{
		case SV_OBJECT_TYPE_BOOL:
		{
			for ( int i = 0; i < msvBoolArray.GetSize(); i++ )
			{
				SVString csTemp;
				
				if ( msvBoolArray[i] )
				{
					csTemp = "TRUE";
				}
				else
				{
					csTemp = "FALSE";
				}

				svData.InsertAt( i, csTemp );
			}

			bOk = TRUE;

			break;
		}
		case SV_OBJECT_TYPE_BYTE:
		{
			for ( int i = 0; i < msvByteArray.GetSize(); i++ )
			{
				SVString csTemp;

				csTemp = SvUl_SF::Format( "%x", msvByteArray[i] );

				svData.InsertAt( i, csTemp );
			}

			bOk = TRUE;

			break;
		}
		case SV_OBJECT_TYPE_CSTRING:
		{
			svData = msvCStringArray;

			bOk = TRUE;

			break;
		}
		case SV_OBJECT_TYPE_DOUBLE:
		{
			for ( int i = 0; i < msvDoubleArray.GetSize(); i++ )
			{
				SVString csTemp;

				csTemp = SvUl_SF::Format( "%f", msvDoubleArray[i] );

				svData.InsertAt( i, csTemp );
			}

			bOk = TRUE;

			break;
		}
		case SV_OBJECT_TYPE_DWORD:
		{
			for ( int i = 0; i < msvDWordArray.GetSize(); i++)
			{
				SVString csTemp;

				csTemp = SvUl_SF::Format( "%lx", msvDWordArray[i] );

				svData.InsertAt( i, csTemp );
			}

			bOk = TRUE;

			break;
		}
		case SV_OBJECT_TYPE_LONG:
		{
			for ( int i = 0; i < msvLongArray.GetSize(); i++)
			{
				SVString csTemp;

				csTemp = SvUl_SF::Format( "%ld", msvLongArray[i] );

				svData.InsertAt( i, csTemp );
			}

			bOk = TRUE;

			break;
		}
		case SV_OBJECT_TYPE_POINT:
		{
			for ( int i = 0; i < msvPointArray.GetSize(); i++)
			{
				const SVString& csTemp = msvPointArray[i].ToString();

				svData.InsertAt( i, csTemp );
			}

			bOk = TRUE;

			break;
		}
		case SV_OBJECT_TYPE_DPOINT:
		{
			for ( int i = 0; i < msvDPointArray.GetSize(); i++)
			{
				SVString csTemp = SvUl_SF::Format( "(%lf,%lf)", msvDPointArray[i].x, msvDPointArray[i].y );

				svData.InsertAt( i, csTemp );
			}

			bOk = TRUE;

			break;
		}
		case SV_OBJECT_TYPE_VARIANT:
		{
			for ( int i = 0; i < msvDPointArray.GetSize(); i++)
			{
				SVString csTemp;

				VARIANT vtTemp = msvVariantArray[i];
				HRESULT hr = ::VariantChangeType( &vtTemp, &vtTemp, VARIANT_ALPHABOOL, VT_BSTR);
				if(hr == S_OK)
				{
					csTemp = SvUl_SF::createSVString(vtTemp);
				}
				else
				{
					csTemp = "";
				}
				svData.InsertAt( i, csTemp );
			}

			bOk = TRUE;

			break;
		}
		case SV_OBJECT_TYPE_INT64:
		{
			for ( int i = 0; i < msvInt64Array.GetSize(); i++)
			{
				SVString csTemp = SvUl_SF::Format( "%I64d", msvInt64Array[i] );

				svData.InsertAt( i, csTemp );
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
}// end GetData(SVObjectArrayClassTemplate<CString>& svData)


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
			for ( int i = 0; i < msvByteArray.GetSize(); i++ )
			{
				double dTemp = (double)msvByteArray[i];

				svData.InsertAt( i, dTemp );
			}

			bOk = TRUE;

			break;
		}
		case SV_OBJECT_TYPE_CSTRING:
		{
			for ( int i = 0; i < msvCStringArray.GetSize(); i++ )
			{
				double dTemp = atof( msvCStringArray[i] );

				svData.InsertAt( i, dTemp );
			}

			bOk = TRUE;

			break;
		}
		case SV_OBJECT_TYPE_DOUBLE:
		{
			svData = msvDoubleArray;

			bOk = TRUE;

			break;
		}
		case SV_OBJECT_TYPE_DWORD:
		{
			for ( int i = 0; i < msvDWordArray.GetSize(); i++)
			{
				double dTemp = (double)msvDWordArray[i];

				svData.InsertAt( i, dTemp );
			}

			bOk = TRUE;

			break;
		}
		case SV_OBJECT_TYPE_LONG:
		{
			for ( int i = 0; i < msvLongArray.GetSize(); i++)
			{
				double dTemp = (double)msvLongArray[i];

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

			for ( int i = 0; i < msvVariantArray.GetSize(); i++)
			{
				VARIANT vtTemp = msvVariantArray[i];
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
			for ( int i = 0; i < msvByteArray.GetSize(); i++ )
			{
				DWORD dwTemp = (DWORD)msvByteArray[i];

				svData.InsertAt( i, dwTemp );
			}

			bOk = TRUE;

			break;
		}
		case SV_OBJECT_TYPE_CSTRING:
		{
			for ( int i = 0; i < msvCStringArray.GetSize(); i++ )
			{
				DWORD dwTemp = (DWORD)atol( msvCStringArray[i] );

				svData.InsertAt( i, dwTemp );
			}

			bOk = TRUE;

			break;
		}
		case SV_OBJECT_TYPE_DOUBLE:
		{
			for ( int i = 0; i < msvDoubleArray.GetSize(); i++)
			{
				DWORD dwTemp = (DWORD)msvDoubleArray[i];

				svData.InsertAt( i, dwTemp );
			}

			bOk = TRUE;

			break;
		}
		case SV_OBJECT_TYPE_DWORD:
		{
			svData = msvDWordArray;

			bOk = TRUE;

			break;
		}
		case SV_OBJECT_TYPE_LONG:
		{
			for ( int i = 0; i < msvLongArray.GetSize(); i++)
			{
				DWORD dwTemp = (DWORD)msvLongArray[i];

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

			for ( int i = 0; i < msvVariantArray.GetSize(); i++)
			{
				VARIANT vtTemp = msvVariantArray[i];
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
			for ( int i = 0; i < msvByteArray.GetSize(); i++ )
			{
				long lTemp = (long)msvByteArray[i];

				svData.InsertAt( i, lTemp );
			}

			bOk = TRUE;

			break;
		}
		case SV_OBJECT_TYPE_CSTRING:
		{
			for ( int i = 0; i < msvCStringArray.GetSize(); i++ )
			{
				long lTemp = atol( msvCStringArray[i] );

				svData.InsertAt( i, lTemp );
			}

			bOk = TRUE;

			break;
		}
		case SV_OBJECT_TYPE_DOUBLE:
		{
			for ( int i = 0; i < msvDoubleArray.GetSize(); i++)
			{
				long lTemp = (long)msvDoubleArray[i];

				svData.InsertAt( i, lTemp );
			}

			bOk = TRUE;

			break;
		}
		case SV_OBJECT_TYPE_DWORD:
		{
			for ( int i = 0; i < msvDWordArray.GetSize(); i++)
			{
				long lTemp = (long)msvDWordArray[i];

				svData.InsertAt( i, lTemp );
			}

			bOk = TRUE;

			break;
		}
		case SV_OBJECT_TYPE_LONG:
		{
			svData = msvLongArray;

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

			for ( int i = 0; i < msvVariantArray.GetSize(); i++)
			{
				VARIANT vtTemp = msvVariantArray[i];
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
		case SV_OBJECT_TYPE_CSTRING:
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

			for ( int i = 0; bOk && i < msvPointArray.GetSize(); i++)
			{
				POINT Point;
				
				msvPointArray[i].GetValue( Point );

				svData.InsertAt( i, SVPOINT( Point ) );
			}

			break;
		}
		case SV_OBJECT_TYPE_DPOINT:
		{
			bOk = TRUE;

			for ( int i = 0; bOk && i < msvDPointArray.GetSize(); i++)
			{
				POINT Point;
				
				msvDPointArray[i].GetValue( Point );

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
		case SV_OBJECT_TYPE_CSTRING:
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

			for ( int i = 0; bOk && i < msvPointArray.GetSize(); i++)
			{
				SVDPointClass Point;
				
				Point = msvPointArray[i];

				svData.InsertAt( i, Point );
			}

			break;
		}
		case SV_OBJECT_TYPE_DPOINT:
		{
			svData = msvDPointArray;

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
			for ( int i = 0; i < msvByteArray.GetSize(); i++ )
			{
				_variant_t vt;
				vt.vt = VT_BOOL;
				vt.boolVal = msvBoolArray[i];

				svData.InsertAt( i, vt );
			}

			bOk = TRUE;

			break;
		}
		case SV_OBJECT_TYPE_BYTE:
		{
			for ( int i = 0; i < msvByteArray.GetSize(); i++ )
			{
				_variant_t vt;
				vt.vt = VT_UI1;
				vt.bVal = msvByteArray[i];

				svData.InsertAt( i, vt );
			}

			bOk = TRUE;

			break;
		}
		case SV_OBJECT_TYPE_CSTRING:
		{
			for ( int i = 0; i < msvByteArray.GetSize(); i++ )
			{
				_variant_t vt;
				vt.vt = VT_BSTR;
				vt.bstrVal = msvCStringArray[i].AllocSysString();

				svData.InsertAt( i, vt );
			}

			bOk = TRUE;

			break;
		}
		case SV_OBJECT_TYPE_DOUBLE:
		{
			for ( int i = 0; i < msvByteArray.GetSize(); i++ )
			{
				_variant_t vt;
				vt.vt = VT_R8;
				vt.dblVal = msvDoubleArray[i];

				svData.InsertAt( i, vt );
			}

			bOk = TRUE;

			break;
		}
		case SV_OBJECT_TYPE_DWORD:
		{
			for ( int i = 0; i < msvByteArray.GetSize(); i++ )
			{
				_variant_t vt;
				vt.vt = VT_UI4;
				//vt.bVal = msvDWordArray[i];
				vt.ulVal = msvDWordArray[i];

				svData.InsertAt( i, vt );
			}

			bOk = TRUE;

			break;
		}
		case SV_OBJECT_TYPE_LONG:
		{
			for ( int i = 0; i < msvByteArray.GetSize(); i++ )
			{
				_variant_t vt;
				vt.vt = VT_I4;
				//vt.bVal = msvLongArray[i];
				vt.lVal = msvLongArray[i];

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

			for ( int i = 0; i < msvVariantArray.GetSize(); i++)
			{
				_variant_t vtTemp;
				HRESULT hr = ::VariantCopy(&vtTemp, &msvVariantArray[i]);
				svData.InsertAt(i, vtTemp);
			}

			bOk = TRUE;
			break;
		}
		case SV_OBJECT_TYPE_INT64:
		{
			for ( int i = 0; i < msvInt64Array.GetSize(); i++ )
			{
				_variant_t vt;
				vt.vt = VT_I8;
				vt.llVal = msvInt64Array[i];

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
		case SV_OBJECT_TYPE_CSTRING:
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
			svData = msvInt64Array;
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
	mcsName = szName;

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
		msvBoolArray.Add( Value );
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

		msvVariantArray.Add( l_Variant );
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
		msvByteArray.Add( Value );
	}

	return bOk;
}

BOOL SVObjectAttributeClass::AddData(LPCTSTR Value)
{
	BOOL bOk = TRUE;

	if ( GetType() != SV_OBJECT_TYPE_CSTRING )
	{
		bOk = SetType( SV_OBJECT_TYPE_CSTRING );
	}

	if ( bOk )
	{
		CString csTemp = Value;

		msvCStringArray.Add( csTemp );
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
		msvDoubleArray.Add( Value );
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
		msvDWordArray.Add( Value );
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
		msvLongArray.Add( Value );
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
		msvPointArray.Add( Value );
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
		msvDPointArray.Add( Value );
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
		msvInt64Array.Add( Value );
	}

	return bOk;
}


// ***** Private Operators *****



BOOL SVObjectAttributeClass::SetType(int iType)
{
	miType = iType;

	msvBoolArray.RemoveAll();
	msvByteArray.RemoveAll();
	msvCStringArray.RemoveAll();
	msvDoubleArray.RemoveAll();
	msvDWordArray.RemoveAll();
	msvLongArray.RemoveAll();
	msvPointArray.RemoveAll();
	msvDPointArray.RemoveAll();
	msvInt64Array.RemoveAll();

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
			bOk = SetType( SV_OBJECT_TYPE_CSTRING );
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

