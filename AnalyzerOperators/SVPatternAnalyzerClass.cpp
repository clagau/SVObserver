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
#include "Definitions/Color.h"
#include "Definitions/GlobalConst.h"
#include "Definitions/SVResetStruct.h"
#include "SVMatroxLibrary/SVMatroxBufferInterface.h"
#include "SVMatroxLibrary/SVMatroxPatternInterface.h"
#include "SVMessage/SVMessage.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVStatusLibrary/MessageTextEnum.h"
#include "InspectionEngine/SVImageClass.h"
#include "InspectionEngine/SVImageProcessingClass.h"
#include "ObjectInterfaces/IInspectionProcess.h"
#include "Operators/SVRange.h"
#include "Operators/SVResultLong.h"
#include "SVStatusLibrary/SVSVIMStateClass.h"
#include "SVStatusLibrary/RunStatus.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "Tools/SVTool.h"
#pragma endregion Includes

namespace SvAo
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

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

SV_IMPLEMENT_CLASS(SVPatternAnalyzerClass, SvPb::PatternAnalyzerClassId);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVPatternAnalyzerClass::SVPatternAnalyzerClass(SVObjectClass* POwner, int StringResourceID)
  : SVImageAnalyzerClass(POwner, StringResourceID)
  , m_bReloadModelFromFile(false)
{
	m_outObjectInfo.m_ObjectTypeInfo.m_SubType = SvPb::SVPatternAnalyzerObjectType;
	m_nPatternIndex = -1;

	// Register Embedded Objects
	RegisterEmbeddedObject(&msv_dpatAcceptanceThreshold, SvPb::PatAcceptThresholdEId, IDS_OBJECTNAME_PAT_ACCEPTTHRESHOLD, false, SvOi::SVResetItemOwner );
	RegisterEmbeddedObject(&msv_dpatCertaintyThreshold, SvPb::PatCertainThresholdEId, IDS_OBJECTNAME_PAT_CERTAINTHRESHOLD, false, SvOi::SVResetItemOwner );
	RegisterEmbeddedObject(&msv_lpatAccuracy, SvPb::PatAccuracyEId, IDS_OBJECTNAME_PAT_ACCURACY, false, SvOi::SVResetItemOwner );
	RegisterEmbeddedObject(&msv_lpatMaxOccurances, SvPb::PatMaxOccurancesEId, IDS_OBJECTNAME_PAT_MAXOCCURANCES, false, SvOi::SVResetItemOwner );
	RegisterEmbeddedObject(&msv_bpatSearchAngleMode, SvPb::PatSearchAngleModeEId, IDS_OBJECTNAME_PAT_SEARCHANGLEMODE, false, SvOi::SVResetItemOwner );
	RegisterEmbeddedObject(&msv_lpatSpeed, SvPb::PatSpeedEId, IDS_OBJECTNAME_PAT_SPEED, false, SvOi::SVResetItemOwner );
	RegisterEmbeddedObject(&msv_szModelImageFile, SvPb::PatModelImageFileEId, IDS_OBJECTNAME_PAT_MODELIMAGEFILE, false, SvOi::SVResetItemOwner );
	RegisterEmbeddedObject(&m_DontCareImageFile, SvPb::PatDontCareImageFileEId, IDS_OBJECTNAME_PAT_DONT_CARE_IMAGEFILE, false, SvOi::SVResetItemOwner );

	RegisterEmbeddedObject(&msv_dpatSearchAngle, SvPb::PatSearchAngleEId, IDS_OBJECTNAME_PAT_SEARCHANGLE, false, SvOi::SVResetItemOwner );
	RegisterEmbeddedObject(&msv_dpatAngleDeltaNeg, SvPb::PatAngleDeltaNegEId, IDS_OBJECTNAME_PAT_DELTANEG, false, SvOi::SVResetItemOwner );
	RegisterEmbeddedObject(&msv_dpatAngleDeltaPos, SvPb::PatAngleDeltaPosEId, IDS_OBJECTNAME_PAT_DELTAPOS, false, SvOi::SVResetItemOwner );
	RegisterEmbeddedObject(&msv_dpatAngleTolerance, SvPb::PatAngleToleranceEId, IDS_OBJECTNAME_PAT_ANGLETOLERANCE, false, SvOi::SVResetItemOwner );
	RegisterEmbeddedObject(&msv_dpatAngleAccuracy, SvPb::PatAngleAccuracyEId, IDS_OBJECTNAME_PAT_ANGLEACCURACY, false, SvOi::SVResetItemOwner );
	RegisterEmbeddedObject(&msv_dpatAngleInterpolation, SvPb::PatAngleInterpolationEId, IDS_OBJECTNAME_PAT_INTERPOLATION, false, SvOi::SVResetItemOwner );

	RegisterEmbeddedObject(&msv_bpatCircularOverscan, SvPb::PatCircularOverscanEId, IDS_OBJECTNAME_PAT_CIRCULAROVERSCAN, false, SvOi::SVResetItemOwner );
	RegisterEmbeddedObject(&m_bpatDontCare, SvPb::PatDontCareEId, IDS_OBJECTNAME_PAT_USE_DONT_CARE, false, SvOi::SVResetItemOwner );

	RegisterEmbeddedObject(&msv_dpatPreliminaryAcceptanceThreshold, SvPb::PatPreliminaryAcceptanceThresholdEId, IDS_OBJECTNAME_PAT_PRELIMINARYACCEPTANCETHRESHOLD, false, SvOi::SVResetItemOwner );
	RegisterEmbeddedObject(&msv_lpatFastFind, SvPb::PatFastFindEId, IDS_OBJECTNAME_PAT_FASTFIND, false, SvOi::SVResetItemOwner );
	RegisterEmbeddedObject(&msv_lpatModelStep, SvPb::PatModelStepEId, IDS_OBJECTNAME_PAT_MODELSTEP, false, SvOi::SVResetItemOwner );
	RegisterEmbeddedObject(&msv_lpatBeginningResolutionLevel, SvPb::PatBeginningResolutionLevelEId, IDS_OBJECTNAME_PAT_BEGINNINGRESOLUTIONLEVEL, false, SvOi::SVResetItemOwner );
	RegisterEmbeddedObject(&msv_lpatFinalResolutionLevel, SvPb::PatFinalResolutionLevelEId, IDS_OBJECTNAME_PAT_FINALRESOLUTIONLEVEL, false, SvOi::SVResetItemOwner );
	RegisterEmbeddedObject(&msv_lpatAdditionalCandidates, SvPb::PatAdditionalCandidatesEId, IDS_OBJECTNAME_PAT_ADDITIONALCANDIDATES, false, SvOi::SVResetItemOwner );
	RegisterEmbeddedObject(&msv_dpatCandidateSpacingXMin, SvPb::PatCandidateSpacingXMinEId, IDS_OBJECTNAME_PAT_CANDIDATESPACINGXMIN, false, SvOi::SVResetItemOwner );
	RegisterEmbeddedObject(&msv_dpatCandidateSpacingYMin, SvPb::PatCandidateSpacingYMinEId, IDS_OBJECTNAME_PAT_CANDIDATESPACINGYMIN, false, SvOi::SVResetItemOwner );

	RegisterEmbeddedObject(&msv_dpatResultMatchScore, SvPb::PatResultMatchScoreEId, IDS_OBJECTNAME_PAT_MATCHSCORE, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject(&msv_dpatResultX, SvPb::PatResultXEId, IDS_OBJECTNAME_PAT_RESULTX, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject(&msv_dpatResultY, SvPb::PatResultYEId, IDS_OBJECTNAME_PAT_RESULTY, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject(&msv_dpatResultAngle, SvPb::PatResultAngleEId, IDS_OBJECTNAME_PAT_RESULTANGLE, false, SvOi::SVResetItemNone );

	RegisterEmbeddedObject(&msv_lpatNumFoundOccurances, SvPb::PatResultNumFoundOccurancesEId, IDS_OBJECTNAME_PAT_NBRFOUNDOCCURANCES, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject(&m_lpatModelWidth, SvPb::PatModelWidthEId, IDS_OBJECTNAME_PAT_MODELWIDTH, false, SvOi::SVResetItemOwner );
	RegisterEmbeddedObject(&m_lpatModelHeight, SvPb::PatModelHeightEId, IDS_OBJECTNAME_PAT_MODELHEIGHT, false, SvOi::SVResetItemOwner );
	RegisterEmbeddedObject(&m_lpatModelCenterX, SvPb::PatModelCenterXEId, IDS_OBJECTNAME_PAT_MODELCENTERX, false, SvOi::SVResetItemOwner );
	RegisterEmbeddedObject(&m_lpatModelCenterY, SvPb::PatModelCenterYEId, IDS_OBJECTNAME_PAT_MODELCENTERY, false, SvOi::SVResetItemOwner );
	RegisterEmbeddedObject(&m_dontCareWidth, SvPb::PatDontCareWidthEId, IDS_OBJECTNAME_PAT_DONTCAREWIDTH, false, SvOi::SVResetItemOwner);
	RegisterEmbeddedObject(&m_dontCareHeight, SvPb::PatDontCareHeightEId, IDS_OBJECTNAME_PAT_DONTCAREHEIGHT, false, SvOi::SVResetItemOwner);

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

	CreateResult();
	
	// Set default inputs and outputs
	addDefaultInputObjects();
}

SVPatternAnalyzerClass::~SVPatternAnalyzerClass()
{
	CloseMIL();
	SVPatternAnalyzerClass::CloseObject();
}

SVObjectClass* SVPatternAnalyzerClass::CreateResult()
{
	// Declare Input Interface of Result...
	SvIe::SVClassInfoStruct resultClassInfo;
	SvDef::SVObjectTypeInfoStruct interfaceInfo;

	interfaceInfo.m_EmbeddedID = SvPb::PatResultNumFoundOccurancesEId;
	resultClassInfo.m_DesiredInputVector.push_back( interfaceInfo );

	resultClassInfo.m_ObjectTypeInfo.m_ObjectType = SvPb::SVResultObjectType;
	resultClassInfo.m_ObjectTypeInfo.m_SubType	= SvPb::SVResultLongObjectType;
	resultClassInfo.m_ClassId = SvPb::LongResultClassId;
	resultClassInfo.m_ClassName = SvUl::LoadStdString( IDS_OBJECTNAME_RESULT );
	std::string Title = SvUl::LoadStdString( IDS_OBJECTNAME_PAT_NBRFOUNDOCCURANCES );
	resultClassInfo.m_ClassName += _T(" ") + Title;

	// Construct the result class
	SvOp::SVLongResult* pResult = dynamic_cast<SvOp::SVLongResult*> (resultClassInfo.Construct());
	if (nullptr == pResult)
	{
		SvStl::MessageManager MesMan(SvStl::MsgType::Log );
		MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16199);
		return nullptr;
	}

	// Set the defaults for the Range Object
	SvOp::SVRange *pRange = pResult->GetResultRange();
	if (nullptr == pRange)
	{
		SvStl::MessageManager MesMan(SvStl::MsgType::Log );
		MesMan.setMessage( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16200);
		return nullptr;
	}

	Add(pResult);
			
	pRange->setDefaultLowValues(1.0, 1.0);
	pRange->setLowValues(1.0, 1.0);

	return pResult;
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
	m_lpatModelWidth.SetDefaultValue(SvDef::cMinPatternModelNewSize);
	m_lpatModelHeight.SetDefaultValue(SvDef::cMinPatternModelNewSize);
	m_dontCareWidth.SetDefaultValue(SvDef::cMinPatternModelNewSize);
	m_dontCareHeight.SetDefaultValue(SvDef::cMinPatternModelNewSize);

	m_vec2dPatResults.fill(0.0);
}

bool SVPatternAnalyzerClass::UpdateModelFromInputImage(long posX, long posY, long modelWidth, long modelHeight)
{
	if (SvDef::cMinPatternModelNewSize > modelWidth)
	{
		modelWidth = SvDef::cMinPatternModelNewSize;
	}
	if (SvDef::cMinPatternModelNewSize > modelHeight)
	{
		modelHeight = SvDef::cMinPatternModelNewSize;
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

	SvIe::SVImageClass *pInputImage = getInputImage();

	if( nullptr != pInputImage)
	{
		// Destroy and Recreate Model Image Buffer
		CreateModelBuffer();

		SVImageInfoClass imageInfo = pInputImage->GetImageInfo();
		
		// Copy Source Image into Model Image
		// Create a child buffer on the Source Image
		SVImageInfoClass childImageInfo;
		SvOi::SVImageBufferHandlePtr childImageHandle;
		
		childImageInfo.SetImageProperty( SvDef::SVImagePropertyEnum::SVImagePropertyPixelDepth, SV8BitUnsigned );
		childImageInfo.SetImageProperty( SvDef::SVImagePropertyEnum::SVImagePropertyBandNumber, 1 );

		childImageInfo.SetExtentProperty( SvPb::SVExtentPropertyPositionPointX, posX );
		childImageInfo.SetExtentProperty( SvPb::SVExtentPropertyPositionPointY, posY );
		long tmp = 0;
		m_lpatModelWidth.GetValue(tmp);
		childImageInfo.SetExtentProperty( SvPb::SVExtentPropertyWidth, tmp );
		m_lpatModelHeight.GetValue(tmp);
		childImageInfo.SetExtentProperty( SvPb::SVExtentPropertyHeight, tmp );

		SvOi::SVImageBufferHandlePtr pImageBuffer = pInputImage->getLastImage(true);

		if (nullptr != pImageBuffer && !pImageBuffer->empty())
		{
			HRESULT hr = SvIe::SVImageProcessingClass::CreateImageChildBuffer(imageInfo, pImageBuffer, childImageInfo, childImageHandle);
			if (S_OK == hr && nullptr != childImageHandle && nullptr != m_patBufferHandlePtr)
			{
				// Copy from source child to Model Image buffer
				SVMatroxBufferInterface::CopyBuffer(m_patBufferHandlePtr->GetBuffer(), childImageHandle->GetBuffer());

				// free child buffer
				childImageHandle.reset();

				// Create the Model Pattern
				bOk = UpdateModelFromBuffer();
			}
			else
			{
				bOk = false;
			}
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

	if (nullptr != m_patBufferHandlePtr)
	{
		HRESULT MatroxCode;

		long modelWidth=0;
		long modelHeight=0;
		MatroxCode = SVMatroxBufferInterface::Get(m_patBufferHandlePtr->GetBuffer(), SVSizeX, modelWidth );
		
		if (S_OK == MatroxCode)
		{
			m_lpatModelWidth.SetValue(modelWidth);
			MatroxCode = SVMatroxBufferInterface::Get(m_patBufferHandlePtr->GetBuffer(), SVSizeY, modelHeight );
			if (S_OK == MatroxCode)
			{
				m_lpatModelHeight.SetValue(modelHeight);
			}
		}

		if (S_OK == MatroxCode)
		{
			MatroxCode = CreateModelHandle(modelWidth, modelHeight);

		}

		bOk = ( M_NULL != m_patContextHandle && S_OK == MatroxCode);
	}
	return bOk;
}

bool SVPatternAnalyzerClass::RestorePattern (const std::string& rImageFile, SvStl::MessageContainerVector *pErrorMessages)
{
	bool bOk = ReloadImage(rImageFile, m_lpatModelWidth, m_lpatModelHeight, m_patBufferHandlePtr, pErrorMessages);


	if ( bOk && !UpdateModelFromBuffer() )
	{
		bOk = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_PatAllocModelFailed, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId() );
			pErrorMessages->push_back(Msg);
		}
	}

	if ( bOk && !SetSearchParameters() )
	{
		bOk = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_Pattern_SetSearchParamFailed, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId() );
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
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_SetValueFailed, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId() );
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
		SvIe::SVImageClass *pImage  = dynamic_cast<SvIe::SVImageClass*> (getInputImage());

		if (nullptr != pImage)
		{
			long	lParam;
			msv_lpatMaxOccurances.GetValue(lParam);

			SvOi::SVImageBufferHandlePtr pImageBuffer = pImage->getLastImage(true);

			SVMatroxBuffer ImageBufId;
			if(nullptr !=  pImageBuffer && !pImageBuffer->empty())
			{
				ImageBufId = pImageBuffer->GetBuffer();
			}

			long modelWidth = 0;
			long modelHeight = 0;
			m_lpatModelWidth.GetValue(modelWidth);
			m_lpatModelHeight.GetValue(modelHeight);
			CreateModelHandle(modelWidth, modelHeight);
		
			HRESULT MatroxCode = SVMatroxPatternInterface::SetNumber( m_patContextHandle, lParam );

			if (S_OK == MatroxCode)
			{
				ResizeResultValues((int)lParam);
				
				MatroxCode = SVMatroxPatternInterface::DestroyResult( m_patResultHandle );

				if (S_OK == MatroxCode)
				{
					MatroxCode = SVMatroxPatternInterface::CreateResult( m_patResultHandle, lParam );
				}

				if (S_OK == MatroxCode)
				{
					long centerX = 0;
					long centerY = 0;
					m_lpatModelCenterX.GetValue(centerX);
					m_lpatModelCenterY.GetValue(centerY);
					MatroxCode = SVMatroxPatternInterface::SetCenter( m_patContextHandle, centerX, centerY );

					if (S_OK == MatroxCode)
					{
						MatroxCode = SVMatroxPatternInterface::SetPosition( m_patContextHandle, SVValueAll, SVValueAll,
										SVValueAll, SVValueAll);
					}

					double	dParam;
					if (S_OK == MatroxCode)
					{
						msv_dpatAcceptanceThreshold.GetValue(dParam);
						MatroxCode = SVMatroxPatternInterface::SetAcceptance( m_patContextHandle, dParam);
					}

					if (S_OK == MatroxCode)
					{
						msv_dpatCertaintyThreshold.GetValue(dParam); 
						MatroxCode = SVMatroxPatternInterface::SetCertainty(m_patContextHandle, dParam);
					}
					
					if (S_OK == MatroxCode)
					{
						msv_lpatAccuracy.GetValue(lParam);
						MatroxCode = SVMatroxPatternInterface::SetAccuracy(m_patContextHandle, lParam);
					}
					
					if (S_OK == MatroxCode)
					{
						msv_lpatSpeed.GetValue(lParam);
						MatroxCode = SVMatroxPatternInterface::SetSpeed( m_patContextHandle, lParam );
					}
					
					BOOL	bParam{false};
					if (S_OK == MatroxCode)
					{
						msv_bpatSearchAngleMode.GetValue( bParam ); 
						MatroxCode = SVMatroxPatternInterface::SetAngle( m_patContextHandle, bParam ? true : false );
					}
					
					if (S_OK == MatroxCode)
					{
						if(bParam)
						{
							msv_dpatSearchAngle.GetValue(dParam);
							MatroxCode = SVMatroxPatternInterface::SetAngle( m_patContextHandle, SVPatSearchAngle, dParam );
					
							if (S_OK == MatroxCode)
							{
								msv_dpatAngleDeltaNeg.GetValue(dParam);
								MatroxCode = SVMatroxPatternInterface::SetAngle( m_patContextHandle, SVPatSearchAngleDeltaNeg, dParam );
							}
					
							if (S_OK == MatroxCode)
							{
								msv_dpatAngleDeltaPos.GetValue(dParam);
								MatroxCode = SVMatroxPatternInterface::SetAngle( m_patContextHandle, SVPatSearchAngleDeltaPos, dParam );
							}

							if (S_OK == MatroxCode)
							{
								msv_dpatAngleTolerance.GetValue(dParam);
								MatroxCode = SVMatroxPatternInterface::SetAngle( m_patContextHandle, SVPatSearchAngleTolerance, dParam );
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
									MatroxCode = SVMatroxPatternInterface::SetAngle( m_patContextHandle, SVPatSearchAngleDeltaNeg, dParam );
									m_bAngleAccuracy = true;
								}
							}

							if (S_OK == MatroxCode)
							{
								msv_dpatAngleInterpolation.GetValue(dParam);
								MatroxCode = SVMatroxPatternInterface::SetAngle( m_patContextHandle, SVPatSearchAngleInterpMode, dParam );
							}
						}
					}

					// Set Advanced Search Parameters
					if (S_OK == MatroxCode)
					{
						msv_dpatPreliminaryAcceptanceThreshold.GetValue(dParam);
						MatroxCode = SVMatroxPatternInterface::Set(m_patContextHandle, SVPatCoarseSearchAcceptance, dParam);
					}

					if (S_OK == MatroxCode)
					{
						msv_lpatFastFind.GetValue(lParam);
						MatroxCode = SVMatroxPatternInterface::Set(m_patContextHandle, SVPatFastFind, lParam);
					}

					if (S_OK == MatroxCode)
					{
						msv_lpatModelStep.GetValue(lParam);
						MatroxCode = SVMatroxPatternInterface::Set(m_patContextHandle, SVPatModelStep, lParam);
					}

					if (S_OK == MatroxCode)
					{
						msv_lpatBeginningResolutionLevel.GetValue(lParam);
						MatroxCode = SVMatroxPatternInterface::Set(m_patContextHandle, SVPatFirstLevel, lParam);
					}

					if (S_OK == MatroxCode)
					{
						msv_lpatFinalResolutionLevel.GetValue(lParam);
						MatroxCode = SVMatroxPatternInterface::Set(m_patContextHandle, SVPatLastLevel, lParam);
					}

					if (S_OK == MatroxCode)
					{
						msv_lpatAdditionalCandidates.GetValue(lParam);
						MatroxCode = SVMatroxPatternInterface::Set(m_patContextHandle, SVPatExtraCandidates, lParam);
					}

					if (S_OK == MatroxCode)
					{
						msv_dpatCandidateSpacingXMin.GetValue(dParam);
						MatroxCode = SVMatroxPatternInterface::Set(m_patContextHandle, SVPatMinSpacingX, dParam);
					}

					if (S_OK == MatroxCode)
					{
						msv_dpatCandidateSpacingYMin.GetValue(dParam);
						MatroxCode = SVMatroxPatternInterface::Set(m_patContextHandle, SVPatMinSpacingY, dParam); 
					}

					if (S_OK == MatroxCode)
					{
						BOOL useDontCare = false;
						m_bpatDontCare.GetValue(useDontCare);
						if (useDontCare)
						{
							if (nullptr != m_DontCareBufferHandlePtr && !m_DontCareBufferHandlePtr->empty())
							{
								MatroxCode = SVMatroxPatternInterface::SetDontCare(m_DontCareBufferHandlePtr->GetBuffer(), m_patContextHandle);
							}
							else
							{
								bOk = false;
							}
						}
					}

					if (S_OK == MatroxCode)
					{
						if( !ImageBufId.empty() )
						{
							MatroxCode = SVMatroxPatternInterface::PreProcModel( m_patContextHandle, ImageBufId );
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
		SvStl::MessageManager Exception(SvStl::MsgType::Log );
		Exception.setMessage( SVMSG_SVO_UNHANDLED_EXCEPTION, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId() );
		bOk = false;
	}
	return bOk;
}

void SVPatternAnalyzerClass::CloseMIL ()
{
	SVMatroxPatternInterface::DestroyContext( m_patContextHandle );
	SVMatroxPatternInterface::DestroyResult( m_patResultHandle );
	m_patBufferHandlePtr.reset();
	m_DontCareBufferHandlePtr.reset();
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
bool SVPatternAnalyzerClass::CreateObject(const SVObjectLevelCreateStruct& rCreateStructure)
{
	bool bOk = SVImageAnalyzerClass::CreateObject(rCreateStructure);

	// Set / Reset Printable Flags
	const UINT cAttributes = SvPb::printable | SvPb::remotelySetable;
	msv_dpatAcceptanceThreshold.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
	msv_dpatCertaintyThreshold.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
	msv_lpatAccuracy.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
	msv_lpatMaxOccurances.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::AddAttribute );
	msv_bpatSearchAngleMode.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::AddAttribute );
	msv_lpatSpeed.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );

	msv_dpatSearchAngle.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::AddAttribute );
	msv_dpatAngleDeltaNeg.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::AddAttribute );
	msv_dpatAngleDeltaPos.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::AddAttribute );
	msv_dpatAngleTolerance.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::AddAttribute );
	msv_dpatAngleAccuracy.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::AddAttribute );
	msv_dpatAngleInterpolation.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::AddAttribute );

	msv_dpatResultMatchScore.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::RemoveAttribute );
	msv_dpatResultX.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::RemoveAttribute );
	msv_dpatResultY.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::RemoveAttribute );
	msv_dpatResultAngle.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::RemoveAttribute );

	msv_szModelImageFile.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );

	msv_bpatCircularOverscan.SetObjectAttributesAllowed( SvPb::remotelySetable, SvOi::SetAttributeType::AddAttribute );
	msv_bpatCircularOverscan.SetObjectAttributesAllowed( SvPb::setableOnline, SvOi::SetAttributeType::RemoveAttribute );
	m_bpatDontCare.SetObjectAttributesAllowed(SvPb::remotelySetable, SvOi::SetAttributeType::AddAttribute);
	m_bpatDontCare.SetObjectAttributesAllowed(SvPb::setableOnline, SvOi::SetAttributeType::RemoveAttribute);
	m_DontCareImageFile.SetObjectAttributesAllowed(cAttributes, SvOi::SetAttributeType::AddAttribute);
	
	msv_dpatPreliminaryAcceptanceThreshold.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
	msv_lpatFastFind.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
	msv_lpatModelStep.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
	msv_lpatBeginningResolutionLevel.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
	msv_lpatFinalResolutionLevel.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
	msv_lpatAdditionalCandidates.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
	msv_dpatCandidateSpacingXMin.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );
	msv_dpatCandidateSpacingYMin.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::AddAttribute );

	msv_lpatNumFoundOccurances.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::RemoveAttribute );

	// Check if Result is present
	if (nullptr == GetResultObject())
	{
		SVObjectClass* pResult = CreateResult();

		// Ensure this Object's inputs get connected
		if (nullptr != pResult)
		{
			pResult->ConnectAllInputs();
		}

		CreateChildObject(pResult);
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
	msv_dpatResultMatchScore.SetResultSize(0);
	msv_dpatResultAngle.SetResultSize(0);
	msv_dpatResultX.SetResultSize(0);
	msv_dpatResultY.SetResultSize(0);
	int ArraySize = msv_dpatResultMatchScore.getArraySize();
	for(int i=0; i < ArraySize; i++)
	{
		msv_dpatResultMatchScore.SetValue(0.0, i);
		msv_dpatResultX.SetValue(0.0, i);
		msv_dpatResultY.SetValue(0.0, i);
		msv_dpatResultAngle.SetValue(0.0, i);
	}
}

