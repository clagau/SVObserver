// ******************************************************************************
// * COPYRIGHT (c) 2009 by Seidenader Vision, Inc.
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : modulename
// * .File Name       : $Workfile:   SVPackedFile.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   03 Feb 2014 16:37:30  $
// ******************************************************************************
#pragma once

#define SV_PACKEDFILE_VERSION 1

class SVPackedFile 
{
public:
	const CString& getConfigFilePath() const;
	//Adds the file(s) specified by szFile to the packed file 
	//specified by szPackedFile. The szFile parameter may 
	//specify one file or may contain wildcard characters to 
	//specify multiple files.
	BOOL PackFiles( const CString& szFile, const CString& szPackedFile );

	//Extracts the files contained in szPackedFile.
	BOOL UnPackFiles( const CString& szPackedFile, const CString& szUnPackDir = _T("") );

	SVPackedFile();

	virtual ~SVPackedFile();

private:
	CString m_configFilePath;
};

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVLibrary\SVPackedFile.h_v  $
 * 
 *    Rev 1.1   03 Feb 2014 16:37:30   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  880
 * SCR Title:  Remove .SEC
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Changed name of private member variable from m_SecFilePath to m_configFilePath.
 * Changed name of public method GetSecFilePath to getConfigFilePath.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   22 Apr 2013 13:37:48   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   11 Feb 2013 10:52:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  812
 * SCR Title:  Add New Remote Command Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated all methods to remove forced CString type requirements for calling methods.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   03 Sep 2009 08:28:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed issues with moved header includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   13 Jun 2001 13:34:40   ed
 * Project:  SVObserver
 * Change Request (SCR) nbr:  200
 * SCR Title:  Update code to implement SVIM COM Server
 * Checked in by:  Ed;  Ed Chobanoff
 * Change Description:  
 *   Added the GetSecFilePath function to allow retreival of the SEC file path after unpacking the files.
 * Modified the UnPackFiles function to add a paramter that specifies the path that the files will be
 * unpacked to. 
 * Modified all CFile::Open calls to use the CFile::shareDenyNone access permission.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   Apr 05 2000 09:10:06   mike
 * Initial revision.
*/
