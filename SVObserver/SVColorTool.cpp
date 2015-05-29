//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVColorTool
//* .File Name       : $Workfile:   SVColorTool.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 11:07:28  $
//******************************************************************************

#include "stdafx.h"
#include "SVHBitmapUtilitiesLibrary/SVImageFormatEnum.h"
#include "SVColorTool.h"
#include "SVStatusLibrary/SVStatusCodes.h"
#include "SVInspectionProcess.h"
#include "SVColorThreshold.h"
#include "SVGlobal.h"
#include "SVRGBMainImage.h"

SV_IMPLEMENT_CLASS( SVColorToolClass, SVColorToolClassGuid );

SVColorToolClass::SVColorToolClass( BOOL BCreateDefaultTaskList, SVObjectClass* POwner, int StringResourceID )
				  :SVToolClass( BCreateDefaultTaskList, POwner, StringResourceID )
{
	init();
}

void SVColorToolClass::init()
{
	toolType		= 'C';

 	// Set up your type...
	outObjectInfo.ObjectTypeInfo.ObjectType = SVToolObjectType;
	outObjectInfo.ObjectTypeInfo.SubType    = SVColorToolObjectType;

	// Register Embedded Objects
	RegisterEmbeddedObject( &band0Image, SVBand0ImageObjectGuid, IDS_OBJECTNAME_BAND0_IMAGE );
	RegisterEmbeddedObject( &band1Image, SVBand1ImageObjectGuid, IDS_OBJECTNAME_BAND1_IMAGE );
	RegisterEmbeddedObject( &band2Image, SVBand2ImageObjectGuid, IDS_OBJECTNAME_BAND2_IMAGE );

	RegisterEmbeddedObject( &convertToHSI, SVConvertToHSIObjectGuid, IDS_OBJECTNAME_CONVERT_TO_HSI, true, SVResetItemIP );

	// Register SourceImageNames Value Object
	RegisterEmbeddedObject( &m_svSourceImageNames, SVSourceImageNamesGuid, IDS_OBJECTNAME_SOURCE_IMAGE_NAMES, false, SVResetItemTool );


	band0Image.InitializeImage( SVImageTypeDependent );
	band1Image.InitializeImage( SVImageTypeDependent );
	band2Image.InitializeImage( SVImageTypeDependent );

	extentTop.SetDefaultValue( 0.0, TRUE );
	extentLeft.SetDefaultValue( 0.0, TRUE );

	m_svToolExtent.SetTranslation( SVExtentTranslationNone );

	// Add the Color Threshold class object
	SVColorThresholdClass* pColorThreshold = new SVColorThresholdClass;
	if( pColorThreshold )
		Add( pColorThreshold );
}

SVColorToolClass::~SVColorToolClass()
{
}

BOOL SVColorToolClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = FALSE;

	if( SVToolClass::CreateObject( PCreateStructure ) )
	{
		// Create 3 output images, one for each band...
		SVImageClass* pInputImage = NULL;

		BOOL useHSI = TRUE;
		// Create HSI Image if required
		convertToHSI.GetValue( useHSI );

		if( useHSI )
		{
			pInputImage = GetHSIImage();
		}
		else
		{
			pInputImage = GetRGBImage();
		}

		if( pInputImage )
		{
			// Create 3 child layers...

			// Create band 0 Layer
			bOk = createBandChildLayer( band0Image, pInputImage, 0 );

			// Create Band 1 Layer
			bOk &= createBandChildLayer( band1Image, pInputImage, 1 );

			// Create Band 2 Layer...
			bOk &= createBandChildLayer( band2Image, pInputImage, 2 );
		}
	}
	
	extentTop.SetDefaultValue( 0.0, TRUE );
	extentLeft.SetDefaultValue( 0.0, TRUE );

	// Set / Reset Printable Flag
	convertToHSI.ObjectAttributesAllowedRef() |= SV_PRINTABLE;

	m_svSourceImageNames.ObjectAttributesAllowedRef() &=~SV_REMOTELY_SETABLE & ~SV_SETABLE_ONLINE;

	isCreated = bOk;

	return bOk;
}

BOOL SVColorToolClass::CloseObject()
{
	return SVToolClass::CloseObject();
}

HRESULT SVColorToolClass::ResetObject()
{
	HRESULT l_hrOk = S_OK;

	// Create 3 output images, one for each band...
	SVImageClass* pInputImage = NULL;

	BOOL useHSI = TRUE;

	// Create HSI Image if required
	convertToHSI.GetValue( useHSI );

	if( useHSI )
	{
		pInputImage = GetHSIImage();
	}
	else
	{
		pInputImage = GetRGBImage();
	}

	createBandChildLayer( band0Image, pInputImage, 0 );

	band0Image.ResetObject();

	// Create Band 1 Layer
	createBandChildLayer( band1Image, pInputImage, 1 );

	band1Image.ResetObject();

	// Create Band 2 Layer...
	createBandChildLayer( band2Image, pInputImage, 2 );

	band2Image.ResetObject();

	if( SVToolClass::ResetObject() != S_OK )
	{
		l_hrOk = S_FALSE;
	}

	CollectInputImageNames();

	UpdateImageWithExtent( 1 );

	return l_hrOk;
}

