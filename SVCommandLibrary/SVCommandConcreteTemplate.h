//******************************************************************************
//* COPYRIGHT (c) 2010 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCommandConcreteTemplate
//* .File Name       : $Workfile:   SVCommandConcreteTemplate.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   18 Apr 2013 18:24:06  $
//******************************************************************************

#ifndef SVCOMMANDCONCRETETEMPLATE_H
#define SVCOMMANDCONCRETETEMPLATE_H

#include <boost/config.hpp>
#include <boost/function.hpp>
#include "SVCommandLibrary/SVCommandAttributesTemplate.h"
#include "SVCommandLibrary/SVCommandTemplate.h"
#include "SVCommandLibrary/SVCommandDataHolder.h"
#include "SVCommandLibrary/SVCommandResultsTemplate.h"

typedef boost::function< HRESULT ( const SVCommandDataHolder&, SVCommandDataHolder& ) > SVCommandExecuteFunctor;

/*
*/
template< unsigned long p_CommandType >
class SVCommandConcreteTemplate : 
	public SVCommandTemplate,
	public SVCommandAttributesTemplate< p_CommandType, SVCommandDataHolder, SVCommandDataHolder::SVNameDataMap >,
	public SVCommandResultsTemplate< SVCommandDataHolder >
{
public:
	typedef SVCommandAttributesTemplate< p_CommandType, SVCommandDataHolder, SVCommandDataHolder::SVNameDataMap > SVCommandAttributes;
	typedef SVCommandResultsTemplate< SVCommandDataHolder > SVCommandResults;

	static unsigned long GetStaticCommandType();

	SVCommandConcreteTemplate( SVCommandExecuteFunctor p_ExecuteFunctor );
	virtual ~SVCommandConcreteTemplate();

	virtual HRESULT Execute();

protected:
	SVCommandExecuteFunctor m_ExecuteFunctor;

};

#include "SVCommandConcreteTemplate.inl"

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVCommandLibrary\SVCommandConcreteTemplate.h_v  $
 * 
 *    Rev 1.0   18 Apr 2013 18:24:06   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Oct 2010 14:31:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  701
 * SCR Title:  Add New Font Functionality to the Existing Command Interface
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Adding initial code for Command subsystem.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
