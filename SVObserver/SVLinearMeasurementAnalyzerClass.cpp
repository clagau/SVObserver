// ******************************************************************************
// * COPYRIGHT (c) 2005 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVLinearMeasurementAnalyzerClass
// * .File Name       : $Workfile:   SVLinearMeasurementAnalyzerClass.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   23 Apr 2013 12:04:02  $
// ******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVLinearMeasurementAnalyzerClass.h"
#include "SVProfileEdgeMarkerAdjustmentPage.h"
#include "SVMessage/SVMessage.h"
#include "SVGlobal.h"
#include "SVOCore/SVImageClass.h"
#include "SVLinearEdgeAProcessingClass.h"
#include "SVLinearEdgeBProcessingClass.h"
#include "SVOCore/SVTool.h"
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

SV_IMPLEMENT_CLASS( SVLinearMeasurementAnalyzerClass, SVLinearMeasurementAnalyzerClassGuid );

SVLinearMeasurementAnalyzerClass::SVLinearMeasurementAnalyzerClass( SVObjectClass* POwner, int StringResourceID )
									 :SVLinearAnalyzerClass( POwner, StringResourceID ) 
{
	m_bEnableDirection = true;
	m_bEnableEdgeSelect = true;
	m_bEnablePolarisation = true;
	m_bEnablePosition = true;
	m_bEnableThreshold = true;

	init();
}

