//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVRemoteFontManager
//* .File Name       : $Workfile:   SVRemoteFontManager.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   01 Dec 2014 13:22:50  $
//******************************************************************************

#include "stdafx.h"
#include "SVRemoteFontManager.h"
#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVImageLibrary/SVImageBufferHandleInterface.h"
#include "SVImageLibrary/SVImageInfoClass.h"
#include "SVTimerLibrary/SVClock.h"
#include "SVImageProcessingClass.h"
#include "SVObserver.h"

DWORD WINAPI SVRemoteFontManager::SVRemoteFontCleanupThread( LPVOID lpParam )
{
	unsigned long l_ulSize;
	HANDLE *l_phArray = NULL;

	SVRemoteFontManager *l_pManager = (SVRemoteFontManager*) lpParam;
	
	try
	{
		l_pManager->GetStatusHandles( l_ulSize, &l_phArray );
	}
	catch ( ... )
	{
	}

	if ( 0 < l_ulSize )
	{
		while ( ::WaitForMultipleObjects( l_ulSize, l_phArray, FALSE, 300000L ) == WAIT_TIMEOUT )
		{
			try
			{
				l_pManager->RemoveFontsOlderThan( 3600L );
			}
			catch ( ... )
			{
				break;
			}
		}
	}

	for ( unsigned long l = 0; l < l_ulSize; l++ )
	{
		::CloseHandle( l_phArray[l] );
		l_phArray[l] = NULL;
	}

	if ( l_phArray != NULL )
	{
		delete[] l_phArray;
		l_phArray = NULL;
	}
	
	return 0L;
}// end SVRemoteFontCleanupThread

SVRemoteFontManager::SVRemoteFontManager()
{
	m_hShutdown = NULL;
	m_hThread = NULL;

	m_bIsOnline = false;
	m_bLockCreated = false;
	
	try
	{
		::InitializeCriticalSection( &m_csLock );

		m_bLockCreated = true;
	}
	catch ( ... )
	{
	}

}// end ctor

SVRemoteFontManager::~SVRemoteFontManager()
{
	if ( m_bLockCreated )
	{
		::DeleteCriticalSection( &m_csLock );
	}
}// end dtor

bool SVRemoteFontManager::Startup()
{
	bool l_bOk = Shutdown();

	if ( m_bLockCreated )
	{
		::EnterCriticalSection( &m_csLock );

		m_hShutdown = ::CreateEvent( NULL, TRUE, FALSE, NULL );
		m_hThread = ::CreateThread( NULL, 0, SVRemoteFontCleanupThread, this, 0, NULL );
		SVThreadManager::Instance().Add( m_hThread, "SVFontManager");
		::LeaveCriticalSection( &m_csLock );
	}

	return l_bOk;
}

bool SVRemoteFontManager::Shutdown()
{
	bool l_bOk = m_bLockCreated;

	m_bIsOnline = false;

	if ( m_bLockCreated )
	{
		::EnterCriticalSection( &m_csLock );

		if ( m_hShutdown != NULL )
		{
			::SetEvent( m_hShutdown );
			::CloseHandle( m_hShutdown );
			m_hShutdown = NULL;
		}

		if ( m_hThread != NULL )
		{
			::CloseHandle( m_hThread );
			SVThreadManager::Instance().Remove( m_hThread );
			m_hThread = NULL;
		}

		long lSize = m_arRemoteFontTimestamps.GetSize();
		for( long l = lSize - 1L; l >= 0L; l-- )
		{
			long l_lIdentifier = m_arRemoteFontIdentifiers[l];
			// remove unused character map
			CharMappings::iterator l_it;
			l_it = m_mpCharMappings.find( l_lIdentifier );
			if( l_it != m_mpCharMappings.end() )
			{
				m_mpCharMappings.erase( l_it );
			}

			m_arRemoteFontIdentifiers.RemoveAt( l );
			SVMatroxOcrInterface::Destroy( m_arRemoteFontHandles[l] );
			m_arRemoteFontTimestamps.RemoveAt( l );
			m_arRemoteFontHandles.RemoveAt( l );

		}// end for

		//clean up Font Image Handles.
		m_arRemoteFontImageHandles.clear();

		::LeaveCriticalSection( &m_csLock );
	}

	return l_bOk;
}

