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
#pragma region Includes
#include "stdafx.h"
#include "SVRemoteFontManager.h"
#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVImageLibrary/SVImageBufferHandleInterface.h"
#include "SVImageLibrary/SVImageInfoClass.h"
#include "SVTimerLibrary/SVClock.h"
#include "SVOCore/SVImageProcessingClass.h"
#include "SVObserver.h"
#include "SVSystemLibrary/SVThreadManager.h"

#pragma endregion Includes

DWORD WINAPI SVRemoteFontManager::SVRemoteFontCleanupThread( LPVOID lpParam )
{
	unsigned long l_ulSize;
	HANDLE* l_phArray = nullptr;

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
		l_phArray[l] = nullptr;
	}

	if ( nullptr != l_phArray )
	{
		delete[] l_phArray;
		l_phArray = nullptr;
	}
	
	return 0L;
}// end SVRemoteFontCleanupThread

SVRemoteFontManager::SVRemoteFontManager()
{
	m_hShutdown = nullptr;
	m_hThread = nullptr;

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

		m_hShutdown = ::CreateEvent( nullptr, true, false, nullptr );
		m_hThread = ::CreateThread( nullptr, 0, SVRemoteFontCleanupThread, this, 0, nullptr );
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

		if ( nullptr != m_hShutdown )
		{
			::SetEvent( m_hShutdown );
			::CloseHandle( m_hShutdown );
			m_hShutdown = nullptr;
		}

		if ( nullptr != m_hThread )
		{
			::CloseHandle( m_hThread );
			SVThreadManager::Instance().Remove( m_hThread );
			m_hThread = nullptr;
		}

		long lSize = static_cast<long> (m_arRemoteFontTimestamps.size());
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

			m_arRemoteFontIdentifiers.erase(m_arRemoteFontIdentifiers.begin() + l);
			SVMatroxOcrInterface::Destroy( m_arRemoteFontHandles[l] );
			m_arRemoteFontTimestamps.erase(m_arRemoteFontTimestamps.begin() + l);
			m_arRemoteFontHandles.erase(m_arRemoteFontHandles.begin() + l);

		}// end for

		//clean up Font Image Handles.
		m_arRemoteFontImageHandles.clear();

		::LeaveCriticalSection( &m_csLock );
	}

	return l_bOk;
}

bool SVRemoteFontManager::CanGoOnline()
{
	bool bRet = nullptr != m_hShutdown && m_bLockCreated;
	
	return bRet;
}// end CanGoOnline