void SVLinearMeasurementAnalyzerClass::init()
{
	// Identify our type
	m_outObjectInfo.m_ObjectTypeInfo.SubType = SVLinearMeasurementAnalyzerObjectType;

	// Register Embedded Objects

	RegisterEmbeddedObject( &mdpEdgeA, SVDPEdgeAObjectGuid, IDS_OBJECTNAME_DPEDGE_A, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &mdpEdgeB, SVDPEdgeBObjectGuid, IDS_OBJECTNAME_DPEDGE_B, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &mdpCenter, SVDPCenterObjectGuid, IDS_OBJECTNAME_DPCENTER, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &mdWidth, SVDWidthObjectGuid, IDS_OBJECTNAME_DWIDTH, false, SvOi::SVResetItemNone );

	RegisterEmbeddedObject( &m_svLinearDistanceA, SVLinearDistanceEdgeAObjectGuid, IDS_OBJECTNAME_LINEAR_DISTANCE_EDGE_A, false, SvOi::SVResetItemNone );
	m_svLinearDistanceA.setSaveValueFlag(false);
	RegisterEmbeddedObject( &m_svLinearDistanceB, SVLinearDistanceEdgeBObjectGuid, IDS_OBJECTNAME_LINEAR_DISTANCE_EDGE_B, false, SvOi::SVResetItemNone );
	m_svLinearDistanceB.setSaveValueFlag(false);

	RegisterEmbeddedObject( &m_svShowAllEdgeAOverlays, SVShowAllEdgeAOverlaysGuid, IDS_OBJECTNAME_SHOW_ALL_EDGE_A_OVERLAYS, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_svShowAllEdgeBOverlays, SVShowAllEdgeBOverlaysGuid, IDS_OBJECTNAME_SHOW_ALL_EDGE_B_OVERLAYS, false, SvOi::SVResetItemNone );

	// Set Edge defaults
	SVLinearEdgeProcessingClass *l_pEdgeA = new SVLinearEdgeAProcessingClass(this);
	SVLinearEdgeProcessingClass *l_pEdgeB = new SVLinearEdgeBProcessingClass(this);

	if( nullptr != l_pEdgeA )
	{
		AddFriend(l_pEdgeA->GetUniqueObjectID());
	}

	if( nullptr != l_pEdgeB )
	{
		AddFriend(l_pEdgeB->GetUniqueObjectID());

		l_pEdgeB->m_svEdgeSelect.SetDefaultValue( SV_LAST_EDGE, true );
	}

	// Set Embedded defaults
	POINT defaultPoint;
	defaultPoint.x = 0;
	defaultPoint.y = 0;
	mdpEdgeA.SetDefaultValue( defaultPoint, true );
	mdpEdgeA.setSaveValueFlag(false);
	mdpEdgeB.SetDefaultValue( defaultPoint, true );
	mdpEdgeB.setSaveValueFlag(false);
	mdpCenter.SetDefaultValue( defaultPoint, true );
	mdpCenter.setSaveValueFlag(false);
	mdWidth.SetDefaultValue( 0.0, true );
	mdWidth.setSaveValueFlag(false);

	// Set default inputs and outputs
	addDefaultInputObjects();
	
	// Populate the available result list
	SVClassInfoStruct resultClassInfo;
	CString strTitle;
	SVObjectTypeInfoStruct interfaceInfo;

	// Declare Input Interface of Sub-pixel Edge A Results...
	interfaceInfo.EmbeddedID = SVDPEdgeAObjectGuid;
	resultClassInfo.m_DesiredInputInterface.Add( interfaceInfo );

	// Add the Sub-pixel EdgeA X Result...
	resultClassInfo.m_ObjectTypeInfo.ObjectType = SVResultObjectType;
	resultClassInfo.m_ObjectTypeInfo.SubType	= SVResultDPointXObjectType;
	resultClassInfo.m_ClassId = SVDPointXResultClassGuid;
	resultClassInfo.m_ClassName = SvUl_SF::LoadSVString( IDS_OBJECTNAME_DPEDGE_A );
	strTitle.LoadString( IDS_CLASSNAME_RESULT_POINT_X );
	resultClassInfo.m_ClassName += _T(" ") + strTitle;
	m_availableChildren.Add( resultClassInfo );
	
	// Add the Sub-pixel EdgeA Y Result...
	resultClassInfo.m_ObjectTypeInfo.ObjectType = SVResultObjectType;
	resultClassInfo.m_ObjectTypeInfo.SubType	= SVResultDPointYObjectType;
	resultClassInfo.m_ClassId = SVDPointYResultClassGuid;
	resultClassInfo.m_ClassName = SvUl_SF::LoadSVString( IDS_OBJECTNAME_DPEDGE_A );
	strTitle.LoadString( IDS_CLASSNAME_RESULT_POINT_Y );
	resultClassInfo.m_ClassName += _T(" ") + strTitle;
	m_availableChildren.Add( resultClassInfo );

	// Declare Input Interface of Sub-pixel Edge B Results...
	resultClassInfo.m_DesiredInputInterface.RemoveAll();
	interfaceInfo.EmbeddedID = SVDPEdgeBObjectGuid;
	resultClassInfo.m_DesiredInputInterface.Add( interfaceInfo );
	
	// Add the Sub-pixel EdgeB X Result...
	resultClassInfo.m_ObjectTypeInfo.ObjectType = SVResultObjectType;
	resultClassInfo.m_ObjectTypeInfo.SubType	= SVResultDPointXObjectType;
	resultClassInfo.m_ClassId = SVDPointXResultClassGuid;
	resultClassInfo.m_ClassName = SvUl_SF::LoadSVString( IDS_OBJECTNAME_DPEDGE_B );
	strTitle.LoadString( IDS_CLASSNAME_RESULT_POINT_X );
	resultClassInfo.m_ClassName += _T(" ") + strTitle;
	m_availableChildren.Add( resultClassInfo );
	
	// Add the Sub-pixel EdgeB Y Result...
	resultClassInfo.m_ObjectTypeInfo.ObjectType = SVResultObjectType;
	resultClassInfo.m_ObjectTypeInfo.SubType	= SVResultDPointYObjectType;
	resultClassInfo.m_ClassId = SVDPointYResultClassGuid;
	resultClassInfo.m_ClassName = SvUl_SF::LoadSVString( IDS_OBJECTNAME_DPEDGE_B );
	strTitle.LoadString( IDS_CLASSNAME_RESULT_POINT_Y );
	resultClassInfo.m_ClassName += _T(" ") + strTitle;
	m_availableChildren.Add( resultClassInfo );

	// Declare Input Interface of Sub-pixel Center Results...
	resultClassInfo.m_DesiredInputInterface.RemoveAll();
	interfaceInfo.EmbeddedID = SVDPCenterObjectGuid;
	resultClassInfo.m_DesiredInputInterface.Add( interfaceInfo );
	
	// Add the Sub-pixel Center X Result...
	resultClassInfo.m_ObjectTypeInfo.ObjectType = SVResultObjectType;
	resultClassInfo.m_ObjectTypeInfo.SubType	= SVResultDPointXObjectType;
	resultClassInfo.m_ClassId = SVDPointXResultClassGuid;
	resultClassInfo.m_ClassName = SvUl_SF::LoadSVString( IDS_OBJECTNAME_DPCENTER );
	strTitle.LoadString( IDS_CLASSNAME_RESULT_POINT_X );
	resultClassInfo.m_ClassName += _T(" ") + strTitle;
	m_availableChildren.Add( resultClassInfo );
	
	// Add the Sub-pixel Center Y Result...
	resultClassInfo.m_ObjectTypeInfo.ObjectType = SVResultObjectType;
	resultClassInfo.m_ObjectTypeInfo.SubType	= SVResultDPointYObjectType;
	resultClassInfo.m_ClassId = SVDPointYResultClassGuid;
	resultClassInfo.m_ClassName = SvUl_SF::LoadSVString( IDS_OBJECTNAME_DPCENTER );
	strTitle.LoadString( IDS_CLASSNAME_RESULT_POINT_Y );
	resultClassInfo.m_ClassName += _T(" ") + strTitle;
	m_availableChildren.Add( resultClassInfo );

	// Declare Input Interface of Sub-pixel Width Result...
	resultClassInfo.m_DesiredInputInterface.RemoveAll();
	interfaceInfo.EmbeddedID = SVDWidthObjectGuid;
	resultClassInfo.m_DesiredInputInterface.Add( interfaceInfo );
	
	// Add the Sub-pixel Width Result...
	resultClassInfo.m_ObjectTypeInfo.ObjectType = SVResultObjectType;
	resultClassInfo.m_ObjectTypeInfo.SubType	= SVResultDoubleObjectType;
	resultClassInfo.m_ClassId = SVDoubleResultClassGuid;
	resultClassInfo.m_ClassName = SvUl_SF::LoadSVString( IDS_OBJECTNAME_DWIDTH );
	strTitle.LoadString( IDS_RESULT_STRING );
	resultClassInfo.m_ClassName += _T(" ") + strTitle;
	m_availableChildren.Add( resultClassInfo );

	resultClassInfo.m_DesiredInputInterface.RemoveAll();
	interfaceInfo.EmbeddedID = SVLinearDistanceEdgeAObjectGuid;
	resultClassInfo.m_DesiredInputInterface.Add( interfaceInfo );

	// Add the Linear Distance Result...
	resultClassInfo.m_ObjectTypeInfo.ObjectType = SVResultObjectType;
	resultClassInfo.m_ObjectTypeInfo.SubType	= SVResultDoubleObjectType;
	resultClassInfo.m_ClassId = SVDoubleResultClassGuid;
	resultClassInfo.m_ClassName = SvUl_SF::LoadSVString( IDS_OBJECTNAME_LINEAR_DISTANCE_EDGE_A );
	strTitle.LoadString( IDS_OBJECTNAME_LINEAR_DISTANCE_EDGE_A_RESULT );
	resultClassInfo.m_ClassName += _T(" ") + strTitle;
	m_availableChildren.Add( resultClassInfo );

	resultClassInfo.m_DesiredInputInterface.RemoveAll();
	interfaceInfo.EmbeddedID = SVLinearDistanceEdgeBObjectGuid;
	resultClassInfo.m_DesiredInputInterface.Add( interfaceInfo );

	// Add the Linear Distance Result...
	resultClassInfo.m_ObjectTypeInfo.ObjectType = SVResultObjectType;
	resultClassInfo.m_ObjectTypeInfo.SubType	= SVResultDoubleObjectType;
	resultClassInfo.m_ClassId = SVDoubleResultClassGuid;
	resultClassInfo.m_ClassName = SvUl_SF::LoadSVString( IDS_OBJECTNAME_LINEAR_DISTANCE_EDGE_B );
	strTitle.LoadString( IDS_OBJECTNAME_LINEAR_DISTANCE_EDGE_B_RESULT );
	resultClassInfo.m_ClassName += _T(" ") + strTitle;
	m_availableChildren.Add( resultClassInfo );
}