HRESULT SVColorToolClass::DoesObjectHaveExtents() const
{
	return SV_SUCCESS_SVOBSERVER_2000_DOESNOTHAVE_EXTENTS;
}

HRESULT SVColorToolClass::UpdateImageWithExtent( unsigned long p_Index )
{
	HRESULT l_hrOk = S_OK;

	l_hrOk = m_svToolExtent.UpdateImageWithExtent( p_Index, SVToolExtentClass::SVColorToolExtent );

	return l_hrOk;
}

BOOL SVColorToolClass::onRun( SVRunStatusClass& RRunStatus )
{
	// Create 3 output images, one for each band...
	SVImageClass* pInputImage = NULL;

	BOOL useHSI = TRUE;

	// Create HSI Image if required
	convertToHSI.GetValue( useHSI );

	if( useHSI )
	{
		pInputImage = GetHSIImage();
	}
	else
	{
		pInputImage = GetRGBImage();
	}

	if( pInputImage != NULL )
	{
		if( band0Image.GetLastResetTimeStamp() <= pInputImage->GetLastResetTimeStamp() )
		{
			band0Image.ResetObject();
		}

		if( band1Image.GetLastResetTimeStamp() <= pInputImage->GetLastResetTimeStamp() )
		{
			band1Image.ResetObject();
		}

		if( band2Image.GetLastResetTimeStamp() <= pInputImage->GetLastResetTimeStamp() )
		{
			band2Image.ResetObject();
		}
	}

	return SVToolClass::onRun( RRunStatus );
}

BOOL SVColorToolClass::createBandChildLayer( SVImageClass& p_rOutputImage, SVImageClass* p_pInputImage, long p_BandLink )
{
	BOOL l_bOk = FALSE;

	SVGUID l_InputID;
	SVImageInfoClass ImageInfo;

	if( p_pInputImage != NULL )
	{
		l_InputID = p_pInputImage->GetUniqueObjectID();
		ImageInfo = p_pInputImage->GetImageInfo();
	}
	else
	{
		ImageInfo = p_rOutputImage.GetImageInfo();
	}

	// Setup...
	ImageInfo.SetOwner( GetUniqueObjectID() );
	ImageInfo.SetImageProperty( SVImagePropertyFormat, SVImageFormatMono8 );
	ImageInfo.SetImageProperty( SVImagePropertyBandNumber, 1 );
	ImageInfo.SetImageProperty( SVImagePropertyBandLink, p_BandLink );
	
	// Restore Output Image Extents
	// Note: this must now be done after setting SVImageInfoClass::POwnerTool
	// So Shadowed Extents get Updated
			
	// Try to create image object...
	l_bOk = ( p_rOutputImage.InitializeImage( l_InputID, ImageInfo ) == S_OK );

	return l_bOk;
}

SVImageClass* SVColorToolClass::GetRGBImage()
{
	SVImageClass* pImage = NULL;

	// Ask the Document for this directly...

	SVInspectionProcess* pInspection = GetInspection();
	if( nullptr != pInspection )
	{
		pImage = pInspection->GetRGBMainImage();
	}

	return pImage;
}

SVImageClass* SVColorToolClass::GetHSIImage()
{
	SVImageClass* pImage = NULL;

	// Ask the Document for this directly...

	SVInspectionProcess* pInspection = GetInspection();
	if( nullptr != pInspection )
	{
		pImage = pInspection->GetHSIMainImage();
	}

	return pImage;
}

SVBoolValueObjectClass* SVColorToolClass::GetConvertToHSIVariable()
{
	return &convertToHSI;
}

DWORD_PTR SVColorToolClass::processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext )
{
	DWORD_PTR DwResult = NULL;
	
	return( SVToolClass::processMessage( DwMessageID, DwMessageValue, DwMessageContext ) | DwResult );
}

// Set String value object for Source Image Names
HRESULT SVColorToolClass::CollectInputImageNames( )
{
	CString l_strName;
	SVImageClass* l_pImage;
	bool l_bConvertToHSI;
	HRESULT l_hr = convertToHSI.GetValue( l_bConvertToHSI );
	if( l_hr == S_OK )
	{
		if( l_bConvertToHSI )
		{
			l_pImage = GetHSIImage();
		}
		else
		{
			l_pImage = GetRGBImage();
		}
		if( l_pImage != NULL )
		{
			l_strName = l_pImage->GetCompleteObjectName();
			m_svSourceImageNames.SetDefaultValue( l_strName, true );
		}
	}
	return l_hr;
}

