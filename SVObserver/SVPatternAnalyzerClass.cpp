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

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <cmath>
#include "SVPatternAnalyzerClass.h"
#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVImageLibrary/MatroxImageData.h"
#include "SVMessage/SVMessage.h"
#include "SVRunControlLibrary/SVRunControlLibrary.h"
#include "SVStatusLibrary\MessageManager.h"
#include "SVGlobal.h"
#include "SVOCore/SVImageClass.h"
#include "SVOCore/SVImageProcessingClass.h"
#include "SVInspectionProcess.h"
#include "SVPatAnalyzeSetupDlgSheet.h"
#include "SVPatAdvancedPageClass.h"
#include "SVPatGeneralPageClass.h"
#include "SVPatResultDlgClass.h"   
#include "SVPatSelectModelPageClass.h"
#include "SVRange.h"
#include "SVResultLong.h"
#include "SVStatusLibrary/SVSVIMStateClass.h"
#include "SVOCore/SVTool.h"
#include "SVUtilityLibrary/SVString.h"
#include "ObjectInterfaces/GlobalConst.h"
#pragma endregion Includes

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

SVPatternAnalyzerClass::SVPatternAnalyzerClass(SVObjectClass* POwner, int StringResourceID)
  : SVImageAnalyzerClass(POwner, StringResourceID)
  , m_bReloadModelFromFile(false)
{
	m_outObjectInfo.m_ObjectTypeInfo.SubType = SVPatternAnalyzerObjectType;
	m_nPatternIndex = -1;

	// Register Embedded Objects
	RegisterEmbeddedObject(&msv_dpatAcceptanceThreshold, SVpatAcceptThresholdObjectGuid, IDS_OBJECTNAME_PAT_ACCEPTTHRESHOLD, false, SvOi::SVResetItemOwner );
	RegisterEmbeddedObject(&msv_dpatCertaintyThreshold, SVpatCertainThresholdObjectGuid, IDS_OBJECTNAME_PAT_CERTAINTHRESHOLD, false, SvOi::SVResetItemOwner );
	RegisterEmbeddedObject(&msv_lpatAccuracy, SVpatAccuracyObjectGuid, IDS_OBJECTNAME_PAT_ACCURACY, false, SvOi::SVResetItemOwner );
	RegisterEmbeddedObject(&msv_lpatMaxOccurances, SVpatMaxOccurancesObjectGuid, IDS_OBJECTNAME_PAT_MAXOCCURANCES, false, SvOi::SVResetItemOwner );
	RegisterEmbeddedObject(&msv_bpatSearchAngleMode, SVpatSearchAngleModeObjectGuid, IDS_OBJECTNAME_PAT_SEARCHANGLEMODE, false, SvOi::SVResetItemOwner );
	RegisterEmbeddedObject(&msv_lpatSpeed, SVpatSpeedObjectGuid, IDS_OBJECTNAME_PAT_SPEED, false, SvOi::SVResetItemOwner );
	RegisterEmbeddedObject(&msv_szModelImageFile, SVpatModelImageFileGuid, IDS_OBJECTNAME_PAT_MODELIMAGEFILE, false, SvOi::SVResetItemOwner );
	RegisterEmbeddedObject(&m_DontCareImageFile, SVpatDontCareImageFileGuid, IDS_OBJECTNAME_PAT_DONT_CARE_IMAGEFILE, false, SvOi::SVResetItemOwner );

	RegisterEmbeddedObject(&msv_dpatSearchAngle, SVpatSearchAngleObjectGuid, IDS_OBJECTNAME_PAT_SEARCHANGLE, false, SvOi::SVResetItemOwner );
	RegisterEmbeddedObject(&msv_dpatAngleDeltaNeg, SVpatAngleDeltaNegObjectGuid, IDS_OBJECTNAME_PAT_DELTANEG, false, SvOi::SVResetItemOwner );
	RegisterEmbeddedObject(&msv_dpatAngleDeltaPos, SVpatAngleDeltaPosObjectGuid, IDS_OBJECTNAME_PAT_DELTAPOS, false, SvOi::SVResetItemOwner );
	RegisterEmbeddedObject(&msv_dpatAngleTolerance, SVpatAngleToleranceObjectGuid, IDS_OBJECTNAME_PAT_ANGLETOLERANCE, false, SvOi::SVResetItemOwner );
	RegisterEmbeddedObject(&msv_dpatAngleAccuracy, SVpatAngleAccuracyObjectGuid, IDS_OBJECTNAME_PAT_ANGLEACCURACY, false, SvOi::SVResetItemOwner );
	RegisterEmbeddedObject(&msv_dpatAngleInterpolation, SVpatAngleInterpolationObjectGuid, IDS_OBJECTNAME_PAT_INTERPOLATION, false, SvOi::SVResetItemOwner );

	RegisterEmbeddedObject(&msv_bpatCircularOverscan, SVpatCircularOverscanObjectGuid, IDS_OBJECTNAME_PAT_CIRCULAROVERSCAN, false, SvOi::SVResetItemOwner );
	RegisterEmbeddedObject(&m_bpatDontCare, SVpatDontCareObjectGuid, IDS_OBJECTNAME_PAT_USE_DONT_CARE, false, SvOi::SVResetItemOwner );

	RegisterEmbeddedObject(&msv_dpatPreliminaryAcceptanceThreshold, SVpatPreliminaryAcceptanceThresholdObjectGuid, IDS_OBJECTNAME_PAT_PRELIMINARYACCEPTANCETHRESHOLD, false, SvOi::SVResetItemOwner );
	RegisterEmbeddedObject(&msv_lpatFastFind, SVpatFastFindObjectGuid, IDS_OBJECTNAME_PAT_FASTFIND, false, SvOi::SVResetItemOwner );
	RegisterEmbeddedObject(&msv_lpatModelStep, SVpatModelStepObjectGuid, IDS_OBJECTNAME_PAT_MODELSTEP, false, SvOi::SVResetItemOwner );
	RegisterEmbeddedObject(&msv_lpatBeginningResolutionLevel, SVpatBeginningResolutionLevelObjectGuid, IDS_OBJECTNAME_PAT_BEGINNINGRESOLUTIONLEVEL, false, SvOi::SVResetItemOwner );
	RegisterEmbeddedObject(&msv_lpatFinalResolutionLevel, SVpatFinalResolutionLevelObjectGuid, IDS_OBJECTNAME_PAT_FINALRESOLUTIONLEVEL, false, SvOi::SVResetItemOwner );
	RegisterEmbeddedObject(&msv_lpatAdditionalCandidates, SVpatAdditionalCandidatesObjectGuid, IDS_OBJECTNAME_PAT_ADDITIONALCANDIDATES, false, SvOi::SVResetItemOwner );
	RegisterEmbeddedObject(&msv_dpatCandidateSpacingXMin, SVpatCandidateSpacingXMinObjectGuid, IDS_OBJECTNAME_PAT_CANDIDATESPACINGXMIN, false, SvOi::SVResetItemOwner );
	RegisterEmbeddedObject(&msv_dpatCandidateSpacingYMin, SVpatCandidateSpacingYMinObjectGuid, IDS_OBJECTNAME_PAT_CANDIDATESPACINGYMIN, false, SvOi::SVResetItemOwner );

	RegisterEmbeddedObject(&msv_dpatResultMatchScore, SVpatResultMatchScoreObjectGuid, IDS_OBJECTNAME_PAT_MATCHSCORE, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject(&msv_dpatResultX, SVpatResultXObjectGuid, IDS_OBJECTNAME_PAT_RESULTX, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject(&msv_dpatResultY, SVpatResultYObjectGuid, IDS_OBJECTNAME_PAT_RESULTY, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject(&msv_dpatResultAngle, SVpatResultAngleObjectGuid, IDS_OBJECTNAME_PAT_RESULTANGLE, false, SvOi::SVResetItemNone );

	RegisterEmbeddedObject(&msv_lpatNumFoundOccurances, SVpatResultNumFoundOccurancesObjectGuid, IDS_OBJECTNAME_PAT_NBRFOUNDOCCURANCES, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject(&m_lpatModelWidth, SVpatModelWidthObjectGuid, IDS_OBJECTNAME_PAT_MODELWIDTH, false, SvOi::SVResetItemOwner );
	RegisterEmbeddedObject(&m_lpatModelHeight, SVpatModelHeightObjectGuid, IDS_OBJECTNAME_PAT_MODELHEIGHT, false, SvOi::SVResetItemOwner );
	RegisterEmbeddedObject(&m_lpatModelCenterX, SVpatModelCenterXObjectGuid, IDS_OBJECTNAME_PAT_MODELCENTERX, false, SvOi::SVResetItemOwner );
	RegisterEmbeddedObject(&m_lpatModelCenterY, SVpatModelCenterYObjectGuid, IDS_OBJECTNAME_PAT_MODELCENTERY, false, SvOi::SVResetItemOwner );
	RegisterEmbeddedObject(&m_dontCareWidth, SVpatDontCareWidthObjectGuid, IDS_OBJECTNAME_PAT_DONTCAREWIDTH, false, SvOi::SVResetItemOwner);
	RegisterEmbeddedObject(&m_dontCareHeight, SVpatDontCareHeightObjectGuid, IDS_OBJECTNAME_PAT_DONTCAREHEIGHT, false, SvOi::SVResetItemOwner);

  // Set default values for search parameters
	SetDefaultSearchValues ();

	msv_szModelImageFile.SetDefaultValue (_T(""));
	m_DontCareImageFile.SetDefaultValue (_T(""));
	msv_dpatResultMatchScore.SetDefaultValue (0.0);
	msv_dpatResultMatchScore.setSaveValueFlag(false);
	msv_dpatResultX.SetDefaultValue (0.0);
	msv_dpatResultX.setSaveValueFlag(false);
	msv_dpatResultY.SetDefaultValue (0.0);
	msv_dpatResultY.setSaveValueFlag(false);
	msv_dpatResultAngle.SetDefaultValue (0.0);
	msv_dpatResultAngle.setSaveValueFlag(false);
	m_lpatModelCenterX.SetDefaultValue(0);
	m_lpatModelCenterY.SetDefaultValue(0);
	msv_lpatNumFoundOccurances.setSaveValueFlag(false);

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

		interfaceInfo.EmbeddedID = SVpatResultNumFoundOccurancesObjectGuid;
		resultClassInfo.m_DesiredInputInterface.Add( interfaceInfo );

		resultClassInfo.m_ObjectTypeInfo.ObjectType = SVResultObjectType;
		resultClassInfo.m_ObjectTypeInfo.SubType	= SVResultLongObjectType;
		resultClassInfo.m_ClassId = SVLongResultClassGuid;
		resultClassInfo.m_ClassName = SvUl_SF::LoadSVString( IDS_OBJECTNAME_RESULT );
		SVString Title = SvUl_SF::LoadSVString( IDS_OBJECTNAME_PAT_NBRFOUNDOCCURANCES );
		resultClassInfo.m_ClassName += _T(" ") + Title;

		while (1)
		{
			// Construct the result class
			SVResultClass* pResult = (SVLongResultClass *)resultClassInfo.Construct();
			if (!pResult)
			{
				SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
				MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16199);
				break;
			}

			// Set the defaults for the Range Object
			SVRangeClass *pRange = pResult->GetResultRange();
			if (!pRange)
			{
				SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
				MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16200);
				break;
			}

			Add(pResult);
			pAnalyzerResult = pResult; //
			
			pRange->FailLow.SetDefaultValue(1.0);
			pRange->WarnLow.SetDefaultValue(1.0);

			pRange->FailLow.SetValue(1.0);
			pRange->WarnLow.SetValue(1.0);

			break;
		}
	}
}

void SVPatternAnalyzerClass::SetDefaultSearchValues()
{
	msv_dpatAcceptanceThreshold.SetDefaultValue(70.0); // Acceptance level 70%
	msv_dpatCertaintyThreshold.SetDefaultValue(80.0);	 // Certainty level 80%
	msv_lpatAccuracy.SetDefaultValue(SVValueMedium);
	msv_lpatMaxOccurances.SetDefaultValue(1);
	msv_bpatSearchAngleMode.SetDefaultValue(BOOL(false));
	msv_lpatSpeed.SetDefaultValue(SVValueMedium);
	msv_dpatSearchAngle.SetDefaultValue(0.0);
	msv_dpatAngleDeltaNeg.SetDefaultValue(0.0);
	msv_dpatAngleDeltaPos.SetDefaultValue(0.0);
	msv_dpatAngleTolerance.SetDefaultValue(5.0);
	msv_dpatAngleAccuracy.SetDefaultValue(SVValueDisable);
	m_bAngleAccuracy = false;
	msv_dpatAngleInterpolation.SetDefaultValue(SVNearestNeighbor);

	msv_bpatCircularOverscan.SetDefaultValue(BOOL(false));
	m_bpatDontCare.SetDefaultValue(BOOL(false));

	msv_dpatPreliminaryAcceptanceThreshold.SetDefaultValue(SVValueDefault);
	msv_lpatFastFind.SetDefaultValue(SVValueDefault);
	msv_lpatModelStep.SetDefaultValue(SVValueDefault);
	msv_lpatBeginningResolutionLevel.SetDefaultValue(SVValueDefault);
	msv_lpatFinalResolutionLevel.SetDefaultValue(SVValueDefault);
	msv_lpatAdditionalCandidates.SetDefaultValue(SVValueDefault);
	msv_dpatCandidateSpacingXMin.SetDefaultValue(SVValueDefault);
	msv_dpatCandidateSpacingYMin.SetDefaultValue(SVValueDefault);
	m_lpatModelWidth.SetDefaultValue(SvOi::cMinPatternModelNewSize);
	m_lpatModelHeight.SetDefaultValue(SvOi::cMinPatternModelNewSize);
	m_dontCareWidth.SetDefaultValue(SvOi::cMinPatternModelNewSize);
	m_dontCareHeight.SetDefaultValue(SvOi::cMinPatternModelNewSize);

	m_vec2dPatResults.fill(0.0);
}

bool SVPatternAnalyzerClass::UpdateModelFromInputImage(long posX, long posY, long modelWidth, long modelHeight)
{
	if (SvOi::cMinPatternModelNewSize > modelWidth)
	{
		modelWidth = SvOi::cMinPatternModelNewSize;
	}
	if (SvOi::cMinPatternModelNewSize > modelHeight)
	{
		modelHeight = SvOi::cMinPatternModelNewSize;
	}

	if ( S_OK == m_lpatModelWidth.SetValue(modelWidth) && S_OK == m_lpatModelHeight.SetValue(modelHeight) )
	{
		return UpdateModelFromInputImage(posX, posY);
	}
	return false;
}

bool SVPatternAnalyzerClass::UpdateModelFromInputImage(long posX, long posY)
{
	bool bOk = false;

	SVImageClass *l_pInputImage = getInputImage();

	if( nullptr != l_pInputImage)
	{
		HRESULT l_Code;

		// Destroy and Recreate Model Image Buffer
		CreateModelBuffer();

		SVSmartHandlePointer l_ImageHandle;
		l_pInputImage->GetImageHandle( l_ImageHandle );

		SVImageInfoClass imageInfo = l_pInputImage->GetImageInfo();
		
		// Copy Source Image into Model Image
		// Create a child buffer on the Source Image
		SVImageInfoClass childImageInfo;
		SVSmartHandlePointer childImageHandle;
		
		childImageInfo.SetImageProperty( SvOi::SVImagePropertyEnum::SVImagePropertyPixelDepth, SV8BitUnsigned );
		childImageInfo.SetImageProperty( SvOi::SVImagePropertyEnum::SVImagePropertyBandNumber, 1 );

		childImageInfo.SetExtentProperty( SVExtentPropertyPositionPointX, posX );
		childImageInfo.SetExtentProperty( SVExtentPropertyPositionPointY, posY );
		long tmp = 0;
		m_lpatModelWidth.GetValue(tmp);
		childImageInfo.SetExtentProperty( SVExtentPropertyWidth, tmp );
		m_lpatModelHeight.GetValue(tmp);
		childImageInfo.SetExtentProperty( SVExtentPropertyHeight, tmp );

		HRESULT hr = SVImageProcessingClass::CreateImageChildBuffer(imageInfo, l_ImageHandle, childImageInfo, childImageHandle);
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
			bOk = UpdateModelFromBuffer();
		}
		else
		{
			bOk = false;
		}
	}

	if (bOk)
	{
		bOk = SetSearchParameters();
	}

	return bOk;
}

