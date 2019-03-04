//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSAFEARRAY
//* .File Name       : $Workfile:   SVSAFEARRAY.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   12 Jun 2013 15:16:12  $
//******************************************************************************

#include "stdafx.h"
#include "SVSafeArray.h"

namespace SvUl
{
SVSAFEARRAY::SVSAFEARRAY()
: m_pSafeArray( nullptr )
{
}

SVSAFEARRAY::SVSAFEARRAY( VARTYPE p_Type, size_type p_Size )
: m_pSafeArray( nullptr )
{
	if( 0 < p_Size )
	{
		SAFEARRAYBOUND l_Bound;

		l_Bound.lLbound = 0;
		l_Bound.cElements = static_cast< ULONG >( p_Size );

		m_pSafeArray = ::SafeArrayCreate( p_Type, 1, &l_Bound );
	}
}

SVSAFEARRAY::SVSAFEARRAY( VARTYPE p_Type, const SVBounds& p_rBounds )
: m_pSafeArray( nullptr )
{
	unsigned int l_Dim( static_cast< unsigned int >( p_rBounds.size() ) );

	if( 0 < l_Dim )
	{
		SAFEARRAYBOUND* l_pBounds( const_cast< SAFEARRAYBOUND* >( &( p_rBounds[ 0 ] ) ) );

		m_pSafeArray = ::SafeArrayCreate( p_Type, l_Dim, l_pBounds );
	}
}

SVSAFEARRAY::SVSAFEARRAY( const SVSAFEARRAY& p_rObject )
: m_pSafeArray( nullptr )
{
	*this = p_rObject;
}

SVSAFEARRAY::SVSAFEARRAY( const VARIANT& p_rVariant )
: m_pSafeArray( nullptr )
{
	*this = p_rVariant;
}

SVSAFEARRAY::SVSAFEARRAY( SAFEARRAY* p_pObject )
: m_pSafeArray( nullptr )
{
	*this = p_pObject;
}

SVSAFEARRAY::~SVSAFEARRAY()
{
	LocalDestroy();
}

bool SVSAFEARRAY::empty() const
{
	return nullptr == m_pSafeArray;
}

void SVSAFEARRAY::clear()
{
	LocalDestroy();
}

SVSAFEARRAY::operator SAFEARRAY * ()
{
	return m_pSafeArray;
}

SVSAFEARRAY::operator _variant_t () const
{
	_variant_t l_Variant;

	if( nullptr != m_pSafeArray )
	{
		VARTYPE l_VarType( VT_EMPTY );

		if( S_OK == GetVartype( l_VarType ) )
		{
			if( S_OK == ::SafeArrayCopy( m_pSafeArray, &( l_Variant.parray ) ) )
			{
				l_Variant.vt = VT_ARRAY | l_VarType;
			}
		}
	}

	return l_Variant;
}

const SVSAFEARRAY& SVSAFEARRAY::operator=( const SVSAFEARRAY& p_rObject )
{
	if( this != &p_rObject )
	{
		*this = const_cast< SAFEARRAY* >( p_rObject.m_pSafeArray );
	}

	return *this;
}

const SVSAFEARRAY& SVSAFEARRAY::operator=( const VARIANT& p_rVariant )
{
	if( ( p_rVariant.vt & VT_ARRAY ) == VT_ARRAY )
	{
		*this = const_cast< SAFEARRAY* >( p_rVariant.parray );
	}
	else
	{
		LocalDestroy();

		if( p_rVariant.vt != VT_EMPTY )
		{
			SAFEARRAYBOUND l_Bound;

			l_Bound.lLbound = 0;
			l_Bound.cElements = static_cast< ULONG >( 1 );

			m_pSafeArray = ::SafeArrayCreate( p_rVariant.vt, 1, &l_Bound );

			PutElement( 0, p_rVariant );
		}
	}

	return *this;
}

const SVSAFEARRAY& SVSAFEARRAY::operator=( SAFEARRAY* p_pObject )
{
	if( m_pSafeArray != p_pObject )
	{
		LocalDestroy();

		if( nullptr != p_pObject )
		{
			::SafeArrayCopy( p_pObject, &m_pSafeArray );
		}
	}

	return *this;
}

HRESULT SVSAFEARRAY::GetVartype( VARTYPE& p_rType ) const
{
	return ::SafeArrayGetVartype( m_pSafeArray, &p_rType );
}

HRESULT SVSAFEARRAY::GetBounds( SVBounds& p_rBounds ) const
{
	HRESULT l_Status = S_OK;

	int l_Dim = ::SafeArrayGetDim( m_pSafeArray );

	if( 0 < l_Dim )
	{
		p_rBounds.resize( l_Dim );

		for( UINT i = 0; i < p_rBounds.size(); ++i )
		{
			HRESULT l_GetStatus = ::SafeArrayGetLBound( m_pSafeArray, ( i + 1 ), &p_rBounds[i].lLbound );

			if( S_OK != l_GetStatus )
			{
				p_rBounds[i].lLbound = 0;

				if( S_OK == l_Status )
				{
					if( S_OK == l_GetStatus )
					{
						l_Status = l_GetStatus;
					}
					else
					{
						l_Status = E_INVALIDARG;
					}
				}
			}

			long l_Upper = 0;

			l_GetStatus = ::SafeArrayGetUBound( m_pSafeArray, ( i + 1 ), &l_Upper );

			if( S_OK == l_GetStatus )
			{
				p_rBounds[i].cElements = l_Upper - p_rBounds[i].lLbound + 1;
			}
			else
			{
				p_rBounds[i].cElements = 0;

				if( S_OK == l_Status )
				{
					if( S_OK == l_GetStatus )
					{
						l_Status = l_GetStatus;
					}
					else
					{
						l_Status = E_INVALIDARG;
					}
				}
			}
		}
	}
	else
	{
		if( l_Dim == 0 )
		{
			l_Status = E_INVALIDARG;
		}
		else
		{
			l_Status = l_Dim;
		}
	}

	return l_Status;
}

SVSAFEARRAY::size_type SVSAFEARRAY::size() const
{
	size_type l_Size( 0 );

	if( 1 == GetDim() )
	{
		long l_Lower( 0 );
		long l_Upper( 0 );

		if( S_OK == ::SafeArrayGetLBound( m_pSafeArray, 1, &l_Lower ) )
		{
			if( S_OK == ::SafeArrayGetUBound( m_pSafeArray, 1, &l_Upper ) )
			{
				l_Size = l_Upper - l_Lower + 1;
			}
		}
	}

	return l_Size;
}

unsigned int SVSAFEARRAY::GetDim() const
{
	return ::SafeArrayGetDim( m_pSafeArray );
}

HRESULT SVSAFEARRAY::GetElement( size_type p_Index, _variant_t& p_rData ) const
{
	HRESULT l_Status( S_OK );

	SVIndex l_Index;

	l_Index.push_back( static_cast< long >( p_Index ) );

	l_Status = GetElement( l_Index, p_rData );

	return l_Status;
}

HRESULT SVSAFEARRAY::GetElement( const SVIndex& p_rIndex, _variant_t& p_rData ) const
{
	HRESULT l_Status( S_OK );

	p_rData.Clear();

	if( nullptr != m_pSafeArray )
	{
		if( 0 < p_rIndex.size() && p_rIndex.size() == GetDim() )
		{
			VARTYPE l_VarType( VT_EMPTY );

			l_Status = GetVartype( l_VarType );

			if( S_OK == l_Status )
			{
				void* l_pData( nullptr );
				long* l_pIndex( const_cast< long* >( &( p_rIndex[0] ) ) );

				_variant_t l_Variant( p_rData );

				l_Status = GetElementVoidAddress( l_Variant, l_pData );

				if( S_OK == l_Status )
				{
					l_Status = ::SafeArrayGetElement( m_pSafeArray, l_pIndex, l_pData );

					if( S_OK == l_Status )
					{
						if( VT_VARIANT != l_VarType )
						{
							l_Variant.vt = l_VarType;
						}

						l_Status = UpdateVariant( l_VarType, l_pData, p_rData );
					}
				}
			}
		}
		else
		{
			l_Status = S_FALSE;
		}
	}
	else
	{
		l_Status = S_FALSE;
	}

	return l_Status;
}

HRESULT SVSAFEARRAY::PutElement( size_type p_Index, const _variant_t& p_rData )
{
	HRESULT l_Status( S_OK );

	SVIndex l_Index;

	l_Index.push_back( static_cast< long >( p_Index ) );

	l_Status = PutElement( l_Index, p_rData );

	return l_Status;
}

HRESULT SVSAFEARRAY::PutElement( const SVIndex& p_rIndex, const _variant_t& p_rData )
{
	HRESULT l_Status( S_OK );

	if( nullptr != m_pSafeArray )
	{
		if( 0 < p_rIndex.size() && p_rIndex.size() == GetDim() )
		{
			VARTYPE l_VarType( VT_EMPTY );

			l_Status = GetVartype( l_VarType );

			if( S_OK == l_Status )
			{
				void* l_pData( nullptr );
				long* l_pIndex( const_cast< long* >( &( p_rIndex[0] ) ) );

				_variant_t l_Variant( p_rData );

				if( VT_VARIANT != l_VarType )
				{
					l_Variant.ChangeType( l_VarType );
				}

				l_Status = PutElementVoidAddress( l_Variant, l_pData );

				if( S_OK == l_Status )
				{
					l_Status = ::SafeArrayPutElement( m_pSafeArray, l_pIndex, l_pData );
				}
			}
		}
		else
		{
			l_Status = S_FALSE;
		}
	}
	else
	{
		l_Status = S_FALSE;
	}

	return l_Status;
}

HRESULT SVSAFEARRAY::Add( const _variant_t& p_rData )
{
	HRESULT l_Status( S_OK );

	if( ( p_rData.vt & VT_ARRAY ) == VT_ARRAY )
	{
		l_Status = E_FAIL;
	}
	else
	{
		SAFEARRAYBOUND l_Bound;

		l_Bound.lLbound = 0;
		l_Bound.cElements = 1;

		if( nullptr == m_pSafeArray )
		{
			m_pSafeArray = ::SafeArrayCreate( p_rData.vt, 1, &l_Bound );

			if( nullptr == m_pSafeArray )
			{
				l_Status = E_FAIL;
			}
		}
		else
		{
			l_Bound.lLbound = m_pSafeArray->rgsabound[0].lLbound;
			l_Bound.cElements = m_pSafeArray->rgsabound[0].cElements + 1;

			l_Status = ::SafeArrayRedim( m_pSafeArray, &l_Bound );
		}

		if( S_OK == l_Status )
		{
			long lIndex = m_pSafeArray->rgsabound[0].cElements - 1;

			l_Status = PutElement( lIndex, p_rData );
		}
	}

	return l_Status;
}

HRESULT SVSAFEARRAY::UpdateVariant( VARTYPE p_Type, const void* p_pData, _variant_t &p_rVariant ) const
{
	HRESULT l_Status( S_OK );

	if( nullptr != p_pData )
	{
		switch( p_Type )
		{
			case VT_I1:
			{
				p_rVariant = *( static_cast< const char* >( p_pData ) );

				break;
			}
			case VT_I2:
			{
				p_rVariant = *( static_cast< const short* >( p_pData ) );

				break;
			}
			case VT_I4:
			{
				p_rVariant = *( static_cast< const long* >( p_pData ) );

				break;
			}
			case VT_I8:
			{
				p_rVariant = *( static_cast< const __int64* >( p_pData ) );
				break;
			}
			case VT_INT:
			{
				p_rVariant = *( static_cast< const int* >( p_pData ) );

				break;
			}
			case VT_UI1:
			{
				p_rVariant = *( static_cast< const unsigned char* >( p_pData ) );

				break;
			}
			case VT_UI2:
			{
				p_rVariant = *( static_cast< const unsigned short* >( p_pData ) );

				break;
			}
			case VT_UI4:
			{
				p_rVariant = *( static_cast< const unsigned long* >( p_pData ) );

				break;
			}
			case VT_UI8:
			{
				p_rVariant = *( static_cast< const unsigned __int64* >( p_pData ) );
				break;
			}
			case VT_UINT:
			{
				p_rVariant = *( static_cast< const unsigned int* >( p_pData ) );

				break;
			}
			case VT_R4:
			{
				p_rVariant = *( static_cast< const float* >( p_pData ) );

				break;
			}
			case VT_R8:
			{
				p_rVariant = *( static_cast< const double* >( p_pData ) );

				break;
			}
			case VT_BOOL:
			{
				p_rVariant = *( static_cast< const bool* >( p_pData ) );

				break;
			}
			case VT_BSTR:
			{
				_bstr_t l_String( *( static_cast< const BSTR* >( p_pData ) ) );
				
				p_rVariant = l_String;

				break;
			}
			case VT_ERROR:
			{
				p_rVariant = *( static_cast< const long* >( p_pData ) );

				p_rVariant.ChangeType( VT_ERROR );

				break;
			}
			case VT_DATE:
			{
				p_rVariant = *( static_cast< const double* >( p_pData ) );

				p_rVariant.ChangeType( VT_DATE );

				break;
			}
			case VT_VARIANT:
			{
				p_rVariant = static_cast< const VARIANT* >( p_pData );

				break;
			}
			default:
			{
				p_rVariant.Clear();

				l_Status = S_FALSE;
			}
		}
	}
	else
	{
		l_Status = S_FALSE;
	}

	return l_Status;
}

HRESULT SVSAFEARRAY::GetGeneralVoidAddress( const _variant_t &p_rVariant, void*& p_rpData ) const
{
	VARTYPE l_VarType( VT_EMPTY );

	HRESULT l_Status( GetVartype( l_VarType ) );

	if( S_OK == l_Status )
	{
		switch( l_VarType )
		{
			case VT_I1:
			{
				p_rpData = static_cast< void* >( const_cast< char* >( &( p_rVariant.cVal ) ) );

				break;
			}
			case VT_I2:
			{
				p_rpData = static_cast< void* >( const_cast< short* >( &( p_rVariant.iVal ) ) );

				break;
			}
			case VT_I4:
			{
				p_rpData = static_cast< void* >( const_cast< long* >( &( p_rVariant.lVal ) ) );

				break;
			}
			case VT_I8:
			{
				p_rpData = static_cast< void* >( const_cast< __int64* >( &( p_rVariant.llVal ) ) );

				break;
			}
			case VT_INT:
			{
				p_rpData = static_cast< void* >( const_cast< int* >( &( p_rVariant.intVal ) ) );

				break;
			}
			case VT_UI1:
			{
				p_rpData = static_cast< void* >( const_cast< unsigned char* >( &( p_rVariant.bVal ) ) );

				break;
			}
			case VT_UI2:
			{
				p_rpData = static_cast< void* >( const_cast< unsigned short* >( &( p_rVariant.uiVal ) ) );

				break;
			}
			case VT_UI4:
			{
				p_rpData = static_cast< void* >( const_cast< unsigned long* >( &( p_rVariant.ulVal ) ) );

				break;
			}
			case VT_UI8:
			{
				p_rpData = static_cast< void* >( const_cast< unsigned __int64* >( &( p_rVariant.ullVal ) ) );

				break;
			}
			case VT_UINT:
			{
				p_rpData = static_cast< void* >( const_cast< unsigned int* >( &( p_rVariant.uintVal ) ) );

				break;
			}
			case VT_R4:
			{
				p_rpData = static_cast< void* >( const_cast< float* >( &( p_rVariant.fltVal ) ) );

				break;
			}
			case VT_R8:
			{
				p_rpData = static_cast< void* >( const_cast< double* >( &( p_rVariant.dblVal ) ) );

				break;
			}
			case VT_BOOL:
			{
				p_rpData = static_cast< void* >( const_cast< VARIANT_BOOL* >( &( p_rVariant.boolVal ) ) );

				break;
			}
			case VT_ERROR:
			{
				p_rpData = static_cast< void* >( const_cast< SCODE* >( &( p_rVariant.scode ) ) );

				break;
			}
			case VT_DATE:
			{
				p_rpData = static_cast< void* >( const_cast< DATE* >( &( p_rVariant.date ) ) );

				break;
			}
			case VT_VARIANT:
			{
				const VARIANT* l_pVariant( &p_rVariant );

				p_rpData = static_cast< void* >( const_cast< VARIANT* >( l_pVariant ) );

				break;
			}
			default:
			{
				p_rpData = nullptr;

				l_Status = S_FALSE;
			}
		}
	}

	return l_Status;
}

HRESULT SVSAFEARRAY::GetElementVoidAddress( const _variant_t &p_rVariant, void*& p_rpData ) const
{
	VARTYPE l_VarType( VT_EMPTY );

	HRESULT l_Status( GetVartype( l_VarType ) );

	if( S_OK == l_Status )
	{
		switch( l_VarType )
		{
			case VT_BSTR:
			{
				p_rpData = static_cast< void* >( const_cast< BSTR* >( &( p_rVariant.bstrVal ) ) );

				break;
			}
			default:
			{
				l_Status = GetGeneralVoidAddress( p_rVariant, p_rpData );

				break;
			}
		}
	}

	return l_Status;
}

HRESULT SVSAFEARRAY::PutElementVoidAddress( const _variant_t &p_rVariant, void*& p_rpData ) const
{
	VARTYPE l_VarType( VT_EMPTY );

	HRESULT l_Status( GetVartype( l_VarType ) );

	if( S_OK == l_Status )
	{
		switch( l_VarType )
		{
			case VT_BSTR:
			{
				p_rpData = static_cast< void* >( p_rVariant.bstrVal );

				break;
			}
			default:
			{
				l_Status = GetGeneralVoidAddress( p_rVariant, p_rpData );

				break;
			}
		}
	}

	return l_Status;
}

void SVSAFEARRAY::LocalDestroy()
{
	if( nullptr != m_pSafeArray )
	{
		::SafeArrayDestroy( m_pSafeArray );

		m_pSafeArray = nullptr;
	}
}

long getArraySizeFromOneDim(const _variant_t& rValue)
{
	long arraySize {-1};
	if (VT_ARRAY == (rValue.vt & VT_ARRAY) && 1 == ::SafeArrayGetDim(rValue.parray))
	{
		long lowerBound {0L};
		long upperBound {0L};

		if (S_OK == ::SafeArrayGetLBound(rValue.parray, 1, &lowerBound))
		{
			if (S_OK == ::SafeArrayGetUBound(rValue.parray, 1, &upperBound))
			{
				arraySize = upperBound - lowerBound + 1;
			}
		}
	}
	return arraySize;
}
} // namespace SvUl