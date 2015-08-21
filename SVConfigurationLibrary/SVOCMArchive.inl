// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVOCMArchive
// * .File Name       : $Workfile:   SVOCMArchive.inl  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.3  $
// * .Check In Date   : $Date:   03 Sep 2014 15:52:50  $
// ******************************************************************************

#pragma once

#include "SVOCMArchive.h"
#include "SVXmlStream.h"
#include "SVOLibrary/SVOLibrary.h"
#include "SVUtilityLibrary/SVGUID.h"
#include "SVUtilityLibrary/SVSAFEARRAY.h"
#include "SVConfigurationTags.h"
#include "SVXMLLibrary/SVXMLClass.h"
#include "SVLibrary/SVBStr.h"
#include "SVXMLLibrary/SVXML2TreeConverter.h"
#include "SVObserverEnums.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Primitives section
// the HandleErrorCondition call is not made within primitives so that more meaningful
// error placement and reporting can be made
// a primitive for this purpose is defined as a basic function which is called from more than
// one place in SVOCMArchive
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

template< typename SVTreeType >
HRESULT SVOCMArchive::CreateTreeFromConfigurationFile (unsigned long  ulSVOVersion, 
																											 unsigned long  ulConfigVersion, 
																											 BSTR			bstrFilename,
																											 SVTreeType& p_rTree)
{
	USES_CONVERSION;

	HRESULT hr( S_OK );

	SVString l_FileName( bstrFilename );

	CString cstrMessage;
	BSTR bstrRevisionHistory = NULL;

	while (1)
	{
		if( ulConfigVersion < 0x00040000 )
		{
			//-		Configuration files prior to version 4.00 should not be able to get
			//-		here.
			hr = -1663;
			break;
		}
		else if( 0x00040000 <= ulConfigVersion && ulConfigVersion < 0x00043200 )
		{
			SVXmlStream< SVTreeType > xml( p_rTree );

			if( !xml.Load_XML_Document( l_FileName.c_str() ) )
			{
				hr = E_FAIL;
			}
		}
		else if( 0x00043200 <= ulConfigVersion )
		{
			BSTR bstrChangedNode( NULL );

			SVTreeType l_XMLTree( p_rTree );
			SVXMLClass l_XMLTreeBuilder;

			hr = l_XMLTreeBuilder.Initialize();

			if( SUCCEEDED( hr ) )
			{
				hr = l_XMLTreeBuilder.PreserveWhitespace( true );
			}

			if( SUCCEEDED( hr ) )
			{
				//hr = l_XMLTreeBuilder.CopyXMLFileToTree( l_XMLTree, ulSVOVersion, bstrFilename, &bstrRevisionHistory, &bstrChangedNode );

				hr = l_XMLTreeBuilder.CopyXMLFileToDOM( bstrFilename, &bstrRevisionHistory );

				if( SUCCEEDED( hr ) )
				{
					hr = SVXML2TreeConverter::CopyToTree( l_XMLTreeBuilder, l_XMLTree, L"Base", false );
				}
			}

			/*
			if( !SUCCEEDED( hr ) )
			{
				cstrMessage.Format("WARNING: Configuration has been changed.  \
													 The last changed node is (other nodes may also have been changed) - %s. \
													 Configuration loading will continue.",
													 W2T (bstrChangedNode));

				AfxMessageBox(cstrMessage);
			}
			*/
		}
		else
		{
			hr = E_FAIL;
		}

		break;
	}

	SysFreeString (bstrRevisionHistory);
	bstrRevisionHistory = NULL;

	return hr;
}

