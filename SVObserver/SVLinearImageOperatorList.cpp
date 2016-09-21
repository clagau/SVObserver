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
#include "SVObjectLibrary/SVAnalyzerLevelCreateStruct.h"
#include "SVLinearToolClass.h"
#include "SVUnaryImageOperatorClass.h"
#include "SVTool.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SV_IMPLEMENT_CLASS( SVLinearImageOperatorListClass, SVLinearImageOperatorListClassGuid )


SVLinearImageOperatorListClass::SVLinearImageOperatorListClass( BOOL BCreateDefaultTaskList, SVObjectClass* POwner, int StringResourceID )
							    :SVStdImageOperatorListClass( BCreateDefaultTaskList, POwner, StringResourceID ) 
{
	init();
}

SVLinearImageOperatorListClass::~SVLinearImageOperatorListClass() 
{ 
	CloseObject();

}

BOOL SVLinearImageOperatorListClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL l_bOk = SVStdImageOperatorListClass::CreateObject( PCreateStructure );

	SVDataBufferInfoClass& rBufferInfo = m_svProfileResultData.GetDataBufferInfo();
	rBufferInfo.Type = SVDataBufferInfoClass::SVProjectResult;

	BOOL m_bUseRotation = TRUE;

	if( nullptr != getUseRotationAngle() &&
		  ( S_OK == getUseRotationAngle()->GetValue( m_bUseRotation ) ) &&
			! m_bUseRotation )
	{
		outputImageObject.InitializeImage( SVImageTypeLogicalAndPhysical );
	}
	else
	{
		outputImageObject.InitializeImage( SVImageTypePhysical );
	}

	l_bOk &= S_OK == UpdateLineExtentData();
	m_svLinearData.ObjectAttributesAllowedRef() &= ~( SV_VIEWABLE | SV_PRINTABLE );

	m_svMaxThreshold.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	m_svMinThreshold.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;


	return l_bOk;
}

BOOL SVLinearImageOperatorListClass::CloseObject()
{
	BOOL l_bOk = m_svProfileResultData.CloseObject();

	l_bOk &= SVStdImageOperatorListClass::CloseObject();

	return l_bOk;
}

HRESULT SVLinearImageOperatorListClass::ResetObject()
{
	BOOL m_bUseRotation = TRUE;

	if( nullptr != getUseRotationAngle() &&
		  ( S_OK == getUseRotationAngle()->GetValue( m_bUseRotation ) ) &&
			! m_bUseRotation )
	{
		outputImageObject.InitializeImage( SVImageTypeLogicalAndPhysical );
	}
	else
	{
		outputImageObject.InitializeImage( SVImageTypePhysical );
	}

	HRESULT l_hrOk = SVStdImageOperatorListClass::ResetObject();

	CollectInputImageNames();

	if( S_OK != UpdateLineExtentData() )
	{
		l_hrOk = S_FALSE;
	}

	return l_hrOk;
}

BOOL SVLinearImageOperatorListClass::OnValidate()
{
	if( SVStdImageOperatorListClass::OnValidate() )
	{
		if( nullptr != getInputProfileOrientation() && 
			  m_aulLineData.size() > 0 )
		{
			return TRUE;
		}
	}
	SetInvalid();
	return FALSE;
}

