//******************************************************************************
//* COPYRIGHT (c) 2010 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCommandLibraryEnums
//* .File Name       : $Workfile:   SVCommandLibraryEnums.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   18 Apr 2013 18:31:40  $
//******************************************************************************

#ifndef SVCOMMANDLIBRARYENUMS_H
#define SVCOMMANDLIBRARYENUMS_H

/*
*/
enum SVCommandType
{
	SV_COMMAND_TYPE_UNKNOWN = 0x00000000,
};

/*
*/
enum SVCommandDataType
{
	SV_COMMAND_DATA_TYPE_UNKNOWN   = 0x00000000,
	SV_COMMAND_DATA_TYPE_READ_ONLY = 0x80000000,
	SV_COMMAND_DATA_TYPE_ITEM_MASK = 0x0000FFFF,
	SV_COMMAND_DATA_TYPE_CONTAINER = 0x00000001,
	SV_COMMAND_DATA_TYPE_VALUE     = 0x00000002,
	SV_COMMAND_DATA_TYPE_BLOCK     = 0x00000004,
	SV_COMMAND_DATA_TYPE_IMAGE     = 0x00000008,
};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVCommandLibrary\SVCommandLibraryEnums.h_v  $
 * 
 *    Rev 1.0   18 Apr 2013 18:31:40   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Oct 2010 14:32:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  701
 * SCR Title:  Add New Font Functionality to the Existing Command Interface
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Adding initial code for Command subsystem.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
