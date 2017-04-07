// ******************************************************************************
// * COPYRIGHT (c) 2005 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVLinearMaximumForegroundObjectLineAnalyzerClass
// * .File Name       : $Workfile:   SVLinearLineMaximumForegroundObjectAnalyzer.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   01 Oct 2013 14:57:20  $
// ******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVLinearLineMaximumForegroundObjectAnalyzer.h"
#include "SVProfileEdgeMarkerAdjustmentPage.h"
#include "SVGlobal.h"
#include "SVLinearEdgeAProcessingClass.h"
#include "SVLinearEdgeBProcessingClass.h"
#include "SVTool.h"
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

SV_IMPLEMENT_CLASS( SVLinearMaximumForegroundObjectLineAnalyzerClass, SVLinearMaximumForegroundObjectLineAnalyzerClassGuid );

SVLinearMaximumForegroundObjectLineAnalyzerClass::SVLinearMaximumForegroundObjectLineAnalyzerClass( BOOL BCreateDefaultTaskList, SVObjectClass* POwner, int StringResourceID )
										   :SVLinearAnalyzerClass( BCreateDefaultTaskList, POwner, StringResourceID ) 
{
	m_bEnableDirection = false;
	m_bEnableEdgeSelect = false;
	m_bEnablePolarisation = false;
	m_bEnablePosition = false;
	m_bEnableThreshold = true;

	init();
}

