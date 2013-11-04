//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : PixelAnalyzer
//* .File Name       : $Workfile:   SVPixelAnalyzer.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:22:34  $
//******************************************************************************

#include "stdafx.h"
#include "SVPixelAnalyzer.h"

#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVMatroxLibrary/SVMatroxLibrary.h"

#include "SVDataBuffer.h"
#include "SVImageClass.h"
#include "SVImageProcessingClass.h"
#include "SVPixelAnalyzerSetup.h" // Required by SVPixelAnalyzerSetupClass
#include "SVResultLong.h"   // Required by SVLongResultClass

//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/
//* Class Name : SVPixelAnalyzerClass
//* Note(s)    : 
//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/

SV_IMPLEMENT_CLASS( SVPixelAnalyzerClass, SVPixelAnalyzerClassGuid );

////////////////////////////////////////////////////////////////////////////////
// .Title       : SVPixelAnalyzerClass()
// -----------------------------------------------------------------------------
// .Description : Standard constructor I
// -----------------------------------------------------------------------------
// .Input(s)
//	 Type				Name				Description
//	: LPCSTR			ObjectName 
//  :
// .Output(s)
//	: // e.g. int&				MyInt				Index Handle 
//  :
// -----------------------------------------------------------------------------
// .Import Function Reference(s)
//	:
// -----------------------------------------------------------------------------
// .Import Variable Reference(s)
//	:
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :27.05.1997 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
SVPixelAnalyzerClass::SVPixelAnalyzerClass( LPCSTR ObjectName )
						  :SVImageAnalyzerClass( ObjectName )
{
	init();
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : SVPixelAnalyzerClass
// -----------------------------------------------------------------------------
// .Description : Standard constructor II
// -----------------------------------------------------------------------------
// .Input(s)
//	 Type				Name				Description
//	: LPCSTR			ObjectName
//  :
// .Output(s)
//	: // e.g. int&				MyInt				Index Handle 
//  :
// -----------------------------------------------------------------------------
// .Import Function Reference(s)
//	:
// -----------------------------------------------------------------------------
// .Import Variable Reference(s)
//	:
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :09.07.1998 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
SVPixelAnalyzerClass::SVPixelAnalyzerClass( 
	BOOL BCreateDefaultTaskList, 
	SVObjectClass* POwner, 
	int StringResourceID )
		:SVImageAnalyzerClass( BCreateDefaultTaskList, POwner, StringResourceID ) 
{
	init();
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : Initialization of class SVPixelAnalyzerClass
// -----------------------------------------------------------------------------
// .Description : Initialization of newly Instantiated Object
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :22.06.1999 SEJ			First Implementation
////////////////////////////////////////////////////////////////////////////////
void SVPixelAnalyzerClass::init()
{
    while (1)
    {
		// Identify our output type
	    // Already identified in the base class (SVImageAnalyzerClass)
	    //outObjectInfo.ObjectTypeInfo.ObjectType = SVAnalyzerObjectType;
	    outObjectInfo.ObjectTypeInfo.SubType = 
            SVPixelAnalyzerObjectType;

	    // Identify our input type needs
		// Done in the base class (SVImageAnalyzerClass)
	    //inputImageObjectInfo.InputObjectInfo.ObjectTypeInfo.ObjectType = SVImageObjectType;
	    //inputImageObjectInfo.SetObject( GetObjectInfo() );

	    // SetObjectDepth() already called in SVObjectClass Ctor

	    // Register Embedded Objects
			RegisterEmbeddedObject( 
				&pixelCount, 
				SVPixelCountObjectGuid, 
				IDS_OBJECTNAME_PIXEL_COUNT,
				false, SVResetItemNone );
			
			RegisterEmbeddedObject( 
				&pixelCountColor, 
				SVPixelColorIndexObjectGuid,
				IDS_OBJECTNAME_PIXEL_COLOR_INDEX,
				false, SVResetItemNone );
			
	    // Set Embedded defaults
	    pixelCountColor. SetDefaultValue( 255, TRUE ); // White
	    pixelCount. SetDefaultValue( 0, TRUE );

	    // Set default inputs and outputs
	    addDefaultInputObjects();
	    
	    // Instantiate Children
        SVLongResultClass *pAnalyzerResult = 
            new SVLongResultClass (TRUE,
                                   this,
                                   IDS_CLASSNAME_SVPIXELANALYZERESULT);

	    if (!pAnalyzerResult)
	    {
            msvError.msvlErrorCd = -1082;
            SV_TRAP_ERROR (msvError, 1082);
	    }

        Add( pAnalyzerResult );

	    // set defaults
        m_alHistValues.clear();
        break;
    }
}


//******************************************************************************
// Destructor(s):
//******************************************************************************

////////////////////////////////////////////////////////////////////////////////
// .Title       : ~SVPixelAnalyzerClass
// -----------------------------------------------------------------------------
// .Description : This destructor deletes ...
//              :
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :27.05.1997 RO			First Implementation
//	:
////////////////////////////////////////////////////////////////////////////////
SVPixelAnalyzerClass::~SVPixelAnalyzerClass()
{
	CloseObject();
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : CloseObject
// -----------------------------------------------------------------------------
// .Description : Closes the Object
//              :
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :06.08.1999 SEJ			First Implementation
//	:
////////////////////////////////////////////////////////////////////////////////
BOOL SVPixelAnalyzerClass::CloseObject()
{
    m_alHistValues.clear();

	if ( !histResultID.empty() )
	{
		
		SVMatroxImageInterface::SVStatusCode l_Code;
		l_Code = SVMatroxImageInterface::Destroy( histResultID );
	}

	return SVImageAnalyzerClass::CloseObject();
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : CreateObject
// -----------------------------------------------------------------------------
// .Description : ...
//              :
// -----------------------------------------------------------------------------
// .Input(s)
//	 Type								Name				Description
//	: SVObjectLevelCreateStruct*		PCreateStructure
//  :
// .Output(s)
//	:
//  :
// .Return Value
//	: BOOL
// -----------------------------------------------------------------------------
// .Import Function Reference(s)
//	:
// -----------------------------------------------------------------------------
// .Import Variable Reference(s)
//	:
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :25.08.1997 RO			First Implementation
//	:
////////////////////////////////////////////////////////////////////////////////
BOOL SVPixelAnalyzerClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	
    SVImageClass *pSVImage;
	
    msvError.ClearLastErrorCd ();
	
    while (1)
    {
        if (! SVImageAnalyzerClass::CreateObject( PCreateStructure ) )
        {
			msvError.msvlErrorCd = -1084;
			SV_TRAP_ERROR_BRK (msvError, 1084);
        }
		
        pSVImage = getInputImage ();
		
        if (!pSVImage)
        {
			msvError.msvlErrorCd = -1085;
			SV_TRAP_ERROR_BRK (msvError, 1085);
        }
		
        msvlHistValueArraySize = 1 <<
            (GetInputPixelDepth () &
			SVBufferSize);
		
        m_alHistValues.resize( msvlHistValueArraySize );
		
		
        for( int i = 0; i < msvlHistValueArraySize; i++ )
			m_alHistValues [i] = 0L;
		
        // &&&
		SVDataBufferInfoClass svData;
		
		svData.Length = msvlHistValueArraySize;
		svData.Type = SVDataBufferInfoClass::SVHistResult;
		svData.HBuffer.milResult = histResultID;
		if ( S_OK == SVImageProcessingClass::Instance().CreateDataBuffer( &svData ) )
		{
			histResultID = svData.HBuffer.milResult;
		}
		
		if( histResultID.empty() )
		{
			msvError.msvlErrorCd = -1087;
			SV_TRAP_ERROR_BRK (msvError, 1087);
        }
		
        break;
    }
	
    if (msvError.GetLastErrorCd () & SV_ERROR_CONDITION)
    {
        isCreated = FALSE;
    }
    else
        isCreated = TRUE;
	
	
	// Set / Reset Printable Flags
	pixelCount.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	pixelCountColor.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	
    return isCreated;
}


//******************************************************************************
// Operation(s) Of Process:
//******************************************************************************

////////////////////////////////////////////////////////////////////////////////
// .Title       : Run
// -----------------------------------------------------------------------------
// .Description : ...
//              :
// -----------------------------------------------------------------------------
// .Input(s)
//	 Type				Name				Description
//	: 
//  :
// .Output(s)
//	:
//  :
// .Return Value
//	: TRUE
// -----------------------------------------------------------------------------
// .Import Function Reference(s)
//	:
// -----------------------------------------------------------------------------
// .Import Variable Reference(s)
//	:
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :27.05.1997 RO			First Implementation
//	:
////////////////////////////////////////////////////////////////////////////////
BOOL SVPixelAnalyzerClass::onRun(SVRunStatusClass &RRunStatus)
{

    SVImageClass     *pInputImage;

    BYTE             byIndex;


    pInputImage = NULL;
    msvError.ClearLastErrorCd ();

    while (1)
    {
		SVSmartHandlePointer ImageHandle;

		
		SVMatroxImageInterface::SVStatusCode l_Code;

        pInputImage = getInputImage ();

        if( ! pInputImage )
        {
            msvError.msvlErrorCd = -1089;
            SV_TRAP_ERROR_BRK_TSTFIRST(msvError, 1089)
        }

        if ( ! pInputImage->GetImageHandle( ImageHandle ) || ImageHandle.empty() )
        {
            msvError.msvlErrorCd = -1089;
            SV_TRAP_ERROR_BRK_TSTFIRST(msvError, 1089)
        }

		SVImageBufferHandleImage l_MilHandle;
		ImageHandle->GetData( l_MilHandle );

		l_Code = SVMatroxImageInterface::Histogram( histResultID, l_MilHandle.GetBuffer() );

		if( l_Code != SVMEE_STATUS_OK )
        {
			//          35 = Invalid MIL ID, for others see milerr.h
            msvError.msvlErrorCd = l_Code | SVMEE_MATROX_ERROR;
            SV_TRAP_ERROR_BRK_TSTFIRST(msvError, 1090)
        }
		
		l_Code = SVMatroxImageInterface::GetResult( histResultID, m_alHistValues );
		
		if( l_Code != SVMEE_STATUS_OK )
        {
            msvError.msvlErrorCd = l_Code | SVMEE_MATROX_ERROR;
            SV_TRAP_ERROR_BRK_TSTFIRST(msvError, 1091)
        }
		
		pixelCountColor.GetValue (byIndex);
		pixelCount.SetValue( RRunStatus.m_lResultDataIndex, m_alHistValues[byIndex] );
		
        break;
    }
	
	
    if (msvError.GetLastErrorCd () & SV_ERROR_CONDITION)
    {
		SetInvalid ();            
		RRunStatus.SetInvalid ();
		
		return FALSE;
    }

    return TRUE;

}

////////////////////////////////////////////////////////////////////////////////
// .Title       : Validate
// -----------------------------------------------------------------------------
// .Description : ...
//              :
// -----------------------------------------------------------------------------
// .Input(s)
//	 Type				Name				Description
//	: 
//  :
// .Output(s)
//	:
//  :
// .Return Value
//	: BOOL
// -----------------------------------------------------------------------------
// .Import Function Reference(s)
//	:
// -----------------------------------------------------------------------------
// .Import Variable Reference(s)
//	:
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :06.08.1999 SEJ			First Implementation
//	:
////////////////////////////////////////////////////////////////////////////////
BOOL SVPixelAnalyzerClass::OnValidate ()
{
   msvError.ClearLastErrorCd ();
   msvError.msvlErrorCd = 0x00000000;

   while (1)
   {
      if (!SVImageAnalyzerClass::OnValidate ())
      {
//       Error code set inside SVImageAnalyzerClass::OnValidate ()
//		 Next line commented out to remove message box.
//       SV_TRAP_ERROR_BRK_TSTFIRST (msvError, 1092);
         break;
      }

      if ( histResultID.empty() )
      {
         msvError.msvlErrorCd = -1093;
         SV_TRAP_ERROR_BRK_TSTFIRST (msvError, 1093);
      }

      if ( m_alHistValues.size() != msvlHistValueArraySize)
      {
         msvError.msvlErrorCd = -1094;
         SV_TRAP_ERROR_BRK_TSTFIRST (msvError, 1094);
      }

      break;
   }

   if( (msvError.GetLastErrorCd () & SV_ERROR_CONDITION) ||
	   (msvError.msvlErrorCd       & SV_ERROR_CONDITION) )
   {
       SetInvalid ();
       return FALSE;
   }

   isObjectValid.SetValue (1, TRUE);
   return TRUE;

}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVPixelAnalyzer.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 13:22:34   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.18   18 Jul 2012 14:19:50   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Removed obsolete methods assiciated with overlay drawling and fixed problem with copying data to byte buffer have wrong format.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.17   10 Feb 2011 15:09:08   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  712
   SCR Title:  Fix issues with black images when using command interface (SIAC)
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to now use shared pointer instead of untracked structure.  This fixes destruction, tracking and rebuilding issues.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.16   27 Jan 2011 11:50:34   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  712
   SCR Title:  Fix issues with black images when using command interface (SIAC)
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to fix issues with Matrox Buffer Management.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.15   04 Nov 2010 14:08:02   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  707
   SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to fix issues with image processing and display image processing classes and associated includes.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.14   22 Jun 2010 10:00:00   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  693
   SCR Title:  Fix Performance Issue with Inspection Process
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Fixed issues with registered inputs.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.13   01 Jun 2010 15:05:08   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  693
   SCR Title:  Fix Performance Issue with Inspection Process
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to improve ability to track performance.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.12   24 Jul 2007 15:44:02   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  597
   SCR Title:  Upgrade Matrox Imaging Library to version 8.0
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Update MIL 8.0
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.11   19 Feb 2007 16:41:24   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  594
   SCR Title:  Remove issue with interpreting status response incorrectly
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated MIL error code methodology to use the new SVMatroxAppManager GetStatusCode operator.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.10   29 Jul 2005 13:18:46   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  500
   SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated object initialization to change the parameters for RegisterEmbeddedObject for a SVValueObjectClass.  The two additional parameters will inform the preperation process before toolset execution.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.9   21 Jun 2005 08:27:54   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  464
   SCR Title:  Add array indexing for value objects
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     object attributes now use accessor methods
   value object functions now use HRESULT
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.8   17 Feb 2005 15:17:24   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  456
   SCR Title:  Update Image and Tool Objects to use the new Extent Classes
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Update class with new methodologies based on new base object, extents and reset structure.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.7   08 Jul 2003 09:56:32   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  320
   SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     Updated CreateObject method to inlcude changes made to SVImageProcessingClass.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.6   22 Apr 2003 13:52:00   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  346
   SCR Title:  Update SVObserver to Version 4.21 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.5   30 Jan 2003 16:01:34   joe
   Project:  SVObserver
   Change Request (SCR) nbr:  301
   SCR Title:  Implement Result Image Circle Buffer
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated onRun method to use the new image circle buffer methodology.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.4   09 Dec 2002 19:54:26   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  295
   SCR Title:  Remove Result Data from Configuration Printout
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     CreateObject method was modified to properly update the isCreated flag and to set the printability of the internal value objects.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.3   20 Nov 2002 10:40:46   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  272
   SCR Title:  Outputs do not always get set correctly.
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     CreateObject
   
   Call SVImageProcessingClass method instead of MIL method
   
   Changed ValueObject semantics to work with buckets
   
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.2   30 Jan 2002 16:10:22   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  278
   SCR Title:  Error message is displayed when removing result objects used by other tools
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
     Modified ::OnValidate to not call the SVErrorClass::Trap error macro which results in a nebulous message box. Instead the error code is just checked and the tool marked invalid if there was an error.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.1   04 Oct 2001 16:57:04   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  229
   SCR Title:  Upgrade SVObserver to Version 3.34 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Modified source code to remove compiler warning message.
   
   /////////////////////////////////////////////////////////////////////////////////////
*/
