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

