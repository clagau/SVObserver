//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLinearImageOperatorList
//* .File Name       : $Workfile:   SVLinearImageOperatorList.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   13 Aug 2013 10:08:52  $
//******************************************************************************

#include "stdafx.h"
#include "SVLinearImageOperatorList.h"
#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVObjectLibrary/SVObjectLevelCreateStruct.h"
#include "SVLinearToolClass.h"
#include "InspectionEngine/SVUnaryImageOperatorClass.h"
#include "InspectionEngine/SVTool.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SV_IMPLEMENT_CLASS( SVLinearImageOperatorListClass, SVLinearImageOperatorListClassGuid )


SVLinearImageOperatorListClass::SVLinearImageOperatorListClass( SVObjectClass* POwner, int StringResourceID )
							    :SVStdImageOperatorListClass( POwner, StringResourceID ) 
{
	init();
}

SVLinearImageOperatorListClass::~SVLinearImageOperatorListClass() 
{ 
	CloseObject();

}

bool SVLinearImageOperatorListClass::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	bool l_bOk = SVStdImageOperatorListClass::CreateObject(rCreateStructure);

	SVDataBufferInfoClass& rBufferInfo = m_svProfileResultData.GetDataBufferInfo();
	rBufferInfo.Type = SVDataBufferInfoClass::SVProjectResult;

	BOOL UseRotation = TRUE;

	if( ( S_OK == getUseRotationAngle( UseRotation ) ) && !UseRotation )
	{
		outputImageObject.InitializeImage( SvDef::SVImageTypeEnum::SVImageTypeLogicalAndPhysical );
	}
	else
	{
		outputImageObject.InitializeImage( SvDef::SVImageTypeEnum::SVImageTypePhysical );
	}

	l_bOk &= S_OK == UpdateLineExtentData();
	m_svLinearData.SetObjectAttributesAllowed( SvDef::SV_VIEWABLE | SvDef::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );

	m_svMaxThreshold.SetObjectAttributesAllowed( SvDef::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );
	m_svMinThreshold.SetObjectAttributesAllowed( SvDef::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );


	return l_bOk;
}

bool SVLinearImageOperatorListClass::CloseObject()
{
	bool l_bOk = m_svProfileResultData.CloseObject();

	l_bOk &= SVStdImageOperatorListClass::CloseObject();

	return l_bOk;
}

bool SVLinearImageOperatorListClass::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	BOOL UseRotation = TRUE;

	if( (S_OK == getUseRotationAngle( UseRotation )) && !UseRotation )
	{
		outputImageObject.InitializeImage( SvDef::SVImageTypeEnum::SVImageTypeLogicalAndPhysical );
	}
	else
	{
		outputImageObject.InitializeImage( SvDef::SVImageTypeEnum::SVImageTypePhysical );
	}

	bool Result = __super::ResetObject(pErrorMessages);

	CollectInputImageNames();

	if( S_OK != UpdateLineExtentData() )
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_UpdateLineExtentDataFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
	}

	SvOi::ITool* pTool = GetToolInterface();
	if (nullptr != pTool)
	{
		pTool->UpdateImageWithExtent();
	}

	return Result;
}

