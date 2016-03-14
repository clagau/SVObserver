//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLine
//* .File Name       : $Workfile:   SVLine.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   15 May 2014 13:12:44  $
//******************************************************************************

#include "stdafx.h"
#include "SVLine.h"
#include "SVImageClass.h"
#include "SVLineAnalyzer.h"
#include "SVObjectLibrary/SVObjectPointArrayClass.h"
#include "SVObjectLibrary/SVObjectAttributeClass.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SVDataArrayClass::SVDataArrayClass( DWORD DataType /* = SV_BYTE */ )
{
	pData   = NULL;
	size    = 0;			// Default size
	oldSize = 0;
	grow    = 20;			// Default grow value
	type    = DataType;		// 1 Byte unsigned: SV_8BIT_INT
}

BOOL SVDataArrayClass::SetSize( int NewSize /* = -1 */, int NewGrow /* = 20 */, DWORD NewDataType /* = 0 */ )
{
	if( NewDataType  )
	{
		if( GetDataSize( NewDataType ) )
		{
			FreeExtra();
			type = NewDataType;
		}
		else
		{
			// No valid new type...
			ASSERT( 0 );
			return FALSE;
		}
	}

	if( NewSize == size )
	{
		grow = NewGrow;
		return TRUE;
	}

	if( NewSize < 0 )
	{
		// Delete complete memory...
		BYTE* pMyData = pData;
		size    = 0;
		oldSize = 0;
		pData   = NULL;
		grow    = NewGrow;
		if( pMyData )
			delete [] pMyData;

		return TRUE;
	}

	if( NewSize > size && NewSize > 0 )
	{
		// Get existing memory...
		if( NewSize <= oldSize )
		{
			size = NewSize;
			grow = NewGrow;
			return TRUE;
		}

		// Allocate complete new memory...
		BYTE* pMyData = new BYTE[ NewSize * GetDataSize() ];
		if( pMyData )
		{
			BYTE* pMyOldData = pData;
			if( size > 0 )
				memcpy( pMyData, pData, size );
			pData   = pMyData;
			size    = NewSize;
			oldSize = NewSize;
			grow    = NewGrow;
			if( pMyOldData )
				delete [] pMyOldData;
			
			return TRUE;
		}
		ASSERT( 0 );
		return FALSE;
	}

	if( NewSize < size )
	{
		int count = size - NewSize;
		size = NewSize;
		// Clear unused memory...
		memset( pData + NewSize, 0, count );
		grow = NewGrow;
		return TRUE;
	}

	ASSERT( 0 );
	return FALSE;
}

BOOL SVDataArrayClass::SetAt( int Index, void* PNewData )
{
	ASSERT( Index >= 0 && Index < size );

	if( pData && Index >= 0 && Index < size )
	{
		switch( type )
		{
			case SV_8BIT_INT:
				*( ( ( unsigned char* ) pData ) + Index ) = *( ( unsigned char* ) PNewData );
				return TRUE;

			case SV_SIGNED_8BIT_INT:
				*( ( ( __int8* ) pData ) + Index ) = *( ( __int8* ) PNewData );
				return TRUE;
			
			case SV_16BIT_INT:
				*( ( ( unsigned short* ) pData ) + Index ) = *( ( unsigned short* ) PNewData );
				return TRUE;

			case SV_SIGNED_16BIT_INT:
				*( ( ( __int16* ) pData ) + Index ) = *( ( __int16* ) PNewData );
				return TRUE;

			case SV_32BIT_INT:
				*( ( ( unsigned long* ) pData ) + Index ) = *( ( unsigned long* ) PNewData );
				return TRUE;

			case SV_SIGNED_32BIT_INT:
				*( ( ( __int32* ) pData ) + Index ) = *( ( __int32* ) PNewData );
				return TRUE;

			case SV_SIGNED_64BIT_INT:
				*( ( ( __int64* ) pData ) + Index ) = *( ( __int64* ) PNewData );
				return TRUE;
			
			case SV_FLOAT:
				*( ( ( float* ) pData ) + Index ) = *( ( float* ) PNewData );
				return TRUE;

			case SV_DOUBLE:
				*( ( ( double* ) pData ) + Index ) = *( ( double* ) PNewData );
				return TRUE;
		}
	}
	return FALSE;
}

DWORD SVDataArrayClass::GetData()
{
	return( ( DWORD ) pData );
}

