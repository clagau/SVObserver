//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVAnalyzer.cpp
//* .File Name       : $Workfile:   SVAnalyzer.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 10:09:10  $
//******************************************************************************

#include "stdafx.h"
#include "SVAnalyzer.h"
#include "SVInspectionProcess.h"
#include "SVTool.h"
#include "SVImageLibrary/SVImageInfoClass.h"
#include "SVImageClass.h"
#include "SVObjectLibrary/SVAnalyzerLevelCreateStruct.h"
#include "SVImageLibrary/SVDrawContext.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SV_IMPLEMENT_CLASS( SVAnalyzerClass, SVAnalyzerClassGuid );

SVAnalyzerClass::SVAnalyzerClass( LPCSTR ObjectName )
				:SVTaskObjectListClass( ObjectName ) 
{
	init();
}

SVAnalyzerClass::SVAnalyzerClass( BOOL BCreateDefaultTaskList, SVObjectClass* POwner, int StringResourceID )
				:SVTaskObjectListClass( BCreateDefaultTaskList, POwner, StringResourceID ) 
{
	init();
}

// Initialization of newly Instantiated Object
void SVAnalyzerClass::init()
{
	pAnalyzerResult			= NULL;
	isDirty					= FALSE;

	// Indentify our type in the Output List
	outObjectInfo.ObjectTypeInfo.ObjectType = SVAnalyzerObjectType;

	// Set up the Defualt Inputs/Outputs
	addDefaultInputObjects();

}

SVAnalyzerClass::~SVAnalyzerClass()
{

}

BOOL SVAnalyzerClass::CloseObject()
{
	return SVTaskObjectListClass::CloseObject();
}

BOOL SVAnalyzerClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = FALSE;

	if( SVTaskObjectClass::CreateObject( PCreateStructure ) )
	{
		if( GetInspection() && GetTool() )
		{
			isDirty	= FALSE;

			bOk = TRUE;
		}
	}

	isCreated = bOk;

	return bOk;
}

void SVAnalyzerClass::MakeDirty()
{
	isDirty = TRUE;
}

SVResultClass* SVAnalyzerClass::GetResultObject()
{
	SVObjectTypeInfoStruct info;
	info.ObjectType = SVResultObjectType;
	SVResultClass* pResult = reinterpret_cast<SVResultClass*>(SVSendMessage( this, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<DWORD_PTR>(&info) ) );
	return pResult;
}

DWORD_PTR SVAnalyzerClass::processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext )
{
	DWORD_PTR DwResult = NULL;

	SVAnalyzerLevelCreateStruct createStruct;

	// Try to process message by yourself...
	DWORD dwPureMessageID = DwMessageID & SVM_PURE_MESSAGE;
	switch( dwPureMessageID )
	{
		case SVMSGID_CREATE_ALL_OBJECTS:
		{
			if( !IsCreated() && !CreateObject( ( SVObjectLevelCreateStruct* ) DwMessageValue ) )
			{
				ASSERT( FALSE );

				DwResult = SVMR_NO_SUCCESS;
			}
			else
			{
				DwResult = SVMR_SUCCESS;
			}

			createStruct.OwnerObjectInfo        = this;
			createStruct.AnalyzerObjectInfo		= this;
			createStruct.ToolObjectInfo			= GetTool();
			createStruct.InspectionObjectInfo	= GetInspection();

			DwMessageValue = reinterpret_cast<DWORD_PTR>(&createStruct);

			break;
		}

		case SVMSGID_CONNECT_ALL_OBJECTS:
		{
			if( ConnectObject( ( SVObjectLevelCreateStruct* ) DwMessageValue ) != S_OK )
			{
				ASSERT( FALSE );

				DwResult = SVMR_NO_SUCCESS;
			}
			else
			{
				DwResult = SVMR_SUCCESS;
			}

			createStruct.OwnerObjectInfo        = this;
			createStruct.AnalyzerObjectInfo		= this;
			createStruct.ToolObjectInfo			= GetTool();
			createStruct.InspectionObjectInfo	= GetInspection();

			DwMessageValue = reinterpret_cast<DWORD_PTR>(&createStruct);

			break;
		}

		case SVMSGID_CREATE_CHILD_OBJECT:
		{
			// ...use second message parameter ( DwMessageValue ) as SVObjectClass* of the child object
			// ...returns SVMR_SUCCESS, SVMR_NO_SUCCESS or SVMR_NOT_PROCESSED
			SVObjectClass* pChildObject = ( SVObjectClass* ) DwMessageValue;
			return CreateChildObject(pChildObject, static_cast<DWORD>(DwMessageContext));
		}

		case SVMSGID_CONNECT_CHILD_OBJECT:
		{
			// ...use second message parameter ( DwMessageValue ) as SVObjectClass* of the child object
			SVObjectClass* pChildObject = ( SVObjectClass* ) DwMessageValue;
			if( SV_IS_KIND_OF( pChildObject, SVObjectClass ) )
			{
				createStruct.OwnerObjectInfo        = this;
				createStruct.AnalyzerObjectInfo		= this;
				createStruct.ToolObjectInfo			= GetTool();
				createStruct.InspectionObjectInfo	= GetInspection();
				
				DWORD_PTR l_Return = SVSendMessage( pChildObject, SVM_CONNECT_ALL_OBJECTS, reinterpret_cast<DWORD_PTR>(&createStruct), NULL );

				return l_Return;
			}
			return SVMR_NOT_PROCESSED;
		}

		case SVMSGID_DISCONNECT_IMAGE_OBJECT:
		{
			SVAnalyzerClass * l_pAnalyzer = reinterpret_cast<SVAnalyzerClass *>(DwMessageValue);
			l_pAnalyzer->DisconnectImages();
			return SVMR_SUCCESS;
		}
	}
	return( SVTaskObjectListClass::processMessage( DwMessageID, DwMessageValue, DwMessageContext ) | DwResult );
}

