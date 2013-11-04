// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVOCMGlobals
// * .File Name       : $Workfile:   SVOCMGlobals.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   18 Apr 2013 18:39:42  $
// ******************************************************************************

#ifndef SVOCMGLOBALS_H
#define SVOCMGLOBALS_H

template< typename SVTreeType >
HRESULT SVOCMArchiveSEC( unsigned long ulSVOVersion, 
                         unsigned long &ulSECVersion, 
                         CArchive &rArchive, 
                         SVTreeType& p_rTree,
												 BSTR *pIODocName );

template< typename SVTreeType >
HRESULT SVOCMArchiveIOD( unsigned long ulSVOVersion, 
                         unsigned long &ulSECVersion, 
                         CArchive &rArchive, 
                         SVTreeType& p_rTree );

template< typename SVTreeType >
HRESULT SVOCMArchiveIPD( unsigned long ulSVOVersion, 
                         unsigned long &ulSECVersion, 
                         CArchive &rArchive, 
                         SVTreeType& p_rTree,
												 typename SVTreeType::SVBranchHandle p_pParent );

template< typename SVTreeType >
HRESULT SVOCMLoadConfiguration( unsigned long ulSVOVersion, 
																unsigned long &ulSECVersion, 
																BSTR bstrFilename,
																SVTreeType& p_rTree );

template< typename SVTreeType >
HRESULT SVOCMSaveConfiguration( unsigned long ulSVOVersion, 
																unsigned long &ulSECVersion, 
																BSTR bstrFilename,
																SVTreeType& p_rTree );

#include "SVOCMGlobals.inl"

#endif

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVConfigurationLibrary\SVOCMGlobals.h_v  $
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
 *    Rev 1.0   18 Sep 2012 17:54:24   jspila
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
 *    Rev 1.0   18 Nov 2002 09:57:14   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   first version of file
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