void SVLinearMaximumForegroundObjectLineAnalyzerClass::init()
{
	// Identify our type
	m_outObjectInfo.m_ObjectTypeInfo.SubType = SVLinearMaximumForegroundObjectAnalyzerObjectType;

	// Register Embedded Objects

	RegisterEmbeddedObject( &mdpEdgeA, SVDPEdgeAObjectGuid, IDS_OBJECTNAME_DPEDGE_A, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &mdpEdgeB, SVDPEdgeBObjectGuid, IDS_OBJECTNAME_DPEDGE_B, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &mdpCenter, SVDPCenterObjectGuid, IDS_OBJECTNAME_DPCENTER, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &mdWidth, SVDWidthObjectGuid, IDS_OBJECTNAME_DWIDTH, false, SvOi::SVResetItemNone );

	RegisterEmbeddedObject( &m_svLinearDistanceA, SVLinearDistanceEdgeAObjectGuid, IDS_OBJECTNAME_LINEAR_DISTANCE_EDGE_A, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_svLinearDistanceB, SVLinearDistanceEdgeBObjectGuid, IDS_OBJECTNAME_LINEAR_DISTANCE_EDGE_B, false, SvOi::SVResetItemNone );

	RegisterEmbeddedObject( &m_svShowAllEdgeAOverlays, SVShowAllEdgeAOverlaysGuid, IDS_OBJECTNAME_SHOW_ALL_EDGE_A_OVERLAYS, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_svShowAllEdgeBOverlays, SVShowAllEdgeBOverlaysGuid, IDS_OBJECTNAME_SHOW_ALL_EDGE_B_OVERLAYS, false, SvOi::SVResetItemNone );

    //
	// Set Edge defaults

	SVLinearEdgeProcessingClass *l_pEdgeA = new SVLinearEdgeAProcessingClass(this);
	SVLinearEdgeProcessingClass *l_pEdgeB = new SVLinearEdgeBProcessingClass(this);

	if( nullptr != l_pEdgeA )
	{
		AddFriend(l_pEdgeA->GetUniqueObjectID());

		l_pEdgeA->m_svPolarisation.SetDefaultValue( SV_POSITIVE_POLARISATION, true );

		l_pEdgeA->m_svEdgeSelect.SetDefaultValue( SV_THIS_EDGE, true );
		l_pEdgeA->m_svEdgeSelectThisValue.SetDefaultValue( 1.0, true );
	}

	if( nullptr != l_pEdgeB )
	{
		AddFriend(l_pEdgeB->GetUniqueObjectID());

		l_pEdgeB->m_svPolarisation.SetDefaultValue( SV_NEGATIVE_POLARISATION, true );

		l_pEdgeB->m_svEdgeSelect.SetDefaultValue( SV_THIS_EDGE, true );
		l_pEdgeB->m_svEdgeSelectThisValue.SetDefaultValue( 1.0, true );
	}

	// Set Embedded defaults
	POINT defaultPoint;
	defaultPoint.x = 0;
	defaultPoint.y = 0;
	mdpEdgeA.SetDefaultValue( defaultPoint, true );
	mdpEdgeB.SetDefaultValue( defaultPoint, true );
	mdpCenter.SetDefaultValue( defaultPoint, true );
	mdWidth.SetDefaultValue( 0.0, true );

	// Set default inputs and outputs
	addDefaultInputObjects();
	
	// Populate the available result list
	SVClassInfoStruct resultClassInfo;
	SVObjectTypeInfoStruct interfaceInfo;

	// Declare Input Interface of Sub-pixel Edge A Results...
	interfaceInfo.EmbeddedID = SVDPEdgeAObjectGuid;
	resultClassInfo.m_DesiredInputInterface.Add( interfaceInfo );

	// Add the Sub-pixel EdgeA X Result...
	resultClassInfo.m_ObjectTypeInfo.ObjectType = SVResultObjectType;
	resultClassInfo.m_ObjectTypeInfo.SubType	= SVResultDPointXObjectType;
	resultClassInfo.m_ClassId = SVDPointXResultClassGuid;
	resultClassInfo.m_ClassName = SvUl_SF::LoadSVString( IDS_OBJECTNAME_DPEDGE_A );
	SVString Title = SvUl_SF::LoadSVString( IDS_CLASSNAME_RESULT_POINT_X );
	resultClassInfo.m_ClassName += _T(" ") + Title;
	m_availableChildren.Add( resultClassInfo );

	// Add the Sub-pixel EdgeA Y Result...
	resultClassInfo.m_ObjectTypeInfo.ObjectType = SVResultObjectType;
	resultClassInfo.m_ObjectTypeInfo.SubType	= SVResultDPointYObjectType;
	resultClassInfo.m_ClassId = SVDPointYResultClassGuid;
	resultClassInfo.m_ClassName = SvUl_SF::LoadSVString( IDS_OBJECTNAME_DPEDGE_A );
	Title = SvUl_SF::LoadSVString( IDS_CLASSNAME_RESULT_POINT_Y );
	resultClassInfo.m_ClassName += _T(" ") + Title;
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
	Title = SvUl_SF::LoadSVString( IDS_CLASSNAME_RESULT_POINT_X );
	resultClassInfo.m_ClassName += _T(" ") + Title;
	m_availableChildren.Add( resultClassInfo );

	// Add the Sub-pixel EdgeB Y Result...
	resultClassInfo.m_ObjectTypeInfo.ObjectType = SVResultObjectType;
	resultClassInfo.m_ObjectTypeInfo.SubType	= SVResultDPointYObjectType;
	resultClassInfo.m_ClassId = SVDPointYResultClassGuid;
	resultClassInfo.m_ClassName = SvUl_SF::LoadSVString( IDS_OBJECTNAME_DPEDGE_B );
	Title = SvUl_SF::LoadSVString( IDS_CLASSNAME_RESULT_POINT_Y );
	resultClassInfo.m_ClassName += _T(" ") + Title;
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
	Title = SvUl_SF::LoadSVString( IDS_CLASSNAME_RESULT_POINT_X );
	resultClassInfo.m_ClassName += _T(" ") + Title;
	m_availableChildren.Add( resultClassInfo );

	// Add the Sub-pixel Center Y Result...
	resultClassInfo.m_ObjectTypeInfo.ObjectType = SVResultObjectType;
	resultClassInfo.m_ObjectTypeInfo.SubType	= SVResultDPointYObjectType;
	resultClassInfo.m_ClassId = SVDPointYResultClassGuid;
	resultClassInfo.m_ClassName = SvUl_SF::LoadSVString( IDS_OBJECTNAME_DPCENTER );
	Title = SvUl_SF::LoadSVString( IDS_CLASSNAME_RESULT_POINT_Y );
	resultClassInfo.m_ClassName += _T(" ") + Title;
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
	Title = SvUl_SF::LoadSVString( IDS_RESULT_STRING );
	resultClassInfo.m_ClassName += _T(" ") + Title;
	m_availableChildren.Add( resultClassInfo );

	resultClassInfo.m_DesiredInputInterface.RemoveAll();
	interfaceInfo.EmbeddedID = SVLinearDistanceEdgeAObjectGuid;
	resultClassInfo.m_DesiredInputInterface.Add( interfaceInfo );

	// Add the Linear Distance Result...
	resultClassInfo.m_ObjectTypeInfo.ObjectType = SVResultObjectType;
	resultClassInfo.m_ObjectTypeInfo.SubType	= SVResultDoubleObjectType;
	resultClassInfo.m_ClassId = SVDoubleResultClassGuid;
	resultClassInfo.m_ClassName = SvUl_SF::LoadSVString( IDS_OBJECTNAME_LINEAR_DISTANCE_EDGE_A );
	Title = SvUl_SF::LoadSVString( IDS_OBJECTNAME_LINEAR_DISTANCE_EDGE_A_RESULT );
	resultClassInfo.m_ClassName += _T(" ") + Title;
	m_availableChildren.Add( resultClassInfo );

	resultClassInfo.m_DesiredInputInterface.RemoveAll();
	interfaceInfo.EmbeddedID = SVLinearDistanceEdgeBObjectGuid;
	resultClassInfo.m_DesiredInputInterface.Add( interfaceInfo );

	// Add the Linear Distance Result...
	resultClassInfo.m_ObjectTypeInfo.ObjectType = SVResultObjectType;
	resultClassInfo.m_ObjectTypeInfo.SubType	= SVResultDoubleObjectType;
	resultClassInfo.m_ClassId = SVDoubleResultClassGuid;
	resultClassInfo.m_ClassName = SvUl_SF::LoadSVString( IDS_OBJECTNAME_LINEAR_DISTANCE_EDGE_B );
	Title = SvUl_SF::LoadSVString( IDS_OBJECTNAME_LINEAR_DISTANCE_EDGE_B_RESULT );
	resultClassInfo.m_ClassName += _T(" ") + Title;
	m_availableChildren.Add( resultClassInfo );
}