bool SVPatternAnalyzerClass::onRun (RunStatus &rRunStatus, SvStl::MessageContainerVector *pErrorMessages)
{
	//@WARNING[MZA][7.50][17.01.2017] Not sure if we need to check ValidateLocal in Run-mode, maybe it is enough to check it in ResetObject
	bool Result = __super::onRun(rRunStatus, pErrorMessages) && ValidateLocal(pErrorMessages);
	try
	{
		if (Result)
		{
			SvIe::SVImageClass* pInputImage = getInputImage(true);
			SvTrc::IImagePtr pImageBuffer = (nullptr!=pInputImage) ? pInputImage->getImageReadOnly(rRunStatus.m_triggerRecord.get()) : nullptr;
			if( nullptr != pImageBuffer && !pImageBuffer->isEmpty())
			{
				SVMatroxBuffer ImageBufId = pImageBuffer->getHandle()->GetBuffer();
				HRESULT MatroxCode = executePatternAndSetResults(ImageBufId);

				// check if one of the matrox calls failed
				if (S_OK != MatroxCode)
				{
					Result = false;
					if (nullptr != pErrorMessages)
					{
						SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId() );
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
					SvStl::MessageContainer Msg( SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16206, getObjectId() );
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
			SvStl::MessageContainer Msg( SVMSG_SVO_UNHANDLED_EXCEPTION, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId() );
			pErrorMessages->push_back(Msg);
		}
		rRunStatus.SetInvalid();
	}

	if (!Result)
	{
		ResetResultValues();
	}
	return Result;
}

bool SVPatternAnalyzerClass::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = __super::ResetObject(pErrorMessages);

	if ( Result )
	{
		SvOi::IInspectionProcess* pInspection = GetInspectionInterface();

		// Insures that the Pattern will be reloaded when going online.
		if ( nullptr != pInspection && pInspection->IsResetStateSet(SvDef::SVResetStateLoadFiles ) )
		{
			ResetPattern();
		}

		if( M_NULL == m_patContextHandle || m_bReloadModelFromFile ) // if the model exists, update the search area according to the 
		{
			m_bReloadModelFromFile = false;

			// Check whether there is a valid model file available.
			// If so, restore the model from the file.
			std::string FileName;

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
					SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_Pattern_GetModelImageNameFailed, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId() );
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
					SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
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
					SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_Pattern_SetSearchParamFailed, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId() );
					pErrorMessages->push_back(Msg);
				}
			}
		}
	}
	return Result;
}