int SVDataArrayClass::GetDataSize( DWORD NewDataType /* = 0L */ )
{
	// Returns valid data type size in bytes

	switch( NewDataType ? NewDataType : type )
	{
		case SV_8BIT_INT:
			return( sizeof( unsigned char ) );

		case SV_SIGNED_8BIT_INT:
			return( sizeof( __int8 ) );
			
		case SV_16BIT_INT:
			return( sizeof( unsigned short ) );

		case SV_SIGNED_16BIT_INT:
			return( sizeof( __int16 ) );

		case SV_32BIT_INT:
			return( sizeof( unsigned long ) );

		case SV_SIGNED_32BIT_INT:
			return( sizeof( __int32 ) );

		case SV_SIGNED_64BIT_INT:
			return( sizeof( __int64 ) );
			
		case SV_FLOAT:
			return( sizeof( float ) );
			
		case SV_DOUBLE:
			return( sizeof( double ) );
	}
	return 0;
}

int SVDataArrayClass::GetSize()
{
	// Returns valid size
	// May be not equal to real size of pData, but lower than real size!
	// Refer to oldSize ( = real size )....

	return size;
}

DWORD SVDataArrayClass::GetAt( int Index, void* PReturnedData /* = NULL */ )
{
	ASSERT( Index >= 0 && Index < size );

	if( pData && Index >= 0 && Index < size )
	{
		switch( type )
		{
			case SV_8BIT_INT:
				if( PReturnedData )
					*( ( unsigned char* ) PReturnedData ) = *( ( ( unsigned char* ) pData ) + Index );
				return( *( ( ( unsigned char* ) pData ) + Index ) );

			case SV_SIGNED_8BIT_INT:
				if( PReturnedData )
					*( ( __int8* ) PReturnedData ) = *( ( ( __int8* ) pData ) + Index );
				return( *( ( ( __int8* ) pData ) + Index ) );
			
			case SV_16BIT_INT:
				if( PReturnedData )
					*( ( unsigned short* ) PReturnedData ) = *( ( ( unsigned short* ) pData ) + Index );
				return( *( ( ( unsigned short* ) pData ) + Index ) );

			case SV_SIGNED_16BIT_INT:
				if( PReturnedData )
					*( ( __int16* ) PReturnedData ) = *( ( ( __int16* ) pData ) + Index );
				return( *( ( ( __int16* ) pData ) + Index ) );

			case SV_32BIT_INT:
				if( PReturnedData )
					*( ( unsigned long* ) PReturnedData ) = *( ( ( unsigned long* ) pData ) + Index );
				return( *( ( ( unsigned long* ) pData ) + Index ) );

			case SV_SIGNED_32BIT_INT:
				if( PReturnedData )
					*( ( __int32* ) PReturnedData ) = *( ( ( __int32* ) pData ) + Index );
				return( *( ( ( __int32* ) pData ) + Index ) );

			case SV_SIGNED_64BIT_INT:
				if( PReturnedData )
					*( ( __int64* ) PReturnedData ) = *( ( ( __int64* ) pData ) + Index );
				return( -1 );

			case SV_FLOAT:
				if( PReturnedData )
					*( ( float* ) PReturnedData ) = *( ( ( float* ) pData ) + Index );
				return( ( DWORD ) *( ( ( float* ) pData ) + Index ) );

			case SV_DOUBLE:
				if( PReturnedData )
					*( ( double* ) PReturnedData ) = *( ( ( double* ) pData ) + Index );
				return( ( DWORD ) *( ( ( double* ) pData ) + Index ) );

		}
	}
	return -1;
}


SV_IMPLEMENT_CLASS( SVLineClass, SVLineClassGuid );

SVLineClass::SVLineClass()
{
	// Indentify yourself...
	outObjectInfo.ObjectTypeInfo.ObjectType = SVLineObjectType;

	mlLinePixelLength = 0L;
	mlLineArrayLength = 0L;

	m_dwLineArrayDataType = SV_BYTE;

	m_lPixelDepth = 8;

	m_dwMaxThreshold = 0;
	m_dwMinThreshold = 0;
	m_dwColorNumber = 0;
	m_dwDisplayedMaxThresholdBarValue = 0;
	m_dwDisplayedMinThresholdBarValue = 0;

	m_bDisplayGraph = FALSE;

	m_svLineExtent.SetTranslation( SVExtentTranslationLine );
}

SVLineClass::~SVLineClass()
{
}

BOOL SVLineClass::CloseObject()
{
	BOOL rc = SVObjectAppClass::CloseObject();

	rc = Destroy() && rc;

	return rc;
}

