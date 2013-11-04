//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVIO
//* .File Name       : $Workfile:   SVIO.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 11:06:00  $
//******************************************************************************

#include "stdafx.h"
#include "SVObserver.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVIO.h"
#include "SVSVIMStateClass.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

static const IID IID_ISVIOClass =
{ 0x35299181, 0x7c5a, 0x11d1, { 0xb7, 0x20, 0xc4, 0x70, 0x56, 0xa5, 0x40, 0x6 } };

SVIOClass::SVIOClass()
{
	IsIOControlValid = FALSE;
}

SVIOClass::~SVIOClass()
{
	IsIOControlValid = FALSE;
}

BOOL SVIOClass::Create( HWND HMainWnd )
{
	hMainFrame = HMainWnd;
	IsIOControlValid = TRUE;
	return TRUE;
}


SVIOEntryClass* SVIOClass::CreateIOEntry( LPCTSTR PName, BOOL IsAnOutput, GUID UniqueObjectID /*= InvalidGUID*/, int ObjectAttribute /*= -1*/ )
{
	SVIOEntryClass* pNewIOEntry = new SVIOEntryClass( PName, 
													  IsAnOutput,
													  -1,
													  NULL,
													  NULL,
													  NULL,
													  NULL,
													  FALSE,
													  FALSE,	// Default Value
													  UniqueObjectID,
													  ObjectAttribute
													);
	if( pNewIOEntry )
	{
		if( ! ioEntryList.Add( pNewIOEntry ) )
		{
			delete pNewIOEntry;
			pNewIOEntry = NULL;
		}


		
		// Warning!
		//
		// Continue with sending ID_UPDATE_ALL_IOVIEWS only 
		//
		//		if a SEC is loaded ( TheSVObserverApp.IsSECLoaded )
		//      and if we are NOT in a SEC canceling mode 
		//			( ! TheSVObserverApp.IsSECCanceling )
		//
		// This checking is necessary, because a this message could be 
		// proccessed in a time the SVModuleIOView or the SVToolIOView
		// would no longer be valid, caused on go offline and save &
		// clean up remote commands. This would release an APPLICATION 
		// ERROR during a configwrite remote command!
		// This would normally occur if we were in the RUN MODE before!

		// Refresh IOViews
		if ( pNewIOEntry && 
		     SVSVIMStateClass::CheckState( SV_STATE_READY | SV_STATE_RUNNING ) &&
		     ! SVSVIMStateClass::CheckState( SV_STATE_CANCELING ) )
		{
			::PostMessage( hMainFrame, WM_COMMAND, ID_UPDATE_ALL_IOVIEWS, 0 );
		}
	}

	return pNewIOEntry;
}


BOOL SVIOClass::DestroyIOEntry( SVIOEntryClass* PIOEntry )
{
	if( PIOEntry && IsIOControlValid )
	{
		if( ioEntryList.Remove( PIOEntry ) )
		{
			delete PIOEntry;


			// Warning!
			//
			// Continue with sending ID_UPDATE_ALL_IOVIEWS only 
			//
			//		if a SEC is loaded ( TheSVObserverApp.IsSECLoaded )
			//      and if we are NOT in a SEC canceling mode 
			//			( ! TheSVObserverApp.IsSECCanceling )
			//
			// This checking is necessary, because a this message could be 
			// proccessed in a time the SVModuleIOView or the SVToolIOView
			// would no longer be valid, caused on go offline and save &
			// clean up remote commands. This would release an APPLICATION 
			// ERROR during a configwrite remote command!
			// This would normally occur if we were in the RUN MODE before!

			// Refresh IOViews
		  if ( SVSVIMStateClass::CheckState( SV_STATE_READY | SV_STATE_RUNNING ) &&
		       ! SVSVIMStateClass::CheckState( SV_STATE_CANCELING ) )
			{
				::PostMessage( hMainFrame, WM_COMMAND, ID_UPDATE_ALL_IOVIEWS, 0 );
			}

			return TRUE;
		}
	}
	return FALSE;
}


SVIOEntryListClass* SVIOClass::GetIOEntryList()
{
	return &ioEntryList;
}



SVIOClass IOControl;
BOOL IsIOControlValid = FALSE;

