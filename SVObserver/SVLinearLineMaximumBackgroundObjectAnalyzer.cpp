// ******************************************************************************
// * COPYRIGHT (c) 2005 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVLinearMaximumBackgroundObjectLineAnalyzerClass
// * .File Name       : $Workfile:   SVLinearLineMaximumBackgroundObjectAnalyzer.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   01 Oct 2013 14:57:20  $
// ******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVLinearLineMaximumBackgroundObjectAnalyzer.h"
#include "Definitions/LinearEdgeEnums.h"
#include "SVLinearEdgeAProcessingClass.h"
#include "SVLinearEdgeBProcessingClass.h"
#include "InspectionEngine/SVTool.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

SV_IMPLEMENT_CLASS( SVLinearMaximumBackgroundObjectLineAnalyzerClass, SVLinearMaximumBackgroundObjectLineAnalyzerClassGuid );

SVLinearMaximumBackgroundObjectLineAnalyzerClass::SVLinearMaximumBackgroundObjectLineAnalyzerClass( SVObjectClass* POwner, int StringResourceID )
									 :SVLinearAnalyzerClass( POwner, StringResourceID ) 
{
	m_bEnableDirection = false;
	m_bEnableEdgeSelect = false;
	m_bEnablePolarisation = false;
	m_bEnablePosition = false;
	m_bEnableThreshold = true;

	init();
}

