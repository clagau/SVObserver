// ******************************************************************************
// * COPYRIGHT (c) 2005 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVLinearAnalyzerClass
// * .File Name       : $Workfile:   SVLinearAnalyzerClass.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.4  $
// * .Check In Date   : $Date:   10 Jul 2014 17:44:52  $
// ******************************************************************************

#include "stdafx.h"
#include "SVLinearAnalyzerClass.h"
#include "SVGlobal.h"
#include "SVInspectionProcess.h"
#include "SVLinearEdgeAProcessingClass.h"
#include "SVOCore/SVImageClass.h"
#include "SVOCore/SVTool.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SV_IMPLEMENT_CLASS( SVLinearAnalyzerClass, SVLinearAnalyzerClassGuid );


SVLinearAnalyzerClass::SVLinearAnalyzerClass( SVObjectClass* POwner, int StringResourceID )
					            :SVAnalyzerClass( POwner, StringResourceID )
{
	// Identify yourself
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SvDef::SVAnalyzerObjectType;

	m_svInputImageObjectInfo.SetInputObjectType(SvDef::SVImageObjectType, SvDef::SVImageMonoType);
	m_svInputImageObjectInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_svInputImageObjectInfo, _T( "LinearAnalyzerImage" ) );

	m_svInputProfileOrientation.SetInputObjectType( SVProfileOrientationGuid, SvDef::SVValueObjectType, SvDef::SVEnumValueObjectType );
	m_svInputProfileOrientation.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_svInputProfileOrientation, _T( "LinearAnalyzerOrientation" ) );

	m_svInputUseRotationAngle.SetInputObjectType( SVLinearToolUseRotationGuid, SvDef::SVValueObjectType, SvDef::SVBoolValueObjectType );
	m_svInputUseRotationAngle.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_svInputUseRotationAngle, _T( "LinearAnalyzerUseRotationAngle" ) );

	m_lPixelDepth = 0;
	m_dwMinThreshold = 0;
	m_dwMaxThreshold = 0;
	m_dwColorNumber = 0;

	m_bEnableDirection = false;
	m_bEnableEdgeSelect = false;
	m_bEnablePolarisation = false;
	m_bEnablePosition = false;
	m_bEnableThreshold = false;

	m_svShowAllEdgeAOverlays.SetDefaultValue( BOOL(false), true );
	m_svShowAllEdgeBOverlays.SetDefaultValue( BOOL(false), true);

	addDefaultInputObjects();
}

SVLinearAnalyzerClass::~SVLinearAnalyzerClass()
{
}

bool SVLinearAnalyzerClass::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	SVImageInfoClass ImageInfo;

	bool bOk = SVAnalyzerClass::CreateObject(rCreateStructure);

	bOk = bOk && S_OK == GetPixelDepth();

	if (bOk)
	{
		// Set range...
		m_dwMinThreshold = static_cast<unsigned long>(SVGetDataTypeMin(m_lPixelDepth));
		m_dwMaxThreshold = static_cast<unsigned long>(SVGetDataTypeMax(m_lPixelDepth));
		m_dwColorNumber = static_cast<unsigned long>(SVGetDataTypeRange(m_lPixelDepth));

		m_svNormalizer.SetNormalRange(m_dwMinThreshold, m_dwMaxThreshold);
		m_svNormalizer.SetRealRange(m_dwMinThreshold, m_dwMaxThreshold);

		m_svShowAllEdgeAOverlays.SetObjectAttributesAllowed(SvDef::SV_SETABLE_ONLINE | SvDef::SV_REMOTELY_SETABLE, SvOi::SetAttributeType::AddAttribute);
		m_svShowAllEdgeBOverlays.SetObjectAttributesAllowed(SvDef::SV_SETABLE_ONLINE | SvDef::SV_REMOTELY_SETABLE, SvOi::SetAttributeType::AddAttribute);
	}

	return bOk;
}

