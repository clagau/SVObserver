//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVStreamCommandFactory
//* .File Name       : $Workfile:   SVStreamCommandFactory.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:14:58  $
//******************************************************************************

#ifndef SVSTREAMCOMMANDFACTORY_H
#define SVSTREAMCOMMANDFACTORY_H

#include "SVCommandLibrary/SVCommandAbstractFactorySingleton.h"
#include "SVCommandLibrary/SVCommandAbstractFactoryTemplate.h"
#include "SVCommandLibrary/SVCommandDataHolder.h"
#include "SVUtilityLibrary/SVGUID.h"

/*
*/
class SVStreamCommandFactory : public SVCommandAbstractFactoryTemplate
{
public:
	static SVCommandFactoryMap m_DefaultFactories;

	static HRESULT QueryInputStreamNames( const SVCommandDataHolder& p_rAttributes, SVCommandDataHolder& p_rResults );
	static HRESULT QueryInputStreamDataItems( const SVCommandDataHolder& p_rAttributes, SVCommandDataHolder& p_rResults );
	static HRESULT ClearInputStreamDataItems( const SVCommandDataHolder& p_rAttributes, SVCommandDataHolder& p_rResults );

	SVStreamCommandFactory();
	virtual ~SVStreamCommandFactory();

};

typedef SVCommandAbstractFactorySingleton< SVStreamCommandFactory > SVStreamCommandFactorySingleton;

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVStreamCommandFactory.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 15:14:58   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 Jan 2012 15:39:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  747
 * SCR Title:  Add New Remote Input Tool to SVObserver
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Added new stream manager to the application.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
