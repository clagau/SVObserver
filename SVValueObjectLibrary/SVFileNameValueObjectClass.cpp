//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVFileNameValueObjectClass
//* .File Name       : $Workfile:   SVFileNameValueObjectClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   01 Oct 2013 14:19:28  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <assert.h>
#include "SVFileNameValueObjectClass.h"
#include "SVObjectLibrary/SVClsids.h"
#include "SVObjectLibrary/SVToolsetScriptTags.h"
#include "SVObjectLibrary/SVObjectAttributeClass.h"
#include "SVSystemLibrary/SVFileNameManagerClass.h"
#include "SVUtilityLibrary/SVStringConversions.h"
#include "ObjectInterfaces\TextDefineSvOi.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS(SVFileNameValueObjectClass, SVFileNameValueObjectClassGuid);

SVFileNameValueObjectClass::SVFileNameValueObjectClass(LPCTSTR ObjectName)
: base(ObjectName) 
{
	LocalInitialize();
}

SVFileNameValueObjectClass::SVFileNameValueObjectClass(	SVObjectClass* POwner, 	int StringResourceID )
: base(POwner, StringResourceID) 
{
	LocalInitialize();
}

SVFileNameValueObjectClass::SVFileNameValueObjectClass( const SVFileNameValueObjectClass& rhs)
: base()
{
	LocalInitialize();
	*this = rhs;
}

const SVFileNameValueObjectClass& SVFileNameValueObjectClass::operator = ( const SVFileNameValueObjectClass& rhs )
{
	base::operator = (rhs);
	return *this;
}

SVFileNameValueObjectClass::~SVFileNameValueObjectClass()
{
	SVFileNameManagerClass::Instance().RemoveItem(&m_svFileName);
}

void SVFileNameValueObjectClass::Persist(SVObjectWriter& rWriter)
{
	SVFileNameManagerClass::Instance().SaveItem(&m_svFileName);
	
	rWriter.StartElement(GetObjectName()); // use internal name for node name

	// Get the Heading (Class Info)
	SVValueObjectClass::Persist( rWriter );

	// Get the Data Values (Member Info, Values)
	SVString tmp = SvUl_SF::Format( _T("%s"), DefaultValue().c_str() );
	
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
		tmp = SvUl_SF::Format( _T("%s"), Element(m_iLastSetIndex, i).c_str() );
		value.SetString( tmp.c_str() );
		list.push_back(value);
		value.Clear();
	}
	rWriter.WriteAttribute(scElementTag, list);
	rWriter.EndElement();

	rWriter.EndElement();
}