HRESULT SVLineClass::ResetObject()
{
	HRESULT l_hrOk = SVObjectAppClass::ResetObject();

	if( l_hrOk == S_OK )
	{
		SVExtentPointStruct l_svStart;
		SVExtentPointStruct l_svEnd;

		if( m_svLineExtent.GetExtentProperty( SVExtentPropertyPositionPoint, l_svStart ) == S_OK &&
		    m_svLineExtent.GetExtentProperty( SVExtentPropertyPositionPointEndOfLine, l_svEnd ) == S_OK )
		{
			double l_dWidth = l_svEnd.m_dPositionX - l_svStart.m_dPositionX;
			double l_dHeight = l_svEnd.m_dPositionY - l_svStart.m_dPositionY;

			mlLinePixelLength = SVCalcLinePixelNumber( static_cast<long>(l_dWidth), static_cast<long>(l_dHeight) );
		}
		else
		{
			l_hrOk = S_FALSE;
		}
	}

	if( l_hrOk == S_OK )
	{
		// Allocate line buffers...
		// Set array sizes...

		mResultArray.clear();

		if ( ! mLineArray.SetSize( mlLinePixelLength + 10, 25, m_dwLineArrayDataType ) )
		{
			l_hrOk = S_FALSE;
		}

		if ( l_hrOk == S_OK )
		{
			if ( ! mNormalizedArray.SetSize( mlLinePixelLength + 10, 25, SV_DOUBLE ) )
			{
				l_hrOk = S_FALSE;
			}
		}

		if ( l_hrOk == S_OK )
		{
			mlLineArrayLength = mlLinePixelLength;
		}
	}

	isCreated = l_hrOk == S_OK;

	return l_hrOk;
}


BOOL SVLineClass::Destroy()
{
	mlLinePixelLength = 0L;
	mlLineArrayLength = 0L;

	mLineArray.RemoveAll();
	mLineArray.FreeExtra();

	mNormalizedArray.RemoveAll();
	mNormalizedArray.FreeExtra();

	mResultArray.clear();

	return TRUE;
}

void *SVLineClass::GetDataBuffer()
{
	return (void *)( mLineArray.GetData() ); 
}

BOOL SVLineClass::ReadLine()
{
	for ( long l = 0; l < mlLineArrayLength; l++ )
	{
		long lValue = 0;
		double dValue = 0.0;

		mLineArray.GetAt( l, (void *)(&lValue) );
		dValue = lValue;
		mNormalizedArray.SetAt( l, (void *)(&dValue) );
	}

	return TRUE;
}

BOOL SVLineClass::ReadData( double dProfileHeight )
{
	if ( dProfileHeight == 0.0 || dProfileHeight == 1.0 )
		return ReadLine();

	for ( long l = 0; l < mlLineArrayLength; l++ )
	{
		long lValue = 0;
		double dValue = 0.0;

		mLineArray.GetAt( l, (void *)(&lValue) );
		dValue = (double)(lValue) / dProfileHeight;
		mNormalizedArray.SetAt( l, (void *)(&dValue) );
	}

	return TRUE;
}

HRESULT SVLineClass::GetHeadPoint( POINT &p_roPoint )
{
	HRESULT l_hrOk = S_FALSE;

	l_hrOk = m_svLineExtent.GetExtentProperty( SVExtentPropertyPositionPoint, p_roPoint );

	return l_hrOk;
}

HRESULT SVLineClass::GetTailPoint( POINT &p_roPoint )
{
	HRESULT l_hrOk = S_FALSE;

	l_hrOk = m_svLineExtent.GetExtentProperty( SVExtentPropertyPositionPointEndOfLine, p_roPoint );

	return l_hrOk;
}

void SVLineClass::Persist(SVObjectWriter& rWriter)
{
	rWriter.StartElement(GetObjectName());  // use internal name for node name
	SVObjectAppClass::Persist(rWriter);
	rWriter.EndElement();
}