SvDef::StringVector SVPatternAnalyzerClass::getSpecialImageList() const
{
	return {SvDef::PatternModelImageName, SvDef::PatternDontCareImageName};
}

bool SVPatternAnalyzerClass::getSpecialImage(const std::string& rName, SvOi::SVImageBufferHandlePtr& rImagePtr) const
{
	if (SvDef::PatternModelImageName == rName)
	{
		rImagePtr = m_patBufferHandlePtr;
		return true;
	}
	else if (SvDef::PatternDontCareImageName == rName)
	{
		rImagePtr = m_DontCareBufferHandlePtr;
		return true;
	}
	return false;
}

SvStl::MessageContainerVector SVPatternAnalyzerClass::validateAndSetEmbeddedValues(const SvOi::SetValueStructVector& rValueVector, bool shouldSet)
{
	SvStl::MessageContainerVector messages;
	long dontCareWidth = 0;
	long dontCareHeight = 0;
	long modelWidth = 0;
	long modelHeight = 0;
	bool useDontCare = getNewUseDontCareValue(rValueVector);

	if (useDontCare)
	{
		validateNewDontCareFileName(rValueVector, dontCareWidth, dontCareHeight, messages);
	}

	bool isModelFileNameToSet = validateNewModelFileName(rValueVector, modelWidth, modelHeight, messages);
	
	IsValidSize(modelWidth, modelHeight, useDontCare, dontCareWidth, dontCareHeight, &messages);
	
	if (messages.empty())
	{
		m_bReloadModelFromFile = isModelFileNameToSet;
		return __super::validateAndSetEmbeddedValues(rValueVector, shouldSet);
	}
	return messages;
}