SVIOEntryClass::SVIOEntryClass( LPCTSTR PName, 
				                        BOOL IsAnOutput, 
				                        DWORD CH, 
																SVIODLLBOOLDWORDBOOLFUNCTION POut, 
																SVIODLLBOOLDWORDBOOLPTRFUNCTION PIn,	
																SVIODLLBOOLDWORDBOOLBOOLFUNCTION PForce,
																SVIODLLBOOLDWORDBOOLFUNCTION PInvert,
				                        BOOL IsAModuleIOListEntry,
				                        BOOL BDefaultValue,
					                      const GUID& UniqueObjectID,
					                      int ObjectAttribute,
					                      BOOL BIsForced,
					                      DWORD DwForcedValue,
					                      BOOL BIsInverted)
{
	strIOEntryName		= new TCHAR[ _tcslen( PName ) + 1 ];
	if( strIOEntryName )
		_tcscpy( strIOEntryName, PName );
	isOutput			= IsAnOutput;
	channel				= CH;	 // 0xffffffff: invalid channel
	pOutFunc			= POut;
	pInFunc				= PIn; 
	pForceFunc			= PForce;
	pInvertFunc			= PInvert;
	isModuleIOListEntry = IsAModuleIOListEntry;


	LastValue			= BDefaultValue;
	DefaultValue		= BDefaultValue;
	IsForced			= BIsForced;
	ForcedValue			= DwForcedValue;

	IsInverted			= BIsInverted;

	hAdjustDialogHWnd	= NULL;

	IOObjectID			= UniqueObjectID;
	IOObjectAttribute   = ObjectAttribute;
}

SVIOEntryClass::~SVIOEntryClass()
{
	if( strIOEntryName )
		delete [] strIOEntryName;
}

BOOL SVIOEntryClass::ResetIOEntry()
{
	if( IsIOControlValid )
	{
		channel				= -1;
		pOutFunc			= NULL;
		pInFunc				= NULL; 
		pForceFunc			= NULL;
		pInvertFunc			= NULL;

		isModuleIOListEntry = FALSE;
		LastValue			= FALSE;
		DefaultValue		= FALSE;
		IsForced			= FALSE;
		ForcedValue			= FALSE;

		IsInverted			= FALSE;

		hAdjustDialogHWnd	= NULL;

		return TRUE;
	}
	return FALSE;
}

BOOL SVIOEntryClass::RebuildIOEntry( SVIOEntryClass* PIOEntry )
{
	if( PIOEntry && IsIOControlValid )
	{
		LPTSTR strOldString = strIOEntryName;
		strIOEntryName	  = _T( "Actual Empty..." );
		if( strOldString )
			delete [] strOldString;
		strOldString		= new TCHAR[ _tcslen( PIOEntry->strIOEntryName ) + 1 ];
		_tcscpy( strOldString, PIOEntry->strIOEntryName );
		strIOEntryName		= strOldString;
		isOutput			= PIOEntry->isOutput;
		channel				= PIOEntry->channel;
		pOutFunc			= PIOEntry->pOutFunc;
		pInFunc				= PIOEntry->pInFunc; 
		pForceFunc			= PIOEntry->pForceFunc;
		pInvertFunc			= PIOEntry->pInvertFunc;
		isModuleIOListEntry = PIOEntry->isModuleIOListEntry;
		LastValue			= PIOEntry->LastValue;
		DefaultValue		= PIOEntry->DefaultValue;
		IsForced			= PIOEntry->IsForced;
		ForcedValue			= PIOEntry->ForcedValue;
		IsInverted			= PIOEntry->IsInverted;
		IOObjectID			= PIOEntry->IOObjectID;
		IOObjectAttribute   = PIOEntry->IOObjectAttribute;
		hAdjustDialogHWnd	= NULL; 

		return TRUE;
	}
	return FALSE;
}

// Use BOOL BOutValue to set the Output Value.
// Returns TRUE, if successfully.
// Returns FALSE, if an error happens.
BOOL SVIOEntryClass::Out( BOOL BOutValue )
{
	BOOL bRetVal = FALSE;
	if( isOutput && pOutFunc )
		if( pOutFunc( channel, BOutValue ) )
		{
			LastValue = BOutValue;
			bRetVal = TRUE;
		}

	return bRetVal;
}

void SVIOEntryClass::SetIOAdjustDialogHWnd(	HWND HDlg )
{
	hAdjustDialogHWnd = HDlg;
}

DWORD SVIOEntryClass::GetChannel()
{
	return channel;
}

DWORD SVIOEntryClass::ResetChannel( DWORD DWNewChannel )
{
	DWORD DWRetValue = channel;
	channel = DWNewChannel;
	Init();
	return( DWRetValue );
}

// Init IO Entry/Channel...
void SVIOEntryClass::Init()
{
	Force( IsForced, ForcedValue );
	Invert( IsInverted );
	if( isOutput )
		Out( LastValue );
}

