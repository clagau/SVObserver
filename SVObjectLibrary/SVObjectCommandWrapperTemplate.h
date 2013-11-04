//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObjectCommandWrapperTemplate
//* .File Name       : $Workfile:   SVObjectCommandWrapperTemplate.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 16:47:14  $
//******************************************************************************

#ifndef SVOBJECTCOMMANDWRAPPERTEMPLATE_H
#define SVOBJECTCOMMANDWRAPPERTEMPLATE_H

#include "SVUtilityLibrary/SVSharedPtr.h"

template< typename SVCommandPtr >
class SVObjectCommandWrapperTemplate
{
public:
	typedef SVSharedPtr< SVObjectCommandWrapperTemplate > SVObjectCommandWrapperPtr;

	SVObjectCommandWrapperTemplate( const SVCommandPtr& p_rCommandPtr );

	virtual ~SVObjectCommandWrapperTemplate();

	HRESULT Execute();

	HRESULT NotifyRequestComplete() const;
	HRESULT IsRequestComplete() const;
	HRESULT WaitForRequest(DWORD p_TimeoutInMilliseconds) const;

	HRESULT GetStatus() const;

private:
	SVCommandPtr m_CommandPtr;

	HANDLE m_WaitHandle;
	HRESULT m_Status;

};

#include "SVObjectCommandWrapperTemplate.inl"

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObjectLibrary\SVObjectCommandWrapperTemplate.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 16:47:14   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   14 Feb 2013 10:57:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  812
 * SCR Title:  Add New Remote Command Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added and updated code to include asychronous command processing fucntionality to the object based command procesing.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   01 Aug 2012 11:35:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new command functionality to allow for synchronous command execution through the inspection thread.  This will fix problems with race conditions with the inspection process.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/





