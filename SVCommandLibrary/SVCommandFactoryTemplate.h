//******************************************************************************
//* COPYRIGHT (c) 2010 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCommandFactoryTemplate
//* .File Name       : $Workfile:   SVCommandFactoryTemplate.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   18 Apr 2013 18:31:36  $
//******************************************************************************

#ifndef SVCOMMANDFACTORYTEMPLATE_H
#define SVCOMMANDFACTORYTEMPLATE_H

#include "SVUtilityLibrary/SVSharedPtr.h"
#include "SVCommandTemplate.h"

/*
*/
class SVCommandFactoryTemplate
{
public:
	virtual ~SVCommandFactoryTemplate();

	unsigned long GetCommandType() const;

	virtual SVCommandTemplatePtr CreateCommand() const = 0;

protected:
	SVCommandFactoryTemplate( unsigned long p_CommandType );

	const unsigned long m_CommandType;

private:
	SVCommandFactoryTemplate();
	SVCommandFactoryTemplate( const SVCommandFactoryTemplate& p_rObject );

	const SVCommandFactoryTemplate& operator=( const SVCommandFactoryTemplate& p_rObject );

};

typedef SVSharedPtr< SVCommandFactoryTemplate > SVCommandFactoryTemplatePtr;

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVCommandLibrary\SVCommandFactoryTemplate.h_v  $
 * 
 *    Rev 1.0   18 Apr 2013 18:31:36   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Oct 2010 14:32:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  701
 * SCR Title:  Add New Font Functionality to the Existing Command Interface
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Adding initial code for Command subsystem.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