bool SVLinearAnalyzerClass::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = __super::ResetObject(pErrorMessages);

	if( S_OK != GetPixelDepth() )
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_GetPixelDepthFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
	}

  if ( Result )
  {
		// Set range...
		m_dwMinThreshold = static_cast<unsigned long>(SVGetDataTypeMin( m_lPixelDepth ));	
		m_dwMaxThreshold = static_cast<unsigned long>(SVGetDataTypeMax( m_lPixelDepth ));	
		m_dwColorNumber = static_cast<unsigned long>(SVGetDataTypeRange( m_lPixelDepth ));

		m_svNormalizer.SetNormalRange( m_dwMinThreshold, m_dwMaxThreshold );
		m_svNormalizer.SetRealRange( m_dwMinThreshold, m_dwMaxThreshold );
	}

	return Result;
}

HRESULT SVLinearAnalyzerClass::GetSelectedEdgeOverlays( SVExtentMultiLineStruct &p_rsvMiltiLine )
{
	SVImageExtentClass l_svAnalyzerExtents;

	HRESULT l_hrOk = GetImageExtent( l_svAnalyzerExtents );

	if( S_OK == l_hrOk && nullptr != GetEdgeA() )
	{
		SVExtentLineStruct l_svLine;

		if( S_OK == GetEdgeA()->GetSelectedEdgeOverlay( l_svLine ) )
		{
			l_svAnalyzerExtents.TranslateFromOutputSpace( l_svLine, l_svLine );

			p_rsvMiltiLine.m_svLineArray.push_back( l_svLine );

			l_hrOk = S_OK;
		}
	}

	if( S_OK == l_hrOk && nullptr != GetEdgeB() )
	{
		SVExtentLineStruct l_svLine;

		if( S_OK == GetEdgeB()->GetSelectedEdgeOverlay( l_svLine ) )
		{
			l_svAnalyzerExtents.TranslateFromOutputSpace( l_svLine, l_svLine );

			p_rsvMiltiLine.m_svLineArray.push_back( l_svLine );

			l_hrOk = S_OK;
		}
	}

	return l_hrOk;
}

HRESULT SVLinearAnalyzerClass::GetImageExtent( SVImageExtentClass &rImageExtent )
{
	SVExtentPointStruct l_svPosition;
	SVExtentPointStruct l_svPositionEndOfLine;
	SVImageExtentClass l_svExtents;

	long Width( 0 );
	long Height( 0 );
	long Angle( 0 );
	bool bUseRotation( false );

	HRESULT l_hrOk = rImageExtent.Initialize();

	SVToolClass* pTool = dynamic_cast<SVToolClass*>(GetTool());
	if( nullptr == pTool || 
	    ( S_OK != GetInputUseRotationAngle( bUseRotation ) ) ||
	    ( S_OK != GetInputProfileOrientation( Angle ) ) ||
			S_OK != pTool->GetImageExtent( l_svExtents ) ||
			S_OK != l_svExtents.GetExtentProperty( SVExtentPropertyOutputWidth, Width ) ||
			S_OK != l_svExtents.GetExtentProperty( SVExtentPropertyOutputHeight, Height ) )
	{
		l_hrOk = S_FALSE;
	}

	if( S_OK == l_hrOk )
	{
		rImageExtent.SetTranslation( SVExtentTranslationLinear );

		if( !bUseRotation )
		{
			if( 90 == Angle )
			{
				l_svPosition.m_dPositionX = Width - 1;
				std::swap( Width, Height );
			}
			else if( 0 != Angle )
			{
				return S_FALSE;
			}
		}
		rImageExtent.SetExtentProperty( SVExtentPropertyRotationAngle, static_cast<double> (Angle) );
		rImageExtent.SetExtentProperty( SVExtentPropertyPositionPoint, l_svPosition );
		rImageExtent.SetExtentProperty( SVExtentPropertyWidth, static_cast<double> (Width) );
		rImageExtent.SetExtentProperty( SVExtentPropertyHeight, static_cast<double> (Height) );

		l_hrOk = rImageExtent.UpdateData();
	}

	return l_hrOk;
}

SVLinearEdgeProcessingClass *SVLinearAnalyzerClass::GetEdgeA()
{
	SVLinearEdgeProcessingClass *l_psvEdge = nullptr;

	SvDef::SVObjectTypeInfoStruct  info;

	info.ObjectType = SvDef::SVLinearEdgeProcessingObjectType;
	info.SubType = SvDef::SVLinearEdgeAProcessingObjectType;

	l_psvEdge = dynamic_cast<SVLinearEdgeProcessingClass *>(getFirstObject(info));

	return l_psvEdge;
}