void SVPatternAnalyzerClass::addParameterForMonitorList(SvStl::MessageContainerVector& , std::back_insert_iterator<SvOi::ParametersForML> inserter) const
{
	inserter = SvOi::ParameterPairForML(msv_dpatResultX.GetCompleteName(), msv_dpatResultX.getObjectId());
	// cppcheck-suppress redundantAssignment symbolName=inserter ; cppCheck doesn't know back_insert_iterator
	inserter = SvOi::ParameterPairForML(msv_dpatResultY.GetCompleteName(), msv_dpatResultY.getObjectId());
	inserter = SvOi::ParameterPairForML(msv_dpatResultAngle.GetCompleteName(), msv_dpatResultAngle.getObjectId());
	inserter = SvOi::ParameterPairForML(msv_dpatResultMatchScore.GetCompleteName(), msv_dpatResultMatchScore.getObjectId());
	// cppcheck-suppress unreadVariable symbolName=inserter ; cppCheck doesn't know back_insert_iterator
	// cppcheck-suppress redundantAssignment symbolName=inserter ; cppCheck doesn't know back_insert_iterator
	inserter = SvOi::ParameterPairForML(msv_lpatNumFoundOccurances.GetCompleteName(), msv_lpatNumFoundOccurances.getObjectId());
}

