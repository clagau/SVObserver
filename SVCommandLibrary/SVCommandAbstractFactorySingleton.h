//******************************************************************************
//* COPYRIGHT (c) 2010 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCommandAbstractFactorySingleton
//* .File Name       : $Workfile:   SVCommandAbstractFactorySingleton.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   18 Apr 2013 18:24:08  $
//******************************************************************************

#ifndef SVCOMMANDABSTRACTFACTORYSINGLETON_H
#define SVCOMMANDABSTRACTFACTORYSINGLETON_H

#include "SVCommandAbstractFactoryTemplate.h"

/*
*/
template< typename SVAbstractFactory >
class SVCommandAbstractFactorySingleton
{
public:
	virtual ~SVCommandAbstractFactorySingleton();

	static SVCommandAbstractFactorySingleton& Instance();

	SVCommandAbstractFactoryTemplatePtr GetAbstractFactory() const;

	bool IsValidCommandType( unsigned long p_CommandType ) const;

	SVCommandTemplatePtr CreateCommand( unsigned long p_CommandType ) const;

protected:
	SVCommandAbstractFactoryTemplatePtr m_pAbstractFactory;

	SVCommandAbstractFactorySingleton();

private:
	SVCommandAbstractFactorySingleton( const SVCommandAbstractFactorySingleton& p_rObject );

	const SVCommandAbstractFactorySingleton& operator=( const SVCommandAbstractFactorySingleton& p_rObject );

};

#include "SVCommandAbstractFactorySingleton.inl"

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVCommandLibrary\SVCommandAbstractFactorySingleton.h_v  $
 * 
 *    Rev 1.0   18 Apr 2013 18:24:08   bWalter
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
