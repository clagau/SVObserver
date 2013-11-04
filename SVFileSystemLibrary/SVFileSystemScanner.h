//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVFileSystemScanner
//* .File Name       : $Workfile:   SVFileSystemScanner.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 09:37:58  $
//******************************************************************************
#ifndef SVFILESYSTEMSCANNER_H
#define SVFILESYSTEMSCANNER_H

template<typename Insertor>
class SVFileSystemScanner
{
public:
	static HRESULT ScanForFiles(LPCTSTR dirName, LPCTSTR filter, Insertor& insertor, bool bScanSubDirs=false);
	static HRESULT ScanForDirectories(LPCTSTR dirName, LPCTSTR filter, Insertor& insertor, bool bScanSubDirs=false);
};

#include "SVFileSystemScanner.inl"

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVFileSystemLibrary\SVFileSystemScanner.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 09:37:58   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   02 Sep 2009 15:35:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Files moved from SVUtilityLibrary to SVFileSystemLibrary
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   10 Sep 2008 10:28:36   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial CheckIn
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/