void SVLinearMaximumBackgroundObjectLineAnalyzerClass::init()
{
	// Identify our type
	m_outObjectInfo.m_ObjectTypeInfo.SubType = SvDef::SVLineMaximumBackgroundObjectAnalyzerObjectType;

	SVLinearEdgeProcessingClass *l_pEdgeA = new SVLinearEdgeAProcessingClass(this);
	SVLinearEdgeProcessingClass *l_pEdgeB = new SVLinearEdgeBProcessingClass(this);

	if( nullptr != l_pEdgeA )
	{
		AddFriend(l_pEdgeA->GetUniqueObjectID());

		l_pEdgeA->m_svPolarisation.SetDefaultValue(SvDef::SV_NEGATIVE_POLARISATION, true);

		l_pEdgeA->m_svEdgeSelect.SetDefaultValue(SvDef::SV_THIS_EDGE, true);
		l_pEdgeA->m_svEdgeSelectThisValue.SetDefaultValue( 1.0, true);
	}

	if( nullptr != l_pEdgeB )
	{
		AddFriend(l_pEdgeB->GetUniqueObjectID());

		l_pEdgeB->m_svPolarisation.SetDefaultValue(SvDef::SV_POSITIVE_POLARISATION, true);

		l_pEdgeB->m_svEdgeSelect.SetDefaultValue(SvDef::SV_THIS_EDGE, true);
		l_pEdgeB->m_svEdgeSelectThisValue.SetDefaultValue( 1.0, true );
	}

	RegisterEmbeddedObject( &m_svLinearDistanceA, SVLinearDistanceEdgeAObjectGuid, IDS_OBJECTNAME_LINEAR_DISTANCE_EDGE_A, false, SvOi::SVResetItemNone );
	m_svLinearDistanceA.setSaveValueFlag(false);
	RegisterEmbeddedObject( &m_svLinearDistanceB, SVLinearDistanceEdgeBObjectGuid, IDS_OBJECTNAME_LINEAR_DISTANCE_EDGE_B, false, SvOi::SVResetItemNone );
	m_svLinearDistanceB.setSaveValueFlag(false);

	// Register Embedded Objects
	RegisterEmbeddedObject( &mdpEdgeA, SVDPEdgeAObjectGuid, IDS_OBJECTNAME_DPEDGE_A, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &mdpEdgeB, SVDPEdgeBObjectGuid, IDS_OBJECTNAME_DPEDGE_B, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &mdpCenter, SVDPCenterObjectGuid, IDS_OBJECTNAME_DPCENTER, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &mdWidth, SVDWidthObjectGuid, IDS_OBJECTNAME_DWIDTH, false, SvOi::SVResetItemNone );

	RegisterEmbeddedObject( &m_svShowAllEdgeAOverlays, SVShowAllEdgeAOverlaysGuid, IDS_OBJECTNAME_SHOW_ALL_EDGE_A_OVERLAYS, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_svShowAllEdgeBOverlays, SVShowAllEdgeBOverlaysGuid, IDS_OBJECTNAME_SHOW_ALL_EDGE_B_OVERLAYS, false, SvOi::SVResetItemNone );

	// Set Edge defaults

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
	SvDef::SVObjectTypeInfoStruct interfaceInfo;

	// Declare Input Interface of Sub-pixel Edge A Results...
	interfaceInfo.EmbeddedID = SVDPEdgeAObjectGuid;
	resultClassInfo.m_DesiredInputVector.push_back( interfaceInfo );

	// Add the Sub-pixel EdgeA X Result...
	resultClassInfo.m_ObjectTypeInfo.ObjectType = SvDef::SVResultObjectType;
	resultClassInfo.m_ObjectTypeInfo.SubType	= SvDef::SVResultDPointXObjectType;
	resultClassInfo.m_ClassId = SVDPointXResultClassGuid;
	resultClassInfo.m_ClassName = SvUl::LoadStdString( IDS_OBJECTNAME_DPEDGE_A );
	std::string Title = SvUl::LoadStdString( IDS_CLASSNAME_RESULT_POINT_X );
	resultClassInfo.m_ClassName += _T(" ") + Title;
	m_availableChildren.push_back( resultClassInfo );

	// Add the Sub-pixel EdgeA Y Result...
	resultClassInfo.m_ObjectTypeInfo.ObjectType = SvDef::SVResultObjectType;
	resultClassInfo.m_ObjectTypeInfo.SubType	= SvDef::SVResultDPointYObjectType;
	resultClassInfo.m_ClassId = SVDPointYResultClassGuid;
	resultClassInfo.m_ClassName = SvUl::LoadStdString( IDS_OBJECTNAME_DPEDGE_A );
	Title = SvUl::LoadStdString( IDS_CLASSNAME_RESULT_POINT_Y );
	resultClassInfo.m_ClassName += _T(" ") + Title;
	m_availableChildren.push_back( resultClassInfo );

	// Declare Input Interface of Sub-pixel Edge B Results...
	resultClassInfo.m_DesiredInputVector.clear();
	interfaceInfo.EmbeddedID = SVDPEdgeBObjectGuid;
	resultClassInfo.m_DesiredInputVector.push_back( interfaceInfo );

	// Add the Sub-pixel EdgeB X Result...
	resultClassInfo.m_ObjectTypeInfo.ObjectType = SvDef::SVResultObjectType;
	resultClassInfo.m_ObjectTypeInfo.SubType	= SvDef::SVResultDPointXObjectType;
	resultClassInfo.m_ClassId = SVDPointXResultClassGuid;
	resultClassInfo.m_ClassName = SvUl::LoadStdString( IDS_OBJECTNAME_DPEDGE_B );
	Title = SvUl::LoadStdString( IDS_CLASSNAME_RESULT_POINT_X );
	resultClassInfo.m_ClassName += _T(" ") + Title;
	m_availableChildren.push_back( resultClassInfo );

	// Add the Sub-pixel EdgeB Y Result...
	resultClassInfo.m_ObjectTypeInfo.ObjectType = SvDef::SVResultObjectType;
	resultClassInfo.m_ObjectTypeInfo.SubType	= SvDef::SVResultDPointYObjectType;
	resultClassInfo.m_ClassId = SVDPointYResultClassGuid;
	resultClassInfo.m_ClassName = SvUl::LoadStdString( IDS_OBJECTNAME_DPEDGE_B );
	Title = SvUl::LoadStdString( IDS_CLASSNAME_RESULT_POINT_Y );
	resultClassInfo.m_ClassName += _T(" ") + Title;
	m_availableChildren.push_back( resultClassInfo );

	// Declare Input Interface of Sub-pixel Center Results...
	resultClassInfo.m_DesiredInputVector.clear();
	interfaceInfo.EmbeddedID = SVDPCenterObjectGuid;
	resultClassInfo.m_DesiredInputVector.push_back( interfaceInfo );

	// Add the Sub-pixel Center X Result...
	resultClassInfo.m_ObjectTypeInfo.ObjectType = SvDef::SVResultObjectType;
	resultClassInfo.m_ObjectTypeInfo.SubType	= SvDef::SVResultDPointXObjectType;
	resultClassInfo.m_ClassId = SVDPointXResultClassGuid;
	resultClassInfo.m_ClassName = SvUl::LoadStdString( IDS_OBJECTNAME_DPCENTER );
	Title = SvUl::LoadStdString( IDS_CLASSNAME_RESULT_POINT_X );
	resultClassInfo.m_ClassName += _T(" ") + Title;
	m_availableChildren.push_back( resultClassInfo );

	// Add the Sub-pixel Center Y Result...
	resultClassInfo.m_ObjectTypeInfo.ObjectType = SvDef::SVResultObjectType;
	resultClassInfo.m_ObjectTypeInfo.SubType	= SvDef::SVResultDPointYObjectType;
	resultClassInfo.m_ClassId = SVDPointYResultClassGuid;
	resultClassInfo.m_ClassName = SvUl::LoadStdString( IDS_OBJECTNAME_DPCENTER );
	Title = SvUl::LoadStdString( IDS_CLASSNAME_RESULT_POINT_Y );
	resultClassInfo.m_ClassName += _T(" ") + Title;
	m_availableChildren.push_back( resultClassInfo );

	// Declare Input Interface of Sub-pixel Width Result...
	resultClassInfo.m_DesiredInputVector.clear();
	interfaceInfo.EmbeddedID = SVDWidthObjectGuid;
	resultClassInfo.m_DesiredInputVector.push_back( interfaceInfo );

	// Add the Sub-pixel Width Result...
	resultClassInfo.m_ObjectTypeInfo.ObjectType = SvDef::SVResultObjectType;
	resultClassInfo.m_ObjectTypeInfo.SubType	= SvDef::SVResultDoubleObjectType;
	resultClassInfo.m_ClassId = SVDoubleResultClassGuid;
	resultClassInfo.m_ClassName = SvUl::LoadStdString( IDS_OBJECTNAME_DWIDTH );
	Title = SvUl::LoadStdString( IDS_RESULT_STRING );
	resultClassInfo.m_ClassName += _T(" ") + Title;
	m_availableChildren.push_back( resultClassInfo );

	resultClassInfo.m_DesiredInputVector.clear();
	interfaceInfo.EmbeddedID = SVLinearDistanceEdgeAObjectGuid;
	resultClassInfo.m_DesiredInputVector.push_back( interfaceInfo );

	// Add the Linear Distance Result...
	resultClassInfo.m_ObjectTypeInfo.ObjectType = SvDef::SVResultObjectType;
	resultClassInfo.m_ObjectTypeInfo.SubType	= SvDef::SVResultDoubleObjectType;
	resultClassInfo.m_ClassId = SVDoubleResultClassGuid;
	resultClassInfo.m_ClassName = SvUl::LoadStdString( IDS_OBJECTNAME_LINEAR_DISTANCE_EDGE_A );
	Title = SvUl::LoadStdString( IDS_OBJECTNAME_LINEAR_DISTANCE_EDGE_A_RESULT );
	resultClassInfo.m_ClassName += _T(" ") + Title;
	m_availableChildren.push_back( resultClassInfo );

	resultClassInfo.m_DesiredInputVector.clear();
	interfaceInfo.EmbeddedID = SVLinearDistanceEdgeBObjectGuid;
	resultClassInfo.m_DesiredInputVector.push_back( interfaceInfo );

	// Add the Linear Distance Result...
	resultClassInfo.m_ObjectTypeInfo.ObjectType = SvDef::SVResultObjectType;
	resultClassInfo.m_ObjectTypeInfo.SubType	= SvDef::SVResultDoubleObjectType;
	resultClassInfo.m_ClassId = SVDoubleResultClassGuid;
	resultClassInfo.m_ClassName = SvUl::LoadStdString( IDS_OBJECTNAME_LINEAR_DISTANCE_EDGE_B );
	Title = SvUl::LoadStdString( IDS_OBJECTNAME_LINEAR_DISTANCE_EDGE_B_RESULT );
	resultClassInfo.m_ClassName += _T(" ") + Title;
	m_availableChildren.push_back( resultClassInfo );
}