// Use BOOL& RBInValue to get the Input Value.
// Returns TRUE, if successfully.
// Returns FALSE, if an error happens.
BOOL SVIOEntryClass::In( BOOL& RBInValue )
{
	BOOL bRetVal = FALSE;
	if( ! isOutput && pInFunc )
		if( pInFunc( channel, &RBInValue ) )
		{
			LastValue = RBInValue;
			bRetVal = TRUE;
		}

	return bRetVal;
}

LPCTSTR SVIOEntryClass::GetName()
{
	return strIOEntryName;
}

BOOL SVIOEntryClass::GetIsForced()
{
	return IsForced;
}

BOOL SVIOEntryClass::GetForcedValue()
{
	return ForcedValue;
}

BOOL SVIOEntryClass::GetIsInverted()
{
	return IsInverted;
}

BOOL SVIOEntryClass::GetValue()
{
	return LastValue;
}

BOOL SVIOEntryClass::IsValidChannel()
{
	return( channel != -1 );
}


void SVIOEntryClass::Force( BOOL ShouldForce, BOOL IsHigh )
{
	if( pForceFunc )
		if( pForceFunc( channel, ShouldForce, IsHigh ) )
		{
			if( IsForced = ShouldForce )
				ForcedValue	= IsHigh;

			// Force Output
			if( isOutput )
				Out( LastValue );
		}
}

// TRUE		--> Inverted
// FALSE	--> Not Inverted
void SVIOEntryClass::Invert( BOOL BInvert )
{
	if( pInvertFunc )
		if( pInvertFunc( channel, BInvert ) )
		{
			IsInverted = BInvert;
			// Force Output
			if( isOutput )
				Out( LastValue );
		}
}








SVIOEntryListClass::SVIOEntryListClass( int Grow )
{
	if( Grow < 1 )
		Grow = 1;

	maxGrow			 = Grow;
	pIOEntryArray	 = new SVIOEntryClass*[ maxGrow ];
	currentSize		 = maxGrow;
	lastValid		 = -1;
	IsChanged		 = TRUE;
	hProtectionMutex = CreateMutex( NULL, FALSE, NULL );
	// Wait 10 seconds for protection mutex
	dwWaitTime		 = 10000;
}

SVIOEntryListClass::~SVIOEntryListClass()
{
	if( pIOEntryArray )
		delete [] pIOEntryArray;

	CloseHandle( hProtectionMutex );
}

SVIOEntryClass* SVIOEntryListClass::GetAt( int i )
{
	if( i >= 0 && i <= lastValid )
		return pIOEntryArray[ i ];

	return NULL;
}

int SVIOEntryListClass::GetSize()
{
	return( lastValid + 1 );
}

LPCTSTR SVIOEntryListClass::GetNameAt( int i )
{
	if( i >= 0 && i <= lastValid )
		return pIOEntryArray[ i ]->strIOEntryName;

	return NULL;
}

SVIOEntryClass* SVIOEntryListClass::Find( LPCTSTR PName )
{
	if( PName )
	{
		for( int i = lastValid; i >= 0; --i )
			if( _tcscmp( pIOEntryArray[ i ]->strIOEntryName, PName ) == 0 )
				return pIOEntryArray[ i ];
	}
	return NULL;
}

SVIOEntryClass* SVIOEntryListClass::Find( const GUID& RUniqueObjectID, int ObjectAttribute )
{
	for( int i = lastValid; i >= 0; --i )
		if( RUniqueObjectID == pIOEntryArray[ i ]->IOObjectID &&
			ObjectAttribute == pIOEntryArray[ i ]->IOObjectAttribute
			)
			return pIOEntryArray[ i ];
	return NULL;
}

SVIOEntryClass* SVIOEntryListClass::Find( const GUID& RUniqueObjectID, int ObjectAttribute, BOOL IsOutput )
{
	for( int i = lastValid; i >= 0; --i )
		if( RUniqueObjectID == pIOEntryArray[ i ]->IOObjectID &&
			ObjectAttribute == pIOEntryArray[ i ]->IOObjectAttribute &&
			pIOEntryArray[ i ]->isOutput == IsOutput
			)
			return pIOEntryArray[ i ];
	return NULL;
}


SVIOEntryClass* SVIOEntryListClass::Find( LPCTSTR PName, BOOL IsOutput )
{
	if( PName )
	{
		for( int i = lastValid; i >= 0; --i )
			if( _tcscmp( pIOEntryArray[ i ]->strIOEntryName, PName ) == 0 &&
				pIOEntryArray[ i ]->isOutput == IsOutput )
				return pIOEntryArray[ i ];
	}
	return NULL;
}