bool SVPatternAnalyzerClass::UpdateModelFromBuffer()
{
	bool bOk = false;

	if ( !m_patBufferHandlePtr.empty() )
	{
		SVImageBufferHandleImage l_PatMilHandle;
		m_patBufferHandlePtr->GetData( l_PatMilHandle );

		HRESULT MatroxCode;

		long modelWidth=0;
		long modelHeight=0;
		MatroxCode = SVMatroxBufferInterface::Get( l_PatMilHandle.GetBuffer(), SVSizeX, modelWidth );
		
		if (S_OK == MatroxCode)
		{
			m_lpatModelWidth.SetValue(modelWidth);
			MatroxCode = SVMatroxBufferInterface::Get( l_PatMilHandle.GetBuffer(), SVSizeY, modelHeight );
			if (S_OK == MatroxCode)
			{
				m_lpatModelHeight.SetValue(modelHeight);
			}
		}

		if (S_OK == MatroxCode)
		{
			MatroxCode = CreateModelHandle(modelWidth, modelHeight);

		}

		bOk = (!m_patModelHandle.empty() && S_OK == MatroxCode);
	}
	return bOk;
}

bool SVPatternAnalyzerClass::RestorePattern (const SVString& rImageFile, SvStl::MessageContainerVector *pErrorMessages)
{
	bool bOk = ReloadImage(rImageFile, m_lpatModelWidth, m_lpatModelHeight, m_patBufferHandlePtr, pErrorMessages);


	if ( bOk && !UpdateModelFromBuffer() )
	{
		bOk = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_PatAllocModelFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
	}

	if ( bOk && !SetSearchParameters() )
	{
		bOk = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_Pattern_SetSearchParamFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
	}

	if ( bOk )
	{
		if(  S_OK != msv_szModelImageFile.SetValue(rImageFile) )
		{
			bOk = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_SetValueFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
				pErrorMessages->push_back(Msg);
			}
		}
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
bool SVPatternAnalyzerClass::SetSearchParameters ()
{
	bool bOk = true;

	try
	{
		SVImageClass *pSVImage;

		if (pSVImage = (SVImageClass *)getInputImage ())
		{
			double	dParam;
			long	lParam;
			BOOL	bParam;

			msv_lpatMaxOccurances.GetValue(lParam);

			SVSmartHandlePointer ImageHandle;
			pSVImage->GetImageHandle( ImageHandle );

			SVImageBufferHandleImage l_MilHandle;
			if(  !( ImageHandle.empty() ) )
			{
				ImageHandle->GetData( l_MilHandle );
			}

			SVMatroxBuffer ImageBufId = l_MilHandle.GetBuffer();

			long modelWidth = 0;
			long modelHeight = 0;
			m_lpatModelWidth.GetValue(modelWidth);
			m_lpatModelHeight.GetValue(modelHeight);
			CreateModelHandle(modelWidth, modelHeight);
		
			HRESULT MatroxCode = SVMatroxPatternInterface::SetNumber( m_patModelHandle, lParam );

			if (S_OK == MatroxCode)
			{
				ResizeResultValues((int)lParam);
				
				if(!m_patResultHandle.empty() )
				{
					MatroxCode = SVMatroxPatternInterface::Destroy( m_patResultHandle );
				}

				if (S_OK == MatroxCode)
				{
					MatroxCode = SVMatroxPatternInterface::Create( m_patResultHandle, lParam );
				}

				if (S_OK == MatroxCode)
				{
					long centerX = 0;
					long centerY = 0;
					m_lpatModelCenterX.GetValue(centerX);
					m_lpatModelCenterY.GetValue(centerY);
					MatroxCode = SVMatroxPatternInterface::SetCenter( m_patModelHandle, centerX, centerY );

					if (S_OK == MatroxCode)
					{
						MatroxCode = SVMatroxPatternInterface::SetPosition( m_patModelHandle, SVValueAll, SVValueAll,
										SVValueAll, SVValueAll);
					}

					if (S_OK == MatroxCode)
					{
						msv_dpatAcceptanceThreshold.GetValue(dParam);
						MatroxCode = SVMatroxPatternInterface::SetAcceptance( m_patModelHandle, dParam);
					}

					if (S_OK == MatroxCode)
					{
						msv_dpatCertaintyThreshold.GetValue(dParam); 
						MatroxCode = SVMatroxPatternInterface::SetCertainty(m_patModelHandle, dParam);
					}
					
					if (S_OK == MatroxCode)
					{
						msv_lpatAccuracy.GetValue(lParam);
						MatroxCode = SVMatroxPatternInterface::SetAccuracy(m_patModelHandle, lParam);
					}
					
					if (S_OK == MatroxCode)
					{
						msv_lpatSpeed.GetValue(lParam);
						MatroxCode = SVMatroxPatternInterface::SetSpeed( m_patModelHandle, lParam );
					}
					
					if (S_OK == MatroxCode)
					{
						msv_bpatSearchAngleMode.GetValue( bParam ); 
						MatroxCode = SVMatroxPatternInterface::SetAngle( m_patModelHandle, bParam ? true : false );
					}
					
					if (S_OK == MatroxCode)
					{
						if(bParam)
						{
							msv_dpatSearchAngle.GetValue(dParam);
							MatroxCode = SVMatroxPatternInterface::SetAngle( m_patModelHandle, SVPatSearchAngle, dParam );
					
							if (S_OK == MatroxCode)
							{
								msv_dpatAngleDeltaNeg.GetValue(dParam);
								MatroxCode = SVMatroxPatternInterface::SetAngle( m_patModelHandle, SVPatSearchAngleDeltaNeg, dParam );
							}
					
							if (S_OK == MatroxCode)
							{
								msv_dpatAngleDeltaPos.GetValue(dParam);
								MatroxCode = SVMatroxPatternInterface::SetAngle( m_patModelHandle, SVPatSearchAngleDeltaPos, dParam );
							}

							if (S_OK == MatroxCode)
							{
								msv_dpatAngleTolerance.GetValue(dParam);
								MatroxCode = SVMatroxPatternInterface::SetAngle( m_patModelHandle, SVPatSearchAngleTolerance, dParam );
							}

							if (S_OK == MatroxCode)
							{
								msv_dpatAngleAccuracy.GetValue(dParam);
																	// M_DEFAULT = 0x10000000L M_DISABLE = -9999L
								if(dParam < 0 || dParam > 1000 )	// These values satify the old M_DEFAULT AND M_DISABLE
								{
									m_bAngleAccuracy = false;
								}
								else
								{
									MatroxCode = SVMatroxPatternInterface::SetAngle( m_patModelHandle, SVPatSearchAngleDeltaNeg, dParam );
									m_bAngleAccuracy = true;
								}
							}

							if (S_OK == MatroxCode)
							{
								msv_dpatAngleInterpolation.GetValue(dParam);
								MatroxCode = SVMatroxPatternInterface::SetAngle( m_patModelHandle, SVPatSearchAngleInterpMode, dParam );
							}
						}
					}

					// Set Advanced Search Parameters
					if (S_OK == MatroxCode)
					{
						msv_dpatPreliminaryAcceptanceThreshold.GetValue(dParam);
						MatroxCode = SVMatroxPatternInterface::Set(m_patModelHandle, SVPatCoarseSearchAcceptance, dParam);
					}

					if (S_OK == MatroxCode)
					{
						msv_lpatFastFind.GetValue(lParam);
						MatroxCode = SVMatroxPatternInterface::Set(m_patModelHandle, SVPatFastFind, lParam);
					}

					if (S_OK == MatroxCode)
					{
						msv_lpatModelStep.GetValue(lParam);
						MatroxCode = SVMatroxPatternInterface::Set(m_patModelHandle, SVPatModelStep, lParam);
					}

					if (S_OK == MatroxCode)
					{
						msv_lpatBeginningResolutionLevel.GetValue(lParam);
						MatroxCode = SVMatroxPatternInterface::Set(m_patModelHandle, SVPatFirstLevel, lParam);
					}

					if (S_OK == MatroxCode)
					{
						msv_lpatFinalResolutionLevel.GetValue(lParam);
						MatroxCode = SVMatroxPatternInterface::Set(m_patModelHandle, SVPatLastLevel, lParam);
					}

					if (S_OK == MatroxCode)
					{
						msv_lpatAdditionalCandidates.GetValue(lParam);
						MatroxCode = SVMatroxPatternInterface::Set(m_patModelHandle, SVPatExtraCandidates, lParam);
					}

					if (S_OK == MatroxCode)
					{
						msv_dpatCandidateSpacingXMin.GetValue(dParam);
						MatroxCode = SVMatroxPatternInterface::Set(m_patModelHandle, SVPatMinSpacingX, dParam);
					}

					if (S_OK == MatroxCode)
					{
						msv_dpatCandidateSpacingYMin.GetValue(dParam);
						MatroxCode = SVMatroxPatternInterface::Set(m_patModelHandle, SVPatMinSpacingY, dParam); 
					}

					if (S_OK == MatroxCode)
					{
						BOOL useDontCare = false;
						m_bpatDontCare.GetValue(useDontCare);
						if (useDontCare)
						{
							if (nullptr != m_DontCareBufferHandlePtr)
							{
								SVImageBufferHandleImage patMilHandle;
								m_DontCareBufferHandlePtr->GetData(patMilHandle);
								if (!patMilHandle.empty())
								{
									MatroxCode = SVMatroxPatternInterface::SetDontCare(patMilHandle.GetBuffer(), m_patModelHandle);
								}
								else
								{
									bOk = false;
								}
							}
						}
					}

					if (S_OK == MatroxCode)
					{
						if( !ImageBufId.empty() )
						{
							MatroxCode = SVMatroxPatternInterface::PreProcModel( m_patModelHandle, ImageBufId );
							if (S_OK == MatroxCode)
							{
								MatroxCode = executePatternAndSetResults( ImageBufId );
							}
						}
						else
						{
							bOk = false;
						}
					}
				}
			}
			// check if one of the matrox calls failed
			if (S_OK != MatroxCode)
			{
				bOk = false;
			}
		}
// getInputImage failed
	}
	catch( ... )
	{
		SvStl::MessageMgrStd Exception( SvStl::LogOnly );
		Exception.setMessage( SVMSG_SVO_UNHANDLED_EXCEPTION, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
		bOk = false;
	}
	return bOk;
}

void SVPatternAnalyzerClass::CloseMIL ()
{
	HRESULT l_Code;
	if ( !m_patModelHandle.empty())
	{
		l_Code = SVMatroxPatternInterface::Destroy( m_patModelHandle );
	}

	if ( !m_patResultHandle.empty() )
	{
		l_Code = SVMatroxPatternInterface::Destroy( m_patResultHandle );
	}
	m_patBufferHandlePtr.clear();
	m_DontCareBufferHandlePtr.clear();
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
bool SVPatternAnalyzerClass::CreateObject(SVObjectLevelCreateStruct* pCreateStructure)
{
	bool bOk = SVImageAnalyzerClass::CreateObject(pCreateStructure);

	// Set / Reset Printable Flags
	const UINT cAttributes = SvOi::SV_PRINTABLE | SvOi::SV_REMOTELY_SETABLE;
	msv_dpatAcceptanceThreshold.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
	msv_dpatCertaintyThreshold.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
	msv_lpatAccuracy.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
	msv_lpatMaxOccurances.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute );
	msv_bpatSearchAngleMode.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute );
	msv_lpatSpeed.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );

	msv_dpatSearchAngle.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute );
	msv_dpatAngleDeltaNeg.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute );
	msv_dpatAngleDeltaPos.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute );
	msv_dpatAngleTolerance.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute );
	msv_dpatAngleAccuracy.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute );
	msv_dpatAngleInterpolation.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute );

	msv_dpatResultMatchScore.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );
	msv_dpatResultX.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );
	msv_dpatResultY.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );
	msv_dpatResultAngle.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );

	msv_szModelImageFile.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );

	msv_bpatCircularOverscan.SetObjectAttributesAllowed( SvOi::SV_REMOTELY_SETABLE, SvOi::SetAttributeType::AddAttribute );
	msv_bpatCircularOverscan.SetObjectAttributesAllowed( SvOi::SV_SETABLE_ONLINE, SvOi::SetAttributeType::RemoveAttribute );
	m_bpatDontCare.SetObjectAttributesAllowed(SvOi::SV_REMOTELY_SETABLE, SvOi::SetAttributeType::AddAttribute);
	m_bpatDontCare.SetObjectAttributesAllowed(SvOi::SV_SETABLE_ONLINE, SvOi::SetAttributeType::RemoveAttribute);
	m_DontCareImageFile.SetObjectAttributesAllowed(cAttributes, SvOi::SetAttributeType::AddAttribute);
	
	msv_dpatPreliminaryAcceptanceThreshold.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
	msv_lpatFastFind.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
	msv_lpatModelStep.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
	msv_lpatBeginningResolutionLevel.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
	msv_lpatFinalResolutionLevel.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
	msv_lpatAdditionalCandidates.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
	msv_dpatCandidateSpacingXMin.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
	msv_dpatCandidateSpacingYMin.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );

	msv_lpatNumFoundOccurances.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );

	// Check if Result is present
	SvOi::IObjectClass* pResult = GetResultObject();
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