HRESULT SVPatternAnalyzerClass::onCollectOverlays(SvIe::SVImageClass* , SVExtentMultiLineStructVector& rMultiLineArray )
{
	// only if ToolSet/Tool was not Disabled
	SvTo::SVToolClass* pTool = dynamic_cast<SvTo::SVToolClass*> (GetTool());
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
			l_multiLine.m_Color = SvDef::DefaultSubFunctionColor1;
			l_multiLine.AssignExtentFigure( figureList[i], SvDef::DefaultSubFunctionColor1 );
			UpdateOverlayIDs( l_multiLine );
			rMultiLineArray.push_back( l_multiLine );
		}
	}
	return S_OK;
}

void SVPatternAnalyzerClass::addOverlayGroups(const SvIe::SVImageClass*, SvPb::Overlay& rOverlay) const
{
	auto* pGroup = rOverlay.add_shapegroups();
	pGroup->set_name("Pattern");
	pGroup->set_detaillevel(SvPb::Level1);
	auto* pShape = pGroup->add_shapes();
	pShape->mutable_color()->set_value(SvDef::DefaultSubFunctionColor1);
	auto* pRectArray = pShape->mutable_rectarray()->mutable_patterndata();
	pRectArray->set_xtrpos(msv_dpatResultX.getTrPos() + 1);
	pRectArray->set_ytrpos(msv_dpatResultY.getTrPos() + 1);
	pRectArray->set_angletrpos(msv_dpatResultAngle.getTrPos() + 1);
	pRectArray->set_numberpos(msv_lpatNumFoundOccurances.getTrPos() + 1);

	long width = 0;
	long height = 0;
	m_lpatModelWidth.GetValue(width);
	m_lpatModelHeight.GetValue(height);
	BOOL bCircularScan;
	msv_bpatCircularOverscan.GetValue(bCircularScan);
	if (bCircularScan)
	{
		POINT pos = { 0, 0 };
		SIZE size = { width, height };

		RECT rect = SVMatroxPatternInterface::CalculateOverscanInnerRect(pos, size);
		width = rect.right - rect.left;
		height = rect.bottom - rect.top;
	}
	pRectArray->set_width(width);
	pRectArray->set_height(height);

	long centerX = 0;
	long centerY = 0;
	m_lpatModelCenterX.GetValue(centerX);
	m_lpatModelCenterY.GetValue(centerY);
	pRectArray->set_centerx(centerX);
	pRectArray->set_centery(centerY);
	SVPoint<double> startPos{ 0., 0. }, centerPos{ static_cast<double>(centerX), static_cast<double>(centerY) };
	pRectArray->set_centerradius(SVGetRadius(startPos, centerPos));
	pRectArray->set_centerangle(SVGetRotationAngle(startPos, centerPos));
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : IsPtOverResult( CPoint point )
// -----------------------------------------------------------------------------
// .Description : When the user right clicks inside a tool figure, checks whether its
//              : on a pattern. If it finds a pattern at that point, gets the index of the  
//				: pattern and returns true.
//				: Called from SVImageView::OnContextMenu
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
		if( S_OK == figureList[i].IsPointOverFigure( SVPoint<double>(rPoint) ) )
		{
			m_nPatternIndex = i;
			return true;
		}
	}

	return false;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : getAnalyzerResult()
