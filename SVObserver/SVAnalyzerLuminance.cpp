//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVAnalyzerLuminance
//* .File Name       : $Workfile:   SVAnalyzerLuminance.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:27:40  $
//******************************************************************************

#include "stdafx.h"
#include "SVAnalyzerLuminance.h"

#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVMatroxLibrary/SVMatroxLibrary.h"

#include "SVAnalyzerLuminanceSetup.h"
#include "SVDataBuffer.h"
#include "SVImageClass.h"
#include "SVImageProcessingClass.h"
#include "SVResultLong.h"

///////////////////////////////////////////////////////////
// SEJ Error Codes used by this program
///////////////////////////////////////////////////////////
#define SEJ_ErrorBase 15000
#define Err_15009 (SEJ_ErrorBase+9)
#define Err_15010 (SEJ_ErrorBase+10)
#define Err_15011 (SEJ_ErrorBase+11)
#define Err_15012 (SEJ_ErrorBase+12)

SV_IMPLEMENT_CLASS( SVLuminanceAnalyzerClass, SVLuminanceAnalyzerClassGuid );

SVLuminanceAnalyzerClass::SVLuminanceAnalyzerClass( LPCSTR ObjectName )
: SVImageAnalyzerClass( ObjectName )
, msvlHistValueArraySize( 0 )
{
	init();
}

SVLuminanceAnalyzerClass::SVLuminanceAnalyzerClass( BOOL BCreateDefaultTaskList, SVObjectClass* POwner, int StringResourceID )
: SVImageAnalyzerClass( BCreateDefaultTaskList, POwner, StringResourceID ) 
, msvlHistValueArraySize( 0 )
{
	init();
}

void SVLuminanceAnalyzerClass::init()
{
	// init () should instantiate any dynamic children and 
	//   add them to the task object list.  
	// Identify output type.
	// Identify input type needs.
	// Register any embedded objects -- all embedded objects are
	//   outputs by default.
	// Set embedded defaults.
	// Set local defaults.
	// 
	
	// Default taskObjectList items:
	
	while (1)
	{
		// Identify our output type
		outObjectInfo.ObjectTypeInfo.SubType = SVLuminanceAnalyzerObjectType;
		
		// Register Embedded Objects
		RegisterEmbeddedObject( 
			&msvLuminanceValue, 
			SVLuminanceValueObjectGuid, 
			IDS_OBJECTNAME_LUMINANCE,
			false, SVResetItemNone );
		
		// Register Embedded Objects
		RegisterEmbeddedObject( 
			&msvCalcStdDevValue,
			SVCalcStdDevObjectGuid,
			IDS_OBJECTNAME_CALCSTDDEV,
			false, SVResetItemNone );
		
		// Register Embedded Objects
		RegisterEmbeddedObject( 
			&msvVarianceValue,
			SVStatVarianceObjectGuid,
			IDS_OBJECTNAME_STATVARIANCE,
			false, SVResetItemNone );
		
		// Register Embedded Objects
		RegisterEmbeddedObject( 
			&msvStdDevValue,
			SVStatStdDevObjectGuid,
			IDS_OBJECTNAME_STATSTDDEV,
			false, SVResetItemNone );
		
		// Set Embedded defaults
		msvLuminanceValue.SetDefaultValue (0, TRUE);
		msvVarianceValue.SetDefaultValue (0, TRUE);
		msvStdDevValue.SetDefaultValue (0, TRUE);
		
		// Set default inputs and outputs
		addDefaultInputObjects();


		// Set Local

		SVLongResultClass* pAnalyzerResult = 
		 new SVLongResultClass( TRUE, this, IDS_CLASSNAME_SVLUMINANCEANALYZERESULT );

		if(!pAnalyzerResult)
		{
			msvError.msvlErrorCd = -1063;
			SV_TRAP_ERROR_BRK (msvError, 1063);
		}

		Add( pAnalyzerResult );

		break;
	}
	
}


