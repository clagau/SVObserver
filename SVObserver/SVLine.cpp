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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVLine.cpp_v  $
 * 
 *    Rev 1.3   15 May 2014 13:12:44   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Revised processMessage to use DWORD_PTR
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   01 Feb 2014 10:48:58   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SVSendmessage and processmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   13 May 2013 12:16:26   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 014.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
      Rev 3.24   02 May 2013 11:21:44   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  810
   SCR Title:  Remove support for Legacy Tools
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Updated process message method to fix formatting issues and updated the reset object section to only assert when the HRESULT is a failed status.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
 *    Rev 1.0   23 Apr 2013 11:47:12   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.23   11 Sep 2012 18:59:04   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  602
   SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated class to correct the persistance problem.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.22   16 Aug 2012 18:55:46   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated code to fix issue when not clearing result array when edge data is not present.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.21   21 Feb 2012 13:24:20   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  748
   SCR Title:  Add Remote Output Steams to SVObserver
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Updated code to fix loading issue associated with POINT.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.20   16 Mar 2011 13:40:46   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  712
   SCR Title:  Fix issues with black images when using command interface (SIAC)
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated extent functionality and renamed extent methods to correct extent issues and add clarity to the logical flow.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.19   28 Oct 2010 13:54:22   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  704
   SCR Title:  Upgrade SVObserver Version for 5.33 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to remove duplicate definition of HRESULT.  The SVHRESULT definition will be removed in place of the Microsoft defined HRESULT.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.18   29 Jun 2010 14:02:26   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  693
   SCR Title:  Fix Performance Issue with Inspection Process
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source to change object validity test and casting changes.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.17   01 Jun 2010 10:49:24   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  693
   SCR Title:  Fix Performance Issue with Inspection Process
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated code to remove redundent methodologies and fix missing or incorrect calling functionality.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.16   16 Dec 2009 11:24:54   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  677
   SCR Title:  Fix problem in camera notify thread
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Fix issues with includes and comments.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.15   21 Jun 2007 13:27:38   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  598
   SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
   Checked in by:  jSpila;  Joseph Spila
   Change Description:  
     These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.14   22 Jun 2005 13:32:02   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  464
   SCR Title:  Add array indexing for value objects
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     SetObjectValue now returns SVHRESULT
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.13   17 Feb 2005 14:37:44   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  456
   SCR Title:  Update Image and Tool Objects to use the new Extent Classes
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Update class with new methodologies based on new base object, extents and reset structure.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.12   22 Apr 2003 10:39:10   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  346
   SCR Title:  Update SVObserver to Version 4.21 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.11   07 Mar 2003 11:03:20   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  333
   SCR Title:  Profile shows wrong image with no analyzr & is run even with false conditional
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
     Made changes to profile tool to always show the correct output image, regardless if it is conditionally disabled or rotated without an analyzer.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.10   30 Jan 2003 12:00:50   joe
   Project:  SVObserver
   Change Request (SCR) nbr:  301
   SCR Title:  Implement Result Image Circle Buffer
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated processMessage, ReadLine and Create methods to use the new image circle buffer methodology.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.9   19 Nov 2002 15:03:48   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  226
   SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Change in Create – comparison to PixelDepth
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.8   30 Jan 2002 13:07:02   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  277
   SCR Title:  Gage Tool that touches right or bottom of image window won't update
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
     Changed ::ReadLine so that when it asks the MIL library for the line data it uses the correct endpoints. The right bottom point should be left + width - 1 and top + height - 1.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.7   08 Nov 2001 14:19:46   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  255
   SCR Title:  Add Sub-pixel resolution to Profile and Line Gage Tool Analyzers
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated SVLineInfoClass::SVLineInfoClass, SVLineInfoClass::operator=, SVLineClass::SVLineClass, SVLineClass::Create, SVLineClass::Destroy, SVLineClass::Move, SVLineClass::ReadLine, and SVLineClass::ReadData methods to caculate sub-pixel results.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.6   04 Oct 2001 15:46:36   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  229
   SCR Title:  Upgrade SVObserver to Version 3.34 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Modified source code to remove compiler warning message.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.5   19 Apr 2001 14:40:26   Steve
   Project:  SVObserver
   Change Request (SCR) nbr:  196
   SCR Title:  Restructure Scripted Load/Save Procedures Functional Requirement
   Checked in by:  Steve;  Stephen E. Steffan
   Change Description:  
     Header file changes due to include file restructuring.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.4   26 Apr 2000 18:42:16   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  129
   SCR Title:  Angular Profile Tool
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised SetExtent, SetAbsoluteExtent,SetAbsolutePosition,
   SetAbsoluteLeft,SetAbsoluteTop,SetWidth, and SetHeight
   methods to send the 
   SVM_SET_SHADOWED_ABSEXTENT message.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.3   08 Mar 2000 21:53:18   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  105
   SCR Title:  Handling of Tool Figures in Absolute, Relative, and Transformed Views
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised SetExtent to also set the width and height variables
   in the AbsoluteExtent variable.
   Revised SetAbsolute to also set the width and height variables in the Extent variable.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.2   03 Mar 2000 12:16:38   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  112
   SCR Title:  Expose Extents of Images
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised Extent and AbsoluteExtent variables to be private.
   Added Get and Set methods to access extents.
   Revised GetObjectScript to write out doubles for extents.
   Revised SetObjectValue to accept doubles for extents
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.1   25 Feb 2000 16:21:38   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  105
   SCR Title:  Handling of Tool Figures in Absolute, Relative, and Transformed Views
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Added variable for translation (POINT) to SVLineInfoClass.
   Revised SVLineClass GetObjectScript/SetObjectValue
   methods to save and restore the translation point.
   REmoved unnecessary initialization in the SVLineClass CTor
   
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.0   23 Nov 1999 12:58:12   mike
   Project:  SVObserver
   Change Request (SCR) nbr:  61
   SCR Title:  Update PVCS versioning to version 3.0.
   Checked in by:  Mike;  Mike McCarl
   Change Description:  
     
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.14   28 Oct 1999 08:42:20   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  9
   SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised GetObjectScript to address performance issues.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.13   Oct 13 1999 15:06:46   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  21
   SCR Title:  Port Gage Tool code from program version 2.42.
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Roberts bug fixes for Beta 10.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.12   Oct 04 1999 10:13:28   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  21
   SCR Title:  Port Gage Tool code from program version 2.42.
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Changed to support 360 Degree Measurement.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.11   Sep 30 1999 14:05:30   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  41
   SCR Title:  Version 3.00 Beta 5 Versioning.
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Code to fix 'lost' extent data during configuration saves and restores.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.10   Sep 20 1999 15:34:36   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  35
   SCR Title:  PPQ mode to wait for input data
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Added Extent Checking.
   Introduced Broadcast code.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.9   15 Sep 1999 16:13:52   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  9
   SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Re-added scripting of BandNumber (lost change)
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.8   Sep 10 1999 17:59:42   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  33
   SCR Title:  Port Profile Tool from v2.42
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Added ReadData()
   Removed old PointDraw(), Draw(), DrawSingleLine()
   Remove obsolete parameter of Line Info.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.6   04 Sep 1999 17:31:12   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  9
   SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised to save/restore extents, absolute extents, 
   rotationAngle, rotationCenter, and pixelDepth via
   ObjectScript.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.5   30 Aug 1999 20:10:52   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  9
   SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Added processMessage method to handle
   SVMSGID_CREATE_ALL_OBJECTS.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.4   29 Aug 1999 14:39:16   robert
   Project:  SVObserver
   Change Request (SCR) nbr:  21
   SCR Title:  Port Gage Tool code from program version 2.42.
   Checked in by:  Robert;  Robert Massinger
   Change Description:  
     Fixed to set isCreated Flag.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.3   27 Aug 1999 14:08:36   robert
   Project:  SVObserver
   Change Request (SCR) nbr:  21
   SCR Title:  Port Gage Tool code from program version 2.42.
   Checked in by:  Robert Massinger
   Change Description:  
     Added Line Object Type.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.2   25 Aug 1999 12:31:58   robert
   Project:  SVObserver
   Change Request (SCR) nbr:  21
   SCR Title:  Port Gage Tool code from program version 2.42.
   Checked in by:  Robert Massinger
   Change Description:  
     Ported to 3.0. Used by Line ROI and Line Analyzers.
   
   /////////////////////////////////////////////////////////////////////////////////////
*/