SVLinearMaximumBackgroundObjectLineAnalyzerClass::~SVLinearMaximumBackgroundObjectLineAnalyzerClass()
{
	CloseObject();
}

bool SVLinearMaximumBackgroundObjectLineAnalyzerClass::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	bool bOk = SVLinearAnalyzerClass::CreateObject(rCreateStructure);

	mdpEdgeA.SetObjectAttributesAllowed( SvDef::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );
	mdpEdgeB.SetObjectAttributesAllowed( SvDef::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );
	mdpCenter.SetObjectAttributesAllowed( SvDef::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );
	mdWidth.SetObjectAttributesAllowed( SvDef::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );
	m_svLinearDistanceA.SetObjectAttributesAllowed( SvDef::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );
	m_svLinearDistanceB.SetObjectAttributesAllowed( SvDef::SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );

	m_isCreated = bOk;

	return bOk;
}

bool SVLinearMaximumBackgroundObjectLineAnalyzerClass::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	return __super::ResetObject(pErrorMessages) && ValidateEdgeA(pErrorMessages) && ValidateEdgeB(pErrorMessages);
}

HRESULT SVLinearMaximumBackgroundObjectLineAnalyzerClass::GetSelectedEdgeOverlays( SVExtentMultiLineStruct &p_MultiLine )
{
	double l_dDistance = 0.0;

	SVImageExtentClass l_svAnalyzerExtents;

	HRESULT l_hrOk = GetImageExtent( l_svAnalyzerExtents );

	if( S_OK == l_hrOk && nullptr != GetEdgeA() && 
	    ( S_OK == m_svLinearDistanceA.GetValue( l_dDistance ) ) )
	{
		SVExtentLineStruct svLine;

		if( S_OK == GetEdgeA()->GetEdgeOverlayFromDistance( l_dDistance, svLine ) )
		{
			l_svAnalyzerExtents.TranslateFromOutputSpace( svLine, svLine );

			p_MultiLine.m_svLineArray.push_back( svLine );

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

			p_MultiLine.m_svLineArray.push_back( l_svLine );

			l_hrOk = S_OK;
		}
	}

	return l_hrOk;
}