bool SVPatternAnalyzerClass::CloseObject()
{
	ResetResultValues(); // SEJ - Why do this here ?

	return SVImageAnalyzerClass::CloseObject();
}

void SVPatternAnalyzerClass::ResetResultValues()
{
// Reset the results to its default values (for the current index, which is One based).
	msv_dpatResultMatchScore.SetValue(0.0);
	msv_dpatResultX.SetValue(0.0);
	msv_dpatResultY.SetValue(0.0);
	msv_dpatResultAngle.SetValue(0.0);
}

bool SVPatternAnalyzerClass::onRun (SVRunStatusClass &rRunStatus, SvStl::MessageContainerVector *pErrorMessages)
{
	//@WARNING[MZA][7.50][17.01.2017] Not sure if we need to check ValidateLocal in Run-mode, maybe it is enough to check it in ResetObject
	bool Result = __super::onRun(rRunStatus, pErrorMessages) && ValidateLocal(pErrorMessages);
	try
	{
		if (Result)
		{
			SVImageClass* pSVImage = (SVImageClass *)getInputImage();
			SVSmartHandlePointer ImageHandle;
			if( nullptr != pSVImage && pSVImage->GetImageHandle( ImageHandle ) )
			{
				SVImageBufferHandleImage l_MilHandle;
				if(  !( ImageHandle.empty() ) )
				{
					ImageHandle->GetData( l_MilHandle );
				}

				SVMatroxBuffer ImageBufId = l_MilHandle.GetBuffer();
				HRESULT MatroxCode = executePatternAndSetResults(ImageBufId);

				// check if one of the matrox calls failed
				if (S_OK != MatroxCode)
				{
					Result = false;
					if (nullptr != pErrorMessages)
					{
						SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
						pErrorMessages->push_back(Msg);
					}
					rRunStatus.SetFailed();
				}
			}
			else
			{
				Result = false;
				if (nullptr != pErrorMessages)
				{
					SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16206, GetUniqueObjectID() );
					pErrorMessages->push_back(Msg);
				}
				rRunStatus.SetInvalid();
			}
		}
	}
	catch( ... )
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_UNHANDLED_EXCEPTION, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
		rRunStatus.SetInvalid();
	}

	if (!Result)
	{
		ResetResultValues();
		SetInvalid();
	}
	return Result;
}

