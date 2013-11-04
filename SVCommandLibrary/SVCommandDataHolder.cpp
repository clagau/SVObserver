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

#include "stdafx.h"
#include "SVCommandDataHolder.h"
#include "SVCommandDataBlock.h"
#include "SVCommandDataContainer.h"
#include "SVCommandDataImage.h"
#include "SVCommandDataValue.h"
#include "SVCommandLibraryEnums.h"

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
		SVString l_Data;

		l_Data.Format( _T( "SVCommandDataHolder::clear() - Erase %s\n" ), l_Iter->first.ToString() );

		::OutputDebugString( l_Data.ToString() );

		l_Iter = m_Data.erase( l_Iter );
	}
}

SVCommandDataHolder::SVNameDataTypeMap SVCommandDataHolder::GetDataInformation() const
{
	SVNameDataTypeMap l_Info;

	SVNameDataMap::const_iterator l_Iter = m_Data.begin();

	while( l_Iter != m_Data.end() )
	{
		if( l_Iter->second.empty() )
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

	SVNameDataMap::const_iterator l_Iter = m_Data.find( p_rName );

	if( l_Iter != m_Data.end() )
	{
		if( ! l_Iter->second.empty() )
		{
			l_Type = l_Iter->second->GetDataType();
		}
	}

	return l_Type;
}

HRESULT SVCommandDataHolder::GetData( const _bstr_t& p_rName, SVCommandDataFacadePtr& p_rData ) const
{
	HRESULT l_Status = S_OK;

	p_rData.clear();

	SVNameDataMap::const_iterator l_Iter = m_Data.find( p_rName );

	if( l_Iter != m_Data.end() )
	{
		if( ! l_Iter->second.empty() )
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
	HRESULT l_Status = S_OK;

	SVCommandDataFacadePtr l_ContainerPtr;

	p_rData.clear();

	l_Status = GetData( p_rName, l_ContainerPtr );

	if( ! l_ContainerPtr.empty() )
	{
		SVCommandDataContainer::SVCommandDataInterface* l_pContainer = dynamic_cast< SVCommandDataContainer::SVCommandDataInterface* >( l_ContainerPtr.get() );

		if( l_pContainer != NULL )
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
		if( l_Status == S_OK )
		{
			l_Status = E_FAIL;
		}
	}

	return l_Status;
}

HRESULT SVCommandDataHolder::GetValue( const _bstr_t& p_rName, VARIANT& p_rData ) const
{
	HRESULT l_Status = S_OK;

	SVCommandDataFacadePtr l_ValuePtr;

	::VariantClear( &p_rData );

	l_Status = GetData( p_rName, l_ValuePtr );

	if( ! l_ValuePtr.empty() )
	{
		SVCommandDataValue::SVCommandDataInterface* l_pValue = dynamic_cast< SVCommandDataValue::SVCommandDataInterface* >( l_ValuePtr.get() );

		if( l_pValue != NULL )
		{
			_variant_t l_Variant;

			l_Status = l_pValue->GetData( l_Variant );

			if( l_Status == S_OK )
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
		if( l_Status == S_OK )
		{
			l_Status = E_FAIL;
		}
	}

	return l_Status;
}

HRESULT SVCommandDataHolder::GetBlock( const _bstr_t& p_rName, SVByteVector& p_rData ) const
{
	HRESULT l_Status = S_OK;

	SVCommandDataFacadePtr l_ValuePtr;

	p_rData.clear();

	l_Status = GetData( p_rName, l_ValuePtr );

	if( ! l_ValuePtr.empty() )
	{
		SVCommandDataBlock::SVCommandDataInterface* l_pValue = dynamic_cast< SVCommandDataBlock::SVCommandDataInterface* >( l_ValuePtr.get() );

		if( l_pValue != NULL )
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
		if( l_Status == S_OK )
		{
			l_Status = E_FAIL;
		}
	}

	return l_Status;
}

HRESULT SVCommandDataHolder::GetImage( const _bstr_t& p_rName, SVByteVector& p_rData ) const
{
	HRESULT l_Status = S_OK;

	p_rData.clear();

	SVCommandDataFacadePtr l_DataPtr;

	l_Status = GetData( p_rName, l_DataPtr );

	if( ! l_DataPtr.empty() )
	{
		SVCommandDataImage::SVCommandDataInterface* l_pData = dynamic_cast< SVCommandDataImage::SVCommandDataInterface* >( l_DataPtr.get() );

		if( l_pData != NULL )
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
		if( l_Status == S_OK )
		{
			l_Status = E_FAIL;
		}
	}

	return l_Status;
}

HRESULT SVCommandDataHolder::SetData( const _bstr_t& p_rName, SVCommandDataFacadePtr p_Data )
{
	HRESULT l_Status = S_OK;

	SVNameDataMap::iterator l_Iter = m_Data.find( p_rName );

	if( l_Iter != m_Data.end() )
	{
		l_Iter->second = p_Data;
	}
	else
	{
		m_Data[ p_rName ] = p_Data;
	}

	return l_Status;
}

HRESULT SVCommandDataHolder::SetContainer( const _bstr_t& p_rName, const SVCommandDataHolder& p_rData, bool p_ReadOnly )
{
	HRESULT l_Status = S_OK;

	SVCommandDataFacadePtr l_ContainerPtr = new SVCommandDataContainer( p_rData, p_ReadOnly );

	if( ! l_ContainerPtr.empty() )
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

	SVCommandDataFacadePtr l_DataPtr = new SVCommandDataValue( p_rData, p_ReadOnly );

	if( ! l_DataPtr.empty() )
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

	SVCommandDataFacadePtr l_DataPtr = new SVCommandDataBlock( p_rData, p_ReadOnly );

	if( ! l_DataPtr.empty() )
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

	SVCommandDataFacadePtr l_DataPtr = new SVCommandDataImage( p_rData, p_ReadOnly );

	if( ! l_DataPtr.empty() )
	{
		l_Status = SetData( p_rName, l_DataPtr );
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVCommandLibrary\SVCommandDataHolder.cpp_v  $
 * 
 *    Rev 1.0   18 Apr 2013 18:30:30   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   18 Feb 2011 09:33:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  700
 * SCR Title:  Remove String Buffer Problems
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source to remove duplicate string class, and fixed string conversion issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Oct 2010 14:32:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  701
 * SCR Title:  Add New Font Functionality to the Existing Command Interface
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Adding initial code for Command subsystem.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