SVLinearEdgeProcessingClass *SVLinearAnalyzerClass::GetEdgeB()
{
	SVLinearEdgeProcessingClass *l_psvEdge = nullptr;

	SvDef::SVObjectTypeInfoStruct  info;

	info.ObjectType = SvDef::SVLinearEdgeProcessingObjectType;
	info.SubType = SvDef::SVLinearEdgeBProcessingObjectType;

	l_psvEdge = dynamic_cast<SVLinearEdgeProcessingClass *>(getFirstObject(info));

	return l_psvEdge;
}

SVImageClass *SVLinearAnalyzerClass::GetInputImage()
{
	SVImageClass *l_psvImage = nullptr;

	if( m_svInputImageObjectInfo.IsConnected() && 
		  nullptr != m_svInputImageObjectInfo.GetInputObjectInfo().m_pObject )
	{
		l_psvImage = dynamic_cast<SVImageClass *>(m_svInputImageObjectInfo.GetInputObjectInfo().m_pObject);
	}

	return l_psvImage;
}

HRESULT SVLinearAnalyzerClass::GetPixelDepth()
{
	HRESULT l_hrOk = S_FALSE;

	if( nullptr != GetInputImage() )
	{
    SVImageInfoClass ImageInfo = GetInputImage()->GetImageInfo();

		l_hrOk = ImageInfo.GetImageProperty( SvDef::SVImagePropertyEnum::SVImagePropertyPixelDepth, m_lPixelDepth );
	}

	return l_hrOk;
}

HRESULT SVLinearAnalyzerClass::GetInputProfileOrientation(long& rProfileOrientation)
{
	HRESULT Result(E_FAIL);
	if( m_svInputProfileOrientation.IsConnected() && nullptr != m_svInputProfileOrientation.GetInputObjectInfo().m_pObject )
	{
		double Value(0.0);
		Result = m_svInputProfileOrientation.GetInputObjectInfo().m_pObject->getValue(Value);
		rProfileOrientation = static_cast<long> (Value);
	}

	return Result;
}

HRESULT SVLinearAnalyzerClass::GetInputUseRotationAngle(bool& rUseRotationAngle)
{
	HRESULT Result(E_FAIL);
	if( m_svInputUseRotationAngle.IsConnected() && nullptr != m_svInputUseRotationAngle.GetInputObjectInfo().m_pObject )
	{
		double Value(0.0);
		Result = m_svInputUseRotationAngle.GetInputObjectInfo().m_pObject->getValue(Value);
		rUseRotationAngle = 0.0 < Value ? true : false;
	}

	return Result;
}

