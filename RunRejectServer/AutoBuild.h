//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : AutoBuild
//* .File Name       : $Workfile:   AutoBuild.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   22 Aug 2014 09:44:40  $
//******************************************************************************

#pragma once

// change the FALSE to TRUE for autoincrement of build number
#define INCREMENT_VERSION TRUE

#define SV_EXPAND( x ) #x
#define SV_STRINGIZE( x ) SV_EXPAND( x )

// FILEVER lay out is as follows: a,b,c,d
//    a is the major version number
//    b is the minor version number
//    c is the beta version number - if c equals 0 or is greater than 254 then the application is marked as Released
//    d is unused

#define FILEVER        1,0,2,0
#define PRODUCTVER     FILEVER
#define STRFILEVER     SV_STRINGIZE( FILEVER )
#define STRPRODUCTVER  SV_STRINGIZE( PRODUCTVER )

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\RunRejectServer\AutoBuild.h_v  $
 * 
 *    Rev 1.1   22 Aug 2014 09:44:40   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Fixed pvcs log entry
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   22 Aug 2014 09:39:58   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/

