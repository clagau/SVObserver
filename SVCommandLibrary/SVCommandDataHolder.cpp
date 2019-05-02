//******************************************************************************
//* COPYRIGHT (c) 2010 by Seidenader Vision Inc., Harrisburg
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

SVCommandDataHolder::SVCommandDataHolder( const SVNameDataMap& p_rData )
: m_Data( p_rData )
{
}

SVCommandDataHolder::SVCommandDataHolder( const SVCommandDataHolder& p_rObject )
: m_Data( p_rObject.m_Data )
{
}

SVCommandDataHolder::~SVCommandDataHolder()
{
	clear();
}

const SVCommandDataHolder& SVCommandDataHolder::operator=( const SVCommandDataHolder& p_rObject )
{
	if( this != &p_rObject )
	{
		m_Data = p_rObject.m_Data;
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
	SVNameDataTypeMap l_Info;

	SVNameDataMap::const_iterator l_Iter = m_Data.begin();

	while( l_Iter != m_Data.end() )
	{
		if( nullptr == l_Iter->second )
		{
			l_Info[ l_Iter->first ] = SV_COMMAND_DATA_TYPE_UNKNOWN;
		}
		else
		{ 
			l_Info[ l_Iter->first ] = l_Iter->second->GetDataType();
		}

		++l_Iter;
	}

	return l_Info;
}

unsigned long SVCommandDataHolder::GetDataType( const _bstr_t& p_rName ) const
{
	unsigned long l_Type = SV_COMMAND_DATA_TYPE_UNKNOWN;

	SVNameDataMap::const_iterator l_Iter = m_Data.find( SvUl::createStdString(p_rName) );

	if( l_Iter != m_Data.end() )
	{
		if( nullptr != l_Iter->second )
		{
			l_Type = l_Iter->second->GetDataType();
		}
	}

	return l_Type;
}

HRESULT SVCommandDataHolder::GetData( const _bstr_t& p_rName, SVCommandDataFacadePtr& p_rData ) const
{
	HRESULT l_Status = S_OK;

	p_rData.reset();

	SVNameDataMap::const_iterator l_Iter = m_Data.find( SvUl::createStdString(p_rName) );

	if( l_Iter != m_Data.end() )
	{
		if(nullptr != l_Iter->second)
		{
			p_rData = l_Iter->second;
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

HRESULT SVCommandDataHolder::GetContainer( const _bstr_t& p_rName, SVCommandDataHolder& p_rData ) const
{
	p_rData.clear();

	SVCommandDataFacadePtr l_ContainerPtr;
	HRESULT l_Status = GetData( p_rName, l_ContainerPtr );

	if( nullptr != l_ContainerPtr )
	{
		SVCommandDataContainer::SVCommandDataInterface* l_pContainer = dynamic_cast< SVCommandDataContainer::SVCommandDataInterface* >( l_ContainerPtr.get() );

		if( nullptr != l_pContainer )
		{
			l_Status = l_pContainer->GetData( p_rData );
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

HRESULT SVCommandDataHolder::GetValue( const _bstr_t& p_rName, VARIANT& p_rData ) const
{
	::VariantClear( &p_rData );

	SVCommandDataFacadePtr l_ValuePtr;
	HRESULT l_Status = GetData( p_rName, l_ValuePtr );

	if(nullptr != l_ValuePtr)
	{
		SVCommandDataValue::SVCommandDataInterface* l_pValue = dynamic_cast< SVCommandDataValue::SVCommandDataInterface* >( l_ValuePtr.get() );

		if( nullptr != l_pValue )
		{
			_variant_t l_Variant;

			l_Status = l_pValue->GetData( l_Variant );

			if( S_OK == l_Status )
			{
				p_rData = l_Variant.Detach();
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

HRESULT SVCommandDataHolder::GetBlock( const _bstr_t& p_rName, SVByteVector& p_rData ) const
{
	p_rData.clear();

	SVCommandDataFacadePtr l_ValuePtr;
	HRESULT l_Status = GetData( p_rName, l_ValuePtr );

	if(nullptr != l_ValuePtr)
	{
		SVCommandDataBlock::SVCommandDataInterface* l_pValue = dynamic_cast< SVCommandDataBlock::SVCommandDataInterface* >( l_ValuePtr.get() );

		if( nullptr != l_pValue )
		{
			l_Status = l_pValue->GetData( p_rData );
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

HRESULT SVCommandDataHolder::GetImage( const _bstr_t& p_rName, SVByteVector& p_rData ) const
{
	p_rData.clear();

	SVCommandDataFacadePtr l_DataPtr;
	HRESULT l_Status = GetData( p_rName, l_DataPtr );

	if(nullptr != l_DataPtr)
	{
		SVCommandDataImage::SVCommandDataInterface* l_pData = dynamic_cast< SVCommandDataImage::SVCommandDataInterface* >( l_DataPtr.get() );

		if( nullptr != l_pData )
		{
			l_Status = l_pData->GetData( p_rData );
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

HRESULT SVCommandDataHolder::SetData( _bstr_t nameBstr, SVCommandDataFacadePtr& rDataPointer )
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

HRESULT SVCommandDataHolder::SetContainer( const _bstr_t& p_rName, const SVCommandDataHolder& p_rData, bool p_ReadOnly )
{
	HRESULT l_Status = S_OK;

	SVCommandDataFacadePtr l_ContainerPtr{ new SVCommandDataContainer(p_rData, p_ReadOnly) };

	if(nullptr != l_ContainerPtr)
	{
		l_Status = SetData( p_rName, l_ContainerPtr );
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVCommandDataHolder::SetValue( const _bstr_t& p_rName, const _variant_t& p_rData, bool p_ReadOnly )
{
	HRESULT l_Status = S_OK;

	SVCommandDataFacadePtr l_DataPtr{ new SVCommandDataValue(p_rData, p_ReadOnly) };

	if(nullptr != l_DataPtr)
	{
		l_Status = SetData( p_rName, l_DataPtr );
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVCommandDataHolder::SetBlock( const _bstr_t& p_rName, const SVByteVector& p_rData, bool p_ReadOnly )
{
	HRESULT l_Status = S_OK;

	SVCommandDataFacadePtr l_DataPtr{ new SVCommandDataBlock(p_rData, p_ReadOnly) };

	if(nullptr != l_DataPtr)
	{
		l_Status = SetData( p_rName, l_DataPtr );
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVCommandDataHolder::SetImage( const _bstr_t& p_rName, const SVByteVector& p_rData, bool p_ReadOnly )
{
	HRESULT l_Status = S_OK;

	SVCommandDataFacadePtr l_DataPtr{ new SVCommandDataImage(p_rData, p_ReadOnly) };

	if(nullptr != l_DataPtr)
	{
		l_Status = SetData( p_rName, l_DataPtr );
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