bool SVRemoteFontManager::CanGoOnline()
{
	bool bRet = m_hShutdown != NULL && m_bLockCreated;
	
	return bRet;
}// end CanGoOnline

bool SVRemoteFontManager::GoOnline()
{
	bool bRet = m_hShutdown != NULL && m_bLockCreated;
	
	if ( bRet )
	{
		m_bIsOnline = true;
	}
	
	return bRet;
}// end GoOnline

bool SVRemoteFontManager::GoOffline()
{
	bool bRet = true;
	
	m_bIsOnline = false;
	
	return bRet;
}// end GoOffline

bool SVRemoteFontManager::AddFont( long &lIdentifier )
{
	bool bRet = false;
	
	if ( m_hShutdown != NULL && m_bLockCreated )
	{
		::EnterCriticalSection( &m_csLock );

		DWORD dwTick = ::GetTickCount();
		m_arRemoteFontIdentifiers.Add( dwTick );

		
		SVMatroxOcrInterface::SVStatusCode l_Code;
		SVMatroxOcr milHandle;
		SVMatroxOcrCreateStruct l_Create;

		l_Create.m_eFontType = SVUserDefinedConstrained;
		l_Create.m_eInitFlag = SVOcrForegroundBlack;
		l_Create.m_lCharCellSizeX = 20;
		l_Create.m_lCharCellSizeY = 20;
		l_Create.m_lCharNumber = 5;
		l_Create.m_lCharOffsetX = 2;
		l_Create.m_lCharOffsetY = 2;
		l_Create.m_lCharSizeX = 16;
		l_Create.m_lCharSizeY = 16;
		l_Create.m_lCharThickness = 3;
		l_Create.m_lStringLength = 10;

		l_Code = SVMatroxOcrInterface::Create( milHandle, l_Create );
		if( l_Code == SVMEE_STATUS_OK )
		{
			m_arRemoteFontHandles.Add( milHandle );
			
			SVClock::SVTimeStamp tsNow = SVClock::GetTimeStamp();
			m_arRemoteFontTimestamps.Add( tsNow );
			
			lIdentifier = dwTick;
			bRet = true;

		}
		::LeaveCriticalSection( &m_csLock );
	}
	
	return bRet;
}// end AddFont

bool SVRemoteFontManager::RemoveFont( long lIdentifier )
{
	bool bRet = false;
	
	if ( m_hShutdown != NULL && m_bLockCreated )
	{
		::EnterCriticalSection( &m_csLock );

		long lSize = m_arRemoteFontTimestamps.GetSize();
		for( long l = lSize - 1L; l >= 0L; l-- )
		{
			if( lIdentifier == m_arRemoteFontIdentifiers[l] )
			{
				
				m_arRemoteFontIdentifiers.RemoveAt( l );
				SVMatroxOcrInterface::Destroy( m_arRemoteFontHandles[l] );
				m_arRemoteFontTimestamps.RemoveAt( l );
				m_arRemoteFontHandles.RemoveAt( l );
				
				bRet = true;
				break;
			}// end if
			
		}// end for

		::LeaveCriticalSection( &m_csLock );
	}
	
	return bRet;
}// end RemoveFont

bool SVRemoteFontManager::IsValidFont( long lIdentifier, SVMatroxOcr &lHandle )
{
	bool bRet = false;
	
	if ( m_hShutdown != NULL && m_bLockCreated )
	{
		::EnterCriticalSection( &m_csLock );

		long lSize = m_arRemoteFontTimestamps.GetSize();
		for( long l = lSize - 1L; l >= 0L; l-- )
		{
			if( lIdentifier == m_arRemoteFontIdentifiers[l] )
			{			
				lHandle = m_arRemoteFontHandles[l];
				bRet = true;
				break;
			}// end if
			
		}// end for

		::LeaveCriticalSection( &m_csLock );
	}
	
	return bRet;
}// end IsValidFont