void SVLinearMaximumBackgroundObjectLineAnalyzerClass::addParameterForMonitorList(SvStl::MessageContainerVector& rMessages, std::back_insert_iterator<SvOi::ParametersForML> inserter) const
{
	std::string name = GetCompleteName() + "." + SvUl::LoadStdString(IDS_OBJECTNAME_DPEDGE_A) + " " + SvUl::LoadStdString(IDS_CLASSNAME_RESULT_POINT_X) + "." + SvUl::LoadStdString(IDS_OBJECTNAME_DX);
	bool isNoError = setParameterToList(name, inserter);
	name = GetCompleteName() + "." + SvUl::LoadStdString(IDS_OBJECTNAME_DPEDGE_A) + " " + SvUl::LoadStdString(IDS_CLASSNAME_RESULT_POINT_Y) + "." + SvUl::LoadStdString(IDS_OBJECTNAME_DY);
	isNoError &= setParameterToList(name, inserter);
	name = GetCompleteName() + "." + SvUl::LoadStdString(IDS_OBJECTNAME_DPEDGE_B) + " " + SvUl::LoadStdString(IDS_CLASSNAME_RESULT_POINT_X) + "." + SvUl::LoadStdString(IDS_OBJECTNAME_DX);
	isNoError &= setParameterToList(name, inserter);
	name = GetCompleteName() + "." + SvUl::LoadStdString(IDS_OBJECTNAME_DPEDGE_B) + " " + SvUl::LoadStdString(IDS_CLASSNAME_RESULT_POINT_Y) + "." + SvUl::LoadStdString(IDS_OBJECTNAME_DY);
	isNoError &= setParameterToList(name, inserter);

	if (!isNoError)
	{
		SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_SetResultParameterToMonitorListFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
		rMessages.push_back(Msg);
	}
}