bool SVPatternAnalyzerClass::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = __super::ResetObject(pErrorMessages);

	if ( Result )
	{
		SVInspectionProcess* pInspection = dynamic_cast<SVInspectionProcess*>(GetInspection());

		// Insures that the Pattern will be reloaded when going online.
		if ( nullptr != pInspection && pInspection->IsResetStateSet(SvOi::SVResetStateLoadFiles ) )
		{
			ResetPattern();
		}

		if( m_patModelHandle.empty() || m_bReloadModelFromFile ) // if the model exists, update the search area according to the 
		{
			m_bReloadModelFromFile = false;

			// Check whether there is a valid model file available.
			// If so, restore the model from the file.
			SVString FileName;

			if( S_OK == msv_szModelImageFile.GetValue( FileName ) )
			{
				if( !FileName.empty() )
				{
					Result = ResetPattern(pErrorMessages) && Result;
				}
				else if( UpdateModelFromInputImage() ) // create a new model
				{
					SetDefaultSearchValues();
				}
			}
			else
			{
				Result = false;
				if (nullptr != pErrorMessages)
				{
					SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_Pattern_GetModelImageNameFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
					pErrorMessages->push_back(Msg);
				}
			}
		}

		if (Result)
		{
			BOOL useDontCare = false;
			if (S_OK == m_bpatDontCare.GetValue(useDontCare))
			{
				if (useDontCare)
				{
					Result = RestoreDontCareImage(pErrorMessages);
				}
			}
			else
			{
				Result = false;
				if (nullptr != pErrorMessages)
				{
					SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
					pErrorMessages->push_back(Msg);
				}
			}
		}

		Result = Result && ValidateLocal(pErrorMessages);

		if( Result )
		{
			if( !SetSearchParameters() )
			{
				Result = false;
				if (nullptr != pErrorMessages)
				{
					SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_Pattern_SetSearchParamFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
					pErrorMessages->push_back(Msg);
				}
			}
		}
	}
	return Result;
}