bool SVRemoteFontManager::UpdateFontTime( long lIdentifier )
{
	bool bRet = false;
	
	if ( m_hShutdown != NULL && m_bLockCreated )
	{
		::EnterCriticalSection( &m_csLock );

		long lSize = m_arRemoteFontTimestamps.GetSize();
		for( long l = lSize - 1L; l >= 0L; l-- )
		{
			if( lIdentifier == m_arRemoteFontIdentifiers[l] )
			{
				SVClock::SVTimeStamp tsNow = SVClock::GetTimeStamp();
				m_arRemoteFontTimestamps.SetAt( l, tsNow );
				
				bRet = true;
				break;
			}// end if
			
		}// end for

		::LeaveCriticalSection( &m_csLock );
	}
	
	return bRet;
}// end UpdateFontTime

bool SVRemoteFontManager::UpdateFontHandle( long lIdentifier, SVMatroxOcr lHandle )
{
	bool bRet = false;
	
	if ( m_hShutdown != NULL && m_bLockCreated )
	{
		::EnterCriticalSection( &m_csLock );

		long lSize = m_arRemoteFontTimestamps.GetSize();
		for( long l = lSize - 1L; l >= 0L; l-- )
		{
			if( lIdentifier == m_arRemoteFontIdentifiers[l] )
			{
				m_arRemoteFontHandles.SetAt( l, lHandle );
				
				bRet = true;
				break;
			}// end if
			
		}// end for

		::LeaveCriticalSection( &m_csLock );
	}
	
	return bRet;
}// end UpdateFontHandle

bool SVRemoteFontManager::GetStatusHandles( unsigned long &p_rulSize, HANDLE **p_pphArray )
{
	bool l_bOk = false;

	p_rulSize = 0;

	if ( p_pphArray != NULL )
	{
		*p_pphArray = NULL;

		if ( m_hShutdown != NULL )
		{
			p_rulSize = 1;

			*p_pphArray = new HANDLE[p_rulSize];
			if ( *p_pphArray )
			{
				
				l_bOk = DuplicateHandle( GetCurrentProcess(), m_hShutdown,
																 GetCurrentProcess(), (*p_pphArray) + 0,
																 0, FALSE, DUPLICATE_SAME_ACCESS ) != FALSE;
				if ( ! l_bOk )
				{
					delete[] *p_pphArray;
					*p_pphArray = NULL;
				}
			}
		}
	}

	return l_bOk;
}

bool SVRemoteFontManager::RemoveFontsOlderThan( SVClock::SVTimeStamp p_Age )
{
	bool bRet = false;

	if ( m_hShutdown != NULL && m_bLockCreated && 
	     ::TryEnterCriticalSection( &m_csLock ) )
	{
		SVClock::SVTimeStamp tsNow = SVClock::GetTimeStamp();
		
		long lSize = m_arRemoteFontTimestamps.GetSize();
		for( long l = lSize - 1L; l >= 0L; l-- )
		{
			SVClock::SVTimeStamp tsThen = m_arRemoteFontTimestamps[l];
			SVClock::SVTimeStamp tsDiff = tsNow - tsThen;
			if( tsDiff > p_Age )
			{
				long l_lIdentifier = m_arRemoteFontIdentifiers[l];
				m_arRemoteFontIdentifiers.RemoveAt( l );
				SVMatroxOcrInterface::Destroy( m_arRemoteFontHandles[l] );
				m_arRemoteFontTimestamps.RemoveAt( l );
				m_arRemoteFontHandles.RemoveAt( l );

				// Remove Font Image Handles
				if( l < static_cast< long >( m_arRemoteFontImageHandles.size() ) )
				{
					m_arRemoteFontImageHandles.RemoveAt( l );
				}

				// remove unused character map
				CharMappings::iterator l_it;
				l_it = m_mpCharMappings.find( l_lIdentifier );
				if( l_it != m_mpCharMappings.end() )
				{
					m_mpCharMappings.erase( l_it );
				}
			}// end if
		}// end for
		bRet = true;

		::LeaveCriticalSection( &m_csLock );
	}
	return bRet;
}

