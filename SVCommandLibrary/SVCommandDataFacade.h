//******************************************************************************
//* COPYRIGHT (c) 2010 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCommandDataFacade
//* .File Name       : $Workfile:   SVCommandDataFacade.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   18 Apr 2013 18:30:30  $
//******************************************************************************

#ifndef SVCOMMANDDATAFACADE_H
#define SVCOMMANDDATAFACADE_H

#include <comdef.h>
#include "SVUtilityLibrary/SVSharedPtr.h"

/*
*/
class SVCommandDataFacade
{
public:
	virtual ~SVCommandDataFacade();

	virtual unsigned long GetDataType() const = 0;

protected:
	SVCommandDataFacade();

private:
	SVCommandDataFacade( const SVCommandDataFacade& p_rObject );

	const SVCommandDataFacade& operator=( const SVCommandDataFacade& p_rObject );

};

typedef SVSharedPtr< SVCommandDataFacade > SVCommandDataFacadePtr;

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVCommandLibrary\SVCommandDataFacade.h_v  $
 * 
 *    Rev 1.0   18 Apr 2013 18:30:30   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Oct 2010 14:32:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  701
 * SCR Title:  Add New Font Functionality to the Existing Command Interface
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Adding initial code for Command subsystem.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
