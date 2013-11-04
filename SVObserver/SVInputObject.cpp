//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVInputObject
//* .File Name       : $Workfile:   SVInputObject.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:59:26  $
//******************************************************************************

#include "stdafx.h"
#include "SVInputObject.h"

SVInputObject::SVInputObject( LPCSTR strObjectName )
				:SVObjectClass( strObjectName )
{
	m_bCreated = FALSE;
}

SVInputObject::SVInputObject( SVObjectClass* POwner, int StringResourceID )
				:SVObjectClass( POwner, StringResourceID )
{
	m_bCreated = FALSE;
}

SVInputObject::~SVInputObject()
{
	if( m_bCreated )
		Destroy();
}

BOOL SVInputObject::Create()
{
	m_bCreated = TRUE;

	return TRUE;
}

BOOL SVInputObject::Destroy()
{
	m_bCreated = FALSE;

	return TRUE;
}

BOOL SVInputObject::IsCreated()
{
	return m_bCreated;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVInputObject.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 10:59:26   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   16 Aug 2012 16:54:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  779
 * SCR Title:  Fix Problems with DDE Inputs and DDE Outputs
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to change the read and write functionality to use a variant as the data type instead of a void pointer.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   21 Jun 2007 13:05:08   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   22 Apr 2003 10:25:04   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   13 Jan 2003 11:36:28   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  307
 * SCR Title:  Implement the ability to remotely change Tool parameters on the SVIM
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Modified the base InputObject class and its derived for easier RemoteInput support.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   09 Dec 2002 19:29:42   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  314
 * SCR Title:  SVIM stops inspecting when triggered very fast
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Updated to fix problems and track issues with result data indexes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Nov 2002 13:46:02   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  272
 * SCR Title:  Outputs do not always get set correctly.
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   This is the first revision of this file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/