bool SVLinearMaximumBackgroundObjectLineAnalyzerClass::onRun( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages )
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

	bool Result = __super::onRun(rRunStatus, pErrorMessages) && ValidateEdgeA(pErrorMessages) && ValidateEdgeB(pErrorMessages);
	
	if ( nullptr == GetTool() || S_OK != GetEdgeA()->m_svLinearEdges.GetArrayValues( AEdges ) || S_OK != GetEdgeB()->m_svLinearEdges.GetArrayValues( BEdges ) )
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
	}

	if( Result )
	{
		int resultAIndex = -1;
		int resultBIndex = -1;
		int offsetB = 0;

		double l_dMaxBackgroundDistance = 0.0;

		int l_iACount = static_cast<int>(AEdges.size());
		int l_iBCount = static_cast<int>(BEdges.size());

		// Calc maximum backgoundobject...
		for( int indexA = 0; indexA < l_iACount && indexA + offsetB < l_iBCount; ++ indexA )
		{
			// Throw lower positive edges away...
			while( AEdges.at( indexA ) >= BEdges.at( indexA + offsetB ) && indexA + ( ++offsetB ) < l_iBCount );

			// Check for indexB bounds...
			if( indexA + offsetB >= l_iBCount )
			{
				break;
			}

			if( BEdges.at( indexA + offsetB ) - AEdges.at( indexA ) > l_dMaxBackgroundDistance )
			{
				l_dMaxBackgroundDistance = BEdges.at( indexA + offsetB ) - AEdges.at( indexA );
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

		Result &= S_OK == GetEdgeA()->GetPointFromDistance( DistanceA, EdgePointA );
		Result &= S_OK == GetEdgeB()->GetPointFromDistance( DistanceB, EdgePointB );
	}

	Result &= S_OK == GetImageExtent( Extents ) &&
			S_OK == Extents.TranslateFromOutputSpace(EdgePointA, EdgePointA) &&
			S_OK == Extents.TranslateFromOutputSpace(EdgePointB, EdgePointB);

	SVToolClass* pTool = dynamic_cast<SVToolClass*>(GetTool());
	Result &= pTool && S_OK == pTool->GetImageExtent( Extents ) &&
			S_OK == Extents.TranslateFromOutputSpace(EdgePointA, EdgePointA) &&
			S_OK == Extents.TranslateFromOutputSpace(EdgePointB, EdgePointB);

	DPointA.x = EdgePointA.m_dPositionX;
	DPointA.y = EdgePointA.m_dPositionY;

	DPointB.x = EdgePointB.m_dPositionX;
	DPointB.y = EdgePointB.m_dPositionY;


	CenterPoint.x = DPointA.x + ((DPointB.x - DPointA.x) / 2.0);
	CenterPoint.y = DPointA.y + ((DPointB.y - DPointA.y) / 2.0);

	double Width = 0.0;

	Width = fabs( DistanceB - DistanceA );

	Result = ( S_OK == m_svLinearDistanceA.SetValue(DistanceA) ) && Result;
	Result = ( S_OK == m_svLinearDistanceB.SetValue(DistanceB) ) && Result;
	
	Result = ( S_OK == mdpEdgeA.SetValue(DPointA) ) && Result;
	Result = ( S_OK == mdpEdgeB.SetValue(DPointB) ) && Result;
	Result = ( S_OK == mdpCenter.SetValue(CenterPoint) ) && Result;
	Result = ( S_OK == mdWidth.SetValue(Width) ) && Result;

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
