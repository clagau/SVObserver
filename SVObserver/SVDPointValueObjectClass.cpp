//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDPointValueObjectClass
//* .File Name       : $Workfile:   SVDPointValueObjectClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   01 Aug 2013 11:21:42  $
//******************************************************************************

#include "stdafx.h"
#include "SVDPointValueObjectClass.h"
#include "SVObjectLibrary\SVToolsetScriptTags.h"
#include "SVObjectLibrary/SVObjectAttributeClass.h"
#include "SVOMFCLibrary/StringMunge.h"
#include "SVOMFCLibrary/SVOMFCLibraryGlobals.h"

namespace	// only for this file
{
	const CString DEFAULT_TAG_SAVE(_T(".Default"));
	const CString BUCKET_TAG_SAVE(_T(".Array"));	// for backwards compatibility
	const CString ARRAY_TAG_SAVE(_T(".Array_Elements"));	// new style; one bucket, all array values

	const CString DEFAULT_TAG_LOAD(_T("Default"));
	const CString BUCKET_TAG_LOAD(_T("Array"));	// for backwards compatibility
	const CString ARRAY_TAG_LOAD(_T("Array_Elements"));	// new style; one bucket, all array values
}	// end file scope namespace

SV_IMPLEMENT_CLASS( SVDPointValueObjectClass, SVDPointValueObjectClassGuid );

SVDPointValueObjectClass::SVDPointValueObjectClass( LPCTSTR ObjectName )
				        : base( ObjectName ) 
{
	LocalInitialize();
}

SVDPointValueObjectClass::SVDPointValueObjectClass( SVObjectClass* POwner, int StringResourceID )
				        : base( POwner, StringResourceID ) 
{
	LocalInitialize();
}

const SVDPointValueObjectClass& SVDPointValueObjectClass::operator = (const SVDPointValueObjectClass& rhs)
{
	base::operator = (rhs);
	return *this;
}
  
SVDPointValueObjectClass::~SVDPointValueObjectClass()
{
	// Delete all object's data arrays...
	SetObjectDepth( 0 );
	ResetObject();
}

BOOL SVDPointValueObjectClass::CreateObject( SVObjectLevelCreateStruct* pCreateStructure )
{
	BOOL bCreate = base::CreateObject( pCreateStructure );
	return bCreate;
}

void SVDPointValueObjectClass::Persist(SVObjectWriter& rWriter)
{
	rWriter.StartElement(GetObjectName()); // use internal name for node name

	// Get the Heading (Class Info)
	SVValueObjectClass::Persist( rWriter );

	// Get the Data Values (Member Info, Values)
	CString tmp;
	tmp.Format( _T( "%lf, %lf"), DefaultValue().x, DefaultValue().y );
	
	_variant_t value;
	value.SetString(tmp);
	rWriter.WriteAttribute(scDefaultTag, value);
	value.Clear();

	rWriter.StartElement(scArrayElementsTag);
	// Where does Object Depth Get put into the Script ??? (maybe at the SVObjectClass)
	// Object Depth is implicit (it's the count of the values)
	SVVariantList list;	

	// for all elements in the array (m_iArraySize)
	for( int i = 0; i < m_iArraySize; i++ )
	{
		tmp.Format( _T("%lf, %lf"), Element(m_iLastSetIndex, i).x, Element(m_iLastSetIndex, i).y );
		value.SetString(tmp);
		list.push_back(value);
		value.Clear();
	}
	rWriter.WriteAttribute(scElementTag, list);
	rWriter.EndElement();

	rWriter.EndElement();
}

HRESULT SVDPointValueObjectClass::SetObjectValue( SVObjectAttributeClass* PDataObject )
{
	HRESULT hr = S_FALSE;
	BOOL bOk = FALSE;

	SVObjectDPointArrayClass svPointArray;
	bucket_type l_Buckets(BucketsNoAssert());

	if ( ( bOk = PDataObject->GetAttributeData( "defaultPoint", svPointArray ) ) )
	{
		for( int i = 0; i < svPointArray.GetSize(); i++ )
		{
			DefaultValue() = svPointArray[i];
		}
	}
	else if ( bOk = PDataObject->GetAttributeData( "pArray", l_Buckets, DefaultValue() ) )
	{
		if ( ArraySize() == 1 )
		{
			ScalarBucket(0)=l_Buckets[0][0];
			ScalarBucket(1)=l_Buckets[1][0];
		}
		else
		{
			std::swap(Buckets(), l_Buckets);
		}
		if ( 1 < m_iNumberOfBuckets )
		{
			m_iLastSetIndex = 1;
		}
	}
	else
	{
		hr = base::SetObjectValue( PDataObject );
		return hr;
	}

	hr = bOk ? S_OK : S_FALSE;
	return hr;
}

