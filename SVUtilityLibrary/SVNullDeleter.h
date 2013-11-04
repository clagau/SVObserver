//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVNullDeleter
//* .File Name       : $Workfile:   SVNullDeleter.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 19:22:58  $
//******************************************************************************
#ifndef SVNULLDELETER_H
#define SVNULLDELETER_H

/**
@SVObjectName Null Deleter

@SVObjectOverview A NULL deleter to use a Shared Pointer with a statically allocated object

@SVObjectOperations

*/

struct SVNullDeleter
{
    void operator()(void const *) const
    {
    }
};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVUtilityLibrary\SVNullDeleter.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 19:22:58   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   11 Sep 2008 13:43:40   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added PVCS Log block
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/