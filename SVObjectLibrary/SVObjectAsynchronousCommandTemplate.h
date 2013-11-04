//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObjectAsynchronousCommandTemplate
//* .File Name       : $Workfile:   SVObjectAsynchronousCommandTemplate.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 16:38:14  $
//******************************************************************************

#ifndef SVOBJECTASYNCHRONOUSCOMMANDTEMPLATE_H
#define SVOBJECTASYNCHRONOUSCOMMANDTEMPLATE_H

#include "SVObjectCommandWrapperTemplate.h"

template< typename SVCommandPtr >
class SVObjectAsynchronousCommandTemplate
{
public:
	SVObjectAsynchronousCommandTemplate();
	SVObjectAsynchronousCommandTemplate( const SVObjectAsynchronousCommandTemplate& p_rObject );
	SVObjectAsynchronousCommandTemplate( const SVGUID& p_rObjectID, const SVCommandPtr& p_rCommandPtr );

	virtual ~SVObjectAsynchronousCommandTemplate();

	HRESULT SubmitCommand();
	HRESULT IsCommandComplete();
	HRESULT WaitForCommandToComplete( DWORD p_TimeoutInMilliseconds );

	const SVCommandPtr& GetCommandPtr() const;

private:
	typedef typename SVObjectCommandWrapperTemplate< SVCommandPtr >::SVObjectCommandWrapperPtr SVObjectCommandWrapperPtr;

	SVGUID m_ObjectID;
	SVCommandPtr m_CommandPtr;
	SVObjectCommandWrapperPtr m_WrapperPtr;

};

#include "SVObjectAsynchronousCommandTemplate.inl"

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObjectLibrary\SVObjectAsynchronousCommandTemplate.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 16:38:14   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   14 Feb 2013 10:57:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  812
 * SCR Title:  Add New Remote Command Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added and updated code to include asychronous command processing fucntionality to the object based command procesing.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/





