//******************************************************************************
//* COPYRIGHT (c) 2010 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCommandDataTemplate
//* .File Name       : $Workfile:   SVCommandDataTemplate.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   18 Apr 2013 18:30:34  $
//******************************************************************************

#ifndef SVCOMMANDDATATEMPLATE_H
#define SVCOMMANDDATATEMPLATE_H

#include "SVCommandDataTemplateFacade.h"

/*
*/
template< unsigned long p_BaseDataType, typename SVTemplateDataType, typename SVTemplateArgType = SVTemplateDataType >
class SVCommandDataTemplate : public SVCommandDataTemplateFacade< p_BaseDataType, SVTemplateDataType, SVTemplateArgType >
{
public:
	typedef SVCommandDataTemplateFacade< p_BaseDataType, SVTemplateDataType, SVTemplateArgType > SVCommandDataInterface;

	SVCommandDataTemplate();
	SVCommandDataTemplate( const SVDataType& p_rValue, bool p_ReadOnly = false );

	virtual ~SVCommandDataTemplate();

	virtual HRESULT GetData( SVArgType& p_rData ) const;
	virtual HRESULT SetData( const SVDataType& p_rData );

protected:
	SVDataType m_Data;

};

#include "SVCommandDataTemplate.inl"

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVCommandLibrary\SVCommandDataTemplate.h_v  $
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
 *    Rev 1.0   15 Oct 2010 14:32:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  701
 * SCR Title:  Add New Font Functionality to the Existing Command Interface
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Adding initial code for Command subsystem.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
