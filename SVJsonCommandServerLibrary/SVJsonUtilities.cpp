//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVJsonUtilities
//* .File Name       : $Workfile:   SVJsonUtilities.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   01 Aug 2013 11:45:48  $
//******************************************************************************

#include "stdafx.h"
//Moved to precompiled header: #include <fstream>
#include "SVJsonUtilities.h"

#include "SVUtilityLibrary/SVSAFEARRAY.h"
#include "SVUtilityLibrary/SVString.h"
#include "SVUtilityLibrary/SVStringConversions.h"

HRESULT SVJsonUtilities::ConvertJsonValueToVariant( const Json::Value& p_rJsonValue, VARIANT& p_rVariant )
{
	HRESULT l_Status = S_OK;

	_variant_t l_VariantWrapper;

	l_VariantWrapper.Attach( p_rVariant );

	switch( p_rJsonValue.type() )
	{
	case Json::intValue:
		{
			l_VariantWrapper = p_rJsonValue.asInt();

			break;
		}
	case Json::uintValue:
		{
			l_VariantWrapper = p_rJsonValue.asUInt();

			break;
		}
	case Json::realValue:
		{
			l_VariantWrapper = p_rJsonValue.asDouble();

			break;
		}
	case Json::stringValue:
		{
			l_VariantWrapper = SVStringConversions::to_utf16( p_rJsonValue.asString() ).c_str();

			break;
		}
	case Json::booleanValue:
		{
			l_VariantWrapper = p_rJsonValue.asBool();

			break;
		}
	case Json::arrayValue:
		{
			SVSAFEARRAY l_SafeArray;

			for( Json::Value::UInt i = 0; i < p_rJsonValue.size(); i++ )
			{
				_variant_t l_Value;

				HRESULT l_TempStatus = ConvertJsonValueToVariant( p_rJsonValue[ i ], l_Value );

				l_SafeArray.Add( l_Value );
			}

			l_VariantWrapper = l_SafeArray;

			break;
		}
	default:
		{
			l_VariantWrapper.Clear();

			l_Status = E_INVALIDARG;

			break;
		}
	}

	p_rVariant = l_VariantWrapper.Detach();

	return l_Status;
}

HRESULT SVJsonUtilities::ConvertVariantToJsonValue( const _variant_t& p_rVariant, Json::Value& p_rJsonValue )
{
	HRESULT l_Status = S_OK;

	if( ( p_rVariant.vt & VT_ARRAY ) == VT_ARRAY )
	{
		SVSAFEARRAY l_SafeArray( p_rVariant );
		SVSAFEARRAY::SVBounds l_Bounds;

		l_Status = l_SafeArray.GetBounds( l_Bounds );

		if( l_Bounds.size() == 1 )
		{
			Json::Value l_Array( Json::arrayValue );

			for( size_t i = 0; i < l_SafeArray.size(); i++ )
			{
				_variant_t l_Value;
				Json::Value l_JsonValue;
				//@mec  fix for SVO775
				SVSAFEARRAY::size_type l_Index = i + l_Bounds[ 0 ].lLbound;

				HRESULT l_TempStatus = l_SafeArray.GetElement( l_Index, l_Value );

				if( l_TempStatus == S_OK )
				{
					l_TempStatus = ConvertVariantToJsonValue( l_Value, l_JsonValue );
				}
				else if( l_Status == S_OK )
				{
					l_Status = l_TempStatus;
				}

				l_Array.append( l_JsonValue );
			}

			p_rJsonValue = l_Array;
		}
		else
		{
			p_rJsonValue.clear();

			l_Status = E_INVALIDARG;
		}
	}
	else
	{
		switch( p_rVariant.vt )
		{
		case VT_I1:
		case VT_I2:
		case VT_I4:
		case VT_INT:
			{
				p_rJsonValue = Json::Value( static_cast< int >( p_rVariant ) );

				break;
			}
		case VT_UI1:
		case VT_UI2:
		case VT_UI4:
		case VT_UINT:
		case VT_ERROR:
			{
				p_rJsonValue = Json::Value( static_cast< unsigned int >( p_rVariant ) );

				break;
			}
		case VT_I8:
		case VT_UI8:
			{
				// The current version of the Json library does not support 64 bit so for now
				// we are putting the result in a string.
				// In the future we may put the result in a its native type.
				SVString l_strTmp = SvUl_SF::Format( _T("0x%016I64x"), p_rVariant.llVal);
				p_rJsonValue = Json::Value( SVStringConversions::to_utf8(_bstr_t(l_strTmp.c_str()) ));
				break;
			}
		case VT_R4:
 		case VT_R8:
			{
				p_rJsonValue = Json::Value( static_cast< double >( p_rVariant ) );

				break;
			}
		case VT_BSTR:
			{
				p_rJsonValue = Json::Value( SVStringConversions::to_utf8( static_cast< _bstr_t >( p_rVariant ) ).c_str() );

				break;
			}
		case VT_BOOL:
			{
				p_rJsonValue = Json::Value( static_cast< bool >( p_rVariant ) );

				break;
			}
		default:
			{
				p_rJsonValue.clear();

				l_Status = E_INVALIDARG;

				break;
			}
		}
	}

	return l_Status;
}

HRESULT SVJsonUtilities::UpdateNameVariantPairToJsonObject( const std::string& p_rName, const _variant_t& p_rVariant, Json::Value& p_rJsonObject )
{
	HRESULT l_Status = S_OK;

	if( !( p_rName.empty() ) && p_rJsonObject.isObject() )
	{
		Json::Value l_JsonValue;

		l_Status = ConvertVariantToJsonValue( p_rVariant, l_JsonValue );

		p_rJsonObject[ p_rName ] = l_JsonValue;
	}
	else
	{
		l_Status = E_INVALIDARG;
	}

	return l_Status;
}

HRESULT SVJsonUtilities::GetTempFileNameUsingPrefixAndExt( std::string& p_rTempFileName, const std::string& p_rPrefix, const std::string& p_rExt )
{
	HRESULT l_Status = S_OK;

	p_rTempFileName.clear();

	SVString l_TempString;
	__int64 l_TimeStamp = 0;

	::QueryPerformanceCounter( reinterpret_cast< LARGE_INTEGER* >( &l_TimeStamp ) );

	l_TempString = SvUl_SF::Format( "%I64d", l_TimeStamp );

	p_rTempFileName += p_rPrefix;
	p_rTempFileName += "-";
	p_rTempFileName += l_TempString.c_str();
	p_rTempFileName += p_rExt;

	return l_Status;
}

HRESULT SVJsonUtilities::WriteJsonValueToFile( const Json::Value& p_rJsonValue, const std::string& p_rFileName )
{
	HRESULT l_Status = S_OK;

	std::ofstream os;
	os.open(p_rFileName.c_str());
	if (os.is_open())
	{
		Json::StyledStreamWriter writer;

		writer.write(os, p_rJsonValue);

		os.close();
	}
	else
	{
		l_Status = E_INVALIDARG;
	}
	

	return l_Status;
}