SVIOEntryClass* SVIOEntryListClass::Find( DWORD Ch, BOOL IsOutput, BOOL IsModule )
{
	for( int i = lastValid; i >= 0; --i )
		if( pIOEntryArray[ i ]->channel				== Ch && 
			pIOEntryArray[ i ]->isOutput			== IsOutput &&
			pIOEntryArray[ i ]->isModuleIOListEntry == IsModule )
			return pIOEntryArray[ i ];

	return NULL;
}

SVIOEntryClass* SVIOEntryListClass::FindNext( BOOL BIsOutput, BOOL BIsModule, int* PIndex )
{
	if( PIndex )
	{
		for( int i = *PIndex; i <= lastValid; ++i )
			if( pIOEntryArray[ i ]->isOutput			== BIsOutput &&
				pIOEntryArray[ i ]->isModuleIOListEntry == BIsModule )
			{
				*PIndex = i + 1;
				return pIOEntryArray[ i ];
			}
	}
	return NULL;
}

SVIOEntryClass* SVIOEntryListClass::FindNext( DWORD DWChannel, BOOL BIsOutput, 
                                              BOOL BIsModule, int* PIndex )
{
	if( PIndex )
	{
		for( int i = *PIndex; i <= lastValid; ++i )
			if( pIOEntryArray[ i ]->channel				== DWChannel && 
				pIOEntryArray[ i ]->isOutput			== BIsOutput &&
				pIOEntryArray[ i ]->isModuleIOListEntry == BIsModule )
			{
				*PIndex = i + 1;
				return pIOEntryArray[ i ];
			}
	}
	return NULL;
}

BOOL SVIOEntryListClass::Add( SVIOEntryClass* PIOEntry )
{
	if( PIOEntry && IsIOControlValid && WaitForSingleObject( hProtectionMutex, dwWaitTime ) == WAIT_OBJECT_0 )
	{
		if( lastValid >= currentSize - 1 )
		{
			// Grow first
			SVIOEntryClass** pPNewIOEntry = new SVIOEntryClass*[ currentSize + maxGrow ];
			if( pPNewIOEntry )
			{
				memmove( pPNewIOEntry, pIOEntryArray, sizeof( SVIOEntryClass* ) * currentSize );
				delete [] pIOEntryArray;
				pIOEntryArray = pPNewIOEntry;
				currentSize += maxGrow;
			}
			else
				return FALSE;
		}
		pIOEntryArray[ ++lastValid ] = PIOEntry;
		IsChanged = TRUE;

		ReleaseMutex( hProtectionMutex );
		return TRUE;
	}
	return FALSE;
}


BOOL SVIOEntryListClass::Rename( LPCTSTR POldName, LPCTSTR PNewName )
{
	if( POldName && PNewName && IsIOControlValid && WaitForSingleObject( hProtectionMutex, dwWaitTime ) == WAIT_OBJECT_0 )
	{
		SVIOEntryClass* pIOEntry;
		LPTSTR			lpchNewString,
						lpchOldString;
		while( pIOEntry = Find( POldName ) )
		{
			lpchNewString = new TCHAR[ _tcslen( PNewName ) + 1 ];
			if( ! lpchNewString )
			{
				ReleaseMutex( hProtectionMutex );
				return FALSE;
			}
			_tcscpy( lpchNewString, PNewName );
			lpchOldString = pIOEntry->strIOEntryName;
			pIOEntry->strIOEntryName = lpchNewString;
			if( lpchOldString )
				delete [] lpchOldString;
		}
		ReleaseMutex( hProtectionMutex );
		return TRUE;
	}
	return FALSE;
}

void SVIOEntryListClass::Refresh()
{
	if( IsIOControlValid && WaitForSingleObject( hProtectionMutex, dwWaitTime ) == WAIT_OBJECT_0 )
	{
		LPTSTR			lpchNewString,
						lpchOldString,
						lpName;
		for( int i = 0; i <= lastValid; ++i )
			if( pIOEntryArray[ i ]->IOObjectID != SVInvalidGUID )
			{
				lpName = NULL;
				// Refresh name...
				SVObjectClass* pObject = SVObjectManagerClass::Instance().GetObject( pIOEntryArray[ i ]->IOObjectID );
				if( pObject )
				{
// H E L L O !
// Only for debugging purpose, must not be integrated in Tracking System.
// AfxMessageBox( pObject->GetName() );
// RO, 15. Sep. 1999
					if( pIOEntryArray[ i ]->IOObjectAttribute >= 0 )
						lpName = pObject->GetCompleteObjectName2( TheSVObserverApp.GetStringResource( IDS_RESULT_ATTRIBUTE_VALUE_STRING + pIOEntryArray[ i ]->IOObjectAttribute ) );
					else
						lpName = pObject->GetCompleteObjectName2();

					if( lpName )
					{
						lpchNewString = new TCHAR[ _tcslen( lpName ) + 1 ];
						if( lpchNewString )
						{
							_tcscpy( lpchNewString, lpName );
							lpchOldString = pIOEntryArray[ i ]->strIOEntryName;
							pIOEntryArray[ i ]->strIOEntryName = lpchNewString;
							if( lpchOldString )
								delete [] lpchOldString;
						}
						delete [] lpName;
					}
				}
			}

		ReleaseMutex( hProtectionMutex );
	}
}


