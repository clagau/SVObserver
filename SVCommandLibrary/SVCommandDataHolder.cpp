//******************************************************************************
//* COPYRIGHT (c) 2010 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCommandDataHolder
//* .File Name       : $Workfile:   SVCommandDataHolder.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   18 Apr 2013 18:30:30  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVCommandDataHolder.h"
#include "SVCommandDataBlock.h"
#include "SVCommandDataContainer.h"
#include "SVCommandDataImage.h"
#include "SVCommandDataValue.h"
#include "SVCommandLibraryEnums.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

SVCommandDataHolder::SVCommandDataHolder()
: m_Data()
{
}

SVCommandDataHolder::SVCommandDataHolder( const SVNameDataMap& rData )
: m_Data( rData )
{
}

SVCommandDataHolder::SVCommandDataHolder( const SVCommandDataHolder& rObject )
: m_Data( rObject.m_Data )
{
}

SVCommandDataHolder::~SVCommandDataHolder()
{
	clear();
}

const SVCommandDataHolder& SVCommandDataHolder::operator=( const SVCommandDataHolder& rObject )
{
	if( this != &rObject )
	{
		m_Data = rObject.m_Data;
	}

	return *this;
}

void SVCommandDataHolder::clear()
{
	SVNameDataMap::iterator l_Iter = m_Data.begin();

	while( l_Iter != m_Data.end() )
	{
#if defined (TRACE_THEM_ALL) || defined (TRACE_OTHER)
		std::string l_Data = SvUl::Format(_T("SVCommandDataHolder::clear() - Erase %s\n"), l_Iter->first.c_str());
		::OutputDebugString( l_Data.c_str() );
#endif

		l_Iter = m_Data.erase( l_Iter );
	}
}

SVCommandDataHolder::SVNameDataTypeMap SVCommandDataHolder::GetDataInformation() const
{
	SVNameDataTypeMap info;

	SVNameDataMap::const_iterator l_Iter = m_Data.begin();

	while( l_Iter != m_Data.end() )
	{
		if( nullptr == l_Iter->second )
		{
			info[ l_Iter->first ] = SV_COMMAND_DATA_TYPE_UNKNOWN;
		}
		else
		{ 
			info[ l_Iter->first ] = l_Iter->second->GetDataType();
		}

		++l_Iter;
	}

	return info;
}

unsigned long SVCommandDataHolder::GetDataType( const _bstr_t& rName ) const
{
	unsigned long l_Type = SV_COMMAND_DATA_TYPE_UNKNOWN;

	SVNameDataMap::const_iterator l_Iter = m_Data.find( SvUl::createStdString(rName) );

	if( l_Iter != m_Data.end() )
	{
		if( nullptr != l_Iter->second )
		{
			l_Type = l_Iter->second->GetDataType();
		}
	}

	return l_Type;
}