BOOL SVLinearImageOperatorListClass::Run( SVRunStatusClass& RRunStatus )
{
	SVSmartHandlePointer output;
	SVSmartHandlePointer input;
	SVImageExtentClass l_svImageExtent;

	BOOL m_bUseRotation = TRUE;

	BOOL bRetVal = S_OK == GetTool()->GetImageExtent( l_svImageExtent );

	bRetVal = bRetVal && nullptr != getUseRotationAngle() && ( S_OK == getUseRotationAngle()->GetValue( m_bUseRotation ) );
	
	SVMatroxImageInterface::SVStatusCode l_Code = SVMEE_STATUS_OK;

	if( m_bUseRotation )
	{
		SVRunStatusClass ChildRunStatus;
		ChildRunStatus.m_lResultDataIndex  = RRunStatus.m_lResultDataIndex;
		ChildRunStatus.Images = RRunStatus.Images;
		ChildRunStatus.m_UpdateCounters = RRunStatus.m_UpdateCounters;
		
		// Run yourself...
		bRetVal &= onRun( RRunStatus );
		
		SVImageClass *l_psvInputImage = getInputImage();

		if( nullptr == l_psvInputImage )
		{
			// Signal something is wrong...
			bRetVal = false;
		}
		
		if( bRetVal )
		{
			bRetVal = outputImageObject.SetImageHandleIndex( RRunStatus.Images );
			if ( bRetVal )
			{
				SVImageBufferHandleImage l_InMilHandle;
				SVImageBufferHandleImage l_OutMilHandle;

				outputImageObject.GetImageHandle( output );
				l_psvInputImage->GetImageHandle( input );
				
				if( !( input.empty() ) && !( output.empty() ) )
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
				l_Code = SVMatroxImageInterface::Rotate( l_OutMilHandle.GetBuffer(), l_Rotate );


				// Run children...
				for( int i = 0; i < GetSize(); i++ )
				{
					ChildRunStatus.ResetRunStateAndToolSetTimes();
					
					SVUnaryImageOperatorClass*  pOperator = dynamic_cast<SVUnaryImageOperatorClass *>(GetAt( i ));
					if( pOperator )
					{
						pOperator->Run( FALSE, output, output, ChildRunStatus );
					}
					else
						bRetVal = FALSE;
					
					// Update our Run Status
					if( ChildRunStatus.IsDisabled() )
						RRunStatus.SetDisabled();
					
					if( ChildRunStatus.IsDisabledByCondition() )
						RRunStatus.SetDisabledByCondition();
					
					if( ChildRunStatus.IsWarned() )
						RRunStatus.SetWarned();
					
					if( ChildRunStatus.IsFailed() )
						RRunStatus.SetFailed();
					
					if( ChildRunStatus.IsPassed() )
						RRunStatus.SetPassed();

					if( ChildRunStatus.IsCriticalFailure() )
						RRunStatus.SetCriticalFailure();
				}
				
				// 'no operator was running' check...
				// RO_22Mar2000
				
			}
		}
	}
	else
	{
		bRetVal &= SVStdImageOperatorListClass::Run( RRunStatus );
	}

	if( bRetVal )
	{
		SVImageBufferHandleImage l_OutMilHandle;

		outputImageObject.GetImageHandle( output );

		if( !( output.empty() ) )
		{
			output->GetData( l_OutMilHandle );
		}

		SVDataBufferInfoClass& rDataBufferInfo = m_svProfileResultData.GetDataBufferInfo();

		double l_dProjectAngle = 0.0;

		SVEnumerateValueObjectClass *l_svProjectAngle = getInputProfileOrientation();

		bRetVal = m_aulLineData.size() != 0 &&
		          nullptr != l_svProjectAngle && 
		          ( S_OK == l_svProjectAngle->GetValue( l_dProjectAngle ) );

		double l_dMin = 9999999.0;
		double l_dMax = 0.0;

		if( bRetVal )
		{
			double l_dProjectHeight = 0.0;

			l_Code = SVMatroxImageInterface::Project( rDataBufferInfo.HBuffer.milResult, l_OutMilHandle.GetBuffer(), l_dProjectAngle );

			l_Code = SVMatroxImageInterface::GetResult( rDataBufferInfo.HBuffer.milResult, m_aulLineData );

			if( l_dProjectAngle == 0.0 )
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

					l_dMin = std::min( l_dValue, l_dMin );
					l_dMax = std::max( l_dValue, l_dMax );

					m_svArray.at( i ) = l_dValue;
				}
			}
			else
			{
				for( unsigned long i = 0; i < m_ulLineLength; i++ )
				{
					double l_dValue = m_aulLineData[ i ] / l_dProjectHeight;

					l_dMin = std::min( l_dValue, l_dMin );
					l_dMax = std::max( l_dValue, l_dMax );

					m_svArray.at( i ) = l_dValue;
				}
			}

			bRetVal = ( S_OK == m_svMinThreshold.SetValue( RRunStatus.m_lResultDataIndex, l_dMin ) ) && bRetVal;
			bRetVal = ( S_OK == m_svMaxThreshold.SetValue( RRunStatus.m_lResultDataIndex, l_dMax ) ) && bRetVal;

			bRetVal = ( S_OK == m_svLinearData.SetArrayValues( RRunStatus.m_lResultDataIndex, m_svArray ) ) && bRetVal;
			ASSERT( bRetVal );
		}
	}

	if( ! bRetVal )
	{
		// Something was wrong...
		
		SetInvalid();
		RRunStatus.SetInvalid();
	}
	
	// Get Status Color...
	DWORD dwValue = RRunStatus.GetStatusColor();
	m_statusColor.SetValue( RRunStatus.m_lResultDataIndex, dwValue );
	
	// Get Status...
	dwValue = RRunStatus.GetState();
	m_statusTag.SetValue( RRunStatus.m_lResultDataIndex, dwValue );
	
	return bRetVal;
}

