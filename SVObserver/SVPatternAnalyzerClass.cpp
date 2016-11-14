//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPatternAnalyzerClass
//* .File Name       : $Workfile:   SVPatternAnalyzerClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.6  $
//* .Check In Date   : $Date:   07 Aug 2014 09:28:48  $
//******************************************************************************

#include "stdafx.h"
//Moved to precompiled header: #include <cmath>
#include "SVPatternAnalyzerClass.h"
#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVMessage/SVMessage.h"
#include "SVRunControlLibrary/SVRunControlLibrary.h"
#include "SVStatusLibrary\MessageManager.h"
#include "SVGlobal.h"
#include "SVImageClass.h"
#include "SVImageProcessingClass.h"
#include "SVInspectionProcess.h"
#include "SVPatAnalyzeSetupDlgSheet.h"
#include "SVPatAdvancedPageClass.h"
#include "SVPatGeneralPageClass.h"
#include "SVPatResultDlgClass.h"   
#include "SVPatSelectModelPageClass.h"
#include "SVRange.h"
#include "SVResultLong.h"
#include "SVSVIMStateClass.h"
#include "SVTool.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

const double wPI = 3.1415926f;

enum
{
	SV_PATTERN_RESULT_MATCHSCORE,
	SV_PATTERN_RESULT_ANGLE,
	SV_PATTERN_RESULT_XPOS,
	SV_PATTERN_RESULT_YPOS,
	SV_NUMBER_OF_PATTERN_RESULTS
};

///////////////////////////////////////////////////////////
// Error Codes used by this program
///////////////////////////////////////////////////////////
#define SRI_ErrorBase		25000

#define ERR_SRI1		(SRI_ErrorBase+1)
#define ERR_SRI2		(SRI_ErrorBase+2)
#define ERR_SRI3		(SRI_ErrorBase+3)
#define ERR_SRI4		(SRI_ErrorBase+4)
#define ERR_SRI5		(SRI_ErrorBase+5)
#define ERR_SRI6		(SRI_ErrorBase+6)
#define ERR_SRI7		(SRI_ErrorBase+7)
#define ERR_SRI8		(SRI_ErrorBase+8)
#define ERR_SRI9		(SRI_ErrorBase+9)
#define ERR_SRI10		(SRI_ErrorBase+10)
#define ERR_SRI11		(SRI_ErrorBase+11)
#define ERR_SRI12		(SRI_ErrorBase+12)
#define ERR_SRI13		(SRI_ErrorBase+13)
#define ERR_SRI14		(SRI_ErrorBase+14)
#define ERR_SRI15		(SRI_ErrorBase+15)
#define ERR_SRI16		(SRI_ErrorBase+16)

SV_IMPLEMENT_CLASS(SVPatternAnalyzerClass, SVPatternAnalyzerClassGuid);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVPatternAnalyzerClass::SVPatternAnalyzerClass(BOOL BCreateDefaultTaskList, SVObjectClass* POwner, int StringResourceID)
  : SVImageAnalyzerClass(BCreateDefaultTaskList, POwner, StringResourceID)
{
	m_outObjectInfo.ObjectTypeInfo.SubType = SVPatternAnalyzerObjectType;
	m_nPatternIndex = -1;
	
	m_lpatModelWidth = 4;	// minimum Width
	m_lpatModelHeight = 4;	// minimum Height
	m_lpatModelOriginX = 0;	
	m_lpatModelOriginY = 0;	

	// Register Embedded Objects
	RegisterEmbeddedObject(&msv_dpatAcceptanceThreshold, SVpatAcceptThresholdObjectGuid, IDS_OBJECTNAME_PAT_ACCEPTTHRESHOLD, false, SVResetItemOwner );
	RegisterEmbeddedObject(&msv_dpatCertaintyThreshold, SVpatCertainThresholdObjectGuid, IDS_OBJECTNAME_PAT_CERTAINTHRESHOLD, false, SVResetItemOwner );
	RegisterEmbeddedObject(&msv_lpatAccuracy, SVpatAccuracyObjectGuid, IDS_OBJECTNAME_PAT_ACCURACY, false, SVResetItemOwner );
	RegisterEmbeddedObject(&msv_lpatMaxOccurances, SVpatMaxOccurancesObjectGuid, IDS_OBJECTNAME_PAT_MAXOCCURANCES, false, SVResetItemOwner );
	RegisterEmbeddedObject(&msv_bpatSearchAngleMode, SVpatSearchAngleModeObjectGuid, IDS_OBJECTNAME_PAT_SEARCHANGLEMODE, false, SVResetItemOwner );
	RegisterEmbeddedObject(&msv_lpatSpeed, SVpatSpeedObjectGuid, IDS_OBJECTNAME_PAT_SPEED, false, SVResetItemOwner );
	RegisterEmbeddedObject(&msv_szModelImageFile, SVpatModelImageFileGuid, IDS_OBJECTNAME_PAT_MODELIMAGEFILE, false, SVResetItemOwner );

	RegisterEmbeddedObject(&msv_dpatSearchAngle, SVpatSearchAngleObjectGuid, IDS_OBJECTNAME_PAT_SEARCHANGLE, false, SVResetItemOwner );
	RegisterEmbeddedObject(&msv_dpatAngleDeltaNeg, SVpatAngleDeltaNegObjectGuid, IDS_OBJECTNAME_PAT_DELTANEG, false, SVResetItemOwner );
	RegisterEmbeddedObject(&msv_dpatAngleDeltaPos, SVpatAngleDeltaPosObjectGuid, IDS_OBJECTNAME_PAT_DELTAPOS, false, SVResetItemOwner );
	RegisterEmbeddedObject(&msv_dpatAngleTolerance, SVpatAngleToleranceObjectGuid, IDS_OBJECTNAME_PAT_ANGLETOLERANCE, false, SVResetItemOwner );
	RegisterEmbeddedObject(&msv_dpatAngleAccuracy, SVpatAngleAccuracyObjectGuid, IDS_OBJECTNAME_PAT_ANGLEACCURACY, false, SVResetItemOwner );
	RegisterEmbeddedObject(&msv_dpatAngleInterpolation, SVpatAngleInterpolationObjectGuid, IDS_OBJECTNAME_PAT_INTERPOLATION, false, SVResetItemOwner );

	RegisterEmbeddedObject(&msv_bpatCircularOverscan, SVpatCircularOverscanObjectGuid, IDS_OBJECTNAME_PAT_CIRCULAROVERSCAN, false, SVResetItemOwner );

	RegisterEmbeddedObject(&msv_dpatPreliminaryAcceptanceThreshold, SVpatPreliminaryAcceptanceThresholdObjectGuid, IDS_OBJECTNAME_PAT_PRELIMINARYACCEPTANCETHRESHOLD, false, SVResetItemOwner );
	RegisterEmbeddedObject(&msv_lpatFastFind, SVpatFastFindObjectGuid, IDS_OBJECTNAME_PAT_FASTFIND, false, SVResetItemOwner );
	RegisterEmbeddedObject(&msv_lpatModelStep, SVpatModelStepObjectGuid, IDS_OBJECTNAME_PAT_MODELSTEP, false, SVResetItemOwner );
	RegisterEmbeddedObject(&msv_lpatBeginningResolutionLevel, SVpatBeginningResolutionLevelObjectGuid, IDS_OBJECTNAME_PAT_BEGINNINGRESOLUTIONLEVEL, false, SVResetItemOwner );
	RegisterEmbeddedObject(&msv_lpatFinalResolutionLevel, SVpatFinalResolutionLevelObjectGuid, IDS_OBJECTNAME_PAT_FINALRESOLUTIONLEVEL, false, SVResetItemOwner );
	RegisterEmbeddedObject(&msv_lpatAdditionalCandidates, SVpatAdditionalCandidatesObjectGuid, IDS_OBJECTNAME_PAT_ADDITIONALCANDIDATES, false, SVResetItemOwner );
	RegisterEmbeddedObject(&msv_dpatCandidateSpacingXMin, SVpatCandidateSpacingXMinObjectGuid, IDS_OBJECTNAME_PAT_CANDIDATESPACINGXMIN, false, SVResetItemOwner );
	RegisterEmbeddedObject(&msv_dpatCandidateSpacingYMin, SVpatCandidateSpacingYMinObjectGuid, IDS_OBJECTNAME_PAT_CANDIDATESPACINGYMIN, false, SVResetItemOwner );

	RegisterEmbeddedObject(&msv_dpatResultMatchScore, SVpatResultMatchScoreObjectGuid, IDS_OBJECTNAME_PAT_MATCHSCORE, false, SVResetItemNone );
	RegisterEmbeddedObject(&msv_dpatResultX, SVpatResultXObjectGuid, IDS_OBJECTNAME_PAT_RESULTX, false, SVResetItemNone );
	RegisterEmbeddedObject(&msv_dpatResultY, SVpatResultYObjectGuid, IDS_OBJECTNAME_PAT_RESULTY, false, SVResetItemNone );
	RegisterEmbeddedObject(&msv_dpatResultAngle, SVpatResultAngleObjectGuid, IDS_OBJECTNAME_PAT_RESULTANGLE, false, SVResetItemNone );

	RegisterEmbeddedObject(&msv_lpatNumFoundOccurances, SVpatResultNumFoundOccurancesObjectGuid, IDS_OBJECTNAME_PAT_NBRFOUNDOCCURANCES, false, SVResetItemNone );

  // Set default values for search parameters
	SetDefaultSearchValues ();

	msv_szModelImageFile.SetDefaultValue (_T(""), TRUE);
	msv_dpatResultMatchScore.SetDefaultValue (0.0, TRUE);
	msv_dpatResultX.SetDefaultValue (0.0, TRUE);
	msv_dpatResultY.SetDefaultValue (0.0, TRUE);
	msv_dpatResultAngle.SetDefaultValue (0.0, TRUE);

	// Setup the result
	pAnalyzerResult = nullptr;
	CreateResult();
	
	// Set default inputs and outputs
	addDefaultInputObjects();
}

