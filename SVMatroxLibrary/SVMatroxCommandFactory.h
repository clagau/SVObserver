//******************************************************************************
//* COPYRIGHT (c) 2010 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxCommandFactory
//* .File Name       : $Workfile:   SVMatroxCommandFactory.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 15:12:20  $
//******************************************************************************

#ifndef SVMATROXCOMMANDFACTORY_H
#define SVMATROXCOMMANDFACTORY_H

#include "SVCommandLibrary/SVCommandAbstractFactorySingleton.h"
#include "SVCommandLibrary/SVCommandAbstractFactoryTemplate.h"

/*
*/
class SVMatroxCommandFactory : public SVCommandAbstractFactoryTemplate
{
public:
	static SVCommandFactoryMap m_DefaultFactories;

	SVMatroxCommandFactory();
	virtual ~SVMatroxCommandFactory();

};

typedef SVCommandAbstractFactorySingleton< SVMatroxCommandFactory > SVMatroxCommandFactorySingleton;

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMatroxLibrary\SVMatroxCommandFactory.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 15:12:20   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Oct 2010 15:00:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  701
 * SCR Title:  Add New Font Functionality to the Existing Command Interface
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include new command functionality using the matrox library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
