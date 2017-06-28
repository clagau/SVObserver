//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDoubleValueObjectClass
//* .File Name       : $Workfile:   SVDoubleValueObjectClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   01 Oct 2013 14:12:22  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <assert.h>
#include "SVDoubleValueObjectClass.h"
#include "SVObjectLibrary\SVClsids.h"
#include "SVObjectLibrary\SVToolsetScriptTags.h"
#include "SVStatusLibrary/MessageManager.h"
#include "ObjectInterfaces/TextDefineSvOi.h"
#include "SVContainerLibrary/SVObjectArrayClassTemplate.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS(SVDoubleValueObjectClass, SVDoubleValueObjectClassGuid);

SVDoubleValueObjectClass::SVDoubleValueObjectClass( LPCTSTR ObjectName )
: SVValueObjectClass<double>( ObjectName )
{
	LocalInitialize();
}

SVDoubleValueObjectClass::SVDoubleValueObjectClass( SVObjectClass* pOwner, int StringResourceID )
: SVValueObjectClass<double>( pOwner, StringResourceID )
{
	LocalInitialize();
}

SVDoubleValueObjectClass::SVDoubleValueObjectClass( const SVDoubleValueObjectClass& rhs )
: SVValueObjectClass<double>()
{
	LocalInitialize();
	*this = rhs;
}

const SVDoubleValueObjectClass& SVDoubleValueObjectClass::operator = (const SVDoubleValueObjectClass& rhs)
{
	__super::operator = (rhs);
	return *this;
}

SVDoubleValueObjectClass::~SVDoubleValueObjectClass()
{
}

// This override provides the ability to correctly load script data from the legacy SVDoubleVectorObjectClass
HRESULT SVDoubleValueObjectClass::SetObjectValue( SVObjectAttributeClass* pDataObject )
{
	HRESULT Result( E_FAIL );
	
	SvCl::SVObjectArrayClassTemplate<ValueType> svArray;	// for default values

	if ( isLegacyVectorObjectCompatibility() )
	{
		if ( pDataObject->GetAttributeData( _T("bDefault"), svArray) )
		{
			if ( svArray.GetSize() > 0 )
			{
				DefaultValue() = svArray[ svArray.GetSize()-1 ];
			}
			Result = S_OK;
			return Result;
		}
		else if ( pDataObject->GetAttributeData( _T("pArray"), svArray )
		          || pDataObject->GetAttributeData( SvOi::cBucketTag, svArray ) )
		{
			SetArraySize( static_cast<int>(svArray.size()) );
			Result = SetArrayValues( svArray.begin(), svArray.end(), 1);
			return Result;
		}
	}

	Result = __super::SetObjectValue( pDataObject );
	return Result;
}

double SVDoubleValueObjectClass::ConvertString2Type( const SVString& rValue ) const
{
	SVString Digits = SvUl_SF::ValidateString( rValue, _T("-0123456789. ") );
	if ( Digits != rValue )
	{
		SVStringVector msgList;
		msgList.push_back(GetName());
		SvStl::MessageMgrStd Exception( SvStl::LogOnly );
		Exception.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_ValueObject_ValidateStringFailed, msgList, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
		Exception.Throw();
	}

	return atof( rValue.c_str() );
}

void SVDoubleValueObjectClass::WriteValues(SVObjectWriter &rWriter)
{
	// Where does Object Depth Get put into the Script ??? (maybe at the SVObjectClass)
	// Object Depth is implicit (it's the count of the values)
	SVVariantList list;

	// for all elements in the array
	for (int i = 0; i < getArraySize(); i++)
	{
		//Make sure this is not a derived virtual method which is called
		_variant_t Value;
		Value.ChangeType(VT_R8);
		SVDoubleValueObjectClass::GetValue(Value.dblVal, GetLastSetIndex(), i);
		list.push_back(Value);
	}
	rWriter.WriteAttribute(scElementTag, list);
}

void SVDoubleValueObjectClass::LocalInitialize()
{
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SVDoubleValueObjectType;
	DefaultValue() = 0.0;
	SetTypeName( _T("Decimal") );
	setOutputFormat( _T("%lf") );
	InitializeBuckets();
}