bool SVLinearImageOperatorListClass::Run( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	SVImageBufferHandlePtr output;
	SVImageBufferHandlePtr input;
	SVImageExtentClass l_svImageExtent;

	BOOL UseRotation = true;

	SVToolClass* pTool  = dynamic_cast<SVToolClass*>(GetTool());
	bool bRetVal = (pTool && S_OK == pTool->GetImageExtent( l_svImageExtent ));

	bRetVal = bRetVal && ( S_OK == getUseRotationAngle( UseRotation ) );
	
	HRESULT MatroxCode(S_OK);

	if( UseRotation )
	{
		clearRunErrorMessages();

		if (!bRetVal)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			m_RunErrorMessages.push_back(Msg);
		}

		SVRunStatusClass ChildRunStatus;
		ChildRunStatus.m_lResultDataIndex  = rRunStatus.m_lResultDataIndex;
		ChildRunStatus.Images = rRunStatus.Images;
		ChildRunStatus.m_UpdateCounters = rRunStatus.m_UpdateCounters;
		
		// Run yourself...
		bRetVal &= onRun( rRunStatus, &m_RunErrorMessages );
		
		SVImageClass *l_psvInputImage = getInputImage();

		if( nullptr == l_psvInputImage )
		{
			// Signal something is wrong...
			bRetVal = false;
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			m_RunErrorMessages.push_back(Msg);
		}
		
		if( bRetVal )
		{
			if ( outputImageObject.SetImageHandleIndex( rRunStatus.Images ) )
			{
				SVImageBufferHandleImage l_InMilHandle;
				SVImageBufferHandleImage l_OutMilHandle;

				outputImageObject.GetImageHandle( output );
				l_psvInputImage->GetImageHandle( input );
				
				if(nullptr != input && nullptr != output )
				{
					input->GetData( l_InMilHandle );
					output->GetData( l_OutMilHandle );
				}

				POINT l_oInPoint;
				POINT l_oOutPoint;
				double dRotationAngle;

				l_svImageExtent.GetExtentProperty(SVExtentPropertyRotationAngle,dRotationAngle);
				l_svImageExtent.GetExtentProperty(SVExtentPropertyPositionPoint,l_oInPoint);
				l_svImageExtent.GetExtentProperty(SVExtentPropertyOutputPositionPoint,l_oOutPoint);

				SVMatroxImageRotateStruct l_Rotate( l_InMilHandle.GetBuffer() );
				l_Rotate.m_dAngle = dRotationAngle;
				l_Rotate.m_dSrcCenX = l_oInPoint.x;
				l_Rotate.m_dSrcCenY = l_oInPoint.y;
				l_Rotate.m_dDstCenX = l_oOutPoint.x;
				l_Rotate.m_dDstCenY = l_oOutPoint.y;
				l_Rotate.m_eInterpolation = SVNearestNeighOverScanClear;
				MatroxCode = SVMatroxImageInterface::Rotate( l_OutMilHandle.GetBuffer(), l_Rotate );


				// Run children...
				for( int i = 0; i < GetSize(); i++ )
				{
					ChildRunStatus.ResetRunStateAndToolSetTimes();
					
					SVUnaryImageOperatorClass*  pOperator = dynamic_cast<SVUnaryImageOperatorClass *>(GetAt( i ));
					if( pOperator )
					{
						pOperator->Run( false, output, output, ChildRunStatus, pErrorMessages );
					}
					else
					{
						bRetVal = false;
						SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
						m_RunErrorMessages.push_back(Msg);
					}
					
					// Update our Run Status
					if( ChildRunStatus.IsDisabled() )
						rRunStatus.SetDisabled();
					
					if( ChildRunStatus.IsDisabledByCondition() )
						rRunStatus.SetDisabledByCondition();
					
					if( ChildRunStatus.IsWarned() )
						rRunStatus.SetWarned();
					
					if( ChildRunStatus.IsFailed() )
						rRunStatus.SetFailed();
					
					if( ChildRunStatus.IsPassed() )
						rRunStatus.SetPassed();

					if( ChildRunStatus.IsCriticalFailure() )
						rRunStatus.SetCriticalFailure();
				}
				
				// 'no operator was running' check...
				// RO_22Mar2000
				
			}
			else
			{
				bRetVal = false;
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_SetImageHandleIndexFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
				m_RunErrorMessages.push_back(Msg);
			}
		}
		if (nullptr != pErrorMessages && !m_RunErrorMessages.empty())
		{
			pErrorMessages->insert(pErrorMessages->end(), m_RunErrorMessages.begin(), m_RunErrorMessages.end());
		}
	}
	else
	{
		if (bRetVal)
		{	//clearRunErrorMessage will be done by SvStdImageOperatorListClass and filling of m_RunErrorMessage also.
			bRetVal = SVStdImageOperatorListClass::Run( rRunStatus, pErrorMessages );
		}
		else
		{
			clearRunErrorMessages();
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			m_RunErrorMessages.push_back(Msg);
			if (nullptr != pErrorMessages)
			{
				pErrorMessages->push_back(Msg);
			}
		}
	}

	if( bRetVal )
	{
		SVImageBufferHandleImage l_OutMilHandle;

		outputImageObject.GetImageHandle( output );

		if(nullptr != output)
		{
			output->GetData( l_OutMilHandle );
		}

		SVDataBufferInfoClass& rDataBufferInfo = m_svProfileResultData.GetDataBufferInfo();

		long ProjectAngle( 0L );

		if ( 0 == m_aulLineData.size()|| S_OK != getInputProfileOrientation( ProjectAngle ) )
		{
			bRetVal = false;
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			m_RunErrorMessages.push_back(Msg);
			if (nullptr != pErrorMessages)
			{
				pErrorMessages->push_back(Msg);
			}
		}

		double dMin( 9999999.0 );
		double dMax( 0.0 );

		if( bRetVal )
		{
			double l_dProjectHeight = 0.0;

			MatroxCode = SVMatroxImageInterface::Project( rDataBufferInfo.HBuffer.milResult, l_OutMilHandle.GetBuffer(), static_cast<double> (ProjectAngle) );

			MatroxCode = SVMatroxImageInterface::GetResult( rDataBufferInfo.HBuffer.milResult, m_aulLineData );

			if( 0 == ProjectAngle )
			{
				l_svImageExtent.GetExtentProperty(SVExtentPropertyHeight,l_dProjectHeight);
			}
			else
			{
				l_svImageExtent.GetExtentProperty(SVExtentPropertyWidth,l_dProjectHeight);
			}

			if( l_dProjectHeight == 0.0 || l_dProjectHeight == 1.0 )
			{
				for( unsigned long i = 0; i < m_ulLineLength; i++ )
				{
					double l_dValue = m_aulLineData[ i ];

					dMin = std::min( l_dValue, dMin );
					dMax = std::max( l_dValue, dMax );

					m_svArray.at( i ) = l_dValue;
				}
			}
			else
			{
				for( unsigned long i = 0; i < m_ulLineLength; i++ )
				{
					double l_dValue = m_aulLineData[ i ] / l_dProjectHeight;

					dMin = std::min( l_dValue, dMin );
					dMax = std::max( l_dValue, dMax );

					m_svArray.at( i ) = l_dValue;
				}
			}

			bRetVal = ( S_OK == m_svMinThreshold.SetValue(dMin) ) && bRetVal;
			bRetVal = ( S_OK == m_svMaxThreshold.SetValue(dMax) ) && bRetVal;

			bRetVal = ( S_OK == m_svLinearData.SetArrayValues(m_svArray) ) && bRetVal;
			ASSERT( bRetVal );
			if (!bRetVal)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_SetValueFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
				m_RunErrorMessages.push_back(Msg);
				if (nullptr != pErrorMessages)
				{
					pErrorMessages->push_back(Msg);
				}
			}
		}
	}

	if( ! bRetVal )
	{
		// Something was wrong...
		
		SetInvalid();
		rRunStatus.SetInvalid();
	}
	
	// Get Status Color...
	DWORD dwValue = rRunStatus.GetStatusColor();
	m_statusColor.SetValue(dwValue);
	
	// Get Status...
	dwValue = rRunStatus.GetState();
	m_statusTag.SetValue(dwValue);
	
	return bRetVal;
}