SVLinearMaximumForegroundObjectLineAnalyzerClass::~SVLinearMaximumForegroundObjectLineAnalyzerClass()
{
	CloseObject();
}

BOOL SVLinearMaximumForegroundObjectLineAnalyzerClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = SVLinearAnalyzerClass::CreateObject( PCreateStructure );
	mdpEdgeA.SetObjectAttributesAllowed( SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );
	mdpEdgeB.SetObjectAttributesAllowed( SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );
	mdpCenter.SetObjectAttributesAllowed( SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );
	mdWidth.SetObjectAttributesAllowed( SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );
	m_svLinearDistanceA.SetObjectAttributesAllowed( SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );
	m_svLinearDistanceB.SetObjectAttributesAllowed( SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );

	m_isCreated = bOk;

	return bOk;
}

BOOL SVLinearMaximumForegroundObjectLineAnalyzerClass::CloseObject()
{
	return SVLinearAnalyzerClass::CloseObject();
}

bool SVLinearMaximumForegroundObjectLineAnalyzerClass::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	return __super::ResetObject(pErrorMessages) && ValidateEdgeA(pErrorMessages) && ValidateEdgeB(pErrorMessages);
}

HRESULT SVLinearMaximumForegroundObjectLineAnalyzerClass::GetSelectedEdgeOverlays( SVExtentMultiLineStruct &p_MultiLine )
{
	double l_dDistance = 0.0;

	SVImageExtentClass l_svAnalyzerExtents;

	HRESULT l_hrOk = GetImageExtent( l_svAnalyzerExtents );

	if( S_OK == l_hrOk && nullptr != GetEdgeA() && 
	    ( S_OK == m_svLinearDistanceA.GetValue( l_dDistance ) ) )
	{
		SVExtentLineStruct l_svLine;

		if( S_OK == GetEdgeA()->GetEdgeOverlayFromDistance( l_dDistance, l_svLine ) )
		{
			l_svAnalyzerExtents.TranslateFromOutputSpace( l_svLine, l_svLine );

			p_MultiLine.m_svLineArray.Add( l_svLine );

			l_hrOk = S_OK;
		}
	}

	if( S_OK == l_hrOk && nullptr != GetEdgeB() &&
	    ( S_OK == m_svLinearDistanceB.GetValue( l_dDistance ) ) )
	{
		SVExtentLineStruct l_svLine;

		if( S_OK == GetEdgeB()->GetEdgeOverlayFromDistance( l_dDistance, l_svLine ) )
		{
			l_svAnalyzerExtents.TranslateFromOutputSpace( l_svLine, l_svLine );

			p_MultiLine.m_svLineArray.Add( l_svLine );

			l_hrOk = S_OK;
		}
	}

	return l_hrOk;
}

