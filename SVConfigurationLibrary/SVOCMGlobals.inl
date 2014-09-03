// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVOCMGlobals
// * .File Name       : $Workfile:   SVOCMGlobals.inl  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.2  $
// * .Check In Date   : $Date:   03 Sep 2014 15:54:44  $
// ******************************************************************************

#pragma once

#include "SVStatusLibrary/SVStatusCodes.h"
#include "SVOCMArchive.h"

template< typename SVTreeType >
HRESULT SVOCMLoadConfiguration(unsigned long ulSVOVersion, 
															 unsigned long& ulSVOConfigVersion, 
															 BSTR bstrFileName, 
															 SVTreeType& p_rTree )
{
	HRESULT hrOk = S_FALSE;

	hrOk = SVOCMArchive::GetSVOSavedVersionNumberFromConfigurationFile( 
		bstrFileName, &ulSVOConfigVersion );

	if( SV_SUCCEEDED( hrOk ) )
	{
		hrOk = SVOCMArchive::CreateTreeFromConfigurationFile( 
			ulSVOVersion, ulSVOConfigVersion, bstrFileName, p_rTree );
	}

	return hrOk;
}

template< typename SVTreeType >
HRESULT SVOCMSaveConfiguration (unsigned long ulSVOVersion, 
																unsigned long &ulSECVersion, 
																BSTR bstrFileName, 
																SVTreeType& p_rTree )
{
	HRESULT hr = S_FALSE;

	CString csFileName(bstrFileName);

	hr = SVOCMArchive::CreateConfigurationFileFromTree (ulSVOVersion, 
		NULL,	// revision history
		ulSECVersion, 
		csFileName, 
		p_rTree);

	return hr;
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVConfigurationLibrary\SVOCMGlobals.inl_v  $
 * 
 *    Rev 1.2   03 Sep 2014 15:54:44   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  880
 * SCR Title:  Remove .SEC
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Removed methods related to CArchive:
 * SVOCMArchiveIOD
 * and SVOCMArchiveIPD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   03 Feb 2014 16:14:36   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  880
 * SCR Title:  Remove .SEC
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Removed method SVOCMArchiveSEC.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Apr 2013 18:39:42   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Sep 2012 17:54:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use the templated XML objects that moved from the SVLibrary to the new XML Library.
 * 
 * This code was moved from the SVOConfigMgr Library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   15 Dec 2009 12:58:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   14 Nov 2008 15:16:50   jbrown
 * Project:  SVObserver
 * Change Request (SCR) nbr:  641
 * SCR Title:  BoundsChecker results
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   Improve error handling within
 * SVOCMLoadConfiguration ().
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   22 Oct 2004 07:38:00   jBrown
 * Project:  SVObserver
 * Change Request (SCR) nbr:  375
 * SCR Title:  Remodel XML usage for configuration storage and SIAC transfers.
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   Modified functions to implement new XML classes for loading and saving vision configurations.
 * 
 * Added header to reference SVBStr.
 * 
 * Modified:
 *   SVOCMLoadConfiguration (), and
 *   SVOCMSaveConfiguration ().
 * 
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Nov 2002 09:56:16   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   first version of the file
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/