HRESULT SVCommandDataHolder::GetData( const _bstr_t& rName, SVCommandDataFacadePtr& rData ) const
{
	HRESULT l_Status = S_OK;

	rData.reset();

	SVNameDataMap::const_iterator l_Iter = m_Data.find( SvUl::createStdString(rName) );

	if( l_Iter != m_Data.end() )
	{
		if(nullptr != l_Iter->second)
		{
			rData = l_Iter->second;
		}
		else
		{
			l_Status = E_FAIL;
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVCommandDataHolder::GetContainer( const _bstr_t& rName, SVCommandDataHolder& rData ) const
{
	rData.clear();

	SVCommandDataFacadePtr containerPtr;
	HRESULT l_Status = GetData( rName, containerPtr );

	if( nullptr != containerPtr )
	{
		SVCommandDataContainer::SVCommandDataInterface* l_pContainer = dynamic_cast< SVCommandDataContainer::SVCommandDataInterface* >( containerPtr.get() );

		if( nullptr != l_pContainer )
		{
			l_Status = l_pContainer->GetData( rData );
		}
		else
		{
			l_Status = E_FAIL;
		}
	}
	else
	{
		if( S_OK == l_Status )
		{
			l_Status = E_FAIL;
		}
	}

	return l_Status;
}

HRESULT SVCommandDataHolder::GetValue( const _bstr_t& rName, VARIANT& rData ) const
{
	::VariantClear( &rData );

	SVCommandDataFacadePtr l_ValuePtr;
	HRESULT l_Status = GetData( rName, l_ValuePtr );

	if(nullptr != l_ValuePtr)
	{
		SVCommandDataValue::SVCommandDataInterface* l_pValue = dynamic_cast< SVCommandDataValue::SVCommandDataInterface* >( l_ValuePtr.get() );

		if( nullptr != l_pValue )
		{
			_variant_t l_Variant;

			l_Status = l_pValue->GetData( l_Variant );

			if( S_OK == l_Status )
			{
				rData = l_Variant.Detach();
			}
		}
		else
		{
			l_Status = E_FAIL;
		}
	}
	else
	{
		if( S_OK == l_Status )
		{
			l_Status = E_FAIL;
		}
	}

	return l_Status;
}

HRESULT SVCommandDataHolder::GetBlock( const _bstr_t& rName, std::vector<unsigned char>& rData ) const
{
	rData.clear();

	SVCommandDataFacadePtr l_ValuePtr;
	HRESULT l_Status = GetData( rName, l_ValuePtr );

	if(nullptr != l_ValuePtr)
	{
		SVCommandDataBlock::SVCommandDataInterface* l_pValue = dynamic_cast< SVCommandDataBlock::SVCommandDataInterface* >( l_ValuePtr.get() );

		if( nullptr != l_pValue )
		{
			l_Status = l_pValue->GetData( rData );
		}
		else
		{
			l_Status = E_FAIL;
		}
	}
	else
	{
		if( S_OK == l_Status )
		{
			l_Status = E_FAIL;
		}
	}

	return l_Status;
}

HRESULT SVCommandDataHolder::GetImage( const _bstr_t& rName, std::vector<unsigned char>& rData ) const
{
	rData.clear();

	SVCommandDataFacadePtr l_DataPtr;
	HRESULT l_Status = GetData( rName, l_DataPtr );

	if(nullptr != l_DataPtr)
	{
		SVCommandDataImage::SVCommandDataInterface* l_pData = dynamic_cast< SVCommandDataImage::SVCommandDataInterface* >( l_DataPtr.get() );

		if( nullptr != l_pData )
		{
			l_Status = l_pData->GetData( rData );
		}
		else
		{
			l_Status = E_FAIL;
		}
	}
	else
	{
		if( S_OK == l_Status )
		{
			l_Status = E_FAIL;
		}
	}

	return l_Status;
}

HRESULT SVCommandDataHolder::SetData( _bstr_t nameBstr, const SVCommandDataFacadePtr& rDataPointer )
{
	HRESULT l_Status = S_OK;
	std::string name = SvUl::createStdString(nameBstr);

	SVNameDataMap::iterator l_Iter = m_Data.find( name );

	if( l_Iter != m_Data.end() )
	{
		l_Iter->second = rDataPointer;
	}
	else
	{
		m_Data[ name ] = rDataPointer;
	}

	return l_Status;
}

HRESULT SVCommandDataHolder::SetContainer( const _bstr_t& rName, const SVCommandDataHolder& rData, bool readOnly )
{
	HRESULT l_Status = S_OK;

	SVCommandDataFacadePtr l_ContainerPtr{ new SVCommandDataContainer(rData, readOnly) };

	if(nullptr != l_ContainerPtr)
	{
		l_Status = SetData( rName, l_ContainerPtr );
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVCommandDataHolder::SetValue( const _bstr_t& rName, const _variant_t& rData, bool readOnly )
{
	HRESULT l_Status = S_OK;

	SVCommandDataFacadePtr l_DataPtr{ new SVCommandDataValue(rData, readOnly) };

	if(nullptr != l_DataPtr)
	{
		l_Status = SetData( rName, l_DataPtr );
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVCommandDataHolder::SetBlock( const _bstr_t& rName, const std::vector<unsigned char>& rData, bool readOnly )
{
	HRESULT l_Status = S_OK;

	SVCommandDataFacadePtr l_DataPtr{ new SVCommandDataBlock(rData, readOnly) };

	if(nullptr != l_DataPtr)
	{
		l_Status = SetData( rName, l_DataPtr );
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVCommandDataHolder::SetImage( const _bstr_t& rName, const std::vector<unsigned char>& rData, bool readOnly )
{
	HRESULT l_Status = S_OK;

	SVCommandDataFacadePtr l_DataPtr{ new SVCommandDataImage(rData, readOnly) };

	if(nullptr != l_DataPtr)
	{
		l_Status = SetData( rName, l_DataPtr );
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