HRESULT SVColorToolClass::GetInputImageNames( SVStringValueObjectClass*& p_pSourceNames )
{
	p_pSourceNames = &m_svSourceImageNames;
	return S_OK;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVColorTool.cpp_v  $
 * 
 *    Rev 1.2   15 May 2014 11:07:28   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Revised processMessage to use DWORD_PTR
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   01 Feb 2014 10:23:14   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed sendmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 09:52:40   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.29   18 Jul 2012 13:23:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed obsolete methods assiciated with overlay drawling and fixed problem with copying data to byte buffer have wrong format.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.28   02 Jul 2012 16:39:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.27   13 Apr 2011 14:59:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated system to fix color processing issues with color configurations.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.26   30 Mar 2011 16:20:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to fix performance problems with the extent changes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.25   21 Mar 2011 12:05:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated system to fix performance issues with extent checks and updates.  Added additional methods to determine if object has extents and moved extent updated to  associated children.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.24   16 Mar 2011 13:23:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated extent functionality and renamed extent methods to correct extent issues and add clarity to the logical flow.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.23   11 Mar 2011 14:17:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix extent and connection issues between image classes and tool classes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.22   01 Mar 2011 10:13:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source to fix issues with not assigning input image to extents.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.21   25 Feb 2011 12:07:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated classes needed to consolidate SVImageClass and SVImageObjectClass updating calls to one method name.  Fixed updating problems with child buffers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   27 Jan 2011 10:58:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   08 Dec 2010 07:45:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   04 Nov 2010 13:30:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with image processing and display image processing classes and associated includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   30 Jul 2009 11:18:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branced changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   09 Apr 2008 07:42:34   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  624
 * SCR Title:  Fix problems with Color Tool when it gets deleted
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Replaced Get Parent operator with Get Parent Image operator when accessing the parent information of the Image Class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   06 Mar 2008 15:16:02   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  616
 * SCR Title:  Add Server-side Configuration Data Def Lists to the SVObserver program
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Source Image Names for tools with input image
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14.2.0   14 Jul 2009 12:27:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to include new error codes and new get parent image method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14.1.0   07 Apr 2008 07:45:34   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  621
 * SCR Title:  Fix Bug in the SVImageClass
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated methods to use the new GetParentImage method instead of the GetParent method to remove confusion with CWnd GetParent method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   16 Jan 2006 17:01:08   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  534
 * SCR Title:  Add Image Select for Auxiliary Extent Source
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to change to using new update methodology for the Auxiliary Extents.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   06 Oct 2005 07:19:00   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  510
 * SCR Title:  Add source image extents to all image using tools
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated object to make Top and Left extent both 0 instead of 10 and changed translation type to none.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   01 Sep 2005 12:37:02   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  510
 * SCR Title:  Add source image extents to all image using tools
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Removed UpdateSourceExtents return code from consideration for validity of operator during a Reset Object Call.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   01 Sep 2005 12:06:44   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  510
 * SCR Title:  Add source image extents to all image using tools
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modifications to UpdateImageData for Auxilliary Extents.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   29 Jul 2005 12:17:30   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated object initialization to change the parameters for RegisterEmbeddedObject for a SVValueObjectClass.  The two additional parameters will inform the preperation process before toolset execution.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   21 Jun 2005 08:02:14   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   object attributes now use accessor methods
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   07 Mar 2005 12:15:30   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed input connection data to HSI and RGB images since it is un-necessary.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   25 Feb 2005 11:04:46   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Modified CreateBandChildLayer to always set the property ImageFormat to SVImageFormatMono8.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   17 Feb 2005 13:24:18   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   10 Nov 2003 12:23:00   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  395
 * SCR Title:  Color ROI does not update unless you move ROI in training
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Made changes to the RGBMainImage and ColorTool to reduce possibilities of getting out of sync and to consolidate the color image specific code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   17 Apr 2003 17:16:34   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   28 Mar 2003 16:02:12   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Updated class to fix HSI problem.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   29 Jan 2003 12:55:36   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated CreateObject and createBandChildLayer methods to use the new image circle buffer methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   09 Dec 2002 19:01:02   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  295
 * SCR Title:  Remove Result Data from Configuration Printout
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   CreateObject method was modified to properly update the isCreated flag and to set the printability of the internal value objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   06 Sep 2000 16:39:50   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  179
 * SCR Title:  Create Color SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   New files added to the SVObserver project for color SVIM changes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
