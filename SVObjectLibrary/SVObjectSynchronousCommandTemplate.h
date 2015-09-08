//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObjectSynchronousCommandTemplate
//* .File Name       : $Workfile:   SVObjectSynchronousCommandTemplate.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 17:00:54  $
//******************************************************************************

#ifndef SVOBJECTSYNCHRONOUSCOMMANDTEMPLATE_H
#define SVOBJECTSYNCHRONOUSCOMMANDTEMPLATE_H

#include "SVUtilityLibrary/SVGUID.h"
template< typename SVCommandPtr >
class SVObjectSynchronousCommandTemplate
{
public:
	SVObjectSynchronousCommandTemplate( const SVGUID& p_rObjectID, const SVCommandPtr& p_rCommandPtr );

	virtual ~SVObjectSynchronousCommandTemplate();

	HRESULT Execute( DWORD p_TimeoutInMilliseconds );

private:
	SVObjectSynchronousCommandTemplate();

	SVGUID m_ObjectID;
	SVCommandPtr m_CommandPtr;
};

enum 
{
	TWO_MINUTE_CMD_TIMEOUT = 120000
};

#include "SVObjectSynchronousCommandTemplate.inl"

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObjectLibrary\SVObjectSynchronousCommandTemplate.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 17:00:54   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   01 Aug 2012 11:35:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new command functionality to allow for synchronous command execution through the inspection thread.  This will fix problems with race conditions with the inspection process.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/