BOOL SVIOEntryListClass::Remove( SVIOEntryClass* PIOEntry )
{
	if( PIOEntry && IsIOControlValid && WaitForSingleObject( hProtectionMutex, dwWaitTime ) == WAIT_OBJECT_0 )
	{
		for( int i = lastValid; i >= 0; --i )
			if( pIOEntryArray[ i ] == PIOEntry )
			{
				// shift all PIOEntries above this entry one step down
				memmove( pIOEntryArray + i, pIOEntryArray + i + 1, sizeof( SVIOEntryClass* ) * ( lastValid - i ) );
				--lastValid;
				IsChanged = TRUE;

				ReleaseMutex( hProtectionMutex );
				return TRUE;
			}
	}
	return FALSE;
}

void SVIOEntryListClass::RemoveAll( BOOL AutoDelete )
{
	if( IsIOControlValid && WaitForSingleObject( hProtectionMutex, dwWaitTime ) == WAIT_OBJECT_0 )
	{
		if( AutoDelete )
		{
			for( int i = lastValid; i >= 0; --i )
				if( pIOEntryArray[ i ] )
					delete pIOEntryArray[ i ];
		}
		lastValid = -1;
		
		ReleaseMutex( hProtectionMutex );
	}
}


BOOL SVIOEntryListClass::ResetAll()
{
	BOOL RetVal = FALSE;
	if( IsIOControlValid && WaitForSingleObject( hProtectionMutex, dwWaitTime ) == WAIT_OBJECT_0 )
	{
		for( int i = lastValid; i >= 0; --i )
			if( pIOEntryArray[ i ] )
				RetVal = pIOEntryArray[ i ]->ResetIOEntry() || RetVal;
			else
				RetVal = FALSE;

		ReleaseMutex( hProtectionMutex );
	}
	return RetVal;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVIO.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 11:06:00   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.0.1.3.1.9   09 Jul 2012 14:03:52   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to change the include for SVIM State.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.0.1.3.1.8   19 Jun 2012 12:50:18   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  744
   SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Optimized the application state functionality.  The application state class was altered to simplify its usage and elements were added to object manager to separate tracking states.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.0.1.3.1.7   27 Jan 2011 11:20:54   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  712
   SCR Title:  Fix issues with black images when using command interface (SIAC)
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to fix issues with Matrox Buffer Management.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.0.1.3.1.6   08 Dec 2010 12:51:58   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  707
   SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to include changes in notification functionality using the Observer Design Pattern.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.0.1.3.1.5   16 Dec 2009 11:08:04   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  677
   SCR Title:  Fix problem in camera notify thread
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Fix issues with includes and comments.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.0.1.3.1.4   17 Feb 2005 08:25:34   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  440
   SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     updated header files
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.0.1.3.1.3   22 Apr 2003 10:31:08   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  346
   SCR Title:  Update SVObserver to Version 4.21 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.0.1.3.1.2   19 Nov 2002 14:11:28   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  226
   SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Destructor
         Removed IsIOControlValid check
   
   Cosmetic change: TheObjectManager -> TheSVObjectManager
   
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.0.1.3.1.1   12 Jun 2001 15:07:20   ed
   Project:  SVObserver
   Change Request (SCR) nbr:  200
   SCR Title:  Update code to implement SVIM COM Server
   Checked in by:  Ed;  Ed Chobanoff
   Change Description:  
     Added  SVIOEntryClass & SVIOEntryListClass
   
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.0.1.3.1.0   10 May 2001 15:37:10   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  197
   SCR Title:  Fix lock-ups in application when loading configuration from SVFocusNT
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
     Changed SVObserver state code in:
   CreateIOEntry
   DestroyIOEntry
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.0.1.3   Jan 19 2000 15:05:02   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  89
   SCR Title:  Outputs not set when going 'OnLine'
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Added source code headers per software procedures.
   
   /////////////////////////////////////////////////////////////////////////////////////
*/
