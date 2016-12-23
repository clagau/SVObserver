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
#include "SVLibrary/StringEscape.h"
#pragma endregion Includes

namespace	// only for this file
{
	const CString DEFAULT_TAG_SAVE(_T(".Default"));
	const CString BUCKET_TAG_SAVE(_T(".Array"));	// for backwards compatibility
	const CString ARRAY_TAG_SAVE(_T(".Array_Elements"));	// new style; one bucket, all array values

	const CString DEFAULT_TAG_LOAD(_T("Default"));
	const CString BUCKET_TAG_LOAD(_T("Array"));	// for backwards compatibility
	const CString ARRAY_TAG_LOAD(_T("Array_Elements"));	// new style; one bucket, all array values
}	// end file scope namespace

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
	// Delete all object's data arrays...
	SetObjectDepth(0);
	CreateBuckets();
	
	SVFileNameManagerClass::RemoveItem(&m_svFileName);
}

void SVFileNameValueObjectClass::Persist(SVObjectWriter& rWriter)
{
	SVFileNameManagerClass::SaveItem(&m_svFileName);
	
	rWriter.StartElement(GetObjectName()); // use internal name for node name

	// Get the Heading (Class Info)
	SVValueObjectClass::Persist( rWriter );

	// Get the Data Values (Member Info, Values)
	CString tmp;
	tmp.Format( _T("%s"), DefaultValue() );
	
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
		tmp.Format( _T("%s"), Element(m_iLastSetIndex, i) );
		value.SetString(tmp);
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
	
	CString  csTemp;
	SvCl::SVObjectCStringArrayClass svArray;
	bucket_type l_Buckets(BucketsNoAssert());
	array_type l_Array;
	
	if ( bOk = pDataObject->GetAttributeData(DEFAULT_TAG_LOAD, svArray) )
	{
		for (int i = 0; i < svArray.GetSize(); i++)
		{
			DefaultValue() = svArray[i];
			
			SvLib::RemoveEscapedSpecialCharacters(DefaultValue(), false);
		}
		
		GetDefaultValue(csTemp);
		SetDefaultValue(csTemp, false);
	}
	else if ( bOk = pDataObject->GetAttributeData(BUCKET_TAG_LOAD, l_Buckets, DefaultValue() ) )
	{
		for (size_t i = 0; i < l_Buckets.size(); i++)
		{
			SvLib::RemoveEscapedSpecialCharacters(l_Buckets[i][0], false);
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

		GetValue(m_iLastSetIndex, csTemp);
		SetValue(m_iLastSetIndex, csTemp);
	}
	// new-style: store all array elements:
	else if ( bOk = pDataObject->GetArrayData(ARRAY_TAG_LOAD, l_Array, DefaultValue() ) )
	{
		for (size_t i = 0; i < l_Array.size(); i++)
		{
			// Remove any escapes
			SvLib::RemoveEscapedSpecialCharacters(l_Array[i], false);
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

		CString strTemp;
		GetValue(m_iLastSetIndex, strTemp);
		SetValue(m_iLastSetIndex, strTemp);
	}
	else if ( bOk = pDataObject->GetAttributeData("StrDefault", svArray) )
	{
		for (int i = 0; i < svArray.GetSize(); i++)
		{
			DefaultValue() = svArray[i];
			
			// Remove any escapes
			SvLib::RemoveEscapedSpecialCharacters(DefaultValue(), false);
		}
		
		GetDefaultValue(csTemp);
		SetDefaultValue(csTemp, false);
	}
	else if ( bOk = pDataObject->GetAttributeData("StrArray", l_Buckets, DefaultValue() ) )
	{
		for (size_t i = 0; i < l_Buckets.size(); i++)
		{
			// Remove any escapes
			SvLib::RemoveEscapedSpecialCharacters(l_Buckets[i][0], false);
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
		
		GetValue(m_iLastSetIndex, csTemp);
		SetValue(m_iLastSetIndex, csTemp);
	}
	else
	{
		hr = base::SetObjectValue(pDataObject);
		return hr;
	}
	
	hr = bOk ? S_OK : S_FALSE;
	return hr;
}

HRESULT SVFileNameValueObjectClass::SetValueAt(int iBucket, int iIndex, const CString& strValue)
{
	HRESULT hr = ValidateIndexes(iBucket, iIndex);

	if ( S_OK == hr || SVMSG_SVO_34_OBJECT_INDEX_OUT_OF_RANGE == hr )
	{
		m_svFileName.SetFullFileName(strValue);
		SVFileNameManagerClass::LoadItem(&m_svFileName);
		hr = base::SetValueAt(iBucket, iIndex, m_svFileName.GetFullFileName());
	}
	
	return hr;
}

HRESULT SVFileNameValueObjectClass::SetValueAt( int iBucket, int iIndex, const VARIANT& rvtValue )
{
	if ( VT_BSTR == rvtValue.vt )
	{
		return SetValueAt(iBucket, iIndex, CString(rvtValue.bstrVal) );
	}
	assert(false);
	return S_FALSE;
}

HRESULT SVFileNameValueObjectClass::SetDefaultValue(LPCTSTR strValue, bool bResetAll)
{
	m_svFileName.SetDefaultFullFileName(strValue);
	
	return base::SetDefaultValue(strValue, bResetAll);
}

HRESULT SVFileNameValueObjectClass::GetValueAt( int iBucket, int iIndex, VARIANT& rvtValue ) const
{
	CString strVal;
	_variant_t l_Temp;
	l_Temp.Attach( rvtValue );
	HRESULT hr = GetValueAt( iBucket, iIndex, strVal );
	if( S_OK == hr )
	{
		l_Temp = strVal;
	}
	else
	{
		l_Temp.Clear();
	}
	rvtValue = l_Temp.Detach();

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

	SVFileNameManagerClass::AddItem(&m_svFileName);
}