SVPatternAnalyzerClass::~SVPatternAnalyzerClass()
{
	CloseMIL();
	CloseObject();
}

void SVPatternAnalyzerClass::CreateResult()
{
	if (nullptr == pAnalyzerResult)
	{
		// Declare Input Interface of Result...
		SVClassInfoStruct resultClassInfo;
		SVObjectTypeInfoStruct interfaceInfo;
		CString	strTitle;

		interfaceInfo.EmbeddedID = SVpatResultNumFoundOccurancesObjectGuid;
		resultClassInfo.DesiredInputInterface.Add( interfaceInfo );

		resultClassInfo.ObjectTypeInfo.ObjectType = SVResultObjectType;
		resultClassInfo.ObjectTypeInfo.SubType	= SVResultLongObjectType;
		resultClassInfo.ClassId = SVLongResultClassGuid;
		resultClassInfo.ClassName.LoadString( IDS_OBJECTNAME_RESULT );
		strTitle.LoadString( IDS_OBJECTNAME_PAT_NBRFOUNDOCCURANCES );
		resultClassInfo.ClassName += SV_TSTR_SPACE + strTitle;

		while (1)
		{
			// Construct the result class
			SVResultClass* pResult = (SVLongResultClass *)resultClassInfo.Construct();
			if (!pResult)
			{
				SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
				MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16199);
				break;
			}

			// Set the defaults for the Range Object
			SVRangeClass *pRange = pResult->GetResultRange();
			if (!pRange)
			{
				SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
				MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16200);
				break;
			}

			Add(pResult);
			pAnalyzerResult = pResult; //
			
			pRange->FailLow.SetDefaultValue(1, true);
			pRange->WarnLow.SetDefaultValue(1, true);

			pRange->FailLow.SetValue(1, 1.0);
			pRange->WarnLow.SetValue(1, 1.0);

			break;
		}
	}
}

void SVPatternAnalyzerClass::SetDefaultSearchValues()
{
	msv_dpatAcceptanceThreshold.SetDefaultValue(70.0, TRUE); // Acceptance level 70%
	msv_dpatCertaintyThreshold.SetDefaultValue(80.0, TRUE);	 // Certainty level 80%
	msv_lpatAccuracy.SetDefaultValue(SVValueMedium, TRUE);
	msv_lpatMaxOccurances.SetDefaultValue(1, TRUE);
	msv_bpatSearchAngleMode.SetDefaultValue(FALSE, TRUE);
	msv_lpatSpeed.SetDefaultValue(SVValueMedium, TRUE);
	msv_dpatSearchAngle.SetDefaultValue(0.0, TRUE);
	msv_dpatAngleDeltaNeg.SetDefaultValue(0.0, TRUE);
	msv_dpatAngleDeltaPos.SetDefaultValue(0.0, TRUE);
	msv_dpatAngleTolerance.SetDefaultValue(5.0, TRUE);
	msv_dpatAngleAccuracy.SetDefaultValue(SVValueDisable, TRUE);
	m_bAngleAccuracy = FALSE;
	msv_dpatAngleInterpolation.SetDefaultValue(SVNearestNeighbor, TRUE);

	msv_bpatCircularOverscan.SetDefaultValue(false, true);

	msv_dpatPreliminaryAcceptanceThreshold.SetDefaultValue(SVValueDefault, true);
	msv_lpatFastFind.SetDefaultValue(SVValueDefault, true);
	msv_lpatModelStep.SetDefaultValue(SVValueDefault, true);
	msv_lpatBeginningResolutionLevel.SetDefaultValue(SVValueDefault, true);
	msv_lpatFinalResolutionLevel.SetDefaultValue(SVValueDefault, true);
	msv_lpatAdditionalCandidates.SetDefaultValue(SVValueDefault, true);
	msv_dpatCandidateSpacingXMin.SetDefaultValue(SVValueDefault, true);
	msv_dpatCandidateSpacingYMin.SetDefaultValue(SVValueDefault, true);

	m_vec2dPatResults.fill(0.0);
}