// -----------------------------------------------------------------------------
// .Description : When the user right clicks on a pattern inside a tool figure and 
//              : selects the Menu item 'Display Analyzer Result', display the 
//				: Match Score, X & Y values and the Angle of this pattern.
//				: Called from SVImageView::OnCommand
SvDef::StringVector SVPatternAnalyzerClass::getAnalyzerResult()
{
	SvDef::StringVector result;

	long lOccurances = 0;
	msv_lpatNumFoundOccurances.GetValue(lOccurances);

	if(m_nPatternIndex < 0 || m_nPatternIndex >= lOccurances)
	{
		return result;
	}

	
	BOOL bValue;
	msv_bpatSearchAngleMode.GetValue(bValue);

	std::string Value;
	double dResultAngle( 0.0 );;
	msv_dpatResultAngle.GetValue( dResultAngle, m_nPatternIndex );
	if(bValue && dResultAngle > 0.0)
	{
		Value= SvUl::Format(_T("%3.1lf"), dResultAngle);
	}
	result.emplace_back(Value);

	double dMatchScore( 0.0 );
	msv_dpatResultMatchScore.GetValue( dMatchScore, m_nPatternIndex );
	Value = SvUl::Format(_T("%3.1lf"), dMatchScore);
	result.emplace_back(Value);

	double dResultXPos( 0.0 );
	msv_dpatResultX.GetValue( dResultXPos, m_nPatternIndex );
	Value = SvUl::Format(_T("%4.1lf"), dResultXPos);
	result.emplace_back(Value);

	double dResultYPos( 0.0 );
	msv_dpatResultY.GetValue( dResultYPos, m_nPatternIndex );
	Value = SvUl::Format(_T("%4.1lf"), dResultYPos);
	result.emplace_back(Value);

	return result;
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
	std::string	FileName;
	
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
						SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_PatAllocModelFailed, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId() );
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
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_Pattern_ResetImageFileFailed, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId() );
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
		SvTo::SVToolClass* pTool = dynamic_cast<SvTo::SVToolClass*> (GetTool());
		if (nullptr != pTool)
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
				SVPoint<double> moveVector = SVRotatePoint(SVPoint<double>(0, 0), SVPoint<double>(centerX, centerY), -dResultAngle);
				dResultXPos -= moveVector.m_x;
				dResultYPos -= moveVector.m_y;
				bError = dResultXPos < 0.0 || dResultYPos < 0.0	||dResultAngle < 0.0;

				if ( !bError )
				{
					SVImageExtentClass patternExtents;

					patternExtents.SetTranslation(SvPb::SVExtentTranslationFlippedRotate );
					patternExtents.SetExtentProperty( SvPb::SVExtentPropertyPositionPointX, dResultXPos );
					patternExtents.SetExtentProperty( SvPb::SVExtentPropertyPositionPointY, dResultYPos );
					patternExtents.SetExtentProperty( SvPb::SVExtentPropertyWidth, lpatModelWidth );
					patternExtents.SetExtentProperty( SvPb::SVExtentPropertyHeight, lpatModelHeight );
					patternExtents.SetExtentProperty( SvPb::SVExtentPropertyRotationAngle, dResultAngle );
					patternExtents.UpdateData();
					SVExtentFigureStruct figure=patternExtents.GetFigure();

					pTool->GetImageExtent().TranslateFromOutputSpace(figure, figure);

					retList.push_back(figure);
				}
			}
		}
	}
	return retList;
}