bool SVRemoteFontManager::AddFontImage(long lIdentifier, SVMatroxBuffer& lHandle)
{
	bool bRet = false;
	if ( m_hShutdown != NULL && m_bLockCreated )
	{
		::EnterCriticalSection( &m_csLock );

		long lSize = m_arRemoteFontTimestamps.GetSize();
		for( long l = lSize - 1L; l >= 0L; l-- )
		{
			if( lIdentifier == m_arRemoteFontIdentifiers[l] )
			{
				long lFSize = m_arRemoteFontImageHandles.GetSize();
				if ( lFSize > l )
				{
					m_arRemoteFontImageHandles.SetAt( l, lHandle );
				}
				else
				{
					m_arRemoteFontImageHandles.SetAtGrow( l, lHandle );
				}
				
				bRet = true;
				break;
			}// end if
			
		}// end for

		::LeaveCriticalSection( &m_csLock );
	}


	return bRet;
}

bool SVRemoteFontManager::GetFontImage( long lIdentifier, SVMatroxBuffer &lHandle )
{
	bool bRet = false;

	if ( m_hShutdown != NULL && m_bLockCreated )
	{
		::EnterCriticalSection( &m_csLock );

		long lSize = m_arRemoteFontTimestamps.GetSize();
		for( long l = lSize - 1L; l >= 0L; l-- )
		{
			if( lIdentifier == m_arRemoteFontIdentifiers[l] )
			{
				if( l < m_arRemoteFontImageHandles.GetSize() )
				{
					lHandle = m_arRemoteFontImageHandles.GetAt(l);
					
					bRet = true;
					break;
				}
			}// end if
			
		}// end for

		::LeaveCriticalSection( &m_csLock );
	}


	return bRet;
}

//
// Return codes not implemented correctly.
// What should happen if copy fails?
//
bool SVRemoteFontManager::CopyFont( SVMatroxOcr& p_CurrentFont, SVMatroxOcr& p_NewFont, long lNumberOfNewChars )
{
	bool bRet = true;

	long lFontType;
	long lFontMethod;
	long lForeground;
	double dCharBoxSizeX;
	double dCharBoxSizeY;
	double dCharThickness;
	double dCharSizeX;
	double dCharSizeY;
	double dCharOffsetX;
	double dCharOffsetY;
	long lCharInCurrFont;
	long lMaxStringLength;
	
	SVMatroxOcrInterface::SVStatusCode l_Code;
	SVMatroxOcrCreateStruct l_Create;

	l_Code = SVMatroxOcrInterface::Get( p_CurrentFont, SVCharNumberInFont, lCharInCurrFont );

	l_Code = SVMatroxOcrInterface::Get( p_CurrentFont, SVFontType, lFontType );

	l_Code = SVMatroxOcrInterface::Get( p_CurrentFont, SVContext, lFontMethod );

	l_Code = SVMatroxOcrInterface::Get( p_CurrentFont, SVOcrForegroundValue, lForeground );

	l_Code = SVMatroxOcrInterface::Get( p_CurrentFont, SVOcrStringSizeMax, lMaxStringLength );

	l_Code = SVMatroxOcrInterface::Get( p_CurrentFont, SVCharCellSizeX, dCharBoxSizeX );

	l_Code = SVMatroxOcrInterface::Get( p_CurrentFont, SVCharCellSizeY, dCharBoxSizeY );

	l_Code = SVMatroxOcrInterface::Get( p_CurrentFont, SVCharThickness, dCharThickness );

	l_Code = SVMatroxOcrInterface::Get( p_CurrentFont, SVOcrCharSizeX, dCharSizeX );
  
	l_Code = SVMatroxOcrInterface::Get( p_CurrentFont, SVOcrCharSizeY, dCharSizeY );
  
	l_Code = SVMatroxOcrInterface::Get( p_CurrentFont, SVCharOffsetX, dCharOffsetX );
  
	l_Code = SVMatroxOcrInterface::Get( p_CurrentFont, SVCharOffsetY, dCharOffsetY );

	long l_CharCount = lCharInCurrFont + lNumberOfNewChars;

	l_Create.m_eFontType = static_cast<SVOcrTypeEnum>(lFontType | lFontMethod);
	l_Create.m_eInitFlag = static_cast<SVOcrInitFlag>(lForeground);
	l_Create.m_lCharCellSizeX = static_cast<long>(dCharBoxSizeX);
	l_Create.m_lCharCellSizeY = static_cast<long>(dCharBoxSizeY);
	l_Create.m_lCharNumber = ( l_CharCount < 2 ) ? 2 : l_CharCount;
	l_Create.m_lCharOffsetX = static_cast<long>(dCharOffsetX);
	l_Create.m_lCharOffsetY = static_cast<long>(dCharOffsetY);
	l_Create.m_lCharSizeX = static_cast<long>(dCharSizeX);
	l_Create.m_lCharSizeY = static_cast<long>(dCharSizeY);
	l_Create.m_lCharThickness = static_cast<long>(dCharThickness);
	l_Create.m_lStringLength = lMaxStringLength;

	l_Code = SVMatroxOcrInterface::Create(p_NewFont, l_Create );


					//get list of char's that are in current font
	SVMatroxString l_strFontChars;
	l_strFontChars.resize( lCharInCurrFont + 1);
	SVMatroxOcrInterface::Get( p_CurrentFont, SVCharInFont, l_strFontChars );
	
	for(long i=0; i<lCharInCurrFont; i++)
	{
		
		SVMatroxBufferInterface::SVStatusCode l_Code;
		SVMatroxBufferCreateStruct l_Create;
		

		l_Create.m_eAttribute = SVBufAttImageProc;
		l_Create.SetImageDepth( 8 );
		l_Create.m_lSizeX = (long)dCharBoxSizeX;
		l_Create.m_lSizeY = (long)dCharBoxSizeY;


		SVMatroxString Str;

		Str = l_strFontChars[i];

		SVMatroxBuffer NewBuf;
		l_Code = SVMatroxBufferInterface::Create( NewBuf, l_Create );

		l_Code = SVMatroxOcrInterface::CopyFont( p_CurrentFont, NewBuf, SVOcrCopyFromFont, Str );

		l_Code = SVMatroxOcrInterface::CopyFont( p_NewFont, NewBuf, SVOcrCopytoFont, Str );

		NewBuf.clear();
	}


	return bRet;
}