void SVPatternAnalyzerClass::GetModelExtents(long& xPos, long& yPos, long& width, long& height)
{
	xPos = m_lpatModelOriginX;
	yPos = m_lpatModelOriginY;

	width = m_lpatModelWidth;
	height = m_lpatModelHeight;

	// Adjust Offsets for Circular Scan 
	// because the result positions of the found pattern will be the inner rect
	bool bCircularScan;
	msv_bpatCircularOverscan.GetValue(1, bCircularScan);
	if (bCircularScan)
	{
		CRect rect(xPos, xPos, xPos + width, yPos + height);
		CRect innerRect = SVMatroxPatternInterface::CalculateOverscanInnerRect(CPoint(rect.left, rect.top), CSize(rect.Width(), rect.Height()));

		xPos -= ((rect.Width() - innerRect.Width()) / 2);
		yPos -= ((rect.Height() - innerRect.Height()) / 2);

		if (xPos < 0)
			xPos = 0;

		if (yPos < 0)
			yPos = 0;
	}
}

void SVPatternAnalyzerClass::SetModelExtents(long xPos, long yPos, long width, long height)
{
	m_lpatModelOriginX = xPos;
	m_lpatModelOriginY = yPos;

	m_lpatModelWidth = width;
	m_lpatModelHeight = height;
}

void SVPatternAnalyzerClass::SetCircularOverscan(bool bCircularOverscan)
{
	// Set Circular Overscan
	msv_bpatCircularOverscan.SetValue(1, bCircularOverscan);
}

BOOL SVPatternAnalyzerClass::CreateModelBuffer()
{
	BOOL l_bOk = DestroyModelBuffer();

	if ( l_bOk )
	{
		// Allocate Buffer for storing the Model Image
		m_patBuffer.SetImageProperty( SVImagePropertyPixelDepth, SV8BitUnsigned );
		m_patBuffer.SetImageProperty( SVImagePropertyBandNumber, 1 );

		m_patBuffer.SetExtentProperty( SVExtentPropertyPositionPoint, 0 );
		m_patBuffer.SetExtentProperty( SVExtentPropertyWidth, m_lpatModelWidth );
		m_patBuffer.SetExtentProperty( SVExtentPropertyHeight, m_lpatModelHeight );

		l_bOk = S_OK == SVImageProcessingClass::Instance().CreateImageBuffer( m_patBuffer, m_patBufferHandlePtr );
	}
	return l_bOk;
}

BOOL SVPatternAnalyzerClass::DestroyModelBuffer()
{
	BOOL l_bOk = TRUE;

	m_patBufferHandlePtr.clear();

	return l_bOk;
}

BOOL SVPatternAnalyzerClass::UpdateModelFromInputImage()
{
	BOOL l_bOk = false;

	SVImageClass *l_pInputImage = getInputImage();

	if( nullptr != l_pInputImage)
	{
		SVMatroxPatternInterface::SVStatusCode l_Code;

		// Destroy and Recreate Model Image Buffer
		CreateModelBuffer();

		SVSmartHandlePointer l_ImageHandle;
		l_pInputImage->GetImageHandle( l_ImageHandle );

		SVImageInfoClass imageInfo = l_pInputImage->GetImageInfo();
		
		// Copy Source Image into Model Image
		// Create a child buffer on the Source Image
		SVImageInfoClass childImageInfo;
		SVSmartHandlePointer childImageHandle;
		
		childImageInfo.SetImageProperty( SVImagePropertyPixelDepth, SV8BitUnsigned );
		childImageInfo.SetImageProperty( SVImagePropertyBandNumber, 1 );

		childImageInfo.SetExtentProperty( SVExtentPropertyPositionPointX, m_lpatModelOriginX );
		childImageInfo.SetExtentProperty( SVExtentPropertyPositionPointY, m_lpatModelOriginY );
		childImageInfo.SetExtentProperty( SVExtentPropertyWidth, m_lpatModelWidth );
		childImageInfo.SetExtentProperty( SVExtentPropertyHeight, m_lpatModelHeight );

		HRESULT hr = SVImageProcessingClass::Instance().CreateImageChildBuffer(imageInfo, l_ImageHandle, childImageInfo, childImageHandle);
		if (S_OK == hr && !( childImageHandle.empty() ) && !( m_patBufferHandlePtr.empty() ) )
		{
			SVImageBufferHandleImage l_ChildMilHandle;
			SVImageBufferHandleImage l_PatMilHandle;

			childImageHandle->GetData( l_ChildMilHandle );
			m_patBufferHandlePtr->GetData( l_PatMilHandle );

			// Copy from source child to Model Image buffer
			l_Code = SVMatroxBufferInterface::CopyBuffer( l_PatMilHandle.GetBuffer(), l_ChildMilHandle.GetBuffer() );

			// free child buffer
			childImageHandle.clear();
			
			// Create the Model Pattern
			l_bOk = UpdateModelFromBuffer();
		}
		else
		{
			l_bOk = false;
		}
	}
	return l_bOk;
}

BOOL SVPatternAnalyzerClass::UpdateModelFromBuffer()
{
	BOOL l_bOk = FALSE;

	if ( !m_patBufferHandlePtr.empty() )
	{
		SVImageBufferHandleImage l_PatMilHandle;
		m_patBufferHandlePtr->GetData( l_PatMilHandle );

		SVMatroxPatternInterface::SVStatusCode l_Code;

		l_Code = SVMatroxBufferInterface::Get( l_PatMilHandle.GetBuffer(), SVSizeX, m_lpatModelWidth );
		
		if (l_Code == SVMEE_STATUS_OK)
		{
			l_Code = SVMatroxBufferInterface::Get( l_PatMilHandle.GetBuffer(), SVSizeY, m_lpatModelHeight );
		}

		if (l_Code == SVMEE_STATUS_OK)
		{
			// Destroy the pattern
			if ( !m_patModelHandle.empty() )
			{
				l_Code = SVMatroxPatternInterface::Destroy( m_patModelHandle );
			}
		
			// Check if Circular Overscan is enabled
			bool bCircularScan;
			msv_bpatCircularOverscan.GetValue(bCircularScan);

			if (bCircularScan)
			{
				POINT pos = { 0, 0 };
				SIZE size = { m_lpatModelWidth, m_lpatModelHeight };
				RECT innerRect = SVMatroxPatternInterface::CalculateOverscanInnerRect(pos, size);
				
				l_Code = SVMatroxPatternInterface::Create( m_patModelHandle, 
															l_PatMilHandle.GetBuffer(),
															innerRect.left, 
															innerRect.top,
															innerRect.right - innerRect.left, 
															innerRect.bottom - innerRect.top,
															SVPatModelTypeNormalized | SVPatModelTypeCircularOverscan );
			}
			else
			{
				l_Code = SVMatroxPatternInterface::Create( m_patModelHandle, 
															l_PatMilHandle.GetBuffer(),
															0, 0,
															m_lpatModelWidth, m_lpatModelHeight );
			}
		}

		l_bOk = (!m_patModelHandle.empty() && l_Code == SVMEE_STATUS_OK);
	}
	return l_bOk;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : RestorePattern
// -----------------------------------------------------------------------------
// .Description : Restore a model from a model file, which is actually an image file.
//              : Import the Image, then create a Model consisting of the entire image.
//				: 
////////////////////////////////////////////////////////////////////////////////
// 	 Date		Author				Comment                                       
//  04-12-00 	Sri				First Implementation
////////////////////////////////////////////////////////////////////////////////
BOOL SVPatternAnalyzerClass::RestorePattern (CString strImageFile, SvOi::MessageTextEnum *pErrMsgId)
{
	DWORD LastError(0);
	BOOL bOk = FALSE;

	SVMatroxBuffer l_ImportHandle;
	SVString l_strImageFile = strImageFile;

	SVMatroxBufferInterface::SVStatusCode l_Code = SVMatroxBufferInterface::Import( l_ImportHandle, l_strImageFile, SVFileBitmap, true );

	if ( l_Code == SVMEE_STATUS_OK )
	{
		l_Code = SVMatroxBufferInterface::Get( l_ImportHandle, SVSizeX, m_lpatModelWidth );
		if ( l_Code == SVMEE_STATUS_OK )
		{
			l_Code = SVMatroxBufferInterface::Get( l_ImportHandle, SVSizeY, m_lpatModelHeight );
		}
		if ( l_Code == SVMEE_STATUS_OK )
		{
			bOk = CreateModelBuffer() && !( m_patBufferHandlePtr.empty() );

			if ( bOk )
			{
				SVImageBufferHandleImage l_PatMilHandle;
				m_patBufferHandlePtr->GetData( l_PatMilHandle );

				l_Code = SVMatroxBufferInterface::CopyBuffer( l_PatMilHandle.GetBuffer(), l_ImportHandle );
			}
		}
		if ( l_Code == SVMEE_STATUS_OK )
		{
			l_ImportHandle.clear();
		}

		// check if one of the matrox calls failed
		if ( l_Code != SVMEE_STATUS_OK )
		{
			bOk = false;
		}
	}
	else
	{
		if ( pErrMsgId )
		{
			*pErrMsgId = SvOi::Tid_PatInvalidFilename;
		}

		SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
		MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16205);
		LastError = -SvOi::Err_16205;
		
	}

	if ( bOk )
	{
		bOk = UpdateModelFromBuffer();
	}

	if ( ! bOk )
	{
		if(pErrMsgId)
		{
			*pErrMsgId = SvOi::Tid_PatAllocModelFailed;
		}

		SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
		MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16206);
		LastError = -SvOi::Err_16206;
	}
	else
	{
		bOk = SetSearchParameters();
	}

	if ( bOk )
	{
		bOk = ( S_OK == msv_szModelImageFile.SetValue( 1, strImageFile ) );
	}
	return bOk;
}	
	