void SVPatternAnalyzerClass::getSpecialImageList(std::vector<SVString>& rList) const
{
	rList.push_back(SvOi::PatternModelImageName);
	rList.push_back(SvOi::PatternDontCareImageName);
}

bool SVPatternAnalyzerClass::getSpecialImage(const SVString& rName, SvOi::MatroxImageSmartHandlePtr& rImagePtr) const
{
	if (SvOi::PatternModelImageName == rName)
	{
		rImagePtr = new MatroxImageData(m_patBufferHandlePtr);
		return true;
	}
	else if (SvOi::PatternDontCareImageName == rName)
	{
		rImagePtr = new MatroxImageData(m_DontCareBufferHandlePtr);
		return true;
	}
	return false;
}

SvStl::MessageContainerVector SVPatternAnalyzerClass::validateAndSetEmmeddedValues(const SvOi::SetValueObjectPairVector& rValueVector, bool shouldSet)
{
	SvStl::MessageContainerVector messages;
	bool isModelFileNameToSet = false;
	bool isDonCareFileNameToSet = false;
	bool checkDontCareSize = false;
	long dontCareWidth = 0;
	long dontCareHeight = 0;
	long modelWidth = 0;
	long modelHeight = 0;
	bool useDontCare = getNewUseDontCareValue(rValueVector);

	if (useDontCare)
	{
		isDonCareFileNameToSet = validateNewDontCareFileName(rValueVector, dontCareWidth, dontCareHeight, messages);
	}

	isModelFileNameToSet = validateNewModelFileName(rValueVector, modelWidth, modelHeight, messages);
	
	IsValidSize(modelWidth, modelHeight, useDontCare, dontCareWidth, dontCareHeight, &messages);
	
	if (messages.empty())
	{
		m_bReloadModelFromFile = isModelFileNameToSet;
		return __super::validateAndSetEmmeddedValues(rValueVector, shouldSet);
	}
	return messages;
}

