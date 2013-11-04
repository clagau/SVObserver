//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObjectNotifyTemplate
//* .File Name       : $Workfile:   SVObjectNotifyTemplate.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 16:58:12  $
//******************************************************************************

#ifndef SVOBJECTNOTIFYTEMPLATE_H
#define SVOBJECTNOTIFYTEMPLATE_H

template< typename SVNotifyData >
class SVObjectNotifyTemplate
{
public:
	virtual HRESULT ProcessNotifyData( SVNotifyData& p_rData ) = 0;

};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObjectLibrary\SVObjectNotifyTemplate.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 16:58:12   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 Jan 2012 15:04:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  747
 * SCR Title:  Add New Remote Input Tool to SVObserver
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated object manager to include the new notification functionality to managed object.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
