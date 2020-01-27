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
#include "Definitions/TextDefineSVDef.h"
#include "Definitions/StringTypeDef.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

namespace SvVol
{

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
	
	std::vector<ValueType> svArray;	// for default values

	if ( isLegacyVectorObjectCompatibility() )
	{
		if ( pDataObject->GetAttributeData( _T("bDefault"), svArray) )
		{
			if (0 < svArray.size())
			{
				DefaultValue() = svArray[ svArray.size()-1 ];
			}
			Result = S_OK;
			return Result;
		}
		else if ( pDataObject->GetAttributeData( _T("pArray"), svArray )
		          || pDataObject->GetAttributeData( SvDef::cBucketTag, svArray ) )
		{
			SetArraySize(static_cast<int32_t> (svArray.size()));
			Result = SetArrayValues(svArray);
			return Result;
		}
	}

	Result = __super::SetObjectValue( pDataObject );
	return Result;
}

HRESULT SVDoubleValueObjectClass::SetOutputFormat(OutputFormat outputFormat)
{
	HRESULT Result = S_OK;
	switch (outputFormat)
	{
		//This displays a double as an integer
		case OutputFormat_int:
			setOutputFormat(_T("%.0lf"));
			break;
		default:
			Result = E_INVALIDARG;
			break;
	}
	return Result;
}

double SVDoubleValueObjectClass::ConvertString2Type( const std::string& rValue ) const
{
	std::string Digits = SvUl::ValidateString( rValue, _T("-0123456789. ") );
	if ( Digits != rValue )
	{
		SvDef::StringVector msgList;
		msgList.push_back(GetName());
		SvStl::MessageMgrStd Exception(SvStl::MsgType::Log );
		Exception.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_ValueObject_ValidateStringFailed, msgList, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
		Exception.Throw();
	}

	return atof( rValue.c_str() );
}

void SVDoubleValueObjectClass::WriteValues(SvOi::IObjectWriter& rWriter)
{
	// Where does Object Depth Get put into the Script ??? (maybe at the SVObjectClass)
	// Object Depth is implicit (it's the count of the values)
	std::vector<_variant_t> list;

	// for all elements in the array
	for (int32_t i = 0; i < getArraySize(); i++)
	{
		//Make sure this is not a derived virtual method which is called
		_variant_t Value;
		Value.ChangeType(VT_R8);
		SVDoubleValueObjectClass::GetValue(Value.dblVal, i);
		list.push_back(Value);
	}
	rWriter.WriteAttribute(scElementTag, list);
}

void SVDoubleValueObjectClass::WriteDefaultValues(SvOi::IObjectWriter& rWriter)
{
	_variant_t Value(GetDefaultValue());
	rWriter.WriteAttribute(scDefaultTag, Value);
}

void SVDoubleValueObjectClass::LocalInitialize()
{
	m_outObjectInfo.m_ObjectTypeInfo.SubType = SvPb::SVDoubleValueObjectType;
	DefaultValue() = 0.0;
	SetTypeName( _T("Decimal") );
	setOutputFormat( _T("%lf") );
	init();
}

} //namespace SvVol