HRESULT SVPatternAnalyzerClass::executePatternAndSetResults( SVMatroxBuffer ImageBufId)
{
	HRESULT MatroxCode = SVMatroxPatternInterface::Execute( m_patResultHandle, ImageBufId, m_patContextHandle );
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

	if ( Result && (M_NULL == m_patContextHandle || M_NULL == m_patResultHandle) )
	{
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_PatAllocModelFailed, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId() );
			pErrorMessages->push_back(Msg);
		}
		Result = false;
	}

	return Result;
}

bool SVPatternAnalyzerClass::RestoreDontCareImage(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = true;
	m_DontCareBufferHandlePtr.reset();
	std::string FileName;

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
				SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_DontCareInvalidFilename, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
			pErrorMessages->push_back(Msg);
		}
	}
	}
	else
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
			pErrorMessages->push_back(Msg);
		}
	}
	return Result;
}

bool SVPatternAnalyzerClass::getNewUseDontCareValue(const SvOi::SetValueStructVector &rValueVector)
{
	BOOL useDontCare = false;
	SvOi::SetValueStructVector::const_iterator iter = std::find_if(rValueVector.begin(), rValueVector.end(), [&](const SvOi::SetValueStructVector::value_type& rEntry)->bool
	{
		return rEntry.m_pValueObject == &m_bpatDontCare;
	}
	);
	if (rValueVector.end() != iter)
	{
		assert(VT_BOOL == iter->m_Value.vt);
		useDontCare = iter->m_Value.boolVal;
	}
	else
	{
		m_bpatDontCare.GetValue(useDontCare);
	}
	return useDontCare ? true : false;
}

