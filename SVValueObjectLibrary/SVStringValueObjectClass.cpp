//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVStringValueObjectClass
//* .File Name       : $Workfile:   SVStringValueObjectClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   02 Oct 2013 08:17:48  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <assert.h>
#include "SVStringValueObjectClass.h"
#include "SVObjectLibrary\SVClsids.h"
#include "SVObjectLibrary\SVToolsetScriptTags.h"
#include "SVObjectLibrary/SVObjectAttributeClass.h"
#include "SVUtilityLibrary/SVStringConversions.h"
#include "ObjectInterfaces/TextDefineSvOi.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS(SVStringValueObjectClass, SVStringValueObjectClassGuid);

SVStringValueObjectClass::SVStringValueObjectClass(LPCTSTR ObjectName)
				         : base(ObjectName) 
{
	LocalInitialize();
}

SVStringValueObjectClass::SVStringValueObjectClass(	SVObjectClass* POwner, 	int StringResourceID )
   : base(POwner, StringResourceID) 
{
	LocalInitialize();
}

SVStringValueObjectClass::SVStringValueObjectClass( const SVStringValueObjectClass& rhs)
	: base()
{
	LocalInitialize();
	*this = rhs;
}

const SVStringValueObjectClass& SVStringValueObjectClass::operator = (const SVStringValueObjectClass& rhs)
{
	base::operator = (rhs);
	return *this;
}

SVStringValueObjectClass::~SVStringValueObjectClass()
{
	// Delete all object's data arrays...
	SetObjectDepth(0);
	CreateBuckets();
}

void SVStringValueObjectClass::Persist(SVObjectWriter& rWriter)
{
	rWriter.StartElement(GetObjectName()); // use internal name for node name

	// Get the ClassID, ObjectID, Attrributes
	SVValueObjectClass::Persist(rWriter);

	// Get the Data Values (Member Info, Values)
	// Check for DoubleQuotes in variable
	SVString tmp = DefaultValue();
	SvUl::AddEscapeSpecialCharacters(tmp, true);
	
	_variant_t value;
	value.SetString(tmp.c_str());
	rWriter.WriteAttribute(scDefaultTag, value);
	value.Clear();

	rWriter.StartElement(scArrayElementsTag);

	// Where does Object Depth Get put into the Script ??? (maybe at the SVObjectClass)
	// Object Depth is implicit (it's the count of the values)
	SVVariantList list;
	
	// for all elements in the array (objectDepth)
	for (int i = 0; i < m_iArraySize; i++)
	{
		tmp = Element(m_iLastSetIndex, i); 
		SvUl::AddEscapeSpecialCharacters(tmp, true);
		value.SetString(tmp.c_str());
		list.push_back(value);
		value.Clear();
	}
	rWriter.WriteAttribute(scElementTag, list);
	rWriter.EndElement();

	rWriter.EndElement();
}

HRESULT  SVStringValueObjectClass::SetObjectValue(SVObjectAttributeClass* pDataObject)
{
	HRESULT hr = S_FALSE;
	BOOL bOk = FALSE;
	
	SVString  csTemp;
	SvCl::SVObjectSVStringArrayClass svArray;
	bucket_type l_Buckets(BucketNoAssert());
	array_type l_Array;
	
	if ( bOk = pDataObject->GetAttributeData(SvOi::cDefaultTag, svArray) )
	{
		for (int i = 0; i < svArray.GetSize(); i++)
		{
			DefaultValue() = svArray[i];
			
			// Remove any escapes
			SvUl::RemoveEscapedSpecialCharacters(DefaultValue(), true);
		}
	}
	else if ( bOk = pDataObject->GetAttributeData(SvOi::cBucketTag, l_Buckets, DefaultValue() ) )
	{
		for (size_t i = 0; i < l_Buckets.size(); i++)
		{
			// Remove any escapes
			SvUl::RemoveEscapedSpecialCharacters(l_Buckets[i][0], true);
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
	}
	// new-style: store all array elements:
	else if ( bOk = pDataObject->GetArrayData(SvOi::cArrayTag, l_Array, DefaultValue() ) )
	{
		for (size_t i = 0; i < l_Array.size(); i++)
		{
			// Remove any escapes
			SvUl::RemoveEscapedSpecialCharacters(l_Array[i], true);
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
	}
	else if ((bOk = pDataObject->GetAttributeData(_T("StrDefault"), svArray)))
	{
		for (int i = 0; i < svArray.GetSize(); i++)
		{
			DefaultValue() = svArray[i];
			
			// Remove any escapes
			SvUl::RemoveEscapedSpecialCharacters(DefaultValue(), true);
		}
	}
	else if ( bOk = pDataObject->GetAttributeData("StrArray", l_Buckets, DefaultValue() ) )
	{
		for (size_t i = 0; i < l_Buckets.size(); i++)
		{
			// Remove any escapes
			SvUl::RemoveEscapedSpecialCharacters(l_Buckets[i][0], true);
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
	}
	else
	{
		hr = base::SetObjectValue(pDataObject);
		return hr;
	}

	hr = bOk ? S_OK : S_FALSE;
	return hr;
}

HRESULT SVStringValueObjectClass::SetValueAt( int iBucket, int iIndex, const VARIANT& rvtValue )
{
	if ( VT_BSTR == rvtValue.vt )
	{
		return base::SetValueAt(iBucket, iIndex, SvUl_SF::createSVString(rvtValue.bstrVal));
	}
	assert(false);
	return S_FALSE;
}

HRESULT SVStringValueObjectClass::GetValueAt( int iBucket, int iIndex, long& rValue) const
{
	SVString Value;

	HRESULT hr =  base::GetValueAt( iBucket, iIndex, Value );
	rValue = atoi( Value.c_str() );

	return hr;
}

HRESULT SVStringValueObjectClass::GetValueAt( int iBucket, int iIndex, double& rValue) const
{
	SVString Value;

	HRESULT hr = base::GetValueAt( iBucket, iIndex, Value );
	rValue = atof( Value.c_str() );

	return hr;
}

HRESULT SVStringValueObjectClass::GetValueAt( int iBucket, int iIndex, VARIANT& rValue ) const
{
	SVString Value;

	_variant_t l_Temp;
	l_Temp.Attach( rValue );
	HRESULT hr = base::GetValueAt( iBucket, iIndex, Value );
	if( S_OK == hr )
	{
		l_Temp = Value.c_str();
	}
	else
	{
		l_Temp.Clear();
	}
	rValue = l_Temp.Detach();

	return hr;
}

void SVStringValueObjectClass::LocalInitialize()
{
	m_outObjectInfo.ObjectTypeInfo.ObjectType = SVStringValueObjectType;
	
	ObjectAttributesAllowedRef() = SV_VIEWABLE | SV_PUBLISHABLE | SV_ARCHIVABLE | SV_EMBEDABLE | SV_PRINTABLE | SV_DD_VALUE;
	SetTypeName( _T("Text") );

	InitializeBuckets();
}