bool SVRemoteFontManager::GetFontCharImage( long p_lFontIdentifier, SVMatroxOcr p_FontHandle, long p_lID, SVMatroxBuffer& p_CharHandle )
{
	bool bRet = false;
	HRESULT hrRet = S_FALSE;

	double dCharBoxSizeX;
	double dCharBoxSizeY;
	

	SVImageInfoClass ImageInfo;
	SVSmartHandlePointer ImageBufferHandle;

	SVMatroxOcrInterface::Get( p_FontHandle, SVCharCellSizeX, dCharBoxSizeX);

	SVMatroxOcrInterface::Get( p_FontHandle, SVCharCellSizeY, dCharBoxSizeY);


	ImageInfo.SetExtentProperty(SVExtentPropertyOutputPositionPoint, 0);
	ImageInfo.SetExtentProperty(SVExtentPropertyWidth, dCharBoxSizeX );
	ImageInfo.SetExtentProperty(SVExtentPropertyHeight, dCharBoxSizeY );

	ImageInfo.SetImageProperty(SVImagePropertyBandNumber,1 );
	ImageInfo.SetImageProperty(SVImagePropertyPixelDepth,  SV8BitUnsigned );

	hrRet = SVImageProcessingClass::Instance().CreateImageBuffer( ImageInfo, ImageBufferHandle );

	if( !( ImageBufferHandle.empty() ) )
	{
		SVImageBufferHandleImage l_MilHandle;
		ImageBufferHandle->GetData( l_MilHandle );

		p_CharHandle = l_MilHandle.GetBuffer();

		// Find the current Font
		CharMappings::iterator l_it;
		l_it = m_mpCharMappings.find( p_lFontIdentifier );

		if( l_it != m_mpCharMappings.end() )
		{
			CharMap& l_cMap = m_mpCharMappings[p_lFontIdentifier];
			CharMap::const_iterator l_charMapIt;
			l_charMapIt = l_cMap.find(p_lID);

			if( l_charMapIt != l_cMap.end() )
			{
				char cLabel = l_cMap[p_lID];
				SVMatroxString Str;
				Str = cLabel;


				if ( !p_CharHandle.empty() )
				{
					SVMatroxOcrInterface::CopyFont( p_FontHandle, p_CharHandle, SVOcrCopyFromFont, Str );
					bRet = true;
				}
			}
		}
	}


	return bRet;
}