////////////////////////////////////////////////////////////////////////////////
// .Title       : SetSearchParameters
// -----------------------------------------------------------------------------
// .Description : When restoring a model from a model file or creating a new model 
//              : through the Setup dialog, set the existing search parameters for
//				: the newly created model.
////////////////////////////////////////////////////////////////////////////////
// 	 Date		Author				Comment                                       
//  04-12-00 	Sri				First Implementation
////////////////////////////////////////////////////////////////////////////////
BOOL SVPatternAnalyzerClass::SetSearchParameters ()
{
	BOOL l_bOk = TRUE;

	try
	{
		SVImageClass *pSVImage;

		if (pSVImage = (SVImageClass *)getInputImage ())
		{
			double	dParam;
			long	lParam;
			BOOL	bParam;
			CString	strValue;

			msv_lpatMaxOccurances.GetValue(lParam);

			SVSmartHandlePointer ImageHandle;
			pSVImage->GetImageHandle( ImageHandle );

			SVImageBufferHandleImage l_MilHandle;
			if(  !( ImageHandle.empty() ) )
			{
				ImageHandle->GetData( l_MilHandle );
			}

			SVMatroxBuffer ImageBufId = l_MilHandle.GetBuffer();
		
			SVMatroxPatternInterface::SVStatusCode l_Code = SVMatroxPatternInterface::SetNumber( m_patModelHandle, lParam );

			if( l_Code == SVMEE_STATUS_OK )
			{
				ResizeResultValues((int)lParam);
				
				if(l_bOk && !m_patResultHandle.empty() )
				{
					l_Code = SVMatroxPatternInterface::Destroy( m_patResultHandle );
				}

				if( l_Code == SVMEE_STATUS_OK )
				{
					l_Code = SVMatroxPatternInterface::Create( m_patResultHandle, lParam );
				}

				if( l_Code == SVMEE_STATUS_OK )
				{
					l_Code = SVMatroxPatternInterface::SetCenter( m_patModelHandle, 0, 0 );

					if( l_Code == SVMEE_STATUS_OK )
					{
						l_Code = SVMatroxPatternInterface::SetPosition( m_patModelHandle, SVValueAll, SVValueAll,
										SVValueAll, SVValueAll);
					}

					if( l_Code == SVMEE_STATUS_OK )
					{
						msv_dpatAcceptanceThreshold.GetValue(dParam);
						l_Code = SVMatroxPatternInterface::SetAcceptance( m_patModelHandle, dParam);
					}

					if( l_Code == SVMEE_STATUS_OK )
					{
						msv_dpatCertaintyThreshold.GetValue(dParam); 
						l_Code = SVMatroxPatternInterface::SetCertainty(m_patModelHandle, dParam);
					}
					
					if( l_Code == SVMEE_STATUS_OK )
					{
						msv_lpatAccuracy.GetValue(lParam);
						l_Code = SVMatroxPatternInterface::SetAccuracy(m_patModelHandle, lParam);
					}
					
					if( l_Code == SVMEE_STATUS_OK )
					{
						msv_lpatSpeed.GetValue(lParam);
						l_Code = SVMatroxPatternInterface::SetSpeed( m_patModelHandle, lParam );
					}
					
					if( l_Code == SVMEE_STATUS_OK )
					{
						msv_bpatSearchAngleMode.GetValue( bParam ); 
						l_Code = SVMatroxPatternInterface::SetAngle( m_patModelHandle, bParam ? true : false );
					}
					
					if( l_Code == SVMEE_STATUS_OK )
					{
						if(bParam)
						{
							msv_dpatSearchAngle.GetValue(dParam);
							l_Code = SVMatroxPatternInterface::SetAngle( m_patModelHandle, SVPatSearchAngle, dParam );
					
							if( l_Code == SVMEE_STATUS_OK )
							{
								msv_dpatAngleDeltaNeg.GetValue(dParam);
								l_Code = SVMatroxPatternInterface::SetAngle( m_patModelHandle, SVPatSearchAngleDeltaNeg, dParam );
							}
					
							if( l_Code == SVMEE_STATUS_OK )
							{
								msv_dpatAngleDeltaPos.GetValue(dParam);
								l_Code = SVMatroxPatternInterface::SetAngle( m_patModelHandle, SVPatSearchAngleDeltaPos, dParam );
							}

							if( l_Code == SVMEE_STATUS_OK )
							{
								msv_dpatAngleTolerance.GetValue(dParam);
								l_Code = SVMatroxPatternInterface::SetAngle( m_patModelHandle, SVPatSearchAngleTolerance, dParam );
							}

							if( l_Code == SVMEE_STATUS_OK )
							{
								msv_dpatAngleAccuracy.GetValue(dParam);
																	// M_DEFAULT = 0x10000000L M_DISABLE = -9999L
								if(dParam < 0 || dParam > 1000 )	// These values satify the old M_DEFAULT AND M_DISABLE
								{
									m_bAngleAccuracy = FALSE;
								}
								else
								{
									l_Code = SVMatroxPatternInterface::SetAngle( m_patModelHandle, SVPatSearchAngleDeltaNeg, dParam );
									m_bAngleAccuracy = TRUE;
								}
							}

							if( l_Code == SVMEE_STATUS_OK )
							{
								msv_dpatAngleInterpolation.GetValue(dParam);
								l_Code = SVMatroxPatternInterface::SetAngle( m_patModelHandle, SVPatSearchAngleInterpMode, dParam );
							}
						}
					}

					// Set Advanced Search Parameters
					if( l_Code == SVMEE_STATUS_OK )
					{
						msv_dpatPreliminaryAcceptanceThreshold.GetValue(dParam);
						l_Code = SVMatroxPatternInterface::Set(m_patModelHandle, SVPatCoarseSearchAcceptance, dParam);
					}

					if( l_Code == SVMEE_STATUS_OK )
					{
						msv_lpatFastFind.GetValue(lParam);
						l_Code = SVMatroxPatternInterface::Set(m_patModelHandle, SVPatFastFind, lParam);
					}

					if( l_Code == SVMEE_STATUS_OK )
					{
						msv_lpatModelStep.GetValue(lParam);
						l_Code = SVMatroxPatternInterface::Set(m_patModelHandle, SVPatModelStep, lParam);
					}

					if( l_Code == SVMEE_STATUS_OK )
					{
						msv_lpatBeginningResolutionLevel.GetValue(lParam);
						l_Code = SVMatroxPatternInterface::Set(m_patModelHandle, SVPatFirstLevel, lParam);
					}

					if( l_Code == SVMEE_STATUS_OK )
					{
						msv_lpatFinalResolutionLevel.GetValue(lParam);
						l_Code = SVMatroxPatternInterface::Set(m_patModelHandle, SVPatLastLevel, lParam);
					}

					if( l_Code == SVMEE_STATUS_OK )
					{
						msv_lpatAdditionalCandidates.GetValue(lParam);
						l_Code = SVMatroxPatternInterface::Set(m_patModelHandle, SVPatExtraCandidates, lParam);
					}

					if( l_Code == SVMEE_STATUS_OK )
					{
						msv_dpatCandidateSpacingXMin.GetValue(dParam);
						l_Code = SVMatroxPatternInterface::Set(m_patModelHandle, SVPatMinSpacingX, dParam);
					}

					if( l_Code == SVMEE_STATUS_OK )
					{
						msv_dpatCandidateSpacingYMin.GetValue(dParam);
						l_Code = SVMatroxPatternInterface::Set(m_patModelHandle, SVPatMinSpacingY, dParam); 
					}

					if( l_Code == SVMEE_STATUS_OK )
					{
						if( !ImageBufId.empty() )
						{
							l_Code = SVMatroxPatternInterface::PreProcModel( m_patModelHandle, ImageBufId );
							if( l_Code == SVMEE_STATUS_OK )
							{
								l_Code = SVMatroxPatternInterface::Execute( m_patResultHandle, ImageBufId, m_patModelHandle );
							}
						}
						else
						{
							l_bOk = FALSE;
						}
					}
				}
			}
			// check if one of the matrox calls failed
			if( l_Code != SVMEE_STATUS_OK )
			{
				l_bOk = FALSE;
			}
		}
// getInputImage failed
	}
	catch( ... )
	{
		SvStl::MessageMgrStd Exception( SvStl::LogOnly );
		Exception.setMessage( SVMSG_SVO_UNHANDLED_EXCEPTION, SvOi::Tid_Empty, SvStl::SourceFileParams(StdMessageParams) );
		l_bOk = FALSE;
	}
	return l_bOk;
}