HRESULT SVPatternAnalyzerClass::onCollectOverlays(SVImageClass* p_pImage, SVExtentMultiLineStructVector& p_rMultiLineArray )
{
	// only if ToolSet/Tool was not Disabled
	SVToolClass* pTool = dynamic_cast<SVToolClass*>(GetTool());
	if (pTool && pTool->WasEnabled())
	{
		long lOccurances;
		msv_lpatNumFoundOccurances.GetValue(lOccurances);

		// If Run mode or test mode, show only one occurance (the best match)
		if ( SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_TEST ) && lOccurances > 0 )
		{
			lOccurances = 1;
		}

		std::vector<SVExtentFigureStruct> figureList = GetResultExtentFigureList(lOccurances);

		for (int i=0; i < figureList.size(); i++)
		{
			SVExtentMultiLineStruct l_multiLine;
			l_multiLine.m_Color = SV_DEFAULT_SUB_FUNCTION_COLOR_1;
			l_multiLine.AssignExtentFigure( figureList[i], SV_DEFAULT_SUB_FUNCTION_COLOR_1 );
			UpdateOverlayIDs( l_multiLine );
			p_rMultiLineArray.Add( l_multiLine );
		}
	}
	return S_OK;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : IsPtOverResult( CPoint point )
// -----------------------------------------------------------------------------
// .Description : When the user right clicks inside a tool figure, checks whether its
//              : on a pattern. If it finds a pattern at that point, gets the index of the  
//				: pattern and returns true.
//				: Called from SVImageViewClass::OnContextMenu
////////////////////////////////////////////////////////////////////////////////
// 	 Date		Author				Comment                                       
//  04-12-00 	Sri				First Implementation
////////////////////////////////////////////////////////////////////////////////
bool SVPatternAnalyzerClass::IsPtOverResult( const POINT& rPoint )
{
	long lOccurances = 0;
	msv_lpatNumFoundOccurances.GetValue(lOccurances);

	m_nPatternIndex = -1;

	std::vector<SVExtentFigureStruct> figureList = GetResultExtentFigureList(lOccurances);

	for (int i=0; i < figureList.size(); i++)
	{
		if( S_OK == figureList[i].IsPointOverFigure( rPoint ) )
		{
			m_nPatternIndex = i;
			return true;
		}
	}

	return false;
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
	SVString Value;
	
	BOOL bValue;
	msv_bpatSearchAngleMode.GetValue(bValue);

	double dResultAngle( 0.0 );;
	msv_dpatResultAngle.GetValue( dResultAngle, m_nPatternIndex );
	if(bValue && dResultAngle > 0.0)
	{
		Value= SvUl_SF::Format(_T("%3.1lf"), dResultAngle);
		resultDlg.m_strAngle = Value.c_str();
	}

	double dMatchScore( 0.0 );
	msv_dpatResultMatchScore.GetValue( dMatchScore, m_nPatternIndex );
	Value = SvUl_SF::Format(_T("%3.1lf"), dMatchScore);
	resultDlg.m_strScore = Value.c_str();

	double dResultXPos( 0.0 );
	msv_dpatResultX.GetValue( dResultXPos, m_nPatternIndex );
	Value = SvUl_SF::Format(_T("%4.1lf"), dResultXPos);
	resultDlg.m_strXPos = Value.c_str();

	double dResultYPos( 0.0 );
	msv_dpatResultY.GetValue( dResultYPos, m_nPatternIndex );
	Value = SvUl_SF::Format(_T("%4.1lf"), dResultYPos);
	resultDlg.m_strYPos = Value.c_str();
	
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

bool SVPatternAnalyzerClass::ResetPattern(SvStl::MessageContainerVector *pErrorMessages)
{
	SVString	FileName;
	
	bool bOk = ( S_OK == msv_szModelImageFile.GetValue( FileName ) );
	
	if ( bOk )
	{
		if ( !FileName.empty() )
		{
			bOk = RestorePattern( FileName, pErrorMessages );
		}
		else
		{
			if ( CreateModelBuffer() )
			{
				if ( !UpdateModelFromBuffer() )
				{
					bOk = false;
					if (nullptr != pErrorMessages)
					{
						SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_PatAllocModelFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
						pErrorMessages->push_back(Msg);
					}
				}
			}
		}
	}
	else
	{
		bOk = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_Pattern_ResetImageFileFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
	}

	return bOk;
}

std::vector<SVExtentFigureStruct> SVPatternAnalyzerClass::GetResultExtentFigureList( long lOccurances )
{
	std::vector<SVExtentFigureStruct> retList;
	if (lOccurances > 0)
	{
		SVImageExtentClass l_svExtents;
		SVToolClass* pTool = dynamic_cast<SVToolClass*>(GetTool());
		if (pTool)
		{
			HRESULT hr = pTool->GetImageExtent( l_svExtents );

			if (S_OK == hr)
			{
				long lpatModelWidth = 0;
				long lpatModelHeight = 0;
				m_lpatModelWidth.GetValue(lpatModelWidth);
				m_lpatModelHeight.GetValue(lpatModelHeight);

				// Check if CircularOverscan was used
				BOOL bCircularScan;
				msv_bpatCircularOverscan.GetValue(bCircularScan);
				if( bCircularScan )
				{
					POINT pos = { 0, 0 };
					SIZE size = { lpatModelWidth, lpatModelHeight};

					RECT rect = SVMatroxPatternInterface::CalculateOverscanInnerRect(pos, size);
					lpatModelWidth = rect.right - rect.left;
					lpatModelHeight = rect.bottom - rect.top;
				}

				BOOL bAngleMode;
				msv_bpatSearchAngleMode.GetValue(bAngleMode); 

				long centerX = 0;
				long centerY = 0;
				m_lpatModelCenterX.GetValue(centerX);
				m_lpatModelCenterY.GetValue(centerY);

				// Add the Poly line Draw Object to the SVDrawObjectList
				bool bError = false;
				for (int i = 0; !bError && i < (int)lOccurances; i++)
				{
					double dResultXPos = 0;
					double dResultYPos = 0;
					double dResultAngle = 0.0;

					msv_dpatResultX.GetValue( dResultXPos, i );
					msv_dpatResultY.GetValue( dResultYPos, i );
					msv_dpatResultAngle.GetValue( dResultAngle, i );
					SVExtentPointStruct moveVector = SVRotatePoint(SVExtentPointStruct(0, 0), SVExtentPointStruct(centerX, centerY), -dResultAngle);
					dResultXPos -= moveVector.m_dPositionX;
					dResultYPos -= moveVector.m_dPositionY;
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

						retList.push_back(l_svFigure);
					}
				}
			}
		}
	}
	return retList;
}

