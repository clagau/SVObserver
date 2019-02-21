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
#include "SVObjectLibrary/SVClsIds.h"
#include "SVMessage/SVMessage.h"
#include "Definitions/LinearEdgeEnums.h"
#include "InspectionEngine/SVImageClass.h"
#include "Operators/SVLinearEdgeAProcessingClass.h"
#include "Operators/SVLinearEdgeBProcessingClass.h"
#include "Tools/SVTool.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

namespace SvAo
{
#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

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
	m_outObjectInfo.m_ObjectTypeInfo.SubType = SvPb::SVLinearMeasurementAnalyzerObjectType;

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
	SvOp::SVLinearEdgeProcessingClass *pEdgeA = new SvOp::SVLinearEdgeAProcessingClass(this);
	SvOp::SVLinearEdgeProcessingClass *pEdgeB = new SvOp::SVLinearEdgeBProcessingClass(this);

	if( nullptr != pEdgeA )
	{
		AddFriend(pEdgeA->GetUniqueObjectID());
	}

	if( nullptr != pEdgeB )
	{
		AddFriend(pEdgeB->GetUniqueObjectID());

		pEdgeB->m_svEdgeSelect.SetDefaultValue(SvDef::SV_LAST_EDGE, true);
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
	SvIe::SVClassInfoStruct resultClassInfo;
	SvDef::SVObjectTypeInfoStruct interfaceInfo;

	// Declare Input Interface of Sub-pixel Edge A Results...
	interfaceInfo.EmbeddedID = SVDPEdgeAObjectGuid;
	resultClassInfo.m_DesiredInputVector.push_back( interfaceInfo );

	// Add the Sub-pixel EdgeA X Result...
	resultClassInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVResultObjectType;
	resultClassInfo.m_ObjectTypeInfo.SubType	= SvPb::SVResultDPointXObjectType;
	resultClassInfo.m_ClassId = SVDPointXResultClassGuid;
	resultClassInfo.m_ClassName = SvUl::LoadStdString( IDS_OBJECTNAME_DPEDGE_A );
	std::string strTitle = SvUl::LoadStdString(IDS_CLASSNAME_RESULT_POINT_X);
	resultClassInfo.m_ClassName += _T(" ") + strTitle;
	m_availableChildren.push_back( resultClassInfo );
	
	// Add the Sub-pixel EdgeA Y Result...
	resultClassInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVResultObjectType;
	resultClassInfo.m_ObjectTypeInfo.SubType	= SvPb::SVResultDPointYObjectType;
	resultClassInfo.m_ClassId = SVDPointYResultClassGuid;
	resultClassInfo.m_ClassName = SvUl::LoadStdString( IDS_OBJECTNAME_DPEDGE_A );
	strTitle = SvUl::LoadStdString(IDS_CLASSNAME_RESULT_POINT_Y);
	resultClassInfo.m_ClassName += _T(" ") + strTitle;
	m_availableChildren.push_back( resultClassInfo );

	// Declare Input Interface of Sub-pixel Edge B Results...
	resultClassInfo.m_DesiredInputVector.clear();
	interfaceInfo.EmbeddedID = SVDPEdgeBObjectGuid;
	resultClassInfo.m_DesiredInputVector.push_back( interfaceInfo );
	
	// Add the Sub-pixel EdgeB X Result...
	resultClassInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVResultObjectType;
	resultClassInfo.m_ObjectTypeInfo.SubType	= SvPb::SVResultDPointXObjectType;
	resultClassInfo.m_ClassId = SVDPointXResultClassGuid;
	resultClassInfo.m_ClassName = SvUl::LoadStdString( IDS_OBJECTNAME_DPEDGE_B );
	strTitle = SvUl::LoadStdString(IDS_CLASSNAME_RESULT_POINT_X);
	resultClassInfo.m_ClassName += _T(" ") + strTitle;
	m_availableChildren.push_back( resultClassInfo );
	
	// Add the Sub-pixel EdgeB Y Result...
	resultClassInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVResultObjectType;
	resultClassInfo.m_ObjectTypeInfo.SubType	= SvPb::SVResultDPointYObjectType;
	resultClassInfo.m_ClassId = SVDPointYResultClassGuid;
	resultClassInfo.m_ClassName = SvUl::LoadStdString( IDS_OBJECTNAME_DPEDGE_B );
	strTitle = SvUl::LoadStdString(IDS_CLASSNAME_RESULT_POINT_Y);
	resultClassInfo.m_ClassName += _T(" ") + strTitle;
	m_availableChildren.push_back( resultClassInfo );

	// Declare Input Interface of Sub-pixel Center Results...
	resultClassInfo.m_DesiredInputVector.clear();
	interfaceInfo.EmbeddedID = SVDPCenterObjectGuid;
	resultClassInfo.m_DesiredInputVector.push_back( interfaceInfo );
	
	// Add the Sub-pixel Center X Result...
	resultClassInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVResultObjectType;
	resultClassInfo.m_ObjectTypeInfo.SubType	= SvPb::SVResultDPointXObjectType;
	resultClassInfo.m_ClassId = SVDPointXResultClassGuid;
	resultClassInfo.m_ClassName = SvUl::LoadStdString( IDS_OBJECTNAME_DPCENTER );
	strTitle = SvUl::LoadStdString(IDS_CLASSNAME_RESULT_POINT_X);
	resultClassInfo.m_ClassName += _T(" ") + strTitle;
	m_availableChildren.push_back( resultClassInfo );
	
	// Add the Sub-pixel Center Y Result...
	resultClassInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVResultObjectType;
	resultClassInfo.m_ObjectTypeInfo.SubType	= SvPb::SVResultDPointYObjectType;
	resultClassInfo.m_ClassId = SVDPointYResultClassGuid;
	resultClassInfo.m_ClassName = SvUl::LoadStdString( IDS_OBJECTNAME_DPCENTER );
	strTitle = SvUl::LoadStdString(IDS_CLASSNAME_RESULT_POINT_Y);
	resultClassInfo.m_ClassName += _T(" ") + strTitle;
	m_availableChildren.push_back( resultClassInfo );

	// Declare Input Interface of Sub-pixel Width Result...
	resultClassInfo.m_DesiredInputVector.clear();
	interfaceInfo.EmbeddedID = SVDWidthObjectGuid;
	resultClassInfo.m_DesiredInputVector.push_back( interfaceInfo );
	
	// Add the Sub-pixel Width Result...
	resultClassInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVResultObjectType;
	resultClassInfo.m_ObjectTypeInfo.SubType	= SvPb::SVResultDoubleObjectType;
	resultClassInfo.m_ClassId = SVDoubleResultClassGuid;
	resultClassInfo.m_ClassName = SvUl::LoadStdString( IDS_OBJECTNAME_DWIDTH );
	strTitle = SvUl::LoadStdString(IDS_RESULT_STRING);
	resultClassInfo.m_ClassName += _T(" ") + strTitle;
	m_availableChildren.push_back( resultClassInfo );

	resultClassInfo.m_DesiredInputVector.clear();
	interfaceInfo.EmbeddedID = SVLinearDistanceEdgeAObjectGuid;
	resultClassInfo.m_DesiredInputVector.push_back( interfaceInfo );

	// Add the Linear Distance Result...
	resultClassInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVResultObjectType;
	resultClassInfo.m_ObjectTypeInfo.SubType	= SvPb::SVResultDoubleObjectType;
	resultClassInfo.m_ClassId = SVDoubleResultClassGuid;
	resultClassInfo.m_ClassName = SvUl::LoadStdString( IDS_OBJECTNAME_LINEAR_DISTANCE_EDGE_A );
	strTitle = SvUl::LoadStdString(IDS_OBJECTNAME_LINEAR_DISTANCE_EDGE_A_RESULT);
	resultClassInfo.m_ClassName += _T(" ") + strTitle;
	m_availableChildren.push_back( resultClassInfo );

	resultClassInfo.m_DesiredInputVector.clear();
	interfaceInfo.EmbeddedID = SVLinearDistanceEdgeBObjectGuid;
	resultClassInfo.m_DesiredInputVector.push_back( interfaceInfo );

	// Add the Linear Distance Result...
	resultClassInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVResultObjectType;
	resultClassInfo.m_ObjectTypeInfo.SubType	= SvPb::SVResultDoubleObjectType;
	resultClassInfo.m_ClassId = SVDoubleResultClassGuid;
	resultClassInfo.m_ClassName = SvUl::LoadStdString( IDS_OBJECTNAME_LINEAR_DISTANCE_EDGE_B );
	strTitle = SvUl::LoadStdString(IDS_OBJECTNAME_LINEAR_DISTANCE_EDGE_B_RESULT);
	resultClassInfo.m_ClassName += _T(" ") + strTitle;
	m_availableChildren.push_back( resultClassInfo );
}

SVLinearMeasurementAnalyzerClass::~SVLinearMeasurementAnalyzerClass()
{
	CloseObject();
}

bool SVLinearMeasurementAnalyzerClass::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	bool bOk = SVLinearAnalyzerClass::CreateObject(rCreateStructure);

