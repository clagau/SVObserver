//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVRemoteFontManager
//* .File Name       : $Workfile:   SVRemoteFontManager.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   23 Jul 2013 18:04:40  $
//******************************************************************************

#ifndef SVREMOTEFONTMANAGER_H
#define SVREMOTEFONTMANAGER_H

#include <map>
#include "SVContainerLibrary/SVVector.h"
#include "SVMatroxLibrary/SVMatroxLibrary.h"
#include "SVTimerLibrary/SVClock.h"

class SVRemoteFontManager  
{
public:
	SVRemoteFontManager();
	virtual ~SVRemoteFontManager();

	bool Startup();
	bool Shutdown();
	
	bool CanGoOnline();
	bool GoOnline();
	bool GoOffline();
	
	bool AddFont( long &lIdentifier );
	bool RemoveFont( long lIdentifier );
	bool IsValidFont( long lIdentifier, SVMatroxOcr &lHandle );
	
	bool UpdateFontTime( long lIdentifier );
	bool UpdateFontHandle( long lIdentifier, SVMatroxOcr lHandle );

	bool AddFontImage( long lIdentifier, SVMatroxBuffer& lHandle );
	bool GetFontImage( long lIdentifier, SVMatroxBuffer& lHandle );

	bool CopyFont( SVMatroxOcr& p_CurrentFont, SVMatroxOcr& p_NewFont, long lNumberOfNewChars );

	bool GetFontCharImage( long p_lFontIdentifier, SVMatroxOcr p_FontHandle, long p_lID, SVMatroxBuffer& CharHandle );
	bool CreateCharMapping(long lCurrentFont, CString p_sChars);

	bool AddFontChar(long lCurrentFont, char p_cNewChar, long *ulCharId);

	bool DeleteFontChar(long lIdentifier, SVMatroxOcr lCurrentFont, unsigned long ulCharId);


	bool GetCharIdSafeArray(long lCurrentFont, SAFEARRAY **psaCharIds);
	bool UpdateCharMapFontId(long CurrentFont, long NewFont);
	
protected:
	bool GetStatusHandles( unsigned long &rulSize, HANDLE **pphArray );
	
	bool RemoveFontsOlderThan( SVClock::SVTimeStamp p_Age );
	
private:
	bool m_bIsOnline;
	
	HANDLE m_hThread;
	HANDLE m_hShutdown;
	
	bool m_bLockCreated;
	CRITICAL_SECTION m_csLock;	
	 
	typedef std::map<unsigned long, char> CharMap;	
	
	SVVector< long > m_arRemoteFontIdentifiers;
	SVVector< SVMatroxOcr > m_arRemoteFontHandles;
	SVVector< SVClock::SVTimeStamp > m_arRemoteFontTimestamps;
	SVVector< SVMatroxBuffer > m_arRemoteFontImageHandles;
	SVVector< long, const unsigned long& > m_arLastCharIds;

	typedef std::map< long, CharMap > CharMappings;

	CharMappings m_mpCharMappings;

	unsigned long m_ulLastCharId;
	
	static DWORD WINAPI SVRemoteFontCleanupThread( LPVOID lpParam );
};

#endif