// Restoration of trivial members
HRESULT SVLineClass::SetObjectValue( SVObjectAttributeClass* PDataObject )
{
	HRESULT hr = S_FALSE;
	BOOL bOk = FALSE;


	SvCl::SVObjectDoubleArrayClass svDoubleArray;
	SvCl::SVObjectLongArrayClass svLongArray;
	SVObjectSVPointArrayClass svPointArray;

	double l_dValue = 0.0;

	if ( ( bOk = PDataObject->GetAttributeData( "ExtentLeft", svDoubleArray ) ) )
	{
		if( 0 < svDoubleArray.GetSize() )
		{
			double l_dValue = svDoubleArray[svDoubleArray.GetSize() - 1];

			m_svLineExtent.SetExtentProperty( SVExtentPropertyOldPositionPointX, l_dValue );
		}
	}
	else if ( ( bOk = PDataObject->GetAttributeData( "ExtentTop", svDoubleArray ) ) )
	{
		if( 0 < svDoubleArray.GetSize() )
		{
			double l_dValue = svDoubleArray[svDoubleArray.GetSize() - 1];

			m_svLineExtent.SetExtentProperty( SVExtentPropertyOldPositionPointY, l_dValue );
		}
	}
	else if ( ( bOk = PDataObject->GetAttributeData( "ExtentWidth", svDoubleArray ) ) )
	{
		/*
		if( 0 < svDoubleArray.GetSize() )
		{
			double l_dValue = svDoubleArray[svDoubleArray.GetSize() - 1];

			m_svLineExtent.SetExtentProperty( SVExtentPropertyOldWidth, l_dValue );
		}
		//*/
	}
	else if ( ( bOk = PDataObject->GetAttributeData( "ExtentHeight", svDoubleArray ) ) )
	{
		/*
		if( 0 < svDoubleArray.GetSize() )
		{
			double l_dValue = svDoubleArray[svDoubleArray.GetSize() - 1];

			m_svLineExtent.SetExtentProperty( SVExtentPropertyOldHeight, l_dValue );
		}
		//*/
	}
	else if ( ( bOk = PDataObject->GetAttributeData( "AbsExtentLeft", svDoubleArray ) ) )
	{
		if( 0 < svDoubleArray.GetSize() )
		{
			double l_dValue = svDoubleArray[svDoubleArray.GetSize() - 1];

			m_svLineExtent.SetExtentProperty( SVExtentPropertyOldAbsPositionPointX, l_dValue );
		}
	}
	else if ( ( bOk = PDataObject->GetAttributeData( "AbsExtentTop", svDoubleArray ) ) )
	{
		if( 0 < svDoubleArray.GetSize() )
		{
			double l_dValue = svDoubleArray[svDoubleArray.GetSize() - 1];

			m_svLineExtent.SetExtentProperty( SVExtentPropertyOldAbsPositionPointY, l_dValue );
		}
	}
	else if ( ( bOk = PDataObject->GetAttributeData( "AbsExtentWidth", svDoubleArray ) ) )
	{
		// Value no longer scripted.
	}
	else if ( ( bOk = PDataObject->GetAttributeData( "AbsExtentHeight", svDoubleArray ) ) )
	{
		// Value no longer scripted.
	}
	else if ( ( bOk = PDataObject->GetAttributeData( "RotationAngle", svDoubleArray ) ) )
	{
		if( 0 < svDoubleArray.GetSize() )
		{
			double l_dValue = svDoubleArray[svDoubleArray.GetSize() - 1];

			m_svLineExtent.SetExtentProperty( SVExtentPropertyOldRotationAngle, l_dValue );
		}
	}
	else if ( ( bOk = PDataObject->GetAttributeData( "RotationCenter", svPointArray ) ) )
	{
		if( 0 < svPointArray.GetSize() )
		{
			POINT l_oValue = svPointArray[svPointArray.GetSize() - 1];

			m_svLineExtent.SetExtentProperty( SVExtentPropertyOldRotationPoint, l_oValue );
		}
	}
	else if ( ( bOk = PDataObject->GetAttributeData( "PixelDepth", svLongArray ) ) )
	{
	}
	else if ( ( bOk = PDataObject->GetAttributeData( "BandNumber", svLongArray ) ) )
	{
	}
	else if ( ( bOk = PDataObject->GetAttributeData( "Translation", svPointArray ) ) )
	{
		if( 0 < svPointArray.GetSize() )
		{
			POINT l_oValue = svPointArray[svPointArray.GetSize() - 1];

			m_svLineExtent.SetExtentProperty( SVExtentPropertyOldTranslationOffset, l_oValue );
		}
	}
	else
	{
		hr = SVObjectAppClass::SetObjectValue( PDataObject );
		return hr;
	}

	hr = bOk ? S_OK : S_FALSE;
	return hr;
}

DWORD_PTR SVLineClass::processMessage(DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext)
{
	DWORD_PTR DwResult = 0;

	switch (DwMessageID & SVM_PURE_MESSAGE)
	{
	case SVMSGID_RESET_ALL_OBJECTS :
		{
			HRESULT l_ResetStatus = ResetObject();
			if( l_ResetStatus != S_OK )
			{
				ASSERT( SUCCEEDED( l_ResetStatus ) );

				DwResult = SVMR_NO_SUCCESS;
			}
			else
			{
				DwResult = SVMR_SUCCESS;
			}
			break;
		}
	}
	return (DwResult | SVObjectAppClass::processMessage(DwMessageID, DwMessageValue, DwMessageContext));
}