template< typename SVTreeType >
HRESULT SVOCMArchive::CreateConfigurationFileFromTree(unsigned long ulSVOVersion, BSTR bstrRevisionHistory, unsigned long ulSECVersion, LPCTSTR pstrFilename, SVTreeType& p_rTree)
{
	HRESULT hr = S_OK;

	SVTreeType l_XMLTree( p_rTree );
	SVXMLClass l_XMLTreeBuilder;
	
	hr = l_XMLTreeBuilder.Initialize( 0,		// do not use checksums
		1, 						// use program version/data revision info
		0);						// do not use encryption

	if( SUCCEEDED( hr ) )
	{
		hr = l_XMLTreeBuilder.PreserveWhitespace( true );
	}

	if( SUCCEEDED( hr ) )
	{
		SVBStr bstrFilename;
		SVBStr bstrFormatName;

		bstrFilename = pstrFilename;

		hr = bstrFormatName.CopyFromWChar (L"SVObserver");

		if( SUCCEEDED( hr ) )
		{
			hr = l_XMLTreeBuilder.CopyTreeToXMLFile( l_XMLTree, ulSVOVersion, bstrFormatName, bstrRevisionHistory, bstrFilename );
		}
	}
	return hr;
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVConfigurationLibrary\SVOCMArchive.inl_v  $
 * 
 *    Rev 1.3   03 Sep 2014 15:52:50   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  880
 * SCR Title:  Remove .SEC
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Removed methods related to CArchive:
 * ArchiveSVObserverApp,
 * ArchiveSVSystemClass,
 * ArchiveSVDigitizerArrayClass,
 * ArchiveSVDigitizerObjectClass,
 * ArchiveSVDigitizerInfoStruct,
 * ArchiveSVAcquisitionSourceListClass,
 * ArchiveSVAcquisitionSourceClass,
 * ArchiveSVImageInfoClass,
 * ArchiveSVLightReferenceArrayClass,
 * ArchiveSVLightReferenceClass,
 * ArchiveSVExtentClass,
 * ArchiveSVPPQIndexHandleOwnerStruct,
 * ArchiveSVDDEEntryStruct,
 * ArchiveSVDDEInputEntryListClass,
 * ArchiveSVPQDataLinkInfoStruct (both versions),
 * ArchiveAddSVPQDataLinkInfoStruct,
 * ArchiveSVByteArrayClass,
 * ArchiveSVPQDigitialInputInfoListClass [sic],
 * ArchiveSVPQDigitialInputInfoStruct [sic],
 * ArchiveSVAcquisitionTriggerEventServerClass,
 * ArchiveSVEventServerClass,
 * ArchiveSVDigitalOutputObjectClass,
 * ArchiveSVPQObjectClass,
 * ArchiveSVPQCheckerObjectClass,
 * ArchiveSVPPQInfoStruct,
 * ArchiveSVDDEControlClass,
 * ArchiveSVDDEOutputEntryListClass,
 * ArchiveSVIPDocInfoClass,
 * ArchiveSVIPDocRequirementInfoClass,
 * ArchiveSVCameraInfoStruct,
 * ArchiveIOD,
 * ArchiveSVIODoc,
 * ArchiveSVTriggerEventServerClass,
 * ArchiveSVTimerTriggerEventServerClass,
 * ArchiveSVIOEntryClass,
 * ArchiveSVDigitalInputObjectClass,
 * ArchiveIPD,
 * ArchiveSVIPDoc,
 * ArchiveSVIPDocViewData,
 * ArchiveSVImageViewScroll,
 * ArchiveSVImageViewClass,
 * ArchiveSVImageViewClassEmbedded,
 * ArchiveSVToolSetTabViewClass,
 * ArchiveSVResultViewClass,
 * ArchiveWindowPlacementData,
 * ArchivePointData,
 * ArchiveRectData,
 * ArchiveSizeData,
 * GetEnvironmentBranch,
 * GetAcquisitionDeviceBranch,
 * GetCameraBranch,
 * GetTriggerBranch,
 * GetInsepctionBranch,
 * GetPPQBranch,
 * GetIOBranch,
 * GetBranch,
 * GetItem,
 * AddItem (both versions),
 * AddArray,
 * and AddBranch.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   03 Feb 2014 16:11:30   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  880
 * SCR Title:  Remove .SEC
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Removed method ArchiveSEC.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   10 Jun 2013 16:44:20   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 017.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   21 May 2013 10:33:44   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  836
 * SCR Title:  Fix loading SEC bug
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Moved SVObserverEnums to SVConfigurationLibrary
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   20 May 2013 08:55:52   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  836
 * SCR Title:  Fix loading SEC bug
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fix loading problems with SEC files.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Apr 2013 18:39:40   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   11 Oct 2012 10:11:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to reflect fixing issues with XML functionality.
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
*    Rev 1.11   22 Jun 2007 10:02:22   Joe
* Project:  SVObserver
* Change Request (SCR) nbr:  598
* SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
* Checked in by:  jSpila;  Joseph Spila
* Change Description:  
*   These changes include removal of all VC6 constraints, project files, and workspace files.  The new VC8 project and solution files will replace the VC6 files with the new tighter compilation rules.
* 
* /////////////////////////////////////////////////////////////////////////////////////
* 
*    Rev 1.10   20 Jun 2007 13:44:58   Joe
* Project:  SVObserver
* Change Request (SCR) nbr:  598
* SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
* Checked in by:  jSpila;  Joseph Spila
* Change Description:  
*   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
* 
* /////////////////////////////////////////////////////////////////////////////////////
* 
*    Rev 1.9   10 Oct 2005 09:27:42   tbair
* Project:  SVObserver
* Change Request (SCR) nbr:  505
* SCR Title:  Encrypt security file
* Checked in by:  JimAdmin;  James A. Brown
* Change Description:  
*   Removed Test Code for encryption.
* 
* /////////////////////////////////////////////////////////////////////////////////////
* 
*    Rev 1.8   22 Sep 2005 16:01:16   tbair
* Project:  SVObserver
* Change Request (SCR) nbr:  505
* SCR Title:  Encrypt security file
* Checked in by:  JimAdmin;  James A. Brown
* Change Description:  
*   Add choice to Save to prevent encryption.
* 
* /////////////////////////////////////////////////////////////////////////////////////
* 
*    Rev 1.7   16 Sep 2005 16:16:36   jBrown
* Project:  SVObserver
* Change Request (SCR) nbr:  505
* SCR Title:  Encrypt security file
* Checked in by:  JimAdmin;  James A. Brown
* Change Description:  
*   Encryption was generically implemented through the SVXMLClass.  Because the configurations use this class, their interface to the SVXMLClass needed modified.  
* 
* Also, the configuration handling was modified to handle actual excrypted configurations because this was believed the best way to test the XML enryption.  Getting the version number needed enhanced since the part getting the version number has no encryption knowledge.
* 
* The following functions were modified:
*  SVOMArchive::CreateConfigurationFileFromTree ()
*  SVOMArchive::CreateTreeFromConfigurationFile ()
*  SVOMArchive::GetSVOSavedVersionNumberFromConfigurationFile (), and
*  SVOMArchive::FindVersion440_ ()
* 
* The following functions were added:
*  SVOMArchive::FindVersion440_Standard (), and
*  SVOMArchive::FindVersion440_RevisionHistory () 
* 
* /////////////////////////////////////////////////////////////////////////////////////
* 
*    Rev 1.6   13 Jan 2005 07:44:12   jBrown
* Project:  SVObserver
* Change Request (SCR) nbr:  375
* SCR Title:  Remodel XML usage for configuration storage and SIAC transfers.
* Checked in by:  JimAdmin;  James A. Brown
* Change Description:  
*   MSXML DOM behavior forced a switch from placing element values in element attributes to placing them in element text.
* 
* Modified....
* SVOCMArchive::CreateTreeFromConfigurationFile (), and
* SVOCMArchive::FindVersion440_ ().
* 
* 
* /////////////////////////////////////////////////////////////////////////////////////
* 
*    Rev 1.6   13 Jan 2005 07:43:36   jBrown
* Project:  SVObserver
* Change Request (SCR) nbr:  375
* SCR Title:  Remodel XML usage for configuration storage and SIAC transfers.
* Checked in by:  JimAdmin;  James A. Brown
* Change Description:  
*   MSXML DOM behavior forced a switch from placing element values in element attributes to placing them in element text.
* 
* Modified....
* SVOCMArchive::CreateTreeFromConfigurationFile (), and
* SVOCMArchive::FindVersion440_ ().
* 
* 
* /////////////////////////////////////////////////////////////////////////////////////
* 
*    Rev 1.5   22 Oct 2004 07:27:28   jBrown
* Project:  SVObserver
* Change Request (SCR) nbr:  375
* SCR Title:  Remodel XML usage for configuration storage and SIAC transfers.
* Checked in by:  JimAdmin;  James A. Brown
* Change Description:  
*   In order to implement the new XML parser classes, load routines where re-written and version detection methods where implemented so that old configuration files would load correctly.
* 
* Added headers for referencing:
*   USES_CONVERSION,
*   SVXMLClass,
*   SVCTreeControlClass,
*   SVBStr.
* 
* Rewrote and exchanged...
*   CreateTreeFromConfigurationFile () 
*   for
*   BuildTreeFromXMLDoc (),
* and
*   CreateConfigurationFileFromTree ()
*   for
*   CreateXMLDocFromTree ().
* 
* Added member functions:
*   GetSVOSavedVersionNumberFromConfigurationFile (),
*   FindVersion401_430 (), and
*   FindVersion440_ ().
* 
* 
* 
* 
* 
* 
* /////////////////////////////////////////////////////////////////////////////////////
* 
*    Rev 1.4   02 Oct 2003 10:43:02   rschock
* Project:  SVObserver
* Change Request (SCR) nbr:  386
* SCR Title:  Change Light Reference handling to match earlier versions of SVObserver
* Checked in by:  eBeyeler;  Eric Beyeler
* Change Description:  
*   Assigned old light reference values that had never been set to a new flag value for processing in the loading process.
* 
* /////////////////////////////////////////////////////////////////////////////////////
* 
*    Rev 1.3   29 Sep 2003 14:47:40   rschock
* Project:  SVObserver
* Change Request (SCR) nbr:  381
* SCR Title:  Combine PPQ Data to Inspection Data for a Digital Output
* Checked in by:  rSchock;  Rosco Schock
* Change Description:  
*   Fixed archive code to only conditionally read new combined settings.
* 
* /////////////////////////////////////////////////////////////////////////////////////
* 
*    Rev 1.2   25 Sep 2003 14:22:38   rschock
* Project:  SVObserver
* Change Request (SCR) nbr:  381
* SCR Title:  Combine PPQ Data to Inspection Data for a Digital Output
* Checked in by:  rSchock;  Rosco Schock
* Change Description:  
*   Added new configuration flags to support combined digital outputs.
* 
* /////////////////////////////////////////////////////////////////////////////////////
* 
*    Rev 1.1   09 Jan 2003 16:05:56   ebeyeler
* Project:  SVObserver
* Change Request (SCR) nbr:  317
* SCR Title:  Load SEC improperly imports Light Reference data for RGB Color
* Checked in by:  eBeyeler;  Eric Beyeler
* Change Description:  
*   Modified the ArchiveSVLightReferenceArrayClass and ArchiveSVLightReferenceClass functions to take as a parameter whether the system is a RGB Color or not (bIsColorMode).
* In ArchiveSVLightReferenceClass, if setting a default value for RGB contrast, check this flag before assigning the value. If color mode, default value=10000 otherwise default is 8100.
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