DWORD_PTR SVAnalyzerClass::createAllObjectsFromChild( SVObjectClass* pChildObject )
{
	SVAnalyzerLevelCreateStruct createStruct;
	createStruct.OwnerObjectInfo        = this;
	createStruct.AnalyzerObjectInfo		= this;
	createStruct.ToolObjectInfo			= GetTool();
	createStruct.InspectionObjectInfo	= GetInspection();
	
	return SVSendMessage( pChildObject, SVM_CREATE_ALL_OBJECTS, reinterpret_cast<DWORD_PTR>(&createStruct), NULL );
}

SV_IMPLEMENT_CLASS( SVImageAnalyzerClass, SVImageAnalyzerClassGuid );

SVImageAnalyzerClass::SVImageAnalyzerClass( LPCSTR ObjectName )
					 :SVAnalyzerClass( ObjectName ) 
{
	init();
}

SVImageAnalyzerClass::SVImageAnalyzerClass( BOOL BCreateDefaultTaskList, SVObjectClass* POwner, int StringResourceID )
					 :SVAnalyzerClass( BCreateDefaultTaskList, POwner, StringResourceID ) 
{
	init();
}

void SVImageAnalyzerClass::init()
{
	// Set object type info...
	outObjectInfo.ObjectTypeInfo.ObjectType = SVAnalyzerObjectType;
	// Set sub type only in derived classes!

	// Set Input requirement
	inputImageObjectInfo.SetInputObjectType( SVImageObjectType );
	inputImageObjectInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &inputImageObjectInfo, _T( "ImageAnalyzerImage" ) );

	addDefaultInputObjects();
}

SVImageAnalyzerClass::~SVImageAnalyzerClass()
{
}

BOOL SVImageAnalyzerClass::CloseObject()
{
	return SVAnalyzerClass::CloseObject();
}

BOOL SVImageAnalyzerClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	isCreated = SVAnalyzerClass::CreateObject( PCreateStructure );

	return isCreated;
}

SVImageClass* SVImageAnalyzerClass::getInputImage()
{
	if( inputImageObjectInfo.IsConnected() && inputImageObjectInfo.GetInputObjectInfo().PObject )
		return ( SVImageClass* ) inputImageObjectInfo.GetInputObjectInfo().PObject;

	return NULL;
}

unsigned long SVImageAnalyzerClass::GetInputPixelDepth ()
{
    SVImageClass *pImage;
    SVImageInfoClass ImageInfo; 

    unsigned long   ulPixelDepth;

//  Calling function must clear last error.

    while (1)
    {
        pImage = getInputImage ();
        if (!pImage)
        {
            msvError.msvlErrorCd = -1101;
            SV_TRAP_ERROR_BRK_TSTFIRST (msvError, 1101);
        }

        ulPixelDepth = pImage->getPixelDepth();
        break;
    }

    if (msvError.GetLastErrorCd () & SV_ERROR_CONDITION)
        return msvError.GetLastErrorCd ();

    return ulPixelDepth;
}