bool SVRemoteFontManager::CreateCharMapping(long lCurrentFont, CString p_sChars)
{
	bool bRet = true;
	unsigned long l_ulCount = 0;

	CharMappings::iterator charIterator;

	int iMapSize = static_cast<int>(m_mpCharMappings.size());

	int iSize = p_sChars.GetLength();

	CharMap l_Map;
	BOOL l_bFillMap = FALSE;

	for (int i = 0; i < iSize; i++ )
	{
		l_Map[i+1] = p_sChars.GetAt(i);
	}
	l_ulCount = iSize;


	if ( iMapSize > 0 )
	{
		//there are elements in the map.  get the correct map by the unique font ID
		charIterator = m_mpCharMappings.find(lCurrentFont);
		if ( charIterator != m_mpCharMappings.end() )
		{
			charIterator->second = l_Map;
		}
		else
		{
			m_mpCharMappings[lCurrentFont] = l_Map;
		}

	}
	else
	{
		//no elements in the map. 
		m_mpCharMappings[lCurrentFont] = l_Map;
	}


	// fill in the CharID Array
	long l_lSize = m_arRemoteFontTimestamps.GetSize();
	for ( long l = l_lSize - 1L; l >= 0L; l-- )
	{
		if ( lCurrentFont == m_arRemoteFontIdentifiers[l] )
		{
			long l_CSize = m_arLastCharIds.GetSize();
			if ( l_CSize > l )
			{
				m_arLastCharIds.SetAt(l,l_ulCount);
			}
			else
			{
				m_arLastCharIds.Add(l_ulCount);
			}
		}
	}



	return bRet;
}

bool SVRemoteFontManager::AddFontChar(long lCurrentFont, char p_cNewChar, long *ulCharId)
{
	bool bRet = true;

	unsigned long l_ulLastCharID;

	if ( m_hShutdown != NULL && m_bLockCreated )
	{
		::EnterCriticalSection( &m_csLock );

		long lSize = m_arRemoteFontTimestamps.GetSize();
		for( long l = lSize - 1L; l >= 0L; l-- )
		{
			if( lCurrentFont == m_arRemoteFontIdentifiers[l] )
			{
				int iLCI = m_arLastCharIds.GetSize();
				l_ulLastCharID = m_arLastCharIds.GetAt(l);
				l_ulLastCharID++;
				m_arLastCharIds.SetAt(l, l_ulLastCharID);

				CharMap l_Map;
				CharMappings::iterator CharIterator;
				CharIterator = m_mpCharMappings.find(lCurrentFont);
				if ( CharIterator->first == lCurrentFont )
				{
					l_Map = CharIterator->second;
					l_Map[l_ulLastCharID] = p_cNewChar;

					*ulCharId = (long)l_ulLastCharID;

					CharIterator->second = l_Map;
				}
				
				bRet = true;
				break;
			}// end if
			
		}// end for

		::LeaveCriticalSection( &m_csLock );
	}




	return bRet;
}

bool SVRemoteFontManager::GetCharIdSafeArray(long lCurrentFont, SAFEARRAY **psaCharIds)
{
	bool bRet = true;

	CharMap l_Map;
	CharMappings::iterator charIterator;

	CharMap::iterator mapIter;

	charIterator = m_mpCharMappings.find(lCurrentFont);
	if ( charIterator != m_mpCharMappings.end() )
	{
		l_Map = charIterator->second;
		long lMapSize = static_cast<long>(l_Map.size());

		long lIndex = 0;
		for ( mapIter = l_Map.begin(); mapIter != l_Map.end(); ++mapIter )
		{
			unsigned long ulVal = mapIter->first;
			char C = mapIter->second;
			::SafeArrayPutElement(*psaCharIds, &lIndex, &ulVal);
			lIndex++;
		}
	}

	return bRet;
}

