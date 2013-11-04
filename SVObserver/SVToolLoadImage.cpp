//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolLoadImage
//* .File Name       : $Workfile:   SVToolLoadImage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   07 May 2013 08:29:16  $
//******************************************************************************

#include "stdafx.h"
#include "SVToolLoadImage.h"
#include "SVAnalyzer.h"
#include "SVGlobal.h"
#include "SVImageProcessingClass.h"
#include "SVInspectionProcess.h"
#include "SVToolSet.h"

//******************************************************************************
//* DEFINITIONS OF MODULE-LOCAL VARIABLES:
//******************************************************************************

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//******************************************************************************
//* CLASS METHOD IMPLEMENTATION(S):
//******************************************************************************





//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/
//* Class Name : SVLoadImageToolClass
//* Note(s)    : 
//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/

//******************************************************************************
// Adjustments
//******************************************************************************

SV_IMPLEMENT_CLASS( SVLoadImageToolClass, SVLoadImageToolClassGuid );

//******************************************************************************
// Constructor(s):
//******************************************************************************

SVLoadImageToolClass::SVLoadImageToolClass( BOOL BCreateDefaultTaskList, SVObjectClass* POwner, int StringResourceID )
					 :SVToolClass( BCreateDefaultTaskList, POwner, StringResourceID )
{
	init();
}


void SVLoadImageToolClass::init()
{
	// Set up your type...
	outObjectInfo.ObjectTypeInfo.ObjectType = SVToolObjectType;
	outObjectInfo.ObjectTypeInfo.SubType    = SVToolLoadImageObjectType;

	// Identify our input type needs
	
	// Register Embedded Objects
	RegisterEmbeddedObject( &m_fileImage, SVOutputImageObjectGuid, IDS_OBJECTNAME_IMAGE1 );
	RegisterEmbeddedObject( &m_currentPathName, SVPathNameObjectGuid, IDS_OBJECTNAME_PATHNAME, false, SVResetItemTool );
	RegisterEmbeddedObject( &m_continuousReload, SVContinuousReloadObjectGuid, IDS_OBJECTNAME_CONTINUOUS_RELOAD, false, SVResetItemNone );

	// Set Embedded defaults
	m_currentPathName.SetDefaultValue( _T( "" ), TRUE );
	m_continuousReload.SetDefaultValue( FALSE, TRUE );

	m_fileImage.InitializeImage( SVImageTypePhysical );

	// Default taskObjectList items:

	// Set Translation
	m_svToolExtent.SetTranslation(SVExtentTranslationFigureShift);

	// Set default inputs and outputs
	addDefaultInputObjects();
}


//******************************************************************************
// Destructor(s):
//******************************************************************************

SVLoadImageToolClass::~SVLoadImageToolClass()
{ 
}

//******************************************************************************
// Operator(s):
//******************************************************************************

BOOL SVLoadImageToolClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = FALSE;

	if( SVToolClass::CreateObject( PCreateStructure ) )
	{
		SVImageClass* pImage = NULL;

		// Create Image...
		SVImageInfoClass l_svImageInfo = m_fileImage.GetImageInfo();

		// Setup...
		l_svImageInfo.SetOwner( GetUniqueObjectID() );
		l_svImageInfo.SetImageProperty( SVImagePropertyFormat, SVImageFormatMono8 );
		l_svImageInfo.SetImageProperty( SVImagePropertyBandNumber, 1 );
		l_svImageInfo.SetImageProperty( SVImagePropertyBandLink, 0 );
		l_svImageInfo.SetImageProperty(SVImagePropertyPixelDepth,8);

		bOk = ( m_fileImage.UpdateImage( l_svImageInfo ) == S_OK );
	}

	// Set / Reset Printable Flags
	m_fileImage.ObjectAttributesAllowedRef() |= SV_REMOTELY_SETABLE | SV_SETABLE_ONLINE;
	m_currentPathName.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	m_continuousReload.ObjectAttributesAllowedRef() |= SV_PRINTABLE;

	isCreated = bOk;

	return bOk;
}