void SVPatternAnalyzerClass::CloseMIL ()
{
	SVMatroxPatternInterface::SVStatusCode l_Code;
	if ( !m_patModelHandle.empty())
	{
		l_Code = SVMatroxPatternInterface::Destroy( m_patModelHandle );
	}

	if ( !m_patResultHandle.empty() )
	{
		l_Code = SVMatroxPatternInterface::Destroy( m_patResultHandle );
	}
	DestroyModelBuffer();
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : CreateObject
// -----------------------------------------------------------------------------
// .Description : There are 3 scenarios here
//              : 1. A model is already created.
//				: Action : Update the Model search area according to the new image extents.
//				: 2. No model but a Model file is Available
//				: Action : Recreate model from the file
//				: 3. No model or Model file 
//				: Action : Create a new model from the tool image
////////////////////////////////////////////////////////////////////////////////
// 	 Date		Author				Comment                                       
//  04-12-00 	Sri				First Implementation
////////////////////////////////////////////////////////////////////////////////
BOOL SVPatternAnalyzerClass::CreateObject(SVObjectLevelCreateStruct* pCreateStructure)
{
	BOOL bOk = SVImageAnalyzerClass::CreateObject(pCreateStructure);

	// Set / Reset Printable Flags
	msv_dpatAcceptanceThreshold.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_REMOTELY_SETABLE;
	msv_dpatCertaintyThreshold.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_REMOTELY_SETABLE;
	msv_lpatAccuracy.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_REMOTELY_SETABLE;
	msv_lpatMaxOccurances.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	msv_bpatSearchAngleMode.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	msv_lpatSpeed.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_REMOTELY_SETABLE;
	msv_szModelImageFile.ObjectAttributesAllowedRef() |= SV_PRINTABLE | SV_REMOTELY_SETABLE;

	msv_dpatSearchAngle.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	msv_dpatAngleDeltaNeg.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	msv_dpatAngleDeltaPos.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	msv_dpatAngleTolerance.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	msv_dpatAngleAccuracy.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	msv_dpatAngleInterpolation.ObjectAttributesAllowedRef() |= SV_PRINTABLE;

	msv_dpatResultMatchScore.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	msv_dpatResultX.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	msv_dpatResultY.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	msv_dpatResultAngle.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;

	msv_szModelImageFile.ObjectAttributesAllowedRef() |= SV_PRINTABLE; // Name of the Model Image File

	msv_bpatCircularOverscan.ObjectAttributesAllowedRef() |= SV_REMOTELY_SETABLE;
	msv_bpatCircularOverscan.ObjectAttributesAllowedRef() &= (~SV_SETABLE_ONLINE);
	
	msv_dpatPreliminaryAcceptanceThreshold.ObjectAttributesAllowedRef() |= (SV_REMOTELY_SETABLE | SV_SETABLE_ONLINE);
	msv_lpatFastFind.ObjectAttributesAllowedRef() |= (SV_REMOTELY_SETABLE | SV_SETABLE_ONLINE);
	msv_lpatModelStep.ObjectAttributesAllowedRef() |= (SV_REMOTELY_SETABLE | SV_SETABLE_ONLINE);
	msv_lpatBeginningResolutionLevel.ObjectAttributesAllowedRef() |= (SV_REMOTELY_SETABLE | SV_SETABLE_ONLINE);
	msv_lpatFinalResolutionLevel.ObjectAttributesAllowedRef() |= (SV_REMOTELY_SETABLE | SV_SETABLE_ONLINE);
	msv_lpatAdditionalCandidates.ObjectAttributesAllowedRef() |= (SV_REMOTELY_SETABLE | SV_SETABLE_ONLINE);
	msv_dpatCandidateSpacingXMin.ObjectAttributesAllowedRef() |= (SV_REMOTELY_SETABLE | SV_SETABLE_ONLINE);
	msv_dpatCandidateSpacingYMin.ObjectAttributesAllowedRef() |= (SV_REMOTELY_SETABLE | SV_SETABLE_ONLINE);

	msv_lpatNumFoundOccurances.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;

	// Check if Result is present
	SVResultClass* pResult = GetResultObject();
	if (nullptr == pResult)
	{
		pAnalyzerResult = nullptr;
		CreateResult();

		// Ensure this Object's inputs get connected
		if (nullptr != pAnalyzerResult)
		{
			pAnalyzerResult->ConnectAllInputs();
		}

		CreateChildObject(pAnalyzerResult);
	}

	m_isCreated = bOk;
	
	return bOk;
}

BOOL SVPatternAnalyzerClass::CloseObject()
{
	ResetResultValues(); // SEJ - Why do this here ?

	return SVImageAnalyzerClass::CloseObject();
}

void SVPatternAnalyzerClass::ResetResultValues()
{
// Reset the results to its default values (for the current index, which is One based).
	msv_dpatResultMatchScore.SetValue(1, 0.0);
	msv_dpatResultX.SetValue(1, 0.0);
	msv_dpatResultY.SetValue(1, 0.0);
	msv_dpatResultAngle.SetValue(1, 0.0);
}

BOOL SVPatternAnalyzerClass::OnValidate()
{
	if (SVImageAnalyzerClass::OnValidate())
	{
		if ( IsValidSize() && !m_patModelHandle.empty() && !m_patResultHandle.empty() )
		{
			m_isObjectValid.SetValue(1, true);
			return true;
		}
	}
	SetInvalid();
	return false;
}

BOOL SVPatternAnalyzerClass::onRun (SVRunStatusClass &RRunStatus)
{
	DWORD LastError(0);
	try
	{
		SVImageClass* pSVImage;
		SVMatroxBuffer ImageBufId;
		SVMatroxPatternInterface::SVStatusCode l_Code;

		while(1)
		{
			SVSmartHandlePointer ImageHandle;
			
			if (!SVImageAnalyzerClass::onRun(RRunStatus))
			{
				break;
			}
			
			pSVImage = (SVImageClass *)getInputImage();
			if( nullptr == pSVImage || ! pSVImage->GetImageHandle( ImageHandle ) )
			{
				SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
				MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvOi::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16206);
				LastError = -SvOi::Err_16206;
				break;
			}
			
			SVImageBufferHandleImage l_MilHandle;
			if(  !( ImageHandle.empty() ) )
			{
				ImageHandle->GetData( l_MilHandle );
			}

			ImageBufId = l_MilHandle.GetBuffer();
			
			l_Code = SVMatroxPatternInterface::Execute( m_patResultHandle, ImageBufId, m_patModelHandle );
			if( l_Code == SVMEE_STATUS_OK )
			{
				m_lpatModelOriginX = 0L; 
				m_lpatModelOriginY = 0L; 
				
				long lOccurances = 0; 
				l_Code = SVMatroxPatternInterface::GetNumber( m_patResultHandle, lOccurances );
				if( l_Code == SVMEE_STATUS_OK )
				{
					msv_lpatNumFoundOccurances.SetValue(RRunStatus.m_lResultDataIndex, lOccurances);

					if( lOccurances > 0 )
					{
						msv_dpatResultMatchScore.SetResultSize( RRunStatus.m_lResultDataIndex, lOccurances );
						msv_dpatResultAngle.SetResultSize( RRunStatus.m_lResultDataIndex, lOccurances );
						msv_dpatResultX.SetResultSize( RRunStatus.m_lResultDataIndex, lOccurances );
						msv_dpatResultY.SetResultSize( RRunStatus.m_lResultDataIndex, lOccurances );

						//
						vector2d<double>::row_type& matchArray = m_vec2dPatResults[SV_PATTERN_RESULT_MATCHSCORE];
						matchArray.resize( lOccurances );
						
						double* pdResultMatchScore = &(matchArray[0]);
						// SEJ - not sure if this is neccessary
						memset( pdResultMatchScore, 0L, sizeof( double ) * matchArray.size() );

						//
						vector2d<double>::row_type& angleArray = m_vec2dPatResults[SV_PATTERN_RESULT_ANGLE];
						angleArray.resize( lOccurances );

						double* pdResultAngle = &(angleArray[0]);
						// SEJ - not sure if this is neccessary
						memset( pdResultAngle, 0L, sizeof( double ) * angleArray.size() );

						//
						vector2d<double>::row_type& xPosArray = m_vec2dPatResults[SV_PATTERN_RESULT_XPOS];
						xPosArray.resize( lOccurances );
						
						double* pdResultXPos = &(xPosArray[0]);
						// SEJ - not sure if this is neccessary
						memset( pdResultXPos, 0L, sizeof( double ) * xPosArray.size() );

						//
						vector2d<double>::row_type& yPosArray = m_vec2dPatResults[SV_PATTERN_RESULT_YPOS];
						yPosArray.resize( lOccurances );
						
						// SEJ - not sure if this is neccessary
						double* pdResultYPos = &(yPosArray[0]);
						memset( pdResultYPos, 0L, sizeof( double ) * yPosArray.size() );
										
						// Get the X & Y coordinates, match score and angle of the result image
						l_Code = SVMatroxPatternInterface::GetResult( m_patResultHandle, SVPatPosX, pdResultXPos );
						if( l_Code == SVMEE_STATUS_OK )
						{
							l_Code = SVMatroxPatternInterface::GetResult( m_patResultHandle, SVPatPosY, pdResultYPos );
						}
						if( l_Code == SVMEE_STATUS_OK )
						{
							l_Code = SVMatroxPatternInterface::GetResult( m_patResultHandle, SVPatScore, pdResultMatchScore );
						}
						if( l_Code == SVMEE_STATUS_OK )
						{
							BOOL bAngleMode;
							msv_bpatSearchAngleMode.GetValue(bAngleMode); 
							if (bAngleMode)
							{
								l_Code = SVMatroxPatternInterface::GetResult( m_patResultHandle, SVPatAngle, pdResultAngle );
							}
						
							if( l_Code == SVMEE_STATUS_OK )
							{
								// populate results values
								for (int i = 0; i < lOccurances; i++)
								{
									msv_dpatResultMatchScore.SetValue( RRunStatus.m_lResultDataIndex, i, ((double)((int)(pdResultMatchScore[i] * 10.0)) / 10.0));
									if (bAngleMode)
									{
										if (pdResultAngle[i] < 0.0)
										{
											pdResultAngle[i] = 0.0;
										}
									
										msv_dpatResultAngle.SetValue( RRunStatus.m_lResultDataIndex, i, ((double)((int)(pdResultAngle[i] * 10.0)) / 10.0));
									}
									else
									{
										msv_dpatResultAngle.SetValue( RRunStatus.m_lResultDataIndex, i, 0.0 );
									}

									msv_dpatResultX.SetValue( RRunStatus.m_lResultDataIndex, i, pdResultXPos[i]);
									msv_dpatResultY.SetValue( RRunStatus.m_lResultDataIndex, i, pdResultYPos[i]);
								}
							
								// Get the Closest matching patterns Origin. This value will be used to display the model in the 
								// Pattern Analyzer Setup dialog
								m_lpatModelOriginX = static_cast<long>(pdResultXPos[0]);
								m_lpatModelOriginY = static_cast<long>(pdResultYPos[0]);
							}
						}
					}
					else
					{
						ResetResultValues();
					}
				}
			}

			// check if one of the matrox calls failed
			if( l_Code != SVMEE_STATUS_OK )
			{
				ResetResultValues();
				SetInvalid();
				RRunStatus.SetFailed();
			}
			break;
		}
		
		if (LastError)
		{
			ResetResultValues();
			SetInvalid();
			RRunStatus.SetInvalid();
			
			return FALSE;
		}
	}
	catch( ... )
	{
		SvStl::MessageMgrStd Exception( SvStl::LogOnly );
		Exception.setMessage( SVMSG_SVO_UNHANDLED_EXCEPTION, SvOi::Tid_Empty, SvStl::SourceFileParams(StdMessageParams) );
		
		ResetResultValues();
		SetInvalid();
		RRunStatus.SetInvalid();
		return FALSE;
	}
	return TRUE;
}