HRESULT SVLinearImageOperatorListClass::getUseRotationAngle(BOOL& rUseRotationAngle)
{
	HRESULT Result(E_FAIL);

	if( inputUseRotationAngle.IsConnected() && nullptr != inputUseRotationAngle.GetInputObjectInfo().getObject() )
	{
		double Value(0.0);
		Result = inputUseRotationAngle.GetInputObjectInfo().getObject()->getValue(Value);
		rUseRotationAngle = 0.0 < Value ? true : false;
	}

	return Result;
}

HRESULT SVLinearImageOperatorListClass::getInputProfileOrientation(long& rProfileOrientation)
{
	HRESULT Result(E_FAIL);

	if( inputProfileOrientation.IsConnected() && nullptr != inputProfileOrientation.GetInputObjectInfo().getObject() )
	{
		double Value(0.0);
		Result = inputProfileOrientation.GetInputObjectInfo().getObject()->getValue(Value);
		rProfileOrientation = static_cast<long> (Value);
	}

	return Result;
}

void SVLinearImageOperatorListClass::init()
{
	// Identify our output type
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SvDef::SVUnaryImageOperatorListObjectType;
	m_outObjectInfo.m_ObjectTypeInfo.SubType	= SvDef::SVLinearImageOperatorListObjectType;

	inputProfileOrientation.SetInputObjectType(SvDef::SVValueObjectType, SvDef::SVEnumValueObjectType, SVProfileOrientationGuid);
	inputProfileOrientation.SetObject( GetObjectInfo() );
	RegisterInputObject( &inputProfileOrientation, _T( "LinearImageOperatorListProfileOrientation" ) );

	inputUseRotationAngle.SetInputObjectType(SvDef::SVValueObjectType, SvDef::SVBoolValueObjectType, SVLinearToolUseRotationGuid);
	inputUseRotationAngle.SetObject( GetObjectInfo() );
	RegisterInputObject( &inputUseRotationAngle, _T( "LinearImageOperatorListUseRotationAngle" ) );

	m_svLinearData.SetLegacyVectorObjectCompatibility();

	RegisterEmbeddedObject(&m_svLinearData, SVLinearDataClassGuid,IDS_CLASSNAME_SVLINEARDATACLASS, false, SvOi::SVResetItemNone );
	m_svLinearData.setSaveValueFlag(false);
	RegisterEmbeddedObject(&m_svMinThreshold, SVLinearThresholdMinObjectGuid, IDS_OBJECTNAME_LINEAR_THRESHOLD_MINVALUE, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject(&m_svMaxThreshold, SVLinearThresholdMaxObjectGuid, IDS_OBJECTNAME_LINEAR_THRESHOLD_MAXVALUE, false, SvOi::SVResetItemNone );

	m_svMinThreshold.SetDefaultValue(0.0);
	m_svMinThreshold.setSaveValueFlag(false);
	m_svMaxThreshold.SetDefaultValue( 0.0);
	m_svMaxThreshold.setSaveValueFlag(false);

	m_ulLineLength = 0;

	outputImageObject.InitializeImage( SvDef::SVImageTypeEnum::SVImageTypePhysical );

	// Set default inputs and outputs
	addDefaultInputObjects();
}

HRESULT SVLinearImageOperatorListClass::UpdateLineExtentData()
{
	HRESULT l_hrOk = S_FALSE;

	SVImageExtentClass l_svExtents;

	long projAngle( 0 );

	RECT l_oRect;

	// This is the new Absolute Extent of the Image
	SVToolClass* pTool = dynamic_cast<SVToolClass*>(GetTool());
	if ( nullptr != pTool &&
			 S_OK == pTool->GetImageExtent( l_svExtents ) &&
			 S_OK == l_svExtents.GetOutputRectangle( l_oRect ) &&
			 ( S_OK == getInputProfileOrientation( projAngle ) ) )
	{
		if( 0 == projAngle )
		{
			m_ulLineLength = l_oRect.right;
		}
		else if( 90 == projAngle )
		{
			m_ulLineLength = l_oRect.bottom;
		}

		if (0 < m_ulLineLength)
		{
			m_svArray.resize( m_ulLineLength );

			m_aulLineData.resize( m_ulLineLength );

			if( m_svProfileResultData.Resize( m_ulLineLength ) )
			{
				l_hrOk = S_OK;
			}

			m_svLinearData.SetArraySize( m_ulLineLength );
		}
	}

	return l_hrOk;
}