bool SVRemoteFontManager::GoOnline()
{
	bool bRet = nullptr != m_hShutdown && m_bLockCreated;
	
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
	
	if ( nullptr != m_hShutdown && m_bLockCreated )
	{
		::EnterCriticalSection( &m_csLock );

		DWORD dwTick = ::GetTickCount();
		m_arRemoteFontIdentifiers.push_back( dwTick );

		
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

		HRESULT MatroxCode = SVMatroxOcrInterface::Create( milHandle, l_Create );
		if( S_OK == MatroxCode )
		{
			m_arRemoteFontHandles.push_back( milHandle );
			
			m_arRemoteFontTimestamps.push_back( SvTl::GetTimeStamp() );
			
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
	
	if ( nullptr != m_hShutdown && m_bLockCreated )
	{
		::EnterCriticalSection( &m_csLock );

		long lSize = static_cast<long> (m_arRemoteFontTimestamps.size());
		for( long l = lSize - 1L; l >= 0L; l-- )
		{
			if( lIdentifier == m_arRemoteFontIdentifiers[l] )
			{
				
				m_arRemoteFontIdentifiers.erase(m_arRemoteFontIdentifiers.begin() + l);
				SVMatroxOcrInterface::Destroy( m_arRemoteFontHandles[l] );
				m_arRemoteFontTimestamps.erase(m_arRemoteFontTimestamps.begin() + l);
				m_arRemoteFontHandles.erase(m_arRemoteFontHandles.begin() + l);
				
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
	
	if ( nullptr != m_hShutdown && m_bLockCreated )
	{
		::EnterCriticalSection( &m_csLock );

		long lSize = static_cast<long> (m_arRemoteFontTimestamps.size());
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
	
	if ( nullptr != m_hShutdown && m_bLockCreated )
	{
		::EnterCriticalSection( &m_csLock );

		long lSize = static_cast<long> (m_arRemoteFontTimestamps.size());
		for( long l = lSize - 1L; l >= 0L; l-- )
		{
			if( lIdentifier == m_arRemoteFontIdentifiers[l] )
			{
				SvTl::SVTimeStamp tsNow = SvTl::GetTimeStamp();
				m_arRemoteFontTimestamps[l] = tsNow;
				
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
	
	if ( nullptr != m_hShutdown && m_bLockCreated )
	{
		::EnterCriticalSection( &m_csLock );

		long lSize = static_cast<long> (m_arRemoteFontTimestamps.size());
		for( long l = lSize - 1L; l >= 0L; l-- )
		{
			if( lIdentifier == m_arRemoteFontIdentifiers[l] )
			{
				m_arRemoteFontHandles[l] = lHandle;
				
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

	if ( nullptr != p_pphArray )
	{
		*p_pphArray = nullptr;

		if ( nullptr != m_hShutdown )
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
					*p_pphArray = nullptr;
				}
			}
		}
	}

	return l_bOk;
}

bool SVRemoteFontManager::RemoveFontsOlderThan( SvTl::SVTimeStamp p_Age )
{
	bool bRet = false;

	if ( nullptr != m_hShutdown && m_bLockCreated && 
	     ::TryEnterCriticalSection( &m_csLock ) )
	{
		SvTl::SVTimeStamp tsNow = SvTl::GetTimeStamp();
		
		long lSize = static_cast<long> (m_arRemoteFontTimestamps.size());
		for( long l = lSize - 1L; l >= 0L; l-- )
		{
			SvTl::SVTimeStamp tsThen = m_arRemoteFontTimestamps[l];
			SvTl::SVTimeStamp tsDiff = tsNow - tsThen;
			if( tsDiff > p_Age )
			{
				long l_lIdentifier = m_arRemoteFontIdentifiers[l];
				m_arRemoteFontIdentifiers.erase(m_arRemoteFontIdentifiers.begin() + l);
				SVMatroxOcrInterface::Destroy( m_arRemoteFontHandles[l] );
				m_arRemoteFontTimestamps.erase(m_arRemoteFontTimestamps.begin() + l);
				m_arRemoteFontHandles.erase(m_arRemoteFontHandles.begin() + l);

				// Remove Font Image Handles
				if( l < static_cast< long >( m_arRemoteFontImageHandles.size() ) )
				{
					m_arRemoteFontImageHandles.erase(m_arRemoteFontImageHandles.begin() + l);
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
	if ( nullptr != m_hShutdown && m_bLockCreated )
	{
		::EnterCriticalSection( &m_csLock );

		long lSize = static_cast<long> (m_arRemoteFontTimestamps.size());
		for( long l = lSize - 1L; l >= 0L; l-- )
		{
			if( lIdentifier == m_arRemoteFontIdentifiers[l] )
			{
				long lFSize = static_cast<long> (m_arRemoteFontImageHandles.size());
				if ( lFSize <= l )
				{
					m_arRemoteFontImageHandles.resize(l+1);
				}
				m_arRemoteFontImageHandles[l] = lHandle;

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

	if ( nullptr != m_hShutdown && m_bLockCreated )
	{
		::EnterCriticalSection( &m_csLock );

		long lSize = static_cast<long> (m_arRemoteFontTimestamps.size());
		for( long l = lSize - 1L; l >= 0L; l-- )
		{
			if( lIdentifier == m_arRemoteFontIdentifiers[l] )
			{
				if( l < static_cast<long> (m_arRemoteFontImageHandles.size()) )
				{
					lHandle = m_arRemoteFontImageHandles[l];
					
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
	
	HRESULT l_Code;
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
	std::string l_strFontChars;
	l_strFontChars.resize( lCharInCurrFont + 1);
	SVMatroxOcrInterface::Get( p_CurrentFont, SVCharInFont, l_strFontChars );
	
	for(long i=0; i<lCharInCurrFont; i++)
	{
		
		HRESULT l_Code;
		SVMatroxBufferCreateStruct l_Create;
		

		l_Create.m_eAttribute = SVBufAttImageProc;
		l_Create.SetImageDepth( 8 );
		l_Create.m_lSizeX = (long)dCharBoxSizeX;
		l_Create.m_lSizeY = (long)dCharBoxSizeY;


		std::string Str;

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

	ImageInfo.SetImageProperty(SvDef::SVImagePropertyEnum::SVImagePropertyBandNumber,1 );
	ImageInfo.SetImageProperty(SvDef::SVImagePropertyEnum::SVImagePropertyPixelDepth,  SV8BitUnsigned );

	hrRet = SVImageProcessingClass::CreateImageBuffer( ImageInfo, ImageBufferHandle );

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
				std::string Str;
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
	long lSize = static_cast<long> (m_arRemoteFontTimestamps.size());
	for ( long l = lSize - 1L; l >= 0L; l-- )
	{
		if ( lCurrentFont == m_arRemoteFontIdentifiers[l] )
		{
			long CSize = static_cast<long> (m_arLastCharIds.size());
			if ( CSize > l )
			{
				m_arLastCharIds[l] = l_ulCount;
			}
			else
			{
				m_arLastCharIds.push_back(l_ulCount);
			}
		}
	}



	return bRet;
}

bool SVRemoteFontManager::AddFontChar(long lCurrentFont, char p_cNewChar, long *ulCharId)
{
	bool bRet = true;

	unsigned long LastCharID{ 0 };

	if ( nullptr != m_hShutdown && m_bLockCreated )
	{
		::EnterCriticalSection( &m_csLock );

		long lSize = static_cast<long> (m_arRemoteFontTimestamps.size());
		for( long l = lSize - 1L; l >= 0L; l-- )
		{
			if( lCurrentFont == m_arRemoteFontIdentifiers[l] )
			{
				LastCharID = m_arLastCharIds[l];
				LastCharID++;
				m_arLastCharIds[l] = LastCharID;

				CharMap l_Map;
				CharMappings::iterator CharIterator;
				CharIterator = m_mpCharMappings.find(lCurrentFont);
				if ( CharIterator->first == lCurrentFont )
				{
					l_Map = CharIterator->second;
					l_Map[LastCharID] = p_cNewChar;

					*ulCharId = (long)LastCharID;

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
			
			HRESULT l_Code;
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

