//******************************************************************************
//* COPYRIGHT (c) 2010 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVFileSystemCommandFactory
//* .File Name       : $Workfile:   SVFileSystemCommandFactory.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 09:37:52  $
//******************************************************************************

#ifndef SVFILESYSTEMCOMMANDFACTORY_H
#define SVFILESYSTEMCOMMANDFACTORY_H

#include "SVCommandLibrary/SVCommandAbstractFactorySingleton.h"
#include "SVCommandLibrary/SVCommandAbstractFactoryTemplate.h"
#include "SVCommandLibrary/SVCommandDataHolder.h"

/*
*/
class SVFileSystemCommandFactory : public SVCommandAbstractFactoryTemplate
{
public:
	static SVCommandFactoryMap m_DefaultFactories;

	static HRESULT DoesFileExist( const SVCommandDataHolder& p_rAttributes, SVCommandDataHolder& p_rResults );

	SVFileSystemCommandFactory();
	virtual ~SVFileSystemCommandFactory();

};

typedef SVCommandAbstractFactorySingleton< SVFileSystemCommandFactory > SVFileSystemCommandFactorySingleton;

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVFileSystemLibrary\SVFileSystemCommandFactory.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 09:37:52   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   11 Mar 2011 13:32:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  701
 * SCR Title:  Add New Font Functionality to the Existing Command Interface
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVObserver to handle file system commands from remote systems via SIAC.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
