// ******************************************************************************
// * COPYRIGHT (c) 2009 by Seidenader Vision, Inc.
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVIMCommand
// * .File Name       : $Workfile:   SVIMCommand.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 11:26:48  $
// ******************************************************************************

#ifndef INC_SVIMCOMMAND_38BAE0BD00DA_INCLUDED
#define INC_SVIMCOMMAND_38BAE0BD00DA_INCLUDED

#include "SVStatusLibrary/SVException.h"

#define SVIOBUFFSIZE 4096

#define SVPUTCONFIG 0 
#define SVGETCONFIG 1
#define SVGETCURRENTCONFIG 2
#define SVSETCURRENTCONFIG 3
#define SVGOOFFLINE 4
#define SVGOONLINE 5
#define SVGETSTATE 6
#define SVCANCEL 7
#define SVDATA 8
#define	SVPUTFILE 9

#define SVIM_CONFIG_LOADING		0x0001
#define SVIM_CONFIG_LOADED		0x0002
#define SVIM_ONLINE_PENDING		0x0004
#define SVIM_ONLINE						0x0008
#define SVIM_RUNNING_TEST			0x0010
#define SVIM_RUNNING					0x0020
#define SVIM_REGRESSION_TEST	0x0040
#define SVIM_SETUPMODE				0x0080
#define SVIM_SAVING_CONFIG		0x0100
#define SVIM_STOPPING					0x0200
#define SVIM_RAID_FAILURE			0x0400

typedef void (CALLBACK * SVCOMMANDCOMPLETEPROC)(CString &szFileName, SVException *pException, void *pUserData);

class SVIMCommand 
{
public:
	BOOL unescape (char *pInput, char *pOutput, int cbOutput);
	BOOL escape (char *pInput, char *pOutput, int cbOutput);

	SVIMCommand();

	virtual ~SVIMCommand();

	int GetCommandToken(char * pszCommandText);

	char *   GetCommandText(int CommandToken   );

private:
	CArray< char*, char* > mszCommandList;

};

#endif

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVIMCommand\SVIMCommand.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 11:26:48   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   09 Nov 2010 15:34:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to change formatting to a common searchable format.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   27 Oct 2010 08:49:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix include issues and comment issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   03 Sep 2009 08:22:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed issues with moved header includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   05 Jan 2006 09:52:02   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  530
 * SCR Title:  Allow SVObserver to monitor Intel SATA RAID status
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added a new state for RAID Failure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   31 Jul 2000 17:06:22   mike
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  25
 * SCR Title:  Download OCR files
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Added new SVIM command "PutFile" that writes a single line of data to the specified file.
 * 
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   10 Apr 2000 15:07:56   mike
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  4
 * SCR Title:  Baseline Development: SVIM Communications
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Added definitions of SVIM States
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   04 Apr 2000 17:05:44   Joe
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  1
 * SCR Title:  Baseline Development: Run Mode
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated to fix linking problem with SVFocus.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   Mar 22 2000 17:23:36   mike
 * Initial revision.
*/