bool SVLinearMaximumForegroundObjectLineAnalyzerClass::onRun( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
{
	SVImageExtentClass Extents;
	std::vector<double> AEdges;
	std::vector<double> BEdges;
	SVDPointClass DPointA, DPointB;
	SVExtentPointStruct EdgePointA;
	SVExtentPointStruct EdgePointB;
	SVDPointClass CenterPoint;
	double DistanceA( 0.0 );
	double DistanceB( 0.0 );

	bool l_bOk = __super::onRun( rRunStatus, pErrorMessages) && ValidateEdgeA(pErrorMessages) && ValidateEdgeB(pErrorMessages);
	
	if (nullptr == GetTool() || S_OK != GetEdgeA()->m_svLinearEdges.GetArrayValues( AEdges ) || S_OK != GetEdgeB()->m_svLinearEdges.GetArrayValues( BEdges ) )
	{
		l_bOk = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
	}

	if( l_bOk )
	{
		int resultAIndex = -1;
		int resultBIndex = -1;
		int offsetB = 0;

		double l_dMaxForegroundDistance = 0.0;

		int l_iACount = static_cast<int>(AEdges.size());
		int l_iBCount = static_cast<int>(BEdges.size());

		// Calc maximum foregoundobject...
		for( int indexA = 0; indexA < l_iACount && indexA + offsetB < l_iBCount; ++ indexA )
		{
			// Throw lower negative edges away...
			while( AEdges.at( indexA ) >= BEdges.at( indexA + offsetB ) && indexA + ( ++offsetB ) < static_cast<int>(BEdges.size()) );

			// Check for indexB bounds...
			if( indexA + offsetB >= static_cast<int>(BEdges.size()) )
				break;

			if( BEdges.at( indexA + offsetB ) - AEdges.at( indexA ) > l_dMaxForegroundDistance )
			{
				l_dMaxForegroundDistance = BEdges.at( indexA + offsetB ) - AEdges.at( indexA );
				resultAIndex = indexA;
				resultBIndex = indexA + offsetB;
			}
		}

		if( 0 <= resultAIndex )
		{
			DistanceA = AEdges.at( resultAIndex );
		}
		else
		{
			rRunStatus.SetFailed();
		}

		if(	0 <= resultBIndex )
		{
			DistanceB = BEdges.at( resultBIndex );
		}
		else
		{
			rRunStatus.SetFailed();
		}

		l_bOk &= S_OK == GetEdgeA()->GetPointFromDistance( DistanceA, EdgePointA );
		l_bOk &= S_OK == GetEdgeB()->GetPointFromDistance( DistanceB, EdgePointB );
	}

	l_bOk &= S_OK == GetImageExtent( Extents ) &&
			S_OK == Extents.TranslateFromOutputSpace(EdgePointA, EdgePointA) &&
			S_OK == Extents.TranslateFromOutputSpace(EdgePointB, EdgePointB);

	SVToolClass* pTool = dynamic_cast<SVToolClass*>(GetTool());
	l_bOk &= pTool && S_OK == pTool->GetImageExtent( Extents ) &&
			S_OK == Extents.TranslateFromOutputSpace(EdgePointA, EdgePointA) &&
			S_OK == Extents.TranslateFromOutputSpace(EdgePointB, EdgePointB);

	DPointA.x = EdgePointA.m_dPositionX;
	DPointA.y = EdgePointA.m_dPositionY;

	DPointB.x = EdgePointB.m_dPositionX;
	DPointB.y = EdgePointB.m_dPositionY;


	CenterPoint.x = DPointA.x + ((DPointB.x - DPointA.x) / 2.0);
	CenterPoint.y = DPointA.y + ((DPointB.y - DPointA.y) / 2.0);

	double Width = fabs( DistanceB - DistanceA );

	l_bOk = ( S_OK == m_svLinearDistanceA.SetValue( DistanceA, rRunStatus.m_lResultDataIndex ) ) && l_bOk;
	l_bOk = ( S_OK == m_svLinearDistanceB.SetValue( DistanceB, rRunStatus.m_lResultDataIndex ) ) && l_bOk;
	
	l_bOk = ( S_OK == mdpEdgeA.SetValue( DPointA, rRunStatus.m_lResultDataIndex ) ) && l_bOk;
	l_bOk = ( S_OK == mdpEdgeB.SetValue( DPointB, rRunStatus.m_lResultDataIndex ) ) && l_bOk;
	l_bOk = ( S_OK == mdpCenter.SetValue( CenterPoint, rRunStatus.m_lResultDataIndex ) ) && l_bOk;
	l_bOk = ( S_OK == mdWidth.SetValue( Width, rRunStatus.m_lResultDataIndex  ) ) && l_bOk;

	if ( !l_bOk )
	{
		SetInvalid();
		rRunStatus.SetInvalid();

		if (nullptr != pErrorMessages && pErrorMessages->empty())
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_RunLinearEdgeFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
	}

	return l_bOk;
}
