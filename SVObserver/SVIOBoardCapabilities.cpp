//******************************************************************************
//* COPYRIGHT (c) 2007 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVIOBoardCapabilities
//* .File Name       : $Workfile:   SVIOBoardCapabilities.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   :     $Date:   23 Apr 2013 11:08:10  $
//******************************************************************************

// SVIOBoardCapabilities.cpp: implementation of the SVIOBoardCapabilities class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SVIOBoardCapabilities.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVIOBoardCapabilities::SVIOBoardCapabilities()
:m_iNumTriggers(4),
m_iNumInputs(8),
m_iNumOutputs(16),
m_iStrobeInverters(4),
m_iTriggerInverters(4)
{
}

SVIOBoardCapabilities::SVIOBoardCapabilities( const SVIOBoardCapabilities& p_rCapable )
{
	if (this != &p_rCapable)
	{
		*this = p_rCapable;
	}
}

const SVIOBoardCapabilities& SVIOBoardCapabilities::operator=(const SVIOBoardCapabilities& p_rCapable )
{
	m_iNumInputs = p_rCapable.m_iNumInputs;
	m_iNumOutputs = p_rCapable.m_iNumOutputs;
	m_iNumTriggers = p_rCapable.m_iNumTriggers;
	m_iStrobeInverters = p_rCapable.m_iStrobeInverters;
	m_iTriggerInverters = p_rCapable.m_iTriggerInverters;

	return *this;
}

SVIOBoardCapabilities::~SVIOBoardCapabilities()
{
}

int SVIOBoardCapabilities::GetTriggerInverterCount() const
{
	return m_iTriggerInverters;
}

int SVIOBoardCapabilities::GetStrobeInverterCount() const
{
	return m_iStrobeInverters;
}

int SVIOBoardCapabilities::GetTriggerCount() const
{
	return m_iNumTriggers;
}

int SVIOBoardCapabilities::GetInputCount() const
{
	return m_iNumInputs;
}

int SVIOBoardCapabilities::GetOutputCount() const
{
	return m_iNumOutputs;
}

void SVIOBoardCapabilities::SetInputCount( int p_iInputs )
{
	m_iNumInputs = p_iInputs;
}

void SVIOBoardCapabilities::SetOutputCount( int p_iOutputs )
{
	m_iNumOutputs = p_iOutputs;
}

void SVIOBoardCapabilities::SetTriggerCount( int p_iTriggers )
{
	m_iNumTriggers = p_iTriggers;
}

void SVIOBoardCapabilities::SetTriggerInverters( int p_iTrigInverters )
{
	m_iTriggerInverters = p_iTrigInverters;
}

void SVIOBoardCapabilities::SetStrobeInverters( int p_iStrobeInverters )
{
	m_iStrobeInverters = p_iStrobeInverters;
}

bool SVIOBoardCapabilities::IsNonIOSVIM() const
{
	return (m_iNumInputs == 0 && m_iNumOutputs == 0);
}

void SVIOBoardCapabilities::SetNonIOSVIM(int numTriggers)
{
	m_iTriggerInverters = 0;
	m_iStrobeInverters = 0;
	m_iNumInputs = 0;
	m_iNumOutputs = 0;
	m_iNumTriggers = numTriggers;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVIOBoardCapabilities.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 11:08:10   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   21 Jan 2013 10:55:12   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  809
 * SCR Title:  Non I/O SVIM
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Removed unused constructor overload
 * Added IsNonIOSVIM method
 * Aded SetNonIOSVIM method
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   12 Dec 2012 19:32:04   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  802
 * SCR Title:  Add new product type GD1A
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   (SVIOBoardCapabilities(SVIMProductEnum)) Added case for SVIM_PRODUCT_X2_GD1A, SVIM_PRODUCT_X2_GD1A_COLOR.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   13 Aug 2012 11:37:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  773
 * SCR Title:  Update X2 GigE Model
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed X2B-GD8A model/configuration information from application.  Converted source code to use X2-GD8A instead of X2B-GD8A.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   02 Jul 2012 14:20:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  774
 * SCR Title:  Update SVObserver to include the new SVIM X2B Product
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new configuration type SVIM X2B-GD8A.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   02 Jul 2012 13:22:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  773
 * SCR Title:  Update X2 GigE Model
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed configuration type SVIM X2-GD4A.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   02 Jul 2012 12:50:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  773
 * SCR Title:  Update X2 GigE Model
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new configuration type SVIM X2-GD8A.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   12 Aug 2011 14:16:00   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  736
 * SCR Title:  Add a 4 Camera GigE System to the list of systems
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added 4 Camera GigE System.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   11 Aug 2011 07:28:06   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  735
 * SCR Title:  Add Gige Color System
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added system type and product type to support GigE color cameras.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   10 Nov 2009 12:47:50   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Removed Gige Color system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   17 Jun 2009 08:48:32   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised Constructor to correct a problem with missing break statement on case statements.
 * Revised Constructor to add SVIM_PRODUCT_X2_GD2A and SVIM_PRODUCT_X2_GD2A_COLOR
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   04 Feb 2009 15:08:58   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  645
 * SCR Title:  Add support for X2 and X2 Color products
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added support for X2 and X2 color
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   21 Jun 2007 13:05:14   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   13 Feb 2007 09:08:24   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  592
 * SCR Title:  Add  X3 product type to SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added PVCS Headers
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
