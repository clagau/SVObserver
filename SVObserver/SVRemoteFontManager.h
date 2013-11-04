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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVRemoteFontManager.h_v  $
 * 
 *    Rev 1.2   23 Jul 2013 18:04:40   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Added include std map
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   08 May 2013 16:31:20   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 012.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   29 Apr 2013 14:27:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  830
 * SCR Title:  Consolidate all Time Stamp and Clock Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new SVClock::SVTimeStamp type for all time stamp variables.  Update to use new SVClock::GetTimeStamp() function to get the time stamp value in milliseconds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 14:47:10   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   27 Jan 2011 11:54:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   08 Nov 2010 14:46:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate container types.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   16 Dec 2009 12:34:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   24 Jul 2007 15:43:58   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Update MIL 8.0
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   13 Mar 2007 15:51:36   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  581
 * SCR Title:  Add additional ActiveX interfaces for management of Matrox fonts through SIAC
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Changed GetFontCharImage to use a long ID instead of a char.  Fixed bug in CreateCharMapping
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   06 Feb 2007 14:55:06   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  581
 * SCR Title:  Add additional ActiveX interfaces for management of Matrox fonts through SIAC
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added new methods to manage the Font Image and the adding and deleting of characters in the font
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
 *    Rev 1.0   04 Feb 2004 14:18:52   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  403
 * SCR Title:  Add ActiveX interfaces to allow management of Matrox fonts through the SIAC
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Added new code to support new Matrox font setup
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