bool SVPatternAnalyzerClass::resetAllObjects( bool shouldNotifyFriends, bool silentReset )
{
	bool Result = ( S_OK == ResetObject() );
	ASSERT( Result );
	return (Result && __super::resetAllObjects(shouldNotifyFriends, silentReset));
}

HRESULT SVPatternAnalyzerClass::ResetObject()
{
	HRESULT l_hrOk = __super::ResetObject();

	if ( S_OK == l_hrOk )
	{
		SVInspectionProcess* pInspection = GetInspection();

		// Insures that the Pattern will be reloaded when going online.
		if ( nullptr != pInspection && pInspection->IsResetStateSet( SVResetStateLoadFiles ) )
		{
			ResetImageFile();
		}

		if( m_patModelHandle.empty() ) // if the model exists, update the search area according to the 
		{
			// Check whether there is a valid model file available.
			// If so, restore the model from the file.
			CString	strFile;

			if( S_OK == msv_szModelImageFile.GetValue( strFile ) )
			{
				if( !strFile.IsEmpty() )
				{
					if( !ResetImageFile() )
					{
						l_hrOk = S_FALSE;
					}
				}
				else if( UpdateModelFromInputImage() ) // create a new model
				{
					SetDefaultSearchValues();
					
					SVMatroxPatternInterface::SVStatusCode l_Code;
					l_Code = SVMatroxPatternInterface::SetCenter( m_patModelHandle, 0, 0 );

					if (l_Code != SVMEE_STATUS_OK)
					{
						l_hrOk = S_FALSE;
					}
				}
			}
			else
			{
				l_hrOk = S_FALSE;
			}
		}

		if( S_OK == l_hrOk )
		{
			if( !m_patModelHandle.empty() )
			{
				if( !SetSearchParameters() )
				{
					l_hrOk = S_FALSE;
				}
			}
			else
			{
				l_hrOk = S_FALSE;
			}
		}
	}
	return l_hrOk;
}