BOOL SVImageAnalyzerClass::OnValidate()
{
	BOOL l_bOk = TRUE;
		

	l_bOk = l_bOk && inputImageObjectInfo.IsConnected();
	l_bOk = l_bOk && inputImageObjectInfo.GetInputObjectInfo().PObject != NULL;
	l_bOk = l_bOk && inputImageObjectInfo.GetInputObjectInfo().PObject->IsValid();

  return l_bOk;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVAnalyzer.cpp_v  $
 * 
 *    Rev 1.2   15 May 2014 10:09:10   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Revised processMessage to use DWORD_PTR
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   01 Feb 2014 10:16:28   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed sendmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 09:27:26   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.28   08 Jan 2013 11:50:42   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  805
   SCR Title:  Fix Value Object Indexing Problem when Shortening PPQ
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Added new virtual method to classes to fix adjusting Object Depth of Value Object and Last Set Index.  Also, updated code with new methodology.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.27   25 Jul 2012 13:59:00   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Removing dead code.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.26   18 Jul 2012 13:23:44   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Removed obsolete methods assiciated with overlay drawling and fixed problem with copying data to byte buffer have wrong format.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.25   16 Jul 2012 08:55:38   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated overlay collection functionality and removed duplicate fucntionality by consolidating it to the task object class.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.24   12 Jul 2012 14:37:56   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to add in an additional string identifier for the input and fixed clear functionality assoicated with input structure.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.23   10 Jul 2012 12:18:56   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated on collect overlay method to fix a problem with missing functionality.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.22   02 Jul 2012 16:11:14   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to promote new display functionality.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.21   18 Jun 2012 17:47:32   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  744
   SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Protected Object Information structure attributes.  This change will add accessor methods to get and set attributes.
   Converted GUID to SVGUID.   This allows the use of standard operators for comparison and methods for conversion.
   
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.20   08 Dec 2010 07:40:20   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  707
   SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to include changes in notification functionality using the Observer Design Pattern.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.19   01 Jun 2010 13:39:18   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  693
   SCR Title:  Fix Performance Issue with Inspection Process
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to improve ability to track performance.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.18   01 Jun 2010 10:34:40   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  693
   SCR Title:  Fix Performance Issue with Inspection Process
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated code to remove redundent methodologies and fix missing or incorrect calling functionality.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.17   15 Dec 2009 14:55:26   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  677
   SCR Title:  Fix problem in camera notify thread
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated include information and comments.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.16   11 Dec 2008 10:16:56   jhanebach
   Project:  SVObserver
   Change Request (SCR) nbr:  629
   SCR Title:  Add Histogram analyzer
   Checked in by:  jHanebach;  Jack Hanebach
   Change Description:  
     Added DisconnectImages() method
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.15   17 Feb 2005 12:51:54   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  456
   SCR Title:  Update Image and Tool Objects to use the new Extent Classes
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated Create, Reset, and Drawing functionality.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.14   16 Feb 2005 13:05:42   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  440
   SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     updated header files
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.13   17 Sep 2003 12:45:56   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  384
   SCR Title:  Add new Matrox OCR/OCV analyzer to SVObserver
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
     Created and integrated a new Matrox based OCR/OCV analyzer to SVObserver.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.12   17 Apr 2003 16:33:28   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  346
   SCR Title:  Update SVObserver to Version 4.21 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.11   29 Jan 2003 10:26:12   joe
   Project:  SVObserver
   Change Request (SCR) nbr:  301
   SCR Title:  Implement Result Image Circle Buffer
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated CreateObject, processMessage and GetInputPixelDepth to used new result image circle buffer.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.10   10 Jan 2003 13:36:26   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  305
   SCR Title:  Implement the ability to perform RunOnce from a SIAC client
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Added new function VerifyImageForOverlay to determine which object has overlays on which image
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.9   09 Dec 2002 18:30:14   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  295
   SCR Title:  Remove Result Data from Configuration Printout
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     CreateObject methods were both modified to properly update the isCreated flag and to set the printability of the internal value objects.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.8   19 Nov 2002 10:16:22   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  226
   SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Changed
          CreateObject, processMessage
    removed references to System 
   
   GetIPDocument 
      changed method of access to IPDoc
   
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.7   26 Apr 2000 13:59:50   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  129
   SCR Title:  Angular Profile Tool
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Added GetFigures method.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.6   14 Apr 2000 10:35:28   sMarakkath
   Project:  SVObserver
   Change Request (SCR) nbr:  120
   SCR Title:  Pattern Analyzer
   Checked in by:  sMarakkath;  Srithaj Marakkath
   Change Description:  
     Added the function 'checkDrawAllowed' to draw or hide the analyzer result according to the tool draw option.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.5   02 Mar 2000 16:34:14   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  105
   SCR Title:  Handling of Tool Figures in Absolute, Relative, and Transformed Views
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised SVImageAnalyzerClass::onDraw to call 
   GetImageInputContextGuid.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.4   25 Feb 2000 16:03:52   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  105
   SCR Title:  Handling of Tool Figures in Absolute, Relative, and Transformed Views
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised onDraw to call pImage->GetTransform() to get contextual transformation.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.3   21 Feb 2000 14:24:16   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  105
   SCR Title:  Handling of Tool Figures in Absolute, Relative, and Transformed Views
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised onDraw to use SVDrawContext.
   Revised onDraw to call checkDrawAllowed.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.2   16 Feb 2000 17:05:18   mike
   Project:  SVObserver
   Change Request (SCR) nbr:  105
   SCR Title:  Handling of Tool Figures in Absolute, Relative, and Transformed Views
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Modified Draw functions to use the DrawContext structure
   and removed unused methods.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.1   08 Feb 2000 10:59:46   mike
   Project:  SVObserver
   Change Request (SCR) nbr:  96
   SCR Title:  Scroll/Zoom images displayed in the inspection document.
   Checked in by:  Mike;  Mike McCarl
   Change Description:  
     Modified the onDraw function to use the current scaling
   factor (lZoomFactor) when drawing an object.
   
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.0   22 Nov 1999 17:19:18   jim
   Project:  SVObserver
   Change Request (SCR) nbr:  61
   SCR Title:  Update PVCS versioning to version 3.0.
   Checked in by:  JimAdmin;  James A. Brown
   Change Description:  
     
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.16   Nov 09 1999 11:03:44   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  32
   SCR Title:  Port Blob Analyzer from v2.42
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     No Change.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.15   Sep 23 1999 14:10:08   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  37
   SCR Title:  Version 3.00 Beta 3 Versioning
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Remove obsolete Serialize() code.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.14   Sep 10 1999 14:36:04   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  32
   SCR Title:  Port Blob Analyzer from v2.42
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Removed SVBlobAnalyzer to separate files.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.13   02 Sep 1999 08:12:54   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  9
   SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised to call immedaite base class CreateObject.
   Revised Run method to onRun.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.12   31 Aug 1999 23:44:22   jim
   Project:  SVObserver
   Change Request (SCR) nbr:  30
   SCR Title:  Change White Pixel Analyzer to work on all Gray Scale values.
   Checked in by:  Jim;  James A. Brown
   Change Description:  
     Added SVImageAnalyzerClass::GetInputPixelDepth ()  in order to get the pixel depth of the input image.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.11   30 Aug 1999 19:45:48   robert
   Project:  SVObserver
   Change Request (SCR) nbr:  21
   SCR Title:  Port Gage Tool code from program version 2.42.
   Checked in by:  Robert;  Robert Massinger
   Change Description:  
     Introduced Input Interface Handling.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.10   30 Aug 1999 18:56:08   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  9
   SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Removed Run method from base SVAnalyzerClass
   Removed Run method from base SVImageAnalyzerClass
   Revised Run function to use SVRunStatusClass
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.9   29 Aug 1999 10:03:30   robert
   Project:  SVObserver
   Change Request (SCR) nbr:  21
   SCR Title:  Port Gage Tool code from program version 2.42.
   Checked in by:  Robert;  Robert Massinger
   Change Description:  
     Changed to support onDraw().
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.8   27 Aug 1999 17:17:46   jim
   Project:  SVObserver
   Change Request (SCR) nbr:  14
   SCR Title:  Integrate luminance analyzer into window tool.
   Checked in by:  James A. Brown
   Change Description:  
     Fix SVImageAnalyzerClass::getInputImage() to pass back correct image.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.7   Aug 25 1999 22:50:26   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  1
   SCR Title:  Integrate a common validity check for all tool base class inputs and outputs.
   Checked in by:  Steve Jones
   Change Description:  
     Revised Validate method to be OnValidate method.
    ( OnValidate - local scope validation).
   Added Validate method (Routes to all owned objects)
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.6   25 Aug 1999 11:04:16   robert
   Project:  SVObserver
   Change Request (SCR) nbr:  21
   SCR Title:  Port Gage Tool code from program version 2.42.
   Checked in by:  Robert Massinger
   Change Description:  
     Removed old Line and Rotate  Analyzers.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.5   Aug 24 1999 09:40:32   jim
   Resolution for 9: Integr. a common object interface to handle and to identify obj. in-/outputs
   
      Rev 1.4   Aug 20 1999 18:32:22   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  9
   SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
   Checked in by:  Steve Jones
   Change Description:  
     Revised getInputImage method in the SvImageAnalyzerClass
   
   /////////////////////////////////////////////////////////////////////////////////////
*/