SVLinearMeasurementAnalyzerClass::~SVLinearMeasurementAnalyzerClass()
{
	CloseObject();
}

bool SVLinearMeasurementAnalyzerClass::CreateObject( SVObjectLevelCreateStruct* pCreateStructure )
{
	bool bOk = SVLinearAnalyzerClass::CreateObject( pCreateStructure );

	// Set / Reset Printable Flag
	mdpEdgeA.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );
	mdpEdgeB.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );
	mdpCenter.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );
	mdWidth.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );

	m_svLinearDistanceA.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );
	m_svLinearDistanceB.SetObjectAttributesAllowed( SvOi::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );

	m_isCreated = bOk;

	return bOk;
}

bool SVLinearMeasurementAnalyzerClass::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	return __super::ResetObject(pErrorMessages) && ValidateEdgeA(pErrorMessages) && ValidateEdgeB(pErrorMessages);
}

bool SVLinearMeasurementAnalyzerClass::onRun( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	bool Result = __super::onRun( rRunStatus, pErrorMessages ) && ValidateEdgeA(pErrorMessages) && ValidateEdgeB(pErrorMessages);

	SVDPointClass DPointA, DPointB;
	SVExtentPointStruct EdgePointA;
	SVExtentPointStruct EdgePointB;
	SVDPointClass CenterPoint;
	double DistanceA( 0.0 );
	double DistanceB( 0.0 );

	if ( nullptr == GetTool())
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
	}

	if ( Result )
	{
		SVImageExtentClass l_svExtents;

		if ( S_OK != GetEdgeA()->GetOutputEdgePoint(EdgePointA) )
		{
			rRunStatus.SetFailed();
		}

		if ( S_OK != GetEdgeA()->GetOutputEdgeDistance(DistanceA) )
		{
			rRunStatus.SetFailed();
		}

		if ( S_OK != GetEdgeB()->GetOutputEdgePoint(EdgePointB) )
		{
			rRunStatus.SetFailed();
		}

		if ( S_OK != GetEdgeB()->GetOutputEdgeDistance(DistanceB) )
		{
			rRunStatus.SetFailed();
		}

		Result &= S_OK == GetImageExtent( l_svExtents ) &&
				S_OK == l_svExtents.TranslateFromOutputSpace(EdgePointA, EdgePointA) &&
				S_OK == l_svExtents.TranslateFromOutputSpace(EdgePointB, EdgePointB);

		SVToolClass* pTool = dynamic_cast<SVToolClass*>(GetTool());
		Result &= pTool && S_OK == pTool->GetImageExtent( l_svExtents ) &&
				S_OK == l_svExtents.TranslateFromOutputSpace(EdgePointA, EdgePointA) &&
				S_OK == l_svExtents.TranslateFromOutputSpace(EdgePointB, EdgePointB);

		DPointA.x = EdgePointA.m_dPositionX;
		DPointA.y = EdgePointA.m_dPositionY;

		DPointB.x = EdgePointB.m_dPositionX;
		DPointB.y = EdgePointB.m_dPositionY;


		CenterPoint.x = DPointA.x + ((DPointB.x - DPointA.x) / 2.0);
		CenterPoint.y = DPointA.y + ((DPointB.y - DPointA.y) / 2.0);

		double Width = fabs( DistanceB - DistanceA + 1.0 );

		Result = ( S_OK == m_svLinearDistanceA.SetValue(DistanceA) ) && Result;
		Result = ( S_OK == m_svLinearDistanceB.SetValue(DistanceB) ) && Result;
		
		Result = ( S_OK == mdpEdgeA.SetValue(DPointA) ) && Result;
		Result = ( S_OK == mdpEdgeB.SetValue(DPointB) ) && Result;
		Result = ( S_OK == mdpCenter.SetValue(CenterPoint) ) && Result;
		Result = ( S_OK == mdWidth.SetValue(Width) ) && Result;
	}

	if ( !Result )
	{
		SetInvalid();
		rRunStatus.SetInvalid();

		if (nullptr != pErrorMessages && pErrorMessages->empty())
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_RunLinearEdgeFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
	}

	return Result;
}