HRESULT SVPatternAnalyzerClass::onCollectOverlays(SVImageClass* p_pImage, SVExtentMultiLineStructCArray& p_rMultiLineArray )
{
	// only if ToolSet/Tool was not Disabled
	if (GetTool()->WasEnabled())
	{
		long lOccurances;
		msv_lpatNumFoundOccurances.GetValue(lOccurances);

		if (lOccurances > 0)
		{
			SVImageExtentClass l_svExtents;
			HRESULT hr = GetTool()->GetImageExtent( l_svExtents );

			if (S_OK == hr)
			{
				long lpatModelWidth = m_lpatModelWidth;
				long lpatModelHeight = m_lpatModelHeight;

				// Check if CircularOverscan was used
				bool bCircularScan;
				msv_bpatCircularOverscan.GetValue(bCircularScan);
				if (bCircularScan)
				{
					POINT pos = { 0, 0 };
					SIZE size = { m_lpatModelWidth, m_lpatModelHeight};

					RECT rect = SVMatroxPatternInterface::CalculateOverscanInnerRect(pos, size);
					lpatModelWidth = rect.right - rect.left;
					lpatModelHeight = rect.bottom - rect.top;
				}

				// If Run mode or test mode, show only one occurance (the best match)
				if ( SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_TEST ) )
				{
					lOccurances = 1;
				}
					
				BOOL bAngleMode;
				msv_bpatSearchAngleMode.GetValue( bAngleMode); 

				// 
				bool bError = false;
				for (int i = 0; !bError && i < (int)lOccurances; i++)
				{
					double dResultXPos = 0.0;
					double dResultYPos = 0.0;
					double dResultAngle = 0.0;

					msv_dpatResultX.GetValue(msv_dpatResultX.GetLastSetIndex(), i, dResultXPos);
					msv_dpatResultY.GetValue(msv_dpatResultY.GetLastSetIndex(), i, dResultYPos);
					msv_dpatResultAngle.GetValue(msv_dpatResultAngle.GetLastSetIndex(), i, dResultAngle);

					bError = dResultXPos < 0.0 || dResultYPos < 0.0	|| dResultAngle < 0.0;

					if ( !bError )
					{
						SVExtentFigureStruct l_svFigure;
						SVImageExtentClass l_svPatternExtents;

						l_svPatternExtents.SetTranslation( SVExtentTranslationFlippedRotate );
						l_svPatternExtents.SetExtentProperty( SVExtentPropertyPositionPointX, dResultXPos );
						l_svPatternExtents.SetExtentProperty( SVExtentPropertyPositionPointY, dResultYPos );
						l_svPatternExtents.SetExtentProperty( SVExtentPropertyWidth, lpatModelWidth );
						l_svPatternExtents.SetExtentProperty( SVExtentPropertyHeight, lpatModelHeight );
						l_svPatternExtents.SetExtentProperty( SVExtentPropertyRotationAngle, dResultAngle );
						l_svPatternExtents.UpdateData();
						l_svPatternExtents.GetFigure( l_svFigure );

						l_svExtents.TranslateFromOutputSpace( l_svFigure, l_svFigure );
				
						SVExtentMultiLineStruct l_multiLine;

						l_multiLine.m_Color = SV_DEFAULT_SUB_FUNCTION_COLOR_1;
						
						l_multiLine.AssignExtentFigure( l_svFigure, SV_DEFAULT_SUB_FUNCTION_COLOR_1 );

						UpdateOverlayIDs( l_multiLine );

						p_rMultiLineArray.Add( l_multiLine );
					}
				}
			}
		}
	}
	return S_OK;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : IsPtOverResult( CPoint point )