HRESULT SVPatternAnalyzerClass::executePatternAndSetResults( SVMatroxBuffer ImageBufId)
{
	HRESULT MatroxCode = SVMatroxPatternInterface::Execute( m_patResultHandle, ImageBufId, m_patModelHandle );
	if (S_OK == MatroxCode)
	{
		long lOccurances = 0; 
		MatroxCode = SVMatroxPatternInterface::GetNumber( m_patResultHandle, lOccurances );
		if (S_OK == MatroxCode)
		{
			msv_lpatNumFoundOccurances.SetValue(lOccurances);

			if( lOccurances > 0 )
			{
				msv_dpatResultMatchScore.SetResultSize( lOccurances );
				msv_dpatResultAngle.SetResultSize( lOccurances );
				msv_dpatResultX.SetResultSize( lOccurances );
				msv_dpatResultY.SetResultSize( lOccurances );

				//
				vector2d<double>::row_type& matchArray = m_vec2dPatResults[SV_PATTERN_RESULT_MATCHSCORE];
				matchArray.resize( lOccurances );

				double* pdResultMatchScore = &(matchArray[0]);
				// SEJ - not sure if this is necessary
				memset( pdResultMatchScore, 0L, sizeof( double ) * matchArray.size() );

				//
				vector2d<double>::row_type& angleArray = m_vec2dPatResults[SV_PATTERN_RESULT_ANGLE];
				angleArray.resize( lOccurances );

				double* pdResultAngle = &(angleArray[0]);
				// SEJ - not sure if this is necessary
				memset( pdResultAngle, 0L, sizeof( double ) * angleArray.size() );

				//
				vector2d<double>::row_type& xPosArray = m_vec2dPatResults[SV_PATTERN_RESULT_XPOS];
				xPosArray.resize( lOccurances );

				double* pdResultXPos = &(xPosArray[0]);
				// SEJ - not sure if this is necessary
				memset( pdResultXPos, 0L, sizeof( double ) * xPosArray.size() );

				//
				vector2d<double>::row_type& yPosArray = m_vec2dPatResults[SV_PATTERN_RESULT_YPOS];
				yPosArray.resize( lOccurances );

				// SEJ - not sure if this is necessary
				double* pdResultYPos = &(yPosArray[0]);
				memset( pdResultYPos, 0L, sizeof( double ) * yPosArray.size() );

				// Get the X & Y coordinates, match score and angle of the result image
				MatroxCode = SVMatroxPatternInterface::GetResult( m_patResultHandle, SVPatPosX, pdResultXPos );
				if (S_OK == MatroxCode)
				{
					MatroxCode = SVMatroxPatternInterface::GetResult( m_patResultHandle, SVPatPosY, pdResultYPos );
				}
				if (S_OK == MatroxCode)
				{
					MatroxCode = SVMatroxPatternInterface::GetResult( m_patResultHandle, SVPatScore, pdResultMatchScore );
				}
				if (S_OK == MatroxCode)
				{
					BOOL bAngleMode;
					msv_bpatSearchAngleMode.GetValue(bAngleMode); 
					if (bAngleMode)
					{
						MatroxCode = SVMatroxPatternInterface::GetResult( m_patResultHandle, SVPatAngle, pdResultAngle );
					}

					if (S_OK == MatroxCode)
					{
						// populate results values
						for (int i = 0; i < lOccurances; i++)
						{
							msv_dpatResultMatchScore.SetValue( static_cast<double> ((int)(pdResultMatchScore[i] * 10.0)) / 10.0, i );
							if (bAngleMode)
							{
								if (pdResultAngle[i] < 0.0)
								{
									pdResultAngle[i] = 0.0;
								}

								msv_dpatResultAngle.SetValue( static_cast<double> ((int)(pdResultAngle[i] * 10.0)) / 10.0, i );
							}
							else
							{
								msv_dpatResultAngle.SetValue( 0.0, i );
							}

							msv_dpatResultX.SetValue( pdResultXPos[i], i );
							msv_dpatResultY.SetValue( pdResultYPos[i], i );
						}
					}
				}
			}
			else
			{
				ResetResultValues();
			}
		}
	}	
	return MatroxCode;
}

bool SVPatternAnalyzerClass::ValidateLocal(SvStl::MessageContainerVector *pErrorMessages) const
{
	bool Result = IsValidSize(pErrorMessages);

	if ( Result && (m_patModelHandle.empty() || m_patResultHandle.empty()) )
	{
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_PatAllocModelFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
		Result = false;
	}

	return Result;
}

bool SVPatternAnalyzerClass::RestoreDontCareImage(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = true;
	m_DontCareBufferHandlePtr.clear();
	SVString FileName;

	if( S_OK == m_DontCareImageFile.GetValue( FileName ) )
	{
		if (!FileName.empty())
		{
			Result = ReloadImage(FileName, m_dontCareWidth, m_dontCareHeight, m_DontCareBufferHandlePtr, pErrorMessages);
		}
		else
		{
			Result = false;
		if (nullptr != pErrorMessages)
		{
				SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_DontCareInvalidFilename, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
			pErrorMessages->push_back(Msg);
		}
	}
	}
	else
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
			pErrorMessages->push_back(Msg);
		}
	}
	return Result;
}

bool SVPatternAnalyzerClass::getNewUseDontCareValue(const SvOi::SetValueObjectPairVector &rValueVector)
{
	BOOL useDontCare = false;
	SvOi::SetValueObjectPairVector::const_iterator iter = std::find_if(rValueVector.begin(), rValueVector.end(), [&](const SvOi::SetValueObjectPairVector::value_type& entry)->bool
	{
		return entry.first == &m_bpatDontCare;
	}
	);
	if (rValueVector.end() != iter)
	{
		assert(VT_BOOL == iter->second.vt);
		useDontCare = iter->second.boolVal;
	}
	else
	{
		m_bpatDontCare.GetValue(useDontCare);
	}
	return useDontCare ? true : false;
}

bool SVPatternAnalyzerClass::validateNewDontCareFileName(const SvOi::SetValueObjectPairVector &rValueVector, long& rDontCareWidth, long& rDontCareHeight, SvStl::MessageContainerVector& rMessages)
{
	bool isValueToSet = false;
	SvOi::SetValueObjectPairVector::const_iterator iter = std::find_if(rValueVector.begin(), rValueVector.end(), [&](const SvOi::SetValueObjectPairVector::value_type& entry)->bool
	{
		return entry.first == &m_DontCareImageFile;
	}
	);
	if (rValueVector.end() != iter)
	{
		assert(VT_BSTR == iter->second.vt);
		SVString newFileName = SvUl_SF::createSVString(iter->second.bstrVal);

		SVMatroxBuffer importHandle;
		if (S_OK != SVMatroxBufferInterface::Import(importHandle, newFileName, SVFileBitmap, true) ||
			S_OK != SVMatroxBufferInterface::Get(importHandle, SVSizeX, rDontCareWidth) ||
			S_OK != SVMatroxBufferInterface::Get(importHandle, SVSizeY, rDontCareHeight))
		{
			SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_DontCareInvalidFilename, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
			rMessages.push_back(Msg);
		}
		else
		{
			isValueToSet = true;
		}
		importHandle.clear();
	}
	else
	{
		m_dontCareWidth.GetValue(rDontCareWidth);
		m_dontCareHeight.GetValue(rDontCareHeight);
	}
	return isValueToSet;
}

bool SVPatternAnalyzerClass::validateNewModelFileName(const SvOi::SetValueObjectPairVector &rValueVector, long& rModelWidth, long& rModelHeight, SvStl::MessageContainerVector &messages)
{
	bool isValueToSet = false;
	SvOi::SetValueObjectPairVector::const_iterator iter = std::find_if(rValueVector.begin(), rValueVector.end(), [&](const SvOi::SetValueObjectPairVector::value_type& entry)->bool
	{
		return entry.first == &msv_szModelImageFile;
	}
	);
	if (rValueVector.end() != iter)
	{
		assert(VT_BSTR == iter->second.vt);
		SVString fileName;
		msv_szModelImageFile.GetValue(fileName);
		SVString newFileName = SvUl_SF::createSVString(iter->second.bstrVal);
		if (fileName != newFileName)
		{
			SVMatroxBuffer importHandle;
			if (S_OK != SVMatroxBufferInterface::Import(importHandle, newFileName, SVFileBitmap, true) ||
				S_OK != SVMatroxBufferInterface::Get(importHandle, SVSizeX, rModelWidth) ||
				S_OK != SVMatroxBufferInterface::Get(importHandle, SVSizeY, rModelHeight))
			{
				SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_PatInvalidFilename, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
				messages.push_back(Msg);
			}
			else
			{
				isValueToSet = true;
			}
			importHandle.clear();
		}
		else
		{
			m_lpatModelWidth.GetValue(rModelWidth);
			m_lpatModelHeight.GetValue(rModelHeight);
		}
	}
	else
	{
		m_lpatModelWidth.GetValue(rModelWidth);
		m_lpatModelHeight.GetValue(rModelHeight);
	}
	return isValueToSet;
}

