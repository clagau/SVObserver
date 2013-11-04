//******************************************************************************
//* COPYRIGHT (c) 2010 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVRemoteCommandData
//* .File Name       : $Workfile:   SVRemoteCommandData.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 14:43:20  $
//******************************************************************************

#include "stdafx.h"
#include <Objidl.h>
#include "SVObserver.h"
#include "SVObserver_i.h"
#include "SVRemoteCommandData.h"

#include "SVCommandLibrary/SVCommandDataBlock.h"
#include "SVCommandLibrary/SVCommandDataContainer.h"
#include "SVCommandLibrary/SVCommandDataImage.h"
#include "SVCommandLibrary/SVCommandDataValue.h"
#include "SVUtilityLibrary/SVSAFEARRAY.h"

SVRemoteCommandData::SVRemoteCommandData()
: m_Data()
{
}

SVRemoteCommandData::~SVRemoteCommandData()
{
	m_Data.clear();
}

void SVRemoteCommandData::SetData( const SVCommandDataHolder& p_rData )
{
	m_Data = p_rData;
}

void SVRemoteCommandData::FinalRelease()
{
	m_Data.clear();
}

STDMETHODIMP SVRemoteCommandData::GetDataInformation( VARIANT* p_pInformation )
{
	HRESULT l_Status = S_OK;

	if( p_pInformation != NULL )
	{
		l_Status = ::VariantClear( p_pInformation );

		if( l_Status == S_OK )
		{
			SVCommandDataHolder::SVNameDataTypeMap l_Info = m_Data.GetDataInformation();

			if( 0 < l_Info.size() )
			{
				SVSAFEARRAY::SVBounds l_Bounds;

				l_Bounds.resize( 2 );

				// First Dimension number of objects in list..
				l_Bounds[ 0 ].lLbound = 0;
				l_Bounds[ 0 ].cElements = l_Info.size();

				// Second Dimension is the parts fo the Information Definition
				// 0 = Name, 1 = Data Type ( see enum SVCommandDataType for values )
				l_Bounds[ 1 ].lLbound = 0;
				l_Bounds[ 1 ].cElements = 2;

				size_t i = 0;
				SVSAFEARRAY l_Temp( VT_VARIANT, l_Bounds );
				SVSAFEARRAY::SVIndex l_Index( 2 );
				SVCommandDataHolder::SVNameDataTypeMap::const_iterator l_Iter = l_Info.begin();

				while( l_Status == S_OK && i < l_Info.size() && l_Iter != l_Info.end() )
				{
					l_Index[ 0 ] = i;
					l_Index[ 1 ] = 0;

					_variant_t l_Data = l_Iter->first.ToVARIANT();
					HRESULT l_State = l_Temp.PutElement( l_Index, l_Data );
					l_Data.Clear();

					if( l_Status == S_OK )
					{
						l_Status = l_State;
					}

					l_Index[ 0 ] = i;
					l_Index[ 1 ] = 1;

					l_Data = SVRemoteCommandDataTypes::Unknown;
						
					// see enum SVCommandDataType for values
					switch( l_Iter->second & SVCommandDataType::SV_COMMAND_DATA_TYPE_ITEM_MASK )
					{
					case SVCommandDataType::SV_COMMAND_DATA_TYPE_CONTAINER:
						{
							l_Data = SVRemoteCommandDataTypes::Container;

							break;
						}
					case SVCommandDataType::SV_COMMAND_DATA_TYPE_VALUE:
						{
							l_Data = SVRemoteCommandDataTypes::Value;

							break;
						}
					case SVCommandDataType::SV_COMMAND_DATA_TYPE_BLOCK:
						{
							l_Data = SVRemoteCommandDataTypes::Block;

							break;
						}
					case SVCommandDataType::SV_COMMAND_DATA_TYPE_IMAGE:
						{
							l_Data = SVRemoteCommandDataTypes::Image;

							break;
						}
					default:
						{
							l_Data = SVRemoteCommandDataTypes::Unknown;

							break;
						}
					}

					l_State = l_Temp.PutElement( l_Index, l_Data );

					if( l_Status == S_OK )
					{
						l_Status = l_State;
					}

					++i;
					++l_Iter;
				}

				if( l_Status == S_OK )
				{
					_variant_t l_Variant = l_Temp;

					*p_pInformation = l_Variant.Detach();
				}
			}
			else
			{
				l_Status = S_FALSE;
			}
		}
	}
	else
	{
		l_Status = E_INVALIDARG;
	}

	return l_Status;
}

