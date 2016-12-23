//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVStaticStringValueObjectClass
//* .File Name       : $Workfile:   SVStaticStringValueObjectClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   19 Nov 2013 09:24:20  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <assert.h>
#include "SVStaticStringValueObjectClass.h"
#include "SVObjectLibrary\SVClsids.h"
#include "SVObjectLibrary\SVToolsetScriptTags.h"
#include "SVObjectLibrary/SVObjectAttributeClass.h"
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

SV_IMPLEMENT_CLASS(SVStaticStringValueObjectClass, SVStaticStringValueObjectClassGuid);

SVStaticStringValueObjectClass::SVStaticStringValueObjectClass(LPCTSTR ObjectName)
				         : base(ObjectName) 
{
	LocalInitialize();
}

SVStaticStringValueObjectClass::SVStaticStringValueObjectClass(	SVObjectClass* POwner, 	int StringResourceID )
   : base(POwner, StringResourceID) 
{
	LocalInitialize();
}

SVStaticStringValueObjectClass::SVStaticStringValueObjectClass( const SVStaticStringValueObjectClass& rhs)
	: base()
{
	LocalInitialize();
	*this = rhs;
}

const SVStaticStringValueObjectClass& SVStaticStringValueObjectClass::operator = (const SVStaticStringValueObjectClass& rhs)
{
	base::operator = (rhs);
	return *this;
}

SVStaticStringValueObjectClass::~SVStaticStringValueObjectClass()
{
	// Delete all object's data arrays...
	SetObjectDepth(0);
	CreateBuckets();
}

void SVStaticStringValueObjectClass::Persist(SVObjectWriter& rWriter)
{
	rWriter.StartElement(GetObjectName()); // use internal name for node name

	// Get the ClassID, ObjectID, Attrributes
	SVValueObjectClass::Persist(rWriter);

	// Get the Data Values (Member Info, Values)
	// Check for DoubleQuotes in variable
	CString tmp = DefaultValue();
	SvLib::AddEscapeSpecialCharacters(tmp, true);
	
	_variant_t value;
	value.SetString(tmp);
	rWriter.WriteAttribute(scDefaultTag, value);
	value.Clear();

	rWriter.StartElement(scArrayElementsTag);

	// Where does Object Depth Get put into the Script ??? (maybe at the SVObjectClass)
	// Object Depth is implicit (it's the count of the values)
	SVVariantList list;
	
	// for all elements in the array (objectDepth)
	for (int i = 0; i < m_iArraySize; i++)
	{
		tmp = Element(0, i); 
		SvLib::AddEscapeSpecialCharacters(tmp, true);
		value.SetString(tmp);
		list.push_back(value);
		value.Clear();
	}
	rWriter.WriteAttribute(scElementTag, list);
	rWriter.EndElement();

	rWriter.EndElement();
}

HRESULT  SVStaticStringValueObjectClass::SetObjectValue(SVObjectAttributeClass* pDataObject)
{
	HRESULT hr = S_FALSE;
	BOOL bOk = FALSE;
	
	CString  csTemp;
	SvCl::SVObjectCStringArrayClass svArray;
	bucket_type l_Buckets(BucketsNoAssert());
	array_type l_Array;
	
	if ( bOk = pDataObject->GetAttributeData(DEFAULT_TAG_LOAD, svArray) )
	{
		for (int i = 0; i < svArray.GetSize(); i++)
		{
			DefaultValue() = svArray[i];
			
			// Remove any escapes
			SvLib::RemoveEscapedSpecialCharacters(DefaultValue(), true);
		}
	}
	else if ( bOk = pDataObject->GetAttributeData(BUCKET_TAG_LOAD, l_Buckets, DefaultValue() ) )
	{
		// Remove any escapes
		SvLib::RemoveEscapedSpecialCharacters(l_Buckets[0][0], true);

		if ( ArraySize() == 1 )
		{
			ScalarBucket(0)=l_Buckets[0][0];
		}
		else
		{
			std::swap(Buckets(), l_Buckets);
		}
		if ( 0 < m_iNumberOfBuckets )
		{
			m_iLastSetIndex = 0;
		}
	}
	// new-style: store all array elements:
	else if ( bOk = pDataObject->GetArrayData(ARRAY_TAG_LOAD, l_Array, DefaultValue() ) )
	{
		for (size_t i = 0; i < l_Array.size(); i++)
		{
			// Remove any escapes
			SvLib::RemoveEscapedSpecialCharacters(l_Array[i], true);
		}

		SetArraySize( static_cast<int>(l_Array.size()) );
		if ( ArraySize() == 1 )
		{
			ScalarBucket(0)=l_Array[0];
		}
		else
		{
			std::swap(Array(0), l_Array);
		}
		m_iLastSetIndex = 0;
	}
	else if ((bOk = pDataObject->GetAttributeData("StrDefault", svArray)))
	{
		for (int i = 0; i < svArray.GetSize(); i++)
		{
			DefaultValue() = svArray[i];
			
			// Remove any escapes
			SvLib::RemoveEscapedSpecialCharacters(DefaultValue(), true);
		}
	}
	else if ( bOk = pDataObject->GetAttributeData("StrArray", l_Buckets, DefaultValue() ) )
	{
		// Remove any escapes
		SvLib::RemoveEscapedSpecialCharacters(l_Buckets[0][0], true);

		if ( ArraySize() == 1 )
		{
			ScalarBucket(0)=l_Buckets[0][0];
		}
		else
		{
			std::swap(Buckets(), l_Buckets);
		}
		if ( 0 < m_iNumberOfBuckets )
		{
			m_iLastSetIndex = 0;
		}
	}
	else
	{
		hr = base::SetObjectValue(pDataObject);
		return hr;
	}

	hr = bOk ? S_OK : S_FALSE;
	return hr;
}

HRESULT SVStaticStringValueObjectClass::SetValueAt( int iBucket, int iIndex, const VARIANT& rvtValue )
{
	if ( VT_BSTR == rvtValue.vt )
	{
		return base::SetValueAt(iBucket, iIndex, CString(rvtValue.bstrVal));
	}
	assert(false);
	return S_FALSE;
}

HRESULT SVStaticStringValueObjectClass::GetValueAt( int iBucket, int iIndex, long& rlValue) const
{
	CString strValue;

	HRESULT hr =  base::GetValueAt( iBucket, iIndex, strValue );
	rlValue = atoi( strValue );

	return hr;
}

HRESULT SVStaticStringValueObjectClass::GetValueAt( int iBucket, int iIndex, double& rdValue) const
{
	CString strValue;

	HRESULT hr = base::GetValueAt( iBucket, iIndex, strValue );
	rdValue = atof( strValue );

	return hr;
}

HRESULT SVStaticStringValueObjectClass::GetValueAt( int iBucket, int iIndex, VARIANT& rvtValue ) const
{
	CString strVal;

	_variant_t l_Temp;
	l_Temp.Attach( rvtValue );
	HRESULT hr = base::GetValueAt( iBucket, iIndex, strVal );
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

void SVStaticStringValueObjectClass::LocalInitialize()
{
	m_outObjectInfo.ObjectTypeInfo.ObjectType = SVStringValueObjectType;
	
	ObjectAttributesAllowedRef() = SV_VIEWABLE | SV_PUBLISHABLE | SV_ARCHIVABLE | SV_EMBEDABLE | SV_PRINTABLE | SV_DD_VALUE;
	SetTypeName( _T("Text") );

	InitializeBuckets();
}