bool SVPatternAnalyzerClass::validateNewDontCareFileName(const SvOi::SetValueStructVector &rValueVector, long& rDontCareWidth, long& rDontCareHeight, SvStl::MessageContainerVector& rMessages)
{
	bool isValueToSet = false;
	SvOi::SetValueStructVector::const_iterator iter = std::find_if(rValueVector.begin(), rValueVector.end(), [&](const SvOi::SetValueStructVector::value_type& rEntry)->bool
	{
		return rEntry.m_pValueObject == &m_DontCareImageFile;
	}
	);
	if (rValueVector.end() != iter)
	{
		assert(VT_BSTR == iter->m_Value.vt);
		std::string newFileName = SvUl::createStdString(iter->m_Value.bstrVal);

		SVMatroxBuffer importHandle;
		if (S_OK != SVMatroxBufferInterface::Import(importHandle, newFileName, SVFileBitmap, true) ||
			S_OK != SVMatroxBufferInterface::Get(importHandle, SVSizeX, rDontCareWidth) ||
			S_OK != SVMatroxBufferInterface::Get(importHandle, SVSizeY, rDontCareHeight))
		{
			SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_DontCareInvalidFilename, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
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
		std::string FileName;
		m_DontCareImageFile.GetValue(FileName);
		if (FileName.empty())
		{
			SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_DontCareInvalidFilename, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
			rMessages.push_back(Msg);
		}
		m_dontCareWidth.GetValue(rDontCareWidth);
		m_dontCareHeight.GetValue(rDontCareHeight);
	}
	return isValueToSet;
}

bool SVPatternAnalyzerClass::validateNewModelFileName(const SvOi::SetValueStructVector &rValueVector, long& rModelWidth, long& rModelHeight, SvStl::MessageContainerVector &messages)
{
	bool isValueToSet = false;
	SvOi::SetValueStructVector::const_iterator iter = std::find_if(rValueVector.begin(), rValueVector.end(), [&](const SvOi::SetValueStructVector::value_type& rEntry)->bool
	{
		return rEntry.m_pValueObject == &msv_szModelImageFile;
	}
	);
	if (rValueVector.end() != iter)
	{
		assert(VT_BSTR == iter->m_Value.vt);
		std::string fileName;
		msv_szModelImageFile.GetValue(fileName);
		std::string newFileName = SvUl::createStdString(iter->m_Value.bstrVal);
		if (fileName != newFileName)
		{
			SVMatroxBuffer importHandle;
			if (S_OK != SVMatroxBufferInterface::Import(importHandle, newFileName, SVFileBitmap, true) ||
				S_OK != SVMatroxBufferInterface::Get(importHandle, SVSizeX, rModelWidth) ||
				S_OK != SVMatroxBufferInterface::Get(importHandle, SVSizeY, rModelHeight))
			{
				SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_PatInvalidFilename, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
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

	SvTo::SVToolClass* pTool = dynamic_cast<SvTo::SVToolClass*> (GetTool());
	if (nullptr != pTool)
	{
		RECT oRec;
		HRESULT hrOk = pTool->GetImageExtent().GetOutputRectangle(oRec);

		if (S_OK == hrOk)
		{

			if (oRec.bottom < modelHeight || oRec.right < modelWidth)
			{
				bRet = false;
				if (nullptr != pErrorMessages)
				{
					SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_Pattern_Model2Large, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
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
						SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_Pattern_DontCare2Small, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
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
			SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
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

bool SVPatternAnalyzerClass::CreateBuffer(long width, long height, SvOi::SVImageBufferHandlePtr& rBufferHandle)
{
	rBufferHandle.reset();

	// Allocate Buffer for storing the Model Image
	SVImageInfoClass patBuffer;
	patBuffer.SetImageProperty(SvDef::SVImagePropertyEnum::SVImagePropertyPixelDepth, SV8BitUnsigned);
	patBuffer.SetImageProperty(SvDef::SVImagePropertyEnum::SVImagePropertyBandNumber, 1);

	patBuffer.SetExtentProperty(SvPb::SVExtentPropertyPositionPoint, SVPoint<double>(0.0, 0.0));
	patBuffer.SetExtentProperty(SvPb::SVExtentPropertyWidth, width);
	patBuffer.SetExtentProperty(SvPb::SVExtentPropertyHeight, height);

	return (S_OK == SvIe::SVImageProcessingClass::CreateImageBuffer(patBuffer, rBufferHandle));
}

bool SVPatternAnalyzerClass::ReloadImage(const std::string& rImageFile, SvVol::SVLongValueObjectClass& rWidthValueObject, SvVol::SVLongValueObjectClass& rHeightValueObject, SvOi::SVImageBufferHandlePtr& rBufferHandle, SvStl::MessageContainerVector* pErrorMessages)
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
			bOk = CreateBuffer(width, height, rBufferHandle) && (nullptr != rBufferHandle);

			if (bOk)
			{
				MatroxCode = SVMatroxBufferInterface::CopyBuffer(rBufferHandle->GetBuffer(), importHandle);
				if (S_OK != MatroxCode)
				{
					bOk = false;
					if (nullptr != pErrorMessages)
					{
						SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_CopyImagesFailed, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
						pErrorMessages->push_back(Msg);
					}
				}
			}
			else
			{
				bOk = false;
				if (nullptr != pErrorMessages)
				{
					SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_PatAllocFailed, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
					pErrorMessages->push_back(Msg);
				}
			}
		}
		else
		{
			bOk = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
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
			SvStl::MessageContainer Msg(SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16205, getObjectId());
			pErrorMessages->push_back(Msg);
		}
	}
	return bOk;
}

HRESULT SVPatternAnalyzerClass::CreateModelHandle(long modelWidth, long modelHeight)
{
	HRESULT MatroxCode = S_OK; 

	// Destroy the pattern
	SVMatroxPatternInterface::DestroyContext(m_patContextHandle);

	// Check if Circular Overscan is enabled
	BOOL bCircularScan;
	msv_bpatCircularOverscan.GetValue(bCircularScan);

	if (bCircularScan)
	{
		POINT pos = { 0, 0 };
		SIZE size = { modelWidth, modelHeight };
		RECT innerRect = SVMatroxPatternInterface::CalculateOverscanInnerRect(pos, size);

		MatroxCode = SVMatroxPatternInterface::CreateContext(m_patContextHandle,
			m_patBufferHandlePtr->GetBuffer(),
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
		MatroxCode = SVMatroxPatternInterface::CreateContext(m_patContextHandle,
			m_patBufferHandlePtr->GetBuffer(),
			0, 0,
			modelWidth, modelHeight);
	}	
	return MatroxCode;
}

} //namespace SvAo
