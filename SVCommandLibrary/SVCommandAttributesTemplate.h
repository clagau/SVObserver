//******************************************************************************
//* COPYRIGHT (c) 2010 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCommandAttributesTemplate
//* .File Name       : $Workfile:   SVCommandAttributesTemplate.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   18 Apr 2013 18:24:12  $
//******************************************************************************

#ifndef SVCOMMANDATTRIBUTESTEMPLATE_H
#define SVCOMMANDATTRIBUTESTEMPLATE_H

#include "SVCommandAttributesFacade.h"

/*
*/
template< unsigned long p_CommandType, typename SVAttributeCollection, typename SVDefaultCollection = SVAttributeCollection >
class SVCommandAttributesTemplate : public SVCommandAttributesFacade< SVAttributeCollection >
{
public:
	typedef SVDefaultCollection SVDefaultAttributes;

	static const SVDefaultAttributes m_DefaultAttributes;

	SVCommandAttributesTemplate();
	SVCommandAttributesTemplate( const SVCommandAttributesTemplate& p_rObject );

	virtual ~SVCommandAttributesTemplate();

	const SVCommandAttributesTemplate& operator=( const SVCommandAttributesTemplate& p_rObject );

	virtual HRESULT GetAttributes( SVAttributeCollection& p_rAttributes ) const;
	virtual HRESULT SetAttributes( const SVAttributeCollection& p_rAttributes );

protected:
	SVAttributeCollection m_Attributes;

};

#include "SVCommandAttributesTemplate.inl"

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVCommandLibrary\SVCommandAttributesTemplate.h_v  $
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