STDMETHODIMP SVRemoteCommandData::GetDataType( BSTR p_Name, long* p_pDataType )
{
	HRESULT l_Status = S_OK;

	if( p_pDataType != NULL )
	{
		*p_pDataType = m_Data.GetDataType( p_Name );
	}
	else
	{
		l_Status = E_INVALIDARG;
	}

	return l_Status;
}

STDMETHODIMP SVRemoteCommandData::ConstructCommandData( ISVRemoteCommandData **p_ppCommandData )
{
	HRESULT l_Status = S_OK;

	if( p_ppCommandData != NULL )
	{
		if( *p_ppCommandData != NULL )
		{
			( *p_ppCommandData )->Release();

			*p_ppCommandData = NULL;
		}

		CComPtr< ISVRemoteCommandData > l_RemoteCommandDataPtr;

		l_Status = l_RemoteCommandDataPtr.CoCreateInstance( __uuidof( SVRemoteCommandData ) );

		if( l_RemoteCommandDataPtr.p == NULL )
		{
			if( l_Status == S_OK )
			{
				l_Status = E_FAIL;
			}
		}

		if( l_Status == S_OK )
		{
			l_Status = l_RemoteCommandDataPtr.QueryInterface( p_ppCommandData );
		}
	}
	else
	{
		l_Status = E_INVALIDARG;
	}

	return l_Status;
}

STDMETHODIMP SVRemoteCommandData::GetContainer( BSTR p_Name, ISVRemoteCommandData** p_ppContainer )
{
	HRESULT l_Status = S_OK;

	if( p_ppContainer != NULL )
	{
		if( ( *p_ppContainer ) != NULL )
		{
			( *p_ppContainer )->Release();

			( *p_ppContainer ) = NULL;
		}

		SVCommandDataHolder l_Data;

		l_Status = m_Data.GetContainer( p_Name, l_Data );

		if( l_Status == S_OK )
		{
			CComPtr< ISVRemoteCommandData > l_CommandDataPtr;

			l_Status = l_CommandDataPtr.CoCreateInstance( __uuidof( SVRemoteCommandData ) );

			if( l_CommandDataPtr.p == NULL )
			{
				if( l_Status == S_OK )
				{
					l_Status = E_FAIL;
				}
			}

			if( l_Status == S_OK )
			{
				// This has issues when using _ATL_DEBUG_INTERFACES...
				SVRemoteCommandData* l_pCommandData = dynamic_cast< SVRemoteCommandData* >( l_CommandDataPtr.p );

				if( l_pCommandData != NULL )
				{
					l_pCommandData->m_Data = l_Data;
				}
				else
				{
					l_Status = E_FAIL;
				}
			}

			if( l_Status == S_OK )
			{
				l_Status = l_CommandDataPtr.QueryInterface( p_ppContainer );
			}
		}
		else
		{
			if( l_Status == S_OK )
			{
				l_Status = E_FAIL;
			}
		}
	}
	else
	{
		l_Status = E_INVALIDARG;
	}

	return l_Status;
}

STDMETHODIMP SVRemoteCommandData::SetContainer( BSTR p_Name, ISVRemoteCommandData* p_pContainer )
{
	HRESULT l_Status = S_OK;

	if( p_pContainer == NULL )
	{
		l_Status = E_INVALIDARG;
	}

	if( l_Status == S_OK )
	{
		// This has issues when using _ATL_DEBUG_INTERFACES...
		SVRemoteCommandData* l_pCommandData = dynamic_cast< SVRemoteCommandData* >( p_pContainer );

		if( l_pCommandData != NULL )
		{
			l_Status = m_Data.SetContainer( p_Name, l_pCommandData->m_Data );
		}
		else
		{
			l_Status = E_FAIL;
		}
	}

	return l_Status;
}

