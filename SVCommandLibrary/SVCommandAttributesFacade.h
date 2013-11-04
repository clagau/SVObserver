//******************************************************************************
//* COPYRIGHT (c) 2010 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCommandAttributesFacade
//* .File Name       : $Workfile:   SVCommandAttributesFacade.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   18 Apr 2013 18:24:12  $
//******************************************************************************

#ifndef SVCOMMANDATTRIBUTESFACADE_H
#define SVCOMMANDATTRIBUTESFACADE_H

/*
*/
template< typename SVAttributeCollection >
class SVCommandAttributesFacade
{
public:
	virtual ~SVCommandAttributesFacade() {}

	virtual HRESULT GetAttributes( SVAttributeCollection& p_rAttributes ) const = 0;
	virtual HRESULT SetAttributes( const SVAttributeCollection& p_rAttributes ) = 0;

};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVCommandLibrary\SVCommandAttributesFacade.h_v  $
 * 
 *    Rev 1.0   18 Apr 2013 18:24:12   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Oct 2010 14:31:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  701
 * SCR Title:  Add New Font Functionality to the Existing Command Interface
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Adding initial code for Command subsystem.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