	// Set / Reset Printable Flag
	mdpEdgeA.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::RemoveAttribute );
	mdpEdgeB.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::RemoveAttribute );
	mdpCenter.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::RemoveAttribute );
	mdWidth.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::RemoveAttribute );

	m_svLinearDistanceA.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::RemoveAttribute );
	m_svLinearDistanceB.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::RemoveAttribute );

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

	SVPoint<double> edgePointA;
	SVPoint<double> edgePointB;
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

		if ( S_OK != GetEdgeA()->GetOutputEdgePoint(edgePointA) )
		{
			rRunStatus.SetFailed();
		}

		if ( S_OK != GetEdgeA()->GetOutputEdgeDistance(DistanceA) )
		{
			rRunStatus.SetFailed();
		}

		if ( S_OK != GetEdgeB()->GetOutputEdgePoint(edgePointB) )
		{
			rRunStatus.SetFailed();
		}

		if ( S_OK != GetEdgeB()->GetOutputEdgeDistance(DistanceB) )
		{
			rRunStatus.SetFailed();
		}

		Result &= S_OK == GetImageExtent().TranslateFromOutputSpace(edgePointA, edgePointA) &&
				S_OK == GetImageExtent().TranslateFromOutputSpace(edgePointB, edgePointB);

		SvTo::SVToolClass* pTool = dynamic_cast<SvTo::SVToolClass*> (GetTool());
		Result &= pTool && S_OK == pTool->GetImageExtent().TranslateFromOutputSpace(edgePointA, edgePointA) &&
				S_OK == pTool->GetImageExtent().TranslateFromOutputSpace(edgePointB, edgePointB);

		SVPoint<double> CenterPoint
		{
			edgePointA.m_x + ((edgePointB.m_x - edgePointA.m_x) / 2.0),
			edgePointA.m_y + ((edgePointB.m_y - edgePointA.m_y) / 2.0)
		};

		double Width = fabs( DistanceB - DistanceA + 1.0 );

		Result = ( S_OK == m_svLinearDistanceA.SetValue(DistanceA) ) && Result;
		Result = ( S_OK == m_svLinearDistanceB.SetValue(DistanceB) ) && Result;
		
		Result = ( S_OK == mdpEdgeA.SetValue(edgePointA) ) && Result;
		Result = ( S_OK == mdpEdgeB.SetValue(edgePointB) ) && Result;
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

std::vector<std::string> SVLinearMeasurementAnalyzerClass::getParameterNamesForML() const
{
	return {
		SvUl::LoadStdString(IDS_OBJECTNAME_DPEDGE_A) + " " + SvUl::LoadStdString(IDS_CLASSNAME_RESULT_POINT_X) + "." + SvUl::LoadStdString(IDS_OBJECTNAME_DX),
		SvUl::LoadStdString(IDS_OBJECTNAME_DPEDGE_A) + " " + SvUl::LoadStdString(IDS_CLASSNAME_RESULT_POINT_Y) + "." + SvUl::LoadStdString(IDS_OBJECTNAME_DY),
		SvUl::LoadStdString(IDS_OBJECTNAME_DPEDGE_B) + " " + SvUl::LoadStdString(IDS_CLASSNAME_RESULT_POINT_X) + "." + SvUl::LoadStdString(IDS_OBJECTNAME_DX),
		SvUl::LoadStdString(IDS_OBJECTNAME_DPEDGE_B) + " " + SvUl::LoadStdString(IDS_CLASSNAME_RESULT_POINT_Y) + "." + SvUl::LoadStdString(IDS_OBJECTNAME_DY)
	};
}

} //namespace SvAo