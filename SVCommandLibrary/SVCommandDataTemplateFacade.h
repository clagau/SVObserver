//******************************************************************************
//* COPYRIGHT (c) 2010 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCommandDataTemplateFacade
//* .File Name       : $Workfile:   SVCommandDataTemplateFacade.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   18 Apr 2013 18:30:34  $
//******************************************************************************

#ifndef SVCOMMANDDATATEMPLATEFACADE_H
#define SVCOMMANDDATATEMPLATEFACADE_H

#include "SVCommandDataFacade.h"

/*
*/
template< unsigned long p_BaseDataType, typename SVTemplateDataType, typename SVTemplateArgType = SVTemplateDataType >
class SVCommandDataTemplateFacade : public SVCommandDataFacade
{
public:
	typedef SVTemplateDataType SVDataType;
	typedef SVTemplateArgType SVArgType;

	virtual ~SVCommandDataTemplateFacade();

	virtual unsigned long GetDataType() const;

	virtual HRESULT GetData( SVArgType& p_rData ) const = 0;
	virtual HRESULT SetData( const SVDataType& p_rData ) = 0;

protected:
	SVCommandDataTemplateFacade();
	SVCommandDataTemplateFacade( bool p_ReadOnly );

	const unsigned long m_DataType;

};

#include "SVCommandDataTemplateFacade.inl"

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVCommandLibrary\SVCommandDataTemplateFacade.h_v  $
 * 
 *    Rev 1.0   18 Apr 2013 18:30:34   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Oct 2010 14:32:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  701
 * SCR Title:  Add New Font Functionality to the Existing Command Interface
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Adding initial code for Command subsystem.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