bool SVRemoteFontManager::DeleteFontChar(long lIdentifier, SVMatroxOcr lCurrentFont, unsigned long ulCharId)
{
	bool bRet = true;
	CharMappings::iterator charIterator;
	CharMap::iterator cIter;
	int iSize = static_cast<int>(m_mpCharMappings.size());

	charIterator = m_mpCharMappings.find(lIdentifier);
	if ( charIterator != m_mpCharMappings.end() )
	{
		CharMap &l_Map = charIterator->second;
		cIter = l_Map.find(ulCharId);
		if ( cIter != l_Map.end() )
		{
			
			SVMatroxOcrInterface ::SVStatusCode l_Code;
			char cTmp;
			cTmp = cIter->second;
			l_Code = SVMatroxOcrInterface::Set( lCurrentFont, SVOcrCharErase, (double)cTmp);
			l_Map.erase(cIter);

		}
	}

	//MocrControl(lFontHandle, M_CHAR_ERASE, (double)sTmpStr.GetAt(i));

	return bRet;
}

bool SVRemoteFontManager::UpdateCharMapFontId(long lCurrentFont, long NewFont)
{
	bool bRet = true;
	CharMappings::iterator charIterator;
	charIterator = m_mpCharMappings.find(lCurrentFont);
	if ( charIterator != m_mpCharMappings.end() )
	{
		CharMap l_Map = charIterator->second;
		m_mpCharMappings.erase(charIterator);
		m_mpCharMappings[NewFont] = l_Map;
	}
	int iMapSize = static_cast<int>(m_mpCharMappings.size());

	return bRet;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVRemoteFontManager.cpp_v  $
 * 
 *    Rev 1.3   01 Dec 2014 13:22:50   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  960
 * SCR Title:  Pipe/core management
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Thread lables and attributes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   02 Oct 2013 07:58:44   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   08 May 2013 16:31:18   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 012.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   29 Apr 2013 14:27:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  830
 * SCR Title:  Consolidate all Time Stamp and Clock Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new SVClock::SVTimeStamp type for all time stamp variables.  Update to use new SVClock::GetTimeStamp() function to get the time stamp value in milliseconds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 14:46:58   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   04 Oct 2012 10:53:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Changed from post incrementor to pre incrementor.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   16 Mar 2011 13:51:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated extent functionality and renamed extent methods to correct extent issues and add clarity to the logical flow.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   10 Feb 2011 15:11:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to now use shared pointer instead of untracked structure.  This fixes destruction, tracking and rebuilding issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   27 Jan 2011 11:54:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   04 Nov 2010 14:11:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with image processing and display image processing classes and associated includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   15 Oct 2010 15:47:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  701
 * SCR Title:  Add New Font Functionality to the Existing Command Interface
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated font manager to fix attribute issue.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   16 Dec 2009 12:34:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   04 Dec 2009 09:04:34   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  676
 * SCR Title:  Fix bug in RemoteFontManager
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed Add to SetAtGrow in AddFontImage.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   18 Nov 2008 11:21:14   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  632
 * SCR Title:  Fix bug in Remote Font Manager
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fixed bugs related to Matrox conversion.
 * in CopyFont.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   24 Jul 2007 15:44:04   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Update MIL 8.0
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   15 Mar 2007 15:04:12   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  581
 * SCR Title:  Add additional ActiveX interfaces for management of Matrox fonts through SIAC
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Fixed index bug in AddFontImage and CreateCharMapping.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   13 Mar 2007 15:51:38   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  581
 * SCR Title:  Add additional ActiveX interfaces for management of Matrox fonts through SIAC
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Changed GetFontCharImage to use a long ID instead of a char.  Fixed bug in CreateCharMapping
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   06 Feb 2007 14:55:06   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  581
 * SCR Title:  Add additional ActiveX interfaces for management of Matrox fonts through SIAC
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added new methods to manage the Font Image and the adding and deleting of characters in the font
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   17 Feb 2005 15:25:30   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   27 May 2004 11:50:50   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  424
 * SCR Title:  Fix Locking and Clean-up Problems with Remote Font Manager for Matrox OCR/OCV
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated all methods to use new attributes for multi-threading and proper shutdown and clean-up procedure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   04 Feb 2004 14:18:54   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  403
 * SCR Title:  Add ActiveX interfaces to allow management of Matrox fonts through the SIAC
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Added new code to support new Matrox font setup
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