STDMETHODIMP SVRemoteCommandData::GetValue( BSTR p_Name, VARIANT* p_pValue )
{
	HRESULT l_Status = S_OK;

	if( p_pValue != NULL )
	{
		::VariantClear( p_pValue );

		l_Status = m_Data.GetValue( p_Name, *p_pValue );
	}
	else
	{
		l_Status = E_INVALIDARG;
	}

	return l_Status;
}

STDMETHODIMP SVRemoteCommandData::SetValue( BSTR p_Name, VARIANT p_Value )
{
	HRESULT l_Status = S_OK;

	if( p_Value.vt != VT_EMPTY && p_Value.vt != VT_DISPATCH )
	{
		l_Status = m_Data.SetValue( p_Name, p_Value );
	}
	else
	{
		l_Status = E_INVALIDARG;
	}
	
	return l_Status;
}

STDMETHODIMP SVRemoteCommandData::GetBlock( BSTR p_Name, IStream** p_ppStream )
{
	HRESULT l_Status = S_OK;

	if( p_ppStream != NULL )
	{
		if( ( *p_ppStream ) != NULL )
		{
			( *p_ppStream )->Release();

			( *p_ppStream ) = NULL;
		}

		SVByteVector l_Block;

		l_Status = m_Data.GetBlock( p_Name, l_Block );

		if( 0 < l_Block.size() )
		{
			CComQIPtr< IStream > l_StreamPtr;

			l_Status = ::CreateStreamOnHGlobal( NULL, true, &l_StreamPtr );

			if( l_StreamPtr.p == NULL )
			{
				if( l_Status == S_OK )
				{
					l_Status = E_FAIL;
				}
			}

			if( l_Status == S_OK )
			{
				ULARGE_INTEGER l_Size;

				l_Size.QuadPart = l_Block.size();

				l_Status = l_StreamPtr->SetSize( l_Size );

				if( l_Status == S_OK )
				{
					LARGE_INTEGER l_Position;

					l_Position.QuadPart = 0;

					l_Status = l_StreamPtr->Seek( l_Position, STREAM_SEEK_SET, NULL );
				}

				if( l_Status == S_OK )
				{
					ULONG l_BytesWritten = 0;

					l_Status = l_StreamPtr->Write( &( l_Block[ 0 ] ), l_Block.size(), &l_BytesWritten );

					if( l_BytesWritten != l_Block.size() )
					{
						if( l_Status == S_OK )
						{
							l_Status = E_FAIL;
						}
					}
				}
			}

			if( l_Status == S_OK )
			{
				l_Status = l_StreamPtr.QueryInterface( p_ppStream );
			}
		}
		else
		{
			if( l_Status == S_OK )
			{
				l_Status = E_FAIL;
			}
		}
	}
	else
	{
		l_Status = E_INVALIDARG;
	}

	return l_Status;
}

STDMETHODIMP SVRemoteCommandData::SetBlock( BSTR p_Name, IStream* p_pStream )
{
	HRESULT l_Status = S_OK;

	if( p_pStream == NULL )
	{
		l_Status = E_INVALIDARG;
	}

	if( l_Status == S_OK )
	{
		STATSTG l_Stat;
		LARGE_INTEGER l_Position;
		CComPtr< IStream > l_StreamPtr = p_pStream;

		l_Position.QuadPart = 0;

		l_Status = l_StreamPtr->Seek( l_Position, STREAM_SEEK_SET, NULL );

		if( l_Status == S_OK )
		{
			l_Status = l_StreamPtr->Stat( &l_Stat, STATFLAG_NONAME );
		}

		if( l_Status == S_OK )
		{
			size_t l_Size = static_cast< size_t >( l_Stat.cbSize.QuadPart );
			SVByteVector l_Block( l_Size );

			if( l_Block.size() == l_Size )
			{
				ULONG l_BytesRead = 0;

				l_Status = l_StreamPtr->Read( &( l_Block[ 0 ] ), l_Block.size(), &l_BytesRead );

				if( l_Block.size() == static_cast< size_t >( l_BytesRead ) )
				{
					l_Status = m_Data.SetBlock( p_Name, l_Block );
				}
				else
				{
					if( l_Status == S_OK )
					{
						l_Status = E_FAIL;
					}
				}
			}
			else
			{
				l_Status = E_FAIL;
			}
		}
	}

	return l_Status;
}

