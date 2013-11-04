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

BOOL SVObjectAttributeClass::GetData(SVObjectBoolArrayClass& svData)
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


BOOL SVObjectAttributeClass::GetData(SVObjectByteArrayClass& svData)
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

BOOL SVObjectAttributeClass::GetData(SVObjectArrayClassTemplate<char>& svData)
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

BOOL SVObjectAttributeClass::GetData(SVObjectArrayClassTemplate<CString>& svData)
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

BOOL SVObjectAttributeClass::GetData(SVObjectArrayClassTemplate<SVString>& svData)
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
				SVString csTemp;

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
				SVString csTemp;

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
				SVString csTemp;

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
				SVString csTemp;

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
				SVString csTemp;

				VARIANT vtTemp = msvVariantArray[i];
				HRESULT hr = ::VariantChangeType( &vtTemp, &vtTemp, VARIANT_ALPHABOOL, VT_BSTR);
				if(hr == S_OK)
				{
					csTemp = vtTemp;
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
				SVString csTemp;

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


BOOL SVObjectAttributeClass::GetData(SVObjectDoubleArrayClass& svData)
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


BOOL SVObjectAttributeClass::GetData(SVObjectDWordArrayClass& svData)
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


BOOL SVObjectAttributeClass::GetData(SVObjectLongArrayClass& svData)
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

BOOL SVObjectAttributeClass::GetData(SVObjectVariantArrayClass& svData)
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

BOOL SVObjectAttributeClass::GetData(SVObjectInt64ArrayClass& svData)
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

BOOL SVObjectAttributeClass::SetData(int iType, int iSize, void * pvData)
{
	BOOL bOk = FALSE;

	switch ( iType )
	{
		case SV_OBJECT_TYPE_BOOL:
		{
			SVObjectBoolArrayClass svArray;

			BOOL *pArray = (BOOL *)(pvData);

			for ( int i = 0; i < iSize; i++ )
			{
				svArray.InsertAt( i, pArray[i] );
			}

			bOk = SetData( svArray );

			break;
		}
		case SV_OBJECT_TYPE_BYTE:
		{
			SVObjectByteArrayClass svArray;

			BYTE *pArray = (BYTE *)(pvData);

			for ( int i = 0; i < iSize; i++ )
			{
				svArray.InsertAt( i, pArray[i] );
			}

			bOk = SetData( svArray );

			break;
		}
		case SV_OBJECT_TYPE_CSTRING:
		{
			bOk = SetData( *((SVObjectCStringArrayClass *)(pvData)) );

			break;
		}
		case SV_OBJECT_TYPE_DOUBLE:
		{
			SVObjectDoubleArrayClass svArray;

			double *pArray = (double *)(pvData);

			for ( int i = 0; i < iSize; i++ )
			{
				svArray.InsertAt( i, pArray[i] );
			}

			bOk = SetData( svArray );

			break;
		}
		case SV_OBJECT_TYPE_DWORD:
		{
			SVObjectDWordArrayClass svArray;

			DWORD *pArray = (DWORD *)(pvData);

			for ( int i = 0; i < iSize; i++ )
			{
				svArray.InsertAt( i, pArray[i] );
			}

			bOk = SetData( svArray );

			break;
		}
		case SV_OBJECT_TYPE_LONG:
		{
			SVObjectLongArrayClass svArray;

			long *pArray = (long *)(pvData);

			for ( int i = 0; i < iSize; i++ )
			{
				svArray.InsertAt( i, pArray[i] );
			}

			bOk = SetData( svArray );

			break;
		}
		case SV_OBJECT_TYPE_POINT:
		{
			SVObjectSVPointArrayClass svArray;

			POINT *pArray = (POINT *)(pvData);

			for ( int i = 0; i < iSize; i++ )
			{
				svArray.InsertAt( i, pArray[i] );
			}

			bOk = SetData( svArray );

			break;
		}
		case SV_OBJECT_TYPE_DPOINT:
		{
			SVObjectDPointArrayClass svArray;

			SVDPointClass *pArray = (SVDPointClass *)(pvData);

			for ( int i = 0; i < iSize; i++ )
			{
				svArray.InsertAt( i, pArray[i] );
			}

			bOk = SetData( svArray );

			break;
		}
		case SV_OBJECT_TYPE_VARIANT:
		{
			SVObjectVariantArrayClass svArray;

			_variant_t* pArray = ( _variant_t* )(pvData);

			for ( int i = 0; i < iSize; i++ )
			{
				svArray.InsertAt( i, pArray[i] );
			}

			bOk = SetData( svArray );

			break;
		}
		case SV_OBJECT_TYPE_INT64:
		{
			SVObjectInt64ArrayClass svArray;

			__int64* pArray = (__int64 *)(pvData);

			for ( int i = 0; i < iSize; i++ )
			{
				svArray.InsertAt( i, pArray[i] );
			}

			bOk = SetData( svArray );

			break;
		}
		default:
		{
			break;
		}
	}

	return bOk;
}// end SetData(int iType, int iSize, void * pvData)

BOOL SVObjectAttributeClass::SetData(SVObjectInt64ArrayClass& svData)
{
	BOOL bOk = SetType( SV_OBJECT_TYPE_INT64 );

	if ( bOk )
	{
		msvInt64Array = svData;
	}

	return bOk;
}

BOOL SVObjectAttributeClass::SetData(SVObjectVariantArrayClass& svData)
{
	BOOL bOk = SetType( SV_OBJECT_TYPE_VARIANT );

	if ( bOk )
	{
		msvVariantArray = svData;
	}

	return bOk;
}



BOOL SVObjectAttributeClass::SetData(SVObjectBoolArrayClass& svData)
{
	BOOL bOk = SetType( SV_OBJECT_TYPE_BOOL );

	if ( bOk )
	{
		msvBoolArray = svData;
	}

	return bOk;
}


BOOL SVObjectAttributeClass::SetData(SVObjectByteArrayClass& svData)
{
	BOOL bOk = SetType( SV_OBJECT_TYPE_BYTE );

	if ( bOk )
	{
		msvByteArray = svData;
	}

	return bOk;
}


BOOL SVObjectAttributeClass::SetData(SVObjectCStringArrayClass& svData)
{
	BOOL bOk = SetType( SV_OBJECT_TYPE_CSTRING );

	if ( bOk )
	{
		msvCStringArray = svData;
	}

	return bOk;
}


BOOL SVObjectAttributeClass::SetData(SVObjectDoubleArrayClass& svData)
{
	BOOL bOk = SetType( SV_OBJECT_TYPE_DOUBLE );

	if ( bOk )
	{
		msvDoubleArray = svData;
	}

	return bOk;
}


BOOL SVObjectAttributeClass::SetData(SVObjectDWordArrayClass& svData)
{
	BOOL bOk = SetType( SV_OBJECT_TYPE_DWORD );

	if ( bOk )
	{
		msvDWordArray = svData;
	}

	return bOk;
}


BOOL SVObjectAttributeClass::SetData(SVObjectLongArrayClass& svData)
{
	BOOL bOk = SetType( SV_OBJECT_TYPE_LONG );

	if ( bOk )
	{
		msvLongArray = svData;
	}

	return bOk;
}


BOOL SVObjectAttributeClass::SetData(SVObjectSVPointArrayClass& svData)
{
	BOOL bOk = SetType( SV_OBJECT_TYPE_POINT );

	if ( bOk )
	{
		msvPointArray = svData;
	}

	return bOk;
}

BOOL SVObjectAttributeClass::SetData(SVObjectDPointArrayClass& svData)
{
	BOOL bOk = SetType( SV_OBJECT_TYPE_DPOINT );

	if ( bOk )
	{
		msvDPointArray = svData;
	}

	return bOk;
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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObjectLibrary\SVObjectAttributeClass.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 16:38:26   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   04 Sep 2012 13:22:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added 64 bit integer type to the parsing and saving functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   21 Feb 2012 13:20:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  748
 * SCR Title:  Add Remote Output Steams to SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated code to fix loading issue associated with POINT.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   18 Feb 2011 09:44:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  700
 * SCR Title:  Remove String Buffer Problems
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source to remove duplicate string class, and fixed string conversion issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   05 Nov 2010 10:05:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  703
 * SCR Title:  Allow SVObserver to Change Remote Input Values
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove redundent data objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   01 Jun 2010 08:29:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate definitions and redundunt containers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   20 Jun 2007 13:27:52   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   20 Jun 2005 15:12:48   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   templatized GetAttributeData
 * added GetData functions for char and CPoint
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   20 Jun 2005 10:01:32   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  450
 * SCR Title:  Add asynchronous functionality to the archive tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added self assignment check in operator =
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   20 Jun 2005 09:58:28   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  450
 * SCR Title:  Add asynchronous functionality to the archive tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added self assignment check in operator =
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Feb 2005 14:52:50   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  440
 * SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   First version of file for SVObjectLibrary
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   07 Jul 2004 08:48:02   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  416
 * SCR Title:  Add Variant Value Object
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added SV_OBJECT_TYPE_VARIANT
 * added #include "SVObjectArrayClassTemplate.h"
 * added GetData, GetAttributeData, SetData, AddData, msvVariantArray for Variant type
 * cleaned up clutter
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   22 Apr 2003 11:34:48   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   08 Nov 2001 15:00:32   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  255
 * SCR Title:  Add Sub-pixel resolution to Profile and Line Gage Tool Analyzers
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated operator=, GetSVObjectScriptDataObjectTypeEnum, GetData, GetAttributeData, SetData, AddData and SetType methods to calculate sub-pixel results.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   12 Jun 2001 16:19:16   ed
 * Project:  SVObserver
 * Change Request (SCR) nbr:  200
 * SCR Title:  Update code to implement SVIM COM Server
 * Checked in by:  Ed;  Ed Chobanoff
 * Change Description:  
 *   Modified the following functions: GetAttributeData.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   19 Apr 2001 14:48:52   Steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  196
 * SCR Title:  Restructure Scripted Load/Save Procedures Functional Requirement
 * Checked in by:  Steve;  Stephen E. Steffan
 * Change Description:  
 *   New file due to header file changes due to include file restructuring.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
