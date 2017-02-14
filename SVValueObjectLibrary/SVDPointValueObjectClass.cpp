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

#pragma region Includes
#include "stdafx.h"
#include "SVDPointValueObjectClass.h"
#include "SVObjectLibrary\SVToolsetScriptTags.h"
#include "SVObjectLibrary\SVClsids.h"
#include "SVObjectLibrary/SVObjectAttributeClass.h"
#include "SVUtilityLibrary/SetBits.h"
#include "SVStatusLibrary/MessageManager.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

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
	SVString tmp;
	tmp = SvUl_SF::Format( _T( "%lf, %lf"), DefaultValue().x, DefaultValue().y );
	
	_variant_t value;
	value.SetString(tmp.c_str());
	rWriter.WriteAttribute(scDefaultTag, value);
	value.Clear();

	rWriter.StartElement(scArrayElementsTag);
	// Where does Object Depth Get put into the Script ??? (maybe at the SVObjectClass)
	// Object Depth is implicit (it's the count of the values)
	SVVariantList list;	

	// for all elements in the array (m_iArraySize)
	for( int i = 0; i < m_iArraySize; i++ )
	{
		tmp = SvUl_SF::Format( _T("%lf, %lf"), Element(m_iLastSetIndex, i).x, Element(m_iLastSetIndex, i).y );
		value.SetString(tmp.c_str());
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
	bucket_type l_Buckets(BucketNoAssert());

	if ( ( bOk = PDataObject->GetAttributeData( "defaultPoint", svPointArray ) ) )
	{
		for( int i = 0; i < svPointArray.GetSize(); i++ )
		{
			DefaultValue() = svPointArray[i];
		}
	}
	else if ( bOk = PDataObject->GetAttributeData( _T("pArray"), l_Buckets, DefaultValue() ) )
	{
		if ( 1 == ArraySize() )
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

HRESULT SVDPointValueObjectClass::SetValueAt( int iBucket, int iIndex, const POINT& Point )
{
	return base::SetValueAt(iBucket, iIndex, SVDPointClass(Point));
}

HRESULT SVDPointValueObjectClass::SetDefaultValue( const POINT& Point, bool bResetAll )
{
	return base::SetDefaultValue( SVDPointClass(Point), bResetAll );
}

HRESULT SVDPointValueObjectClass::GetDefaultValue( POINT& rPoint ) const
{
	DefaultValue().GetValue( rPoint );
	return IsCreated() ? S_OK : S_FALSE;
}

HRESULT SVDPointValueObjectClass::SetValueAt( int iBucket, int iIndex, const SVString& strValue )
{
	try
	{
		SVDPointClass value = convertString2DPoint(strValue);
		return base::SetValueAt(iBucket, iIndex, value );
	}
	catch (const SvStl::MessageContainer&)
	{
		assert(false);
		return S_FALSE;
	}
}

HRESULT SVDPointValueObjectClass::SetValueAt( int iBucket, int iIndex, double value )
{
	assert(false);
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
	assert(false);
	return S_FALSE;
}

HRESULT SVDPointValueObjectClass::GetValueAt( int iBucket, int iIndex, SVString& rValue ) const
{
	SVDPointClass value;

	HRESULT hr = base::GetValueAt( iBucket, iIndex, value );
	//if ( S_OK == hr ) // @WARNING - log an error here?
	{
		rValue = SvUl_SF::Format( _T( "( %lf, %lf )" ), value.x, value.y );
	}

	return hr;
}

HRESULT SVDPointValueObjectClass::GetValueAt( int iBucket, int iIndex, VARIANT& rValue ) const
{
	SVDPointClass l_dPoint;

	_variant_t vtValue;
	vtValue.Attach( rValue );

	HRESULT hr = base::GetValueAt( iBucket, iIndex, l_dPoint );
	if( S_OK == hr )
	{
		SVString Temp = SvUl_SF::Format(  _T("( %lf, %lf )"), l_dPoint.x, l_dPoint.y);
		vtValue = Temp.c_str();
	}
	else
	{
		vtValue.Clear();
	}
	rValue = vtValue.Detach();

	return hr;
}

void SVDPointValueObjectClass::ValidateValue( int iBucket, int iIndex, const SVString& rValue ) const
{
	convertString2DPoint(rValue.c_str());
	base::ValidateValue( iBucket, iIndex, rValue );
}

void SVDPointValueObjectClass::LocalInitialize()
{
	m_outObjectInfo.ObjectTypeInfo.ObjectType = SVDPointValueObjectType;

	ObjectAttributesAllowedRef() = SV_VIEWABLE | SV_ARCHIVABLE | SV_EMBEDABLE | SV_PRINTABLE | SV_DD_VALUE;

	SetTypeName( _T("Point") );

	InitializeBuckets();
}

SVDPointClass SVDPointValueObjectClass::convertString2DPoint(const SVString& rValue ) const
{
	SVString LegalChars = SvUl_SF::ValidateString( rValue, _T("0123456789()-., ") );	// floats
	if ( LegalChars == rValue )
	{
		SvUl_SF::RemoveCharacters( LegalChars, _T("()") );
		size_t Pos = LegalChars.find(_T(','));
		if ( SVString::npos != Pos )
		{
			SVString sX = SvUl_SF::Left( LegalChars, Pos );
			SVString sY = SvUl_SF::Mid( LegalChars, Pos + 1 );
			return SVDPointClass(atof(sX.c_str()), atof(sY.c_str()));
		}
	}
	SVStringVector msgList;
	msgList.push_back(SVString(rValue));
	msgList.push_back(GetName());
	SvStl::MessageMgrStd Exception( SvStl::LogOnly );
	Exception.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_ValueObject_ValidateStringFailed, msgList, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
	Exception.Throw();
	return SVDPointClass(); //will never reached, because the exception will throw before. But this line avoid a warning
}