STDMETHODIMP SVRemoteCommandData::GetImage( BSTR p_Name, IStream** p_ppStream )
{
	HRESULT l_Status = S_OK;

	if( p_ppStream != NULL )
	{
		if( ( *p_ppStream ) != NULL )
		{
			( *p_ppStream )->Release();

			( *p_ppStream ) = NULL;
		}

		SVByteVector l_Block;

		l_Status = m_Data.GetImage( p_Name, l_Block );

		if( 0 < l_Block.size() )
		{
			CComQIPtr< IStream > l_StreamPtr;

			l_Status = ::CreateStreamOnHGlobal( NULL, true, &l_StreamPtr );

			if( l_StreamPtr.p == NULL )
			{
				if( l_Status == S_OK )
				{
					l_Status = E_FAIL;
				}
			}

			if( l_Status == S_OK )
			{
				ULARGE_INTEGER l_Size;

				l_Size.QuadPart = l_Block.size();

				l_Status = l_StreamPtr->SetSize( l_Size );

				if( l_Status == S_OK )
				{
					LARGE_INTEGER l_Position;

					l_Position.QuadPart = 0;

					l_Status = l_StreamPtr->Seek( l_Position, STREAM_SEEK_SET, NULL );
				}

				if( l_Status == S_OK )
				{
					ULONG l_BytesWritten = 0;

					l_Status = l_StreamPtr->Write( &( l_Block[ 0 ] ), l_Block.size(), &l_BytesWritten );

					if( l_BytesWritten != l_Block.size() )
					{
						if( l_Status == S_OK )
						{
							l_Status = E_FAIL;
						}
					}
				}
			}

			if( l_Status == S_OK )
			{
				l_Status = l_StreamPtr.QueryInterface( p_ppStream );
			}
		}
		else
		{
			if( l_Status == S_OK )
			{
				l_Status = E_FAIL;
			}
		}
	}
	else
	{
		l_Status = E_INVALIDARG;
	}

	return l_Status;
}

STDMETHODIMP SVRemoteCommandData::SetImage( BSTR p_Name, IStream* p_pStream )
{
	HRESULT l_Status = S_OK;

	if( p_pStream == NULL )
	{
		l_Status = E_INVALIDARG;
	}

	if( l_Status == S_OK )
	{
		STATSTG l_Stat;
		LARGE_INTEGER l_Position;
		CComPtr< IStream > l_StreamPtr = p_pStream;

		l_Position.QuadPart = 0;

		l_Status = l_StreamPtr->Seek( l_Position, STREAM_SEEK_SET, NULL );

		if( l_Status == S_OK )
		{
			l_Status = l_StreamPtr->Stat( &l_Stat, STATFLAG_NONAME );
		}

		if( l_Status == S_OK )
		{
			size_t l_Size = static_cast< size_t >( l_Stat.cbSize.QuadPart );
			SVByteVector l_Block( l_Size );

			if( l_Block.size() == l_Size )
			{
				ULONG l_BytesRead = 0;

				l_Status = l_StreamPtr->Read( &( l_Block[ 0 ] ), l_Block.size(), &l_BytesRead );

				if( l_Block.size() == static_cast< size_t >( l_BytesRead ) )
				{
					l_Status = m_Data.SetImage( p_Name, l_Block );
				}
				else
				{
					if( l_Status == S_OK )
					{
						l_Status = E_FAIL;
					}
				}
			}
			else
			{
				l_Status = E_FAIL;
			}
		}
	}

	return l_Status;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVRemoteCommandData.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 14:43:20   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   16 Jan 2012 15:36:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  747
 * SCR Title:  Add New Remote Input Tool to SVObserver
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Added new remote functionality to the application.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   13 Dec 2010 11:31:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Change the usage of _variant_t to clear the variable after use to eliminate invalid variant type for assignment.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Oct 2010 15:29:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  701
 * SCR Title:  Add New Font Functionality to the Existing Command Interface
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to add new methods and object to the SVObserver command interface.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/