HRESULT SVFileNameValueObjectClass::SetObjectValue(SVObjectAttributeClass* pDataObject)
{
	HRESULT hr  = S_FALSE;
	BOOL	  bOk = FALSE;
	
	SVString  Temp;
	SvCl::SVObjectSVStringArrayClass svArray;
	bucket_type l_Buckets(BucketNoAssert());
	array_type l_Array;
	
	if ( bOk = pDataObject->GetAttributeData(SvOi::cDefaultTag, svArray) )
	{
		for (int i = 0; i < svArray.GetSize(); i++)
		{
			DefaultValue() = svArray[i];
			
			SvUl::RemoveEscapedSpecialCharacters(DefaultValue(), false);
		}
		
		GetDefaultValue(Temp);
		SetDefaultValue(Temp.c_str(), false);
	}
	else if ( bOk = pDataObject->GetAttributeData(SvOi::cBucketTag, l_Buckets, DefaultValue() ) )
	{
		for (size_t i = 0; i < l_Buckets.size(); i++)
		{
			SvUl::RemoveEscapedSpecialCharacters(l_Buckets[i][0], false);
		}

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

		GetValue(m_iLastSetIndex, Temp);
		SetValue(m_iLastSetIndex, Temp);
	}
	// new-style: store all array elements:
	else if ( bOk = pDataObject->GetArrayData( SvOi::cArrayTag, l_Array, DefaultValue() ) )
	{
		for (size_t i = 0; i < l_Array.size(); i++)
		{
			// Remove any escapes
			SvUl::RemoveEscapedSpecialCharacters(l_Array[i], false);
		}

		SetArraySize( static_cast<int>(l_Array.size()) );
		if ( ArraySize() == 1 )
		{
			ScalarBucket(1)=l_Array[0];
		}
		else
		{
			std::swap(Array(1), l_Array);
		}
		m_iLastSetIndex = 1;

		SVString strTemp;
		GetValue(m_iLastSetIndex, strTemp);
		SetValue(m_iLastSetIndex, strTemp);
	}
	else if ( bOk = pDataObject->GetAttributeData(_T("StrDefault"), svArray) )
	{
		for (int i = 0; i < svArray.GetSize(); i++)
		{
			DefaultValue() = svArray[i];
			
			// Remove any escapes
			SvUl::RemoveEscapedSpecialCharacters(DefaultValue(), false);
		}
		
		GetDefaultValue(Temp);
		SetDefaultValue(Temp.c_str(), false);
	}
	else if ( bOk = pDataObject->GetAttributeData(_T("StrArray"), l_Buckets, DefaultValue() ) )
	{
		for (size_t i = 0; i < l_Buckets.size(); i++)
		{
			// Remove any escapes
			SvUl::RemoveEscapedSpecialCharacters(l_Buckets[i][0], false);
		}
		
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
		
		GetValue(m_iLastSetIndex, Temp);
		SetValue(m_iLastSetIndex, Temp);
	}
	else
	{
		hr = base::SetObjectValue(pDataObject);
		return hr;
	}
	
	hr = bOk ? S_OK : S_FALSE;
	return hr;
}

HRESULT SVFileNameValueObjectClass::SetValueAt(int iBucket, int iIndex, const SVString& rValue)
{
	HRESULT hr = ValidateIndexes(iBucket, iIndex);

	if ( S_OK == hr || SVMSG_SVO_34_OBJECT_INDEX_OUT_OF_RANGE == hr )
	{
		m_svFileName.SetFullFileName( rValue.c_str() );
		SVFileNameManagerClass::Instance().LoadItem(&m_svFileName);
		hr = base::SetValueAt(iBucket, iIndex, m_svFileName.GetFullFileName());
	}
	
	return hr;
}

HRESULT SVFileNameValueObjectClass::SetValueAt( int iBucket, int iIndex, const VARIANT& rValue )
{
	if ( VT_BSTR == rValue.vt )
	{
		return SetValueAt(iBucket, iIndex, SvUl_SF::createSVString( rValue.bstrVal ) );
	}
	assert(false);
	return S_FALSE;
}

HRESULT SVFileNameValueObjectClass::SetDefaultValue(LPCTSTR strValue, bool bResetAll)
{
	m_svFileName.SetDefaultFullFileName(strValue);
	
	return base::SetDefaultValue(strValue, bResetAll);
}

HRESULT SVFileNameValueObjectClass::GetValueAt( int iBucket, int iIndex, VARIANT& rValue ) const
{
	SVString strVal;
	_variant_t l_Temp;
	l_Temp.Attach( rValue );
	HRESULT hr = GetValueAt( iBucket, iIndex, strVal );
	if( S_OK == hr )
	{
		l_Temp = strVal.c_str();
	}
	else
	{
		l_Temp.Clear();
	}
	rValue = l_Temp.Detach();

	return hr;
}

HRESULT SVFileNameValueObjectClass::GetValueAt( int iBucket, int iIndex, double& rValue ) const
{
	assert(false);
	return S_FALSE;
}

void SVFileNameValueObjectClass::LocalInitialize()
{
	m_outObjectInfo.ObjectTypeInfo.ObjectType = SVStringValueObjectType;
	
	ObjectAttributesAllowedRef() = SV_VIEWABLE | SV_PUBLISHABLE | SV_ARCHIVABLE | SV_EMBEDABLE | SV_PRINTABLE | SV_DD_VALUE;

	SetTypeName( _T("FileName") );
	
	InitializeBuckets();

	SVFileNameManagerClass::Instance().AddItem(&m_svFileName);
}