BOOL SVLuminanceAnalyzerClass::OnValidate ()
{

   msvError.ClearLastErrorCd ();   
   msvError.msvlErrorCd = 0x00000000;

   while (1)
   {
      if (!SVImageAnalyzerClass::OnValidate ())
      {
//       Error code set inside SVImageAnalyzerClass::OnValidate ()
//		 Next line commented out to remove message box.
//       SV_TRAP_ERROR_BRK_TSTFIRST (msvError, 1081);
         break;
      }

      if (msvHistResultID.empty())
      {
         msvError.msvlErrorCd = -1066;
         SV_TRAP_ERROR_BRK_TSTFIRST (msvError, 1066);
      }

      if (msvplHistValues.size() == 0)
      {
         msvError.msvlErrorCd = -1073;
         SV_TRAP_ERROR_BRK_TSTFIRST (msvError, 1073);
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

SVLuminanceAnalyzerClass::~SVLuminanceAnalyzerClass()
{
   CloseObject ();
}

BOOL SVLuminanceAnalyzerClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{

    SVImageClass *pSVImage;

    msvError.ClearLastErrorCd ();

    while (1)
    {
        if (! SVImageAnalyzerClass::CreateObject( PCreateStructure ) )
        {
         msvError.msvlErrorCd = -1069;
         SV_TRAP_ERROR_BRK (msvError, 1069);
        }

        pSVImage = getInputImage ();

        if (!pSVImage)
        {
         msvError.msvlErrorCd = -1076;
         SV_TRAP_ERROR_BRK (msvError, 1076);
        }

        msvlHistValueArraySize = 1 << (pSVImage->getPixelDepth() & SVBufferSize);

        msvplHistValues.resize( msvlHistValueArraySize );

        if (msvplHistValues.size() == 0)
        {
         msvError.msvlErrorCd = -1072;
         SV_TRAP_ERROR_BRK (msvError, 1072);
        }

        for( int i = 0; i < msvlHistValueArraySize; i++ )
           msvplHistValues [i] = 0L;

		SVDataBufferInfoClass svData;

		svData.Length = msvlHistValueArraySize;
		svData.Type = SVDataBufferInfoClass::SVHistResult;
		svData.HBuffer.milResult = msvHistResultID;
		if ( SVImageProcessingClass::Instance().CreateDataBuffer( &svData ) == S_OK )
		{
			msvHistResultID = svData.HBuffer.milResult;
		}

        if (msvHistResultID.empty())
        {
         msvError.msvlErrorCd = -1070;
         SV_TRAP_ERROR_BRK (msvError, 1070);
        }

        break;
    }


    
    if (msvError.GetLastErrorCd () & SV_ERROR_CONDITION)
    {
        isCreated = FALSE;
    }
    else
        isCreated = TRUE;


		msvLuminanceValue.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
		msvVarianceValue.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
		msvStdDevValue.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
		msvCalcStdDevValue.ObjectAttributesAllowedRef() |= SV_PRINTABLE;

    return isCreated;
}


BOOL SVLuminanceAnalyzerClass::CloseObject()
{
   msvError.ClearLastErrorCd ();

   while (1)
   {
      
      msvplHistValues.clear();

	  SVMatroxImageInterface l_lIntf;

      l_lIntf.Destroy( msvHistResultID );

      SVImageAnalyzerClass::CloseObject ();
      break;
   }

   if (msvError.GetLastErrorCd () & SV_ERROR_CONDITION)
   {
      return FALSE;
   }

   return TRUE;
}


SVResultClass* SVLuminanceAnalyzerClass::GetResultObject()
{
	SVResultClass* pAnalyzerResult = NULL;

	// Find the result Object in Our List
	pAnalyzerResult = (SVResultClass *)GetAt(0);

	return( pAnalyzerResult );
}

BOOL SVLuminanceAnalyzerClass::onRun( SVRunStatusClass& RRunStatus )
{
	SVImageClass     *pInputImage;
	
	__int64          sum;
	size_t           lI;
	long             lNbrPixels;
	double			 value = 0.0;

	SVMatroxImageInterface::SVStatusCode l_Code;

	pInputImage = NULL;
	msvError.ClearLastErrorCd ();
	sum = 0;
	lNbrPixels = 0;
	
	while (1)
	{
		///////////////////////////////////////////////////
		// Clear Variance and Standard Deviation
		///////////////////////////////////////////////////
		msvVarianceValue.SetValue( RRunStatus.m_lResultDataIndex, value );
		msvStdDevValue.SetValue( RRunStatus.m_lResultDataIndex, value );
		
		if( !SVImageAnalyzerClass::onRun( RRunStatus ) )
		{
			break;
		}
		
		pInputImage = getInputImage ();
		
		if( ! pInputImage )
		{
			SetInvalid ();            
			msvError.msvlErrorCd = -1077;
			SV_TRAP_ERROR_BRK_TSTFIRST(msvError, 1077)
		}
		
		SVSmartHandlePointer ImageHandle;
		
		if( ! pInputImage->GetImageHandle( ImageHandle ) || ImageHandle.empty() )
		{
			msvError.msvlErrorCd = -1067;
			SV_TRAP_ERROR_BRK_TSTFIRST(msvError, 1067)
		}
				
		SVImageBufferHandleImage l_MilBuffer;

		HRESULT l_Status = ImageHandle->GetData( l_MilBuffer );

		if( l_Status != S_OK )
		{
			msvError.msvlErrorCd = l_Status;
			SV_TRAP_ERROR_BRK_TSTFIRST(msvError, 1067)
		}

		SVMatroxImageInterface l_lImageIntf;

		l_Code = l_lImageIntf.Histogram(msvHistResultID, l_MilBuffer.GetBuffer() );
		if( l_Code != SVMEE_STATUS_OK )
		{
			//          35 = Invalid MIL ID, for others see milerr.h
			msvError.msvlErrorCd = l_Code | SV_ERROR_CONDITION;
			SV_TRAP_ERROR_BRK_TSTFIRST(msvError, 1067)
		}
		
		l_Code = l_lImageIntf.GetResult(msvHistResultID,  msvplHistValues );

		
		if( l_Code != SVMEE_STATUS_OK )
		{
				msvError.msvlErrorCd = l_Code | SV_ERROR_CONDITION;
				SV_TRAP_ERROR_BRK_TSTFIRST(msvError, 1071)
		}
		
		for (lI = 0; lI < msvplHistValues.size(); lI++)
		{
				sum = sum + (msvplHistValues [lI] * lI);
				lNbrPixels = lNbrPixels + msvplHistValues [lI];
		}
			
		if (lNbrPixels == 0)
		{
				msvError.msvlErrorCd = -1075;
				SV_TRAP_ERROR_BRK_TSTFIRST(msvError, 1075)
		}

		if ( S_OK != msvLuminanceValue.SetValue( RRunStatus.m_lResultDataIndex, (long)(sum / lNbrPixels) ) )
		{
				msvError.msvlErrorCd = -1074;
				SV_TRAP_ERROR_BRK_TSTFIRST(msvError, 1074)
		}
		
		///////////////////////////////////////////////////
		// Get calculate StdDeviation flag
		///////////////////////////////////////////////////
		BOOL calcStdDev = FALSE;
		
		if ( S_OK != msvCalcStdDevValue.GetValue( calcStdDev ))
		{
			msvError.msvlErrorCd = -Err_15009;
			SV_TRAP_ERROR_BRK_TSTFIRST(msvError, Err_15009)
		}
		
		if (calcStdDev)
		{
			double accumulatedSquares = 0.0;	// for calculating variance
			
			for (lI = 0; lI < msvplHistValues.size(); lI++)
			{
				///////////////////////////////////////////////////
				// Accumulate the Squares of the luminance
				///////////////////////////////////////////////////
				value = ( double )( ( double )msvplHistValues [lI] * ( double )lI * ( double )lI );
				accumulatedSquares += value;
			}
			
			///////////////////////////////////////////////////////////////////
			// Calculate Standard Deviation on the Histogram
			// First need to calculate the Variance
			///////////////////////////////////////////////////////////////////
			double numberOfSamples = ( double )lNbrPixels;
			if( numberOfSamples > 1 )
			{
				///////////////////////////////////////////////////////////////////
				// Caluculate the average
				///////////////////////////////////////////////////////////////////
				double averageValue = ( double )( sum / numberOfSamples );
				value = calculateVariance( numberOfSamples, averageValue, accumulatedSquares );
				
				///////////////////////////////////////////////////////////////////
				// Save Variance
				///////////////////////////////////////////////////////////////////
				if (S_OK != msvVarianceValue.SetValue( RRunStatus.m_lResultDataIndex, value ))
				{
					msvError.msvlErrorCd = -Err_15010;
					SV_TRAP_ERROR_BRK_TSTFIRST(msvError, Err_15010);
				}
				
				///////////////////////////////////////////////////////////////////
				// Standard Deviation is the positive square root of the Variance
				// Note: Variance cannot be negative !!!
				///////////////////////////////////////////////////////////////////
				if( value )
				{
					if (value < 0)
					{
						msvError.msvlErrorCd = -Err_15011;
						SV_TRAP_ERROR_BRK_TSTFIRST(msvError, Err_15011);
					}
					else
						value = fabs( sqrt( value ) );
				}
				///////////////////////////////////////////////////////////////////
				// Save Standard Deviation
				///////////////////////////////////////////////////////////////////
				if (S_OK != msvStdDevValue.SetValue( RRunStatus.m_lResultDataIndex, value ))
				{
					msvError.msvlErrorCd = -Err_15012;
					SV_TRAP_ERROR_BRK_TSTFIRST(msvError, 1075);
				}
			}
		}
		
		break;
  }
	
	
  if (msvError.GetLastErrorCd () & SV_ERROR_CONDITION)
  {
		SetInvalid ();
		RRunStatus.SetInvalid();
		return FALSE;
  }
	
  return TRUE;
}

double SVLuminanceAnalyzerClass::calculateVariance( double aNumberOfSamples, double aAverageValue, double aAccumulatedSquares )
{
	///////////////////////////////////////////////////////////////////
	// Variance is a cumulative measure of the
	// Squares of the difference of all the data values from the Mean
	// Note: Calculating Sample Variation not Population Variance
	///////////////////////////////////////////////////////////////////
	double averageTimesSamples = aAverageValue * aNumberOfSamples; 
	double averageTimesSamplesSquared = (averageTimesSamples * averageTimesSamples);

	double value =
	( 1.0 / ( aNumberOfSamples * ( aNumberOfSamples - 1.0 ) ) ) *
							( aNumberOfSamples * aAccumulatedSquares - ( averageTimesSamplesSquared) );
	
	return value;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVAnalyzerLuminance.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 09:27:40   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.23   04 Oct 2012 11:10:42   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  602
   SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated construction functionality to include additional attribute initialization.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.22   03 Aug 2012 10:48:32   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Fixed issues with error number definitions.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.21   25 Jul 2012 13:59:02   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Removing dead code.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.20   02 Jul 2012 16:11:14   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to promote new display functionality.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.19   10 Feb 2011 14:24:32   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  712
   SCR Title:  Fix issues with black images when using command interface (SIAC)
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to now use shared pointer instead of untracked structure.  This fixes destruction, tracking and rebuilding issues.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.18   27 Jan 2011 10:47:10   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  712
   SCR Title:  Fix issues with black images when using command interface (SIAC)
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to fix issues with Matrox Buffer Management.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.17   04 Nov 2010 13:18:20   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  707
   SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to fix issues with image processing and display image processing classes and associated includes.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.16   01 Jun 2010 13:39:20   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  693
   SCR Title:  Fix Performance Issue with Inspection Process
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to improve ability to track performance.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.15   15 Dec 2009 14:55:26   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  677
   SCR Title:  Fix problem in camera notify thread
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated include information and comments.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.14   25 Jul 2007 08:39:22   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  597
   SCR Title:  Upgrade Matrox Imaging Library to version 8.0
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Upgrade to MIL 8.0
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.13   19 Feb 2007 15:47:06   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  594
   SCR Title:  Remove issue with interpreting status response incorrectly
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated MIL error code methodology to use the new SVMatroxAppManager GetStatusCode operator.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.12   08 Aug 2005 15:21:32   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  500
   SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated objects and dialogs to use the ActiveX Interface to update inspection data.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.11   29 Jul 2005 11:38:12   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  500
   SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated object initialization to change the parameters for RegisterEmbeddedObject for a SVValueObjectClass.  The two additional parameters will inform the preperation process before toolset execution.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.10   21 Jun 2005 08:02:16   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  464
   SCR Title:  Add array indexing for value objects
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     object attributes now use accessor methods
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.9   17 Feb 2005 12:53:30   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  456
   SCR Title:  Update Image and Tool Objects to use the new Extent Classes
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Removed old extent class variable.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.8   08 Jul 2003 08:45:50   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  322
   SCR Title:  Add Additional Digital IO Resources to SVObserver
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     changed mpsvImaging to use HRESULT instead of BOOL
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.7   17 Apr 2003 16:33:32   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  346
   SCR Title:  Update SVObserver to Version 4.21 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.6   29 Jan 2003 10:29:38   joe
   Project:  SVObserver
   Change Request (SCR) nbr:  301
   SCR Title:  Implement Result Image Circle Buffer
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated onRun and CreateObject to use new result image circle buffer methodology.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.5   09 Dec 2002 18:33:14   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  295
   SCR Title:  Remove Result Data from Configuration Printout
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     CreateObject method was modified to properly update the isCreated flag and to set the printability of the internal value objects.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.4   19 Nov 2002 10:24:36   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  226
   SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Changed
          OnValidate
          onRun
          SetCalcStdDev
         Changed ValueObject semantics to work with buckets
         CreateObject
         Call SVImageProcessingClass method instead of MIL method
   
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.3   30 Jan 2002 16:10:16   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  278
   SCR Title:  Error message is displayed when removing result objects used by other tools
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
     Modified ::OnValidate to not call the SVErrorClass::Trap error macro which results in a nebulous message box. Instead the error code is just checked and the tool marked invalid if there was an error.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.2   04 Oct 2001 13:15:22   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  229
   SCR Title:  Upgrade SVObserver to Version 3.34 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Modified source code to remove compiler warning message.
   
   /////////////////////////////////////////////////////////////////////////////////////
*/