HRESULT SVLinearAnalyzerClass::onCollectOverlays(SVImageClass *p_Image,SVExtentMultiLineStructVector &p_MultiLineArray)
{
	HRESULT l_hrRet = S_OK;
	
	SVExtentFigureStruct l_svFigure;
	SVExtentMultiLineStruct l_svMultiLine;

	SVImageExtentClass l_svToolExtents;
	SVImageExtentClass l_svAnalyzerExtents;
	bool l_bOk = true;

	SVToolClass* pTool = dynamic_cast<SVToolClass*>(GetTool());
	l_bOk = l_bOk && nullptr != pTool && S_OK == pTool->GetImageExtent( l_svToolExtents );
	l_bOk = l_bOk && S_OK == GetImageExtent( l_svAnalyzerExtents );
	
	if ( S_OK == l_svAnalyzerExtents.GetFigure(l_svFigure) )
	{
		COLORREF l_Color = 0;

		m_statusColor.GetValue( l_Color );

		l_svMultiLine.m_Color = l_Color;

		l_svToolExtents.TranslateFromOutputSpace(l_svFigure,l_svFigure);
		l_svMultiLine.AssignExtentFigure( l_svFigure, l_Color );

		l_svMultiLine.m_csString = "";

		UpdateOverlayIDs( l_svMultiLine );

		p_MultiLineArray.push_back( l_svMultiLine );
	
		l_svMultiLine.Initialize();

		BOOL l_bShow = false;
		if ( nullptr != GetEdgeA() )
		{ 
			if( nullptr != m_svShowAllEdgeAOverlays.GetOwner() && 
				( S_OK == m_svShowAllEdgeAOverlays.GetValue(l_bShow) ) && l_bShow )
			{
				SVExtentLineStruct l_svLine;

				l_svLine.m_dwColor = l_Color;

				if ( S_OK == GetEdgeA()->GetHistogramOverlay(l_svLine) )
				{
					l_svAnalyzerExtents.TranslateFromOutputSpace(l_svLine,l_svLine);
					l_svToolExtents.TranslateFromOutputSpace(l_svLine,l_svLine);
					l_svMultiLine.m_svLineArray.push_back(l_svLine);
					p_MultiLineArray.push_back(l_svMultiLine);
					l_svMultiLine.Initialize();
				}
				
				if ( S_OK == GetEdgeA()->GetThresholdBarsOverlay(l_svMultiLine) )
				{
					l_svAnalyzerExtents.TranslateFromOutputSpace(l_svMultiLine,l_svMultiLine);
					l_svToolExtents.TranslateFromOutputSpace(l_svMultiLine,l_svMultiLine);
					p_MultiLineArray.push_back(l_svMultiLine);
					l_svMultiLine.Initialize();
				}
				if ( S_OK == GetEdgeA()->GetEdgesOverlay(l_svMultiLine) )
				{
					l_svAnalyzerExtents.TranslateFromOutputSpace(l_svMultiLine,l_svMultiLine);
					l_svToolExtents.TranslateFromOutputSpace(l_svMultiLine,l_svMultiLine);
					p_MultiLineArray.push_back(l_svMultiLine);
					l_svMultiLine.Initialize();
				}

			}
		}
		if ( nullptr != GetEdgeB() )
		{ 
			if( nullptr != m_svShowAllEdgeBOverlays.GetOwner() && 
				( S_OK == m_svShowAllEdgeBOverlays.GetValue(l_bShow) ) && l_bShow )
			{
				SVExtentLineStruct svLine;

				svLine.m_dwColor = l_Color;

				if ( S_OK == GetEdgeB()->GetHistogramOverlay(svLine) )
				{
					l_svAnalyzerExtents.TranslateFromOutputSpace(svLine,svLine);
					l_svToolExtents.TranslateFromOutputSpace(svLine,svLine);
					l_svMultiLine.m_svLineArray.push_back(svLine);
					p_MultiLineArray.push_back(l_svMultiLine);
					l_svMultiLine.Initialize();
				}
				
				if ( S_OK == GetEdgeB()->GetThresholdBarsOverlay(l_svMultiLine) )
				{
					l_svAnalyzerExtents.TranslateFromOutputSpace(l_svMultiLine,l_svMultiLine);
					l_svToolExtents.TranslateFromOutputSpace(l_svMultiLine,l_svMultiLine);
					p_MultiLineArray.push_back(l_svMultiLine);
					l_svMultiLine.Initialize();
				}
				if ( S_OK == GetEdgeB()->GetEdgesOverlay(l_svMultiLine) )
				{
					l_svAnalyzerExtents.TranslateFromOutputSpace(l_svMultiLine,l_svMultiLine);
					l_svToolExtents.TranslateFromOutputSpace(l_svMultiLine,l_svMultiLine);
					p_MultiLineArray.push_back(l_svMultiLine);
					l_svMultiLine.Initialize();
				}

			}
		}

		l_hrRet = S_OK;
	}
	else
	{
		l_hrRet = S_FALSE;
	}

	if ( l_bOk )
	{
		SVExtentMultiLineStruct l_svMultiLine;

		if ( S_OK == GetSelectedEdgeOverlays( l_svMultiLine ) )
		{
			l_svToolExtents.TranslateFromOutputSpace( l_svMultiLine, l_svMultiLine );

			m_statusColor.GetValue(l_svMultiLine.m_Color);

			UpdateOverlayIDs( l_svMultiLine );

			p_MultiLineArray.push_back( l_svMultiLine );
		}
	}
	return l_hrRet;
}

bool SVLinearAnalyzerClass::ValidateEdgeA(SvStl::MessageContainerVector *pErrorMessages)
{
	if (nullptr == GetEdgeA())
	{
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_GetEdgeAFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
		return false;
	}
	return true;
}

bool SVLinearAnalyzerClass::ValidateEdgeB(SvStl::MessageContainerVector *pErrorMessages)
{
	if (nullptr == GetEdgeB())
	{
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_GetEdgeBFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
		return false;
	}
	return true;
}