HRESULT SVDPointValueObjectClass::SetValueAt( int iBucket, int iIndex, const POINT Point )
{
	return base::SetValueAt(iBucket, iIndex, SVDPointClass(Point));
}

HRESULT SVDPointValueObjectClass::SetValueAt( int iBucket, int iIndex, const CPoint Point )
{
	return base::SetValueAt(iBucket, iIndex, SVDPointClass(Point));
}

HRESULT SVDPointValueObjectClass::SetDefaultValue( const POINT Point, bool bResetAll )
{
	return base::SetDefaultValue( SVDPointClass(Point), bResetAll );
}

HRESULT SVDPointValueObjectClass::GetDefaultValue( POINT& rPoint )
{
	DefaultValue().GetValue( rPoint );
	return IsCreated() ? S_OK : S_FALSE;
}

HRESULT SVDPointValueObjectClass::SetDefaultValue( const CPoint Point, bool bResetAll )
{
	return base::SetDefaultValue( SVDPointClass(Point), bResetAll );
}

HRESULT SVDPointValueObjectClass::GetDefaultValue( CPoint& rPoint )
{
	DefaultValue().GetValue( rPoint );
	return IsCreated() ? S_OK : S_FALSE;
}

HRESULT SVDPointValueObjectClass::SetValueAt( int iBucket, int iIndex, CString strValue )
{
	CString strLegalChars (strValue);
	StringMunge::KeepChars( &strLegalChars, _T("0123456789()-., ") );	// floats
	if ( strLegalChars == strValue )
	{
		StringMunge::StripChars( &strLegalChars, _T("()") );
		int iComma = strLegalChars.Find(_T(','));
		if ( iComma > 0 && iComma < strLegalChars.GetLength() - 1 )
		{
			CString sX = strLegalChars.Left( iComma );
			CString sY = strLegalChars.Mid( iComma + 1 );
			SVDPointClass ptValue (atof(sX), atof(sY));
			return base::SetValueAt(iBucket, iIndex, ptValue );
		}
	}
	ASSERT(FALSE);
	return S_FALSE;
}

HRESULT SVDPointValueObjectClass::SetValueAt( int iBucket, int iIndex, double value )
{
	ASSERT(FALSE);
	return S_FALSE;
}

HRESULT SVDPointValueObjectClass::GetValueAt( int iBucket, int iIndex, POINT& rPoint ) const
{
	SVDPointClass value;

	HRESULT hr = base::GetValueAt(iBucket, iIndex, value);
	value.GetValue(rPoint);

	return hr;
}

HRESULT SVDPointValueObjectClass::GetValueAt( int nBucket, int iIndex, double& rstrValue ) const
{
	ASSERT(FALSE);
	return S_FALSE;
}

HRESULT SVDPointValueObjectClass::GetValueAt( int iBucket, int iIndex, CPoint& rPoint ) const
{
	SVDPointClass value;
	
	HRESULT hr = base::GetValueAt(iBucket, iIndex, value);
	value.GetValue(rPoint);

	return hr;
}

HRESULT SVDPointValueObjectClass::GetValueAt( int iBucket, int iIndex, CString& rstrValue ) const
{
	SVDPointClass value;

	HRESULT hr = base::GetValueAt( iBucket, iIndex, value );
	//if ( S_OK == hr ) // @WARNING - log an error here?
	{
		rstrValue.Format( _T( "( %lf, %lf )" ), value.x, value.y );
	}

	return hr;
}

HRESULT SVDPointValueObjectClass::GetValueAt( int iBucket, int iIndex, VARIANT& rValue ) const
{
	SVDPointClass l_dPoint;

	_variant_t l_Temp;
	l_Temp.Attach( rValue );

	HRESULT hr = base::GetValueAt( iBucket, iIndex, l_dPoint );
	if( S_OK == hr )
	{
		CString l_strTmp;
		l_strTmp.Format(  _T( "( %lf, %lf )"),l_dPoint.x, l_dPoint.y);
		l_Temp = l_strTmp;
	}
	else
	{
		l_Temp.Clear();
	}
	rValue = l_Temp.Detach();

	return hr;
}

void SVDPointValueObjectClass::LocalInitialize()
{
	m_outObjectInfo.ObjectTypeInfo.ObjectType = SVDPointValueObjectType;

	ObjectAttributesAllowedRef() = SV_VIEWABLE | SV_ARCHIVABLE | SV_EMBEDABLE | SV_PRINTABLE | SV_DD_VALUE;

	m_strTypeName = "Point";

	InitializeBuckets();
}