SVBoolValueObjectClass* SVLinearImageOperatorListClass::getUseRotationAngle()
{
	if( inputUseRotationAngle.IsConnected() && 
		  inputUseRotationAngle.GetInputObjectInfo().PObject )
		return dynamic_cast<SVBoolValueObjectClass *>(inputUseRotationAngle.GetInputObjectInfo().PObject);

	return nullptr;
}

SVEnumerateValueObjectClass* SVLinearImageOperatorListClass::getInputProfileOrientation()
{
	if( inputProfileOrientation.IsConnected() && 
		  inputProfileOrientation.GetInputObjectInfo().PObject )
		return dynamic_cast<SVEnumerateValueObjectClass *>(inputProfileOrientation.GetInputObjectInfo().PObject);

	return nullptr;
}

void SVLinearImageOperatorListClass::init()
{
	// Identify our output type
	m_outObjectInfo.ObjectTypeInfo.ObjectType = SVUnaryImageOperatorListObjectType;
	m_outObjectInfo.ObjectTypeInfo.SubType	= SVLinearImageOperatorListObjectType;

	inputProfileOrientation.SetInputObjectType( SVProfileOrientationGuid, SVEnumValueObjectType );
	inputProfileOrientation.SetObject( GetObjectInfo() );
	RegisterInputObject( &inputProfileOrientation, _T( "LinearImageOperatorListProfileOrientation" ) );

	inputUseRotationAngle.SetInputObjectType( SVLinearToolUseRotationGuid, SVBoolValueObjectType );
	inputUseRotationAngle.SetObject( GetObjectInfo() );
	RegisterInputObject( &inputUseRotationAngle, _T( "LinearImageOperatorListUseRotationAngle" ) );

	m_svLinearData.SetLegacyVectorObjectCompatibility();

	RegisterEmbeddedObject(&m_svLinearData, SVLinearDataClassGuid,IDS_CLASSNAME_SVLINEARDATACLASS, false, SVResetItemNone );

	RegisterEmbeddedObject(&m_svMinThreshold, SVLinearThresholdMinObjectGuid, IDS_OBJECTNAME_LINEAR_THRESHOLD_MINVALUE, false, SVResetItemNone );
	RegisterEmbeddedObject(&m_svMaxThreshold, SVLinearThresholdMaxObjectGuid, IDS_OBJECTNAME_LINEAR_THRESHOLD_MAXVALUE, false, SVResetItemNone );

	m_svMinThreshold.SetDefaultValue( 0.0, TRUE );
	m_svMaxThreshold.SetDefaultValue( 0.0, TRUE );

	m_ulLineLength = 0;

	outputImageObject.InitializeImage( SVImageTypePhysical );

	// Set default inputs and outputs
	addDefaultInputObjects();

}

HRESULT SVLinearImageOperatorListClass::UpdateLineExtentData()
{
	HRESULT l_hrOk = S_FALSE;

	SVImageExtentClass l_svExtents;

	SVEnumerateValueObjectClass* pProjAngle = getInputProfileOrientation();

	double projAngle = 0.0;

	RECT l_oRect;

	// This is the new Absolute Extent of the Image
	if ( nullptr != GetTool() && nullptr != pProjAngle &&
			 S_OK == GetTool()->GetImageExtent( l_svExtents ) &&
			 S_OK == l_svExtents.GetOutputRectangle( l_oRect ) &&
			 ( S_OK == pProjAngle->GetValue( projAngle ) ) )
	{
		if( projAngle == 0.0 )
		{
			m_ulLineLength = l_oRect.right;
		}
		else if( projAngle == 90.0 )
		{
			m_ulLineLength = l_oRect.bottom;
		}

		m_svArray.resize( m_ulLineLength );

		m_aulLineData.resize( m_ulLineLength );

		if( m_svProfileResultData.Resize( m_ulLineLength ) )
		{
			l_hrOk = S_OK;
		}

		m_svLinearData.SetArraySize( m_ulLineLength );
	}

	return l_hrOk;
}