// -----------------------------------------------------------------------------
// .Description : When the user right clicks inside a tool figure, checks whether its
//              : on a pattern. If it finds a pattern at that point, gets the index of the  
//				: pattern and returns TRUE.
//				: Called from SVImageViewClass::OnContextMenu
////////////////////////////////////////////////////////////////////////////////
// 	 Date		Author				Comment                                       
//  04-12-00 	Sri				First Implementation
////////////////////////////////////////////////////////////////////////////////
BOOL SVPatternAnalyzerClass::IsPtOverResult( CPoint point )
{
	long lOccurances = 0;
	msv_lpatNumFoundOccurances.GetValue(lOccurances);

	m_nPatternIndex = -1;

	if (lOccurances > 0)
	{
		SVImageExtentClass l_svExtents;
		HRESULT hr = GetTool()->GetImageExtent( l_svExtents );

		if (S_OK == hr)
		{
			long lpatModelWidth = m_lpatModelWidth;
			long lpatModelHeight = m_lpatModelHeight;

			// Check if CircularOverscan was used
			bool bCircularScan;
			msv_bpatCircularOverscan.GetValue(bCircularScan);
			if (bCircularScan)
			{
				POINT pos = { 0, 0 };
				SIZE size = { m_lpatModelWidth, m_lpatModelHeight};

				RECT rect = SVMatroxPatternInterface::CalculateOverscanInnerRect(pos, size);
				lpatModelWidth = rect.right - rect.left;
				lpatModelHeight = rect.bottom - rect.top;
			}

			BOOL bAngleMode;
			msv_bpatSearchAngleMode.GetValue(bAngleMode); 

			// Add the Poly line Draw Object to the SVDrawObjectList
			bool bError = false;
			for (int i = 0; !bError && i < (int)lOccurances; i++)
			{
				double dResultXPos = 0.0;
				double dResultYPos = 0.0;
				double dResultAngle = 0.0;

				msv_dpatResultX.GetValue(msv_dpatResultX.GetLastSetIndex(), i, dResultXPos);
				msv_dpatResultY.GetValue(msv_dpatResultY.GetLastSetIndex(), i, dResultYPos);
				msv_dpatResultAngle.GetValue(msv_dpatResultAngle.GetLastSetIndex(), i, dResultAngle);

				bError = dResultXPos < 0.0 || dResultYPos < 0.0	||dResultAngle < 0.0;

				if ( !bError )
				{
					SVExtentFigureStruct l_svFigure;
					SVImageExtentClass l_svPatternExtents;

					l_svPatternExtents.SetTranslation( SVExtentTranslationFlippedRotate );
					l_svPatternExtents.SetExtentProperty( SVExtentPropertyPositionPointX, dResultXPos );
					l_svPatternExtents.SetExtentProperty( SVExtentPropertyPositionPointY, dResultYPos );
					l_svPatternExtents.SetExtentProperty( SVExtentPropertyWidth, lpatModelWidth );
					l_svPatternExtents.SetExtentProperty( SVExtentPropertyHeight, lpatModelHeight );
					l_svPatternExtents.SetExtentProperty( SVExtentPropertyRotationAngle, dResultAngle );
					l_svPatternExtents.UpdateData();
					l_svPatternExtents.GetFigure( l_svFigure );

					l_svExtents.TranslateFromOutputSpace( l_svFigure, l_svFigure );

					if( S_OK == l_svFigure.IsPointOverFigure( point ) )
					{
						m_nPatternIndex = i;

						break;
					}
				}
			}
		}
	}

	return (m_nPatternIndex != -1);
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : DisplayAnalyzerResult()
// -----------------------------------------------------------------------------
// .Description : When the user right clicks on a pattern inside a tool figure and 
//              : selects the Menu item 'Display Analyzer Result', display the 
//				: Match Score, X & Y values and the Angle of this pattern.
//				: Called from SVImageViewClass::OnCommand
////////////////////////////////////////////////////////////////////////////////
// 	 Date		Author				Comment                                       
//  04-12-00 	Sri				First Implementation
////////////////////////////////////////////////////////////////////////////////
void SVPatternAnalyzerClass::DisplayAnalyzerResult()
{
	long lOccurances = 0;
	msv_lpatNumFoundOccurances.GetValue(lOccurances);

	if(m_nPatternIndex < 0 || m_nPatternIndex >= lOccurances)
		return;

	SVPatResultDlgClass	resultDlg;
	CString strValue;
	
	BOOL bValue;
	msv_bpatSearchAngleMode.GetValue(bValue);

	double dResultAngle;
	msv_dpatResultAngle.GetValue(msv_dpatResultAngle.GetLastSetIndex(), m_nPatternIndex, dResultAngle);
	if(bValue && dResultAngle > 0.0)
	{
		strValue.Format("%3.1lf", dResultAngle);
		resultDlg.m_strAngle = strValue;
	}

	double dMatchScore;
	msv_dpatResultMatchScore.GetValue(msv_dpatResultMatchScore.GetLastSetIndex(), m_nPatternIndex, dMatchScore);
	strValue.Format("%3.1lf", dMatchScore);
	resultDlg.m_strScore = strValue;

	double dResultXPos;
	msv_dpatResultX.GetValue(msv_dpatResultX.GetLastSetIndex(), m_nPatternIndex, dResultXPos);
	strValue.Format("%4.1lf", dResultXPos);
	resultDlg.m_strXPos = strValue;

	double dResultYPos;
	msv_dpatResultY.GetValue(msv_dpatResultY.GetLastSetIndex(), m_nPatternIndex, dResultYPos);
	strValue.Format("%4.1lf", dResultYPos);
	resultDlg.m_strYPos = strValue;
	
	resultDlg.DoModal();
}

void SVPatternAnalyzerClass::ResizeResultValues(int nNum)
{
	// Set array sizes
	msv_dpatResultMatchScore.SetArraySize(nNum);
	msv_dpatResultAngle.SetArraySize(nNum);
	msv_dpatResultX.SetArraySize(nNum);
	msv_dpatResultY.SetArraySize(nNum);

	m_vec2dPatResults.resize( SV_NUMBER_OF_PATTERN_RESULTS, nNum );
}

BOOL SVPatternAnalyzerClass::ResetImageFile()
{
	CString	strFile;
	
	BOOL bOk = ( S_OK == msv_szModelImageFile.GetValue( strFile ) );
	
	if ( bOk )
	{
		if ( ! strFile.IsEmpty() )
		{
			bOk = RestorePattern( strFile );
		}
		else
		{
			if ( CreateModelBuffer() )
			{
				bOk = UpdateModelFromBuffer();
			}
		}
	}
	return bOk;
}

BOOL SVPatternAnalyzerClass::GetModelImageFileName( CString &csFileName )
{
	return ( S_OK == msv_szModelImageFile.GetValue( csFileName ) );
}

bool SVPatternAnalyzerClass::IsValidSize()
{
	bool bRet = true;
	SVImageExtentClass svExtents;

	if ( nullptr != GetTool() && S_OK == GetTool()->GetImageExtent( svExtents ) )
	{
		RECT oRec;
		HRESULT hrOk = svExtents.GetOutputRectangle( oRec );

		if ( S_OK == hrOk )
		{
			if( oRec.bottom < m_lpatModelHeight || oRec.right < m_lpatModelWidth )
			{
				bRet = false;
			}
		}
	}
	else
	{
		bRet = false;
	}
	return bRet;
}