BOOL SVLoadImageToolClass::CloseObject()
{
	if( SVToolClass::CloseObject() )
	{
		BOOL bRetVal = m_fileImage.CloseObject();

		return bRetVal;
	}
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
//
// Make sure we have an image file to load.
//
BOOL SVLoadImageToolClass::OnValidate ()
{
	if (SVToolClass::OnValidate ())
	{
		CString strPathName;
		m_currentPathName.GetValue( strPathName );
		
		if( ::SVFileExists((LPCTSTR)strPathName) ) // No can do! || strPathName.IsEmpty())
		{
			return TRUE;
		}
	}
	SetInvalid ();
	return FALSE;
}


BOOL SVLoadImageToolClass::onRun( SVRunStatusClass& RRunStatus )
{
	// All inputs and outputs must be validated first
	if( SVToolClass::onRun( RRunStatus ) )
	{
		BOOL bReload = FALSE;
		CString strImagePathName;

		if( S_OK != m_continuousReload.GetValue( bReload ) )
		{
			RRunStatus.SetInvalid();
			SetInvalid();
			return FALSE;
		}
		
		if( S_OK != m_currentPathName.GetValue( strImagePathName ) )
		{
			RRunStatus.SetInvalid();
			SetInvalid();
			return FALSE;
		}

		if( bReload || m_bResetFileImage)
		{
			if( m_fileImage.LoadImage( strImagePathName, RRunStatus.Images ) != S_OK )
			{
				RRunStatus.SetInvalid();
				SetInvalid();

				return FALSE;
			}

			m_bResetFileImage = false;
		}
		else
		{
			//copy forward
			if( ! m_fileImage.CopyImageTo( RRunStatus.Images ) )
			{
				RRunStatus.SetInvalid();
				SetInvalid();

				return FALSE;
			}
		}
		
		return TRUE;
	}

	RRunStatus.SetInvalid();

	return FALSE;
}

HRESULT SVLoadImageToolClass::ResetObject()
{
	HRESULT l_hrOk = S_OK;
	
	if ( SVToolClass::ResetObject() != S_OK )
	{
		l_hrOk = S_FALSE;
	}

	m_bResetFileImage = l_hrOk == S_OK;

	UpdateImageWithExtent( 1 );

	return l_hrOk;
}

HRESULT SVLoadImageToolClass::IsInputImage(SVImageClass *p_psvImage)
{
	HRESULT l_hrOk = S_FALSE;

	if ( p_psvImage != NULL )
	{
		if ( p_psvImage == pCurrentToolSet->getCurrentImage() )
		{
			l_hrOk = S_OK;
		}
		else
		{
			l_hrOk = SVToolClass::IsInputImage( p_psvImage );
		}
	}

	return l_hrOk;
}


SVTaskObjectClass *SVLoadImageToolClass::GetObjectAtPoint( const SVExtentPointStruct &p_rsvPoint )
{
	SVImageExtentClass l_svExtents;

	SVTaskObjectClass *l_psvObject = NULL;

	if( m_svToolExtent.GetImageExtent( l_svExtents ) == S_OK &&
	    l_svExtents.GetLocationPropertyAt( p_rsvPoint ) != SVExtentLocationPropertyUnknown )
	{
		l_psvObject = this;
	}

	return l_psvObject;
}

HRESULT SVLoadImageToolClass::DoesObjectHaveExtents() const
{
	return S_OK;
}

HRESULT SVLoadImageToolClass::SetImageExtent( unsigned long p_ulIndex, SVImageExtentClass p_svImageExtent )
{
	HRESULT l_hrOk = S_FALSE;

	SVExtentFigureStruct l_svFigure;

	p_svImageExtent.GetFigure(l_svFigure);

	if ( (l_svFigure.m_svTopLeft.m_dPositionX >= 0) && (l_svFigure.m_svTopLeft.m_dPositionY >= 0) )
	{
		l_hrOk = SVToolClass::SetImageExtent( p_ulIndex, p_svImageExtent );
	}
	else
	{
		l_hrOk = S_FALSE;
	}

	return l_hrOk;
}

HRESULT SVLoadImageToolClass::SetImageExtentToParent(unsigned long p_ulIndex )
{
	HRESULT l_hrOk = S_OK;
	SVImageExtentClass l_NewExtent;
	CString strImagePathName;

	if( l_hrOk == S_OK )
	{
		l_hrOk = m_currentPathName.GetValue( strImagePathName );
	}

	if( l_hrOk == S_OK )
	{
		l_hrOk = m_fileImage.LoadImageFullSize( strImagePathName, l_NewExtent);
	}
		
	if( l_hrOk == S_OK )
	{
		l_hrOk = SVToolClass::SetImageExtent( p_ulIndex, l_NewExtent );
	}
	return l_hrOk;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVToolLoadImage.cpp_v  $
 * 
 *    Rev 1.1   07 May 2013 08:29:16   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 011.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.48   19 Apr 2013 09:22:42   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  827
 * SCR Title:  Add button to the adjust tool size/position to set size to parent image.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Full Image button support to Load Image Tool.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 15:45:52   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.47   25 Jul 2012 14:54:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed dead code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.46   16 Jul 2012 15:53:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   removed Update Overlay Name due to a correction in the Task Object class to collect the correct name point.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.45   16 Jul 2012 14:55:20   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new virtual method UpdateOverlayName
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.44   16 Jul 2012 13:03:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated overlay collection functionality and removed duplicate fucntionality by consolidating it to the task object class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.43   02 Jul 2012 17:50:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.42   30 Mar 2011 16:28:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to fix performance problems with the extent changes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.41   21 Mar 2011 12:19:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated system to fix performance issues with extent checks and updates.  Added additional methods to determine if object has extents and moved extent updated to  associated children.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.40   16 Mar 2011 13:59:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated extent functionality and renamed extent methods to correct extent issues and add clarity to the logical flow.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.39   11 Mar 2011 14:33:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix extent and connection issues between image classes and tool classes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.38   25 Feb 2011 12:25:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated classes needed to consolidate SVImageClass and SVImageObjectClass updating calls to one method name.  Fixed updating problems with child buffers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.37   27 Jan 2011 12:03:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.36   04 Nov 2010 14:15:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with image processing and display image processing classes and associated includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.35   01 Jun 2010 15:16:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to improve ability to track performance.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.34   30 Jul 2009 13:09:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branched changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.33   07 Apr 2008 09:13:48   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  621
 * SCR Title:  Fix Bug in the SVImageClass
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated methods to use the new GetParentImage method instead of the GetParent method to remove confusion with CWnd GetParent method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.32   02 Nov 2005 08:39:44   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  512
 * SCR Title:  Add Published Result Images functionality
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed RunStatus.m_lResultImageIndex to RunStatus.Images
 * added m_ to member variables
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.31   10 Aug 2005 12:07:44   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  487
 * SCR Title:  Add new blob feature for source extents of blob box center
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Changed Translation type to SVExtentTranslationFigureShift
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.30   29 Jul 2005 13:32:40   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated object initialization to change the parameters for RegisterEmbeddedObject for a SVValueObjectClass.  The two additional parameters will inform the preperation process before toolset execution.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.29   21 Jun 2005 08:36:28   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   object attributes now use accessor methods
 * value object functions now use HRESULT
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.28   22 Feb 2005 10:50:52   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed load image tool problems.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.27   17 Feb 2005 13:41:02   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added Reset Methodology to support new extents.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.26   20 Nov 2003 11:52:08   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  385
 * SCR Title:  Expose additional parameters for SIAC
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed attributes for the value objects to be setable online
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.25   28 Oct 2003 09:19:04   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  380
 * SCR Title:  Add Image Mask option
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Made change to force load image tool image to band 0 since
 * it is always a Mono 8-bit image.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.24   08 Jul 2003 09:04:28   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Updated loadImage and CreateObject methods to used updated SVImageProcessingClass and remove Coreco specific elements.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.23   22 May 2003 10:25:08   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  363
 * SCR Title:  Load Image Tool on Color SVIM does not work correctly
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   During the CreateObject, the ImageInfo will be always be set to a 1 band 8bit mono image
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.22   22 Apr 2003 16:55:36   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.21   21 Mar 2003 12:34:04   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  330
 * SCR Title:  Add SetImageList to ActiveX Interface
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   checks the m_bFileSetup flag to determine if it should load the image
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.20   19 Mar 2003 13:31:40   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  330
 * SCR Title:  Add SetImageList to ActiveX Interface
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   set the attribute for fileImage to allow it to be set remotely
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.19   13 Mar 2003 13:35:18   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  327
 * SCR Title:  Load Image Tool - unless the reload is checked you can not preview the image
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Fixed problem - if loadimage fails it will set the tool to be invalid.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.18   04 Mar 2003 11:43:54   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  327
 * SCR Title:  Load Image Tool - unless the reload is checked you can not preview the image
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   In LoadImage, instead of doing the DestroyImageBuffer will now just do a ClearBuffer.
 * In onRun, will load the image if continuous reload is on or if the system is not running
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.17   31 Jan 2003 09:23:36   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated CreateObject, loadImage, onUpdateFigure and onRun methods to use the new image circle buffer methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.16   10 Jan 2003 16:16:00   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  305
 * SCR Title:  Implement the ability to perform RunOnce from a SIAC client
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Made changes related to changing the SVDrawObjectClass to work in a consistent manner with SVDrawObjectListClass.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.15   09 Dec 2002 20:14:28   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  295
 * SCR Title:  Remove Result Data from Configuration Printout
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   CreateObject method was modified to properly update the isCreated flag and to set the printability of the internal value objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.14   20 Nov 2002 14:38:50   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Removed System Class
 * Change loadImagebuffer
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.13   02 Oct 2001 15:43:42   ed
 * Project:  SVObserver
 * Change Request (SCR) nbr:  241
 * SCR Title:  Color SVIM:  Load image tool output not available in window tool as input.
 * Checked in by:  Ed;  Ed Chobanoff
 * Change Description:  
 *   Modified function, SVLoadImageToolClass::loadImage.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.12   13 Jun 2001 10:29:16   ed
 * Project:  SVObserver
 * Change Request (SCR) nbr:  200
 * SCR Title:  Update code to implement SVIM COM Server
 * Checked in by:  Ed;  Ed Chobanoff
 * Change Description:  
 *    Modified the following functions: init, CreateObject, processMessage.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.11   19 Apr 2001 20:00:32   Steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  196
 * SCR Title:  Restructure Scripted Load/Save Procedures Functional Requirement
 * Checked in by:  Steve;  Stephen E. Steffan
 * Change Description:  
 *   Header file changes due to include file restructuring.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.10   12 Apr 2001 14:05:30   cSchmittinger
 * Project:  SVObserver
 * Change Request (SCR) nbr:  191
 * SCR Title:  Restructure File Management
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Browse needs to select a file not just a path.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.9   12 Apr 2001 13:03:58   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  191
 * SCR Title:  Restructure File Management
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   This revision is to correct a problem with an incorret call to extract a full file name from the SVFileNameClass.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.8   12 Apr 2001 12:58:34   joe
 * No changes to files.  Need to attempt to clear user error in tracker.
 * 
 *    Rev 3.7   25 Jan 2001 16:19:52   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  191
 * SCR Title:  Restructure File Management
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   These changes fixed the SVObserverApp file handling and associated systems.  These systems include the .SEC, .IOD, and .IPD files as well as the camera files.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.6   04 Mar 2000 11:42:46   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  105
 * SCR Title:  Handling of Tool Figures in Absolute, Relative, and Transformed Views
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Removed handler for Move/Sizing messages.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.5   03 Mar 2000 13:53:46   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  112
 * SCR Title:  Expose Extents of Images
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to use SVImageInfoClass methods to get/set image
 * extents.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   25 Feb 2000 16:11:46   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  105
 * SCR Title:  Handling of Tool Figures in Absolute, Relative, and Transformed Views
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised handler for SVM_GETFIRST_MOVEABLE/RESIZEABLE
 * to restirct moving if output image has dependents.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   21 Feb 2000 15:04:18   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  105
 * SCR Title:  Handling of Tool Figures in Absolute, Relative, and Transformed Views
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised CreateObject to set Display context image.
 * Revised onUpdateFigure to use relative extents.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   Jan 03 2000 14:18:46   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  75
 * SCR Title:  Image tool Does not Update if Load Image changes
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Add code to load the image during system message 'Reset All Objects'.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   Nov 30 1999 16:21:34   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  69
 * SCR Title:  Load Image tool fails to load image file
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Repaired loading saved image tool.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 13:51:36   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   Sep 24 1999 08:59:18   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  4
 * SCR Title:  Add "Load image" option
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Provides for tool for loading an image from a file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