bool SVPatternAnalyzerClass::IsValidSize(SvStl::MessageContainerVector *pErrorMessages) const
{
	long modelWidth = 0;
	long modelHeight = 0;
	BOOL useDontCare = false;
	long dontCareWidth = 0;
	long dontCareHeight = 0;
	m_lpatModelWidth.GetValue(modelWidth);
	m_lpatModelHeight.GetValue(modelHeight);
	m_bpatDontCare.GetValue(useDontCare);
	m_dontCareWidth.GetValue(dontCareWidth);
	m_dontCareHeight.GetValue(dontCareHeight);
	return IsValidSize(modelWidth, modelHeight, useDontCare?true:false, dontCareWidth, dontCareHeight, pErrorMessages);
}

bool SVPatternAnalyzerClass::IsValidSize(long modelWidth, long modelHeight, bool useDontCare, long dontCareWidth, long dontCareHeight, SvStl::MessageContainerVector *pErrorMessages) const
{
	bool bRet = true;
	SVImageExtentClass svExtents;

	SVToolClass* pTool = dynamic_cast<SVToolClass*>(GetTool());
	if (pTool && S_OK == pTool->GetImageExtent(svExtents))
	{
		RECT oRec;
		HRESULT hrOk = svExtents.GetOutputRectangle(oRec);

		if (S_OK == hrOk)
		{

			if (oRec.bottom < modelHeight || oRec.right < modelWidth)
			{
				bRet = false;
				if (nullptr != pErrorMessages)
				{
					SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_Pattern_Model2Large, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
					pErrorMessages->push_back(Msg);
				}
			}

			if (useDontCare)
			{
				if (modelWidth > dontCareWidth || modelHeight > dontCareHeight)
				{
					bRet = false;
					if (nullptr != pErrorMessages)
					{
						SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_Pattern_DontCare2Small, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
						pErrorMessages->push_back(Msg);
					}
				}
			}
		}
	}
	else
	{
		bRet = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
			pErrorMessages->push_back(Msg);
		}
	}
	return bRet;
}

bool SVPatternAnalyzerClass::CreateModelBuffer()
{
	long width = 0, height = 0;
	m_lpatModelWidth.GetValue(width);
	m_lpatModelHeight.GetValue(height);
	return CreateBuffer(width, height, m_patBufferHandlePtr);
}

bool SVPatternAnalyzerClass::CreateBuffer(long width, long height, SVSmartHandlePointer& rBufferHandle)
{
	rBufferHandle.clear();

	// Allocate Buffer for storing the Model Image
	SVImageInfoClass patBuffer;
	patBuffer.SetImageProperty(SvOi::SVImagePropertyEnum::SVImagePropertyPixelDepth, SV8BitUnsigned);
	patBuffer.SetImageProperty(SvOi::SVImagePropertyEnum::SVImagePropertyBandNumber, 1);

	patBuffer.SetExtentProperty(SVExtentPropertyPositionPoint, 0);
	patBuffer.SetExtentProperty(SVExtentPropertyWidth, width);
	patBuffer.SetExtentProperty(SVExtentPropertyHeight, height);

	return (S_OK == SVImageProcessingClass::CreateImageBuffer(patBuffer, rBufferHandle));
}

bool SVPatternAnalyzerClass::ReloadImage(const SVString& rImageFile, SVLongValueObjectClass& rWidthValueObject, SVLongValueObjectClass& rHeightValueObject, SVSmartHandlePointer& rBufferHandle, SvStl::MessageContainerVector* pErrorMessages)
{
	bool bOk = true;
	SVMatroxBuffer importHandle;

	HRESULT MatroxCode = SVMatroxBufferInterface::Import(importHandle, rImageFile, SVFileBitmap, true);

	if (S_OK == MatroxCode)
	{
		long width = 0, height = 0;;
		MatroxCode = SVMatroxBufferInterface::Get(importHandle, SVSizeX, width);
		if (S_OK == MatroxCode)
		{
			rWidthValueObject.SetValue(width);
			MatroxCode = SVMatroxBufferInterface::Get(importHandle, SVSizeY, height);
			if (S_OK == MatroxCode)
			{
				rHeightValueObject.SetValue(height);
			}
		}
		if (S_OK == MatroxCode)
		{
			bOk = CreateBuffer(width, height, rBufferHandle) && !(rBufferHandle.empty());

			if (bOk)
			{
				SVImageBufferHandleImage l_PatMilHandle;
				rBufferHandle->GetData(l_PatMilHandle);

				MatroxCode = SVMatroxBufferInterface::CopyBuffer(l_PatMilHandle.GetBuffer(), importHandle);
				if (S_OK != MatroxCode)
				{
					bOk = false;
					if (nullptr != pErrorMessages)
					{
						SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_CopyImagesFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
						pErrorMessages->push_back(Msg);
					}
				}
			}
			else
			{
				bOk = false;
				if (nullptr != pErrorMessages)
				{
					SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_PatAllocFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
					pErrorMessages->push_back(Msg);
				}
			}
		}
		else
		{
			bOk = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
				pErrorMessages->push_back(Msg);
			}
		}

		if (bOk)
		{
			importHandle.clear();
		}
	}
	else
	{
		bOk = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg(SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16205, GetUniqueObjectID());
			pErrorMessages->push_back(Msg);
		}
	}
	return bOk;
}

HRESULT SVPatternAnalyzerClass::CreateModelHandle(long modelWidth, long modelHeight)
{
	HRESULT MatroxCode = S_OK; 

	// Destroy the pattern
	if (!m_patModelHandle.empty())
	{
		SVMatroxPatternInterface::Destroy(m_patModelHandle);
	}

	SVImageBufferHandleImage imageHandle;
	m_patBufferHandlePtr->GetData(imageHandle);

	// Check if Circular Overscan is enabled
	BOOL bCircularScan;
	msv_bpatCircularOverscan.GetValue(bCircularScan);

	if (bCircularScan)
	{
		POINT pos = { 0, 0 };
		SIZE size = { modelWidth, modelHeight };
		RECT innerRect = SVMatroxPatternInterface::CalculateOverscanInnerRect(pos, size);

		MatroxCode = SVMatroxPatternInterface::Create(m_patModelHandle,
			imageHandle.GetBuffer(),
			innerRect.left,
			innerRect.top,
			innerRect.right - innerRect.left,
			innerRect.bottom - innerRect.top,
			SVPatModelTypeNormalized | SVPatModelTypeCircularOverscan);

		//if CircularScan, then set Use don't care to false, because it is not possible with it
		BOOL useDontCare = false;
		m_bpatDontCare.GetValue(useDontCare);
		if (useDontCare)
		{
			m_bpatDontCare.SetValue(false);
		}
	}
	else
	{
		MatroxCode = SVMatroxPatternInterface::Create(m_patModelHandle,
			imageHandle.GetBuffer(),
			0, 0,
			modelWidth, modelHeight);
	}	
	return MatroxCode;
}
