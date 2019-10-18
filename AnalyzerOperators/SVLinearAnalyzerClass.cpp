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
#include "SVObjectLibrary/SVClsIds.h"
#include "Operators/SVLinearEdgeProcessingClass.h"
#include "InspectionEngine/SVImageClass.h"
#include "Tools/SVTool.h"

namespace SvAo
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS( SVLinearAnalyzerClass, SVLinearAnalyzerClassGuid );


SVLinearAnalyzerClass::SVLinearAnalyzerClass( SVObjectClass* POwner, int StringResourceID )
					            :SVAnalyzerClass( POwner, StringResourceID )
{
	// Identify yourself
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVAnalyzerObjectType;

	m_InputImageObjectInfo.SetInputObjectType(SvPb::SVImageObjectType, SvPb::SVImageMonoType);
	m_InputImageObjectInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_InputImageObjectInfo, _T( "LinearAnalyzerImage" ) );

	m_InputProfileOrientation.SetInputObjectType(SvPb::SVValueObjectType, SvPb::SVEnumValueObjectType, SVProfileOrientationGuid);
	m_InputProfileOrientation.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_InputProfileOrientation, _T( "LinearAnalyzerOrientation" ) );

	m_InputUseRotationAngle.SetInputObjectType(SvPb::SVValueObjectType, SvPb::SVBoolValueObjectType, SVLinearToolUseRotationGuid);
	m_InputUseRotationAngle.SetObject( GetObjectInfo() );
	RegisterInputObject( &m_InputUseRotationAngle, _T( "LinearAnalyzerUseRotationAngle" ) );

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
		m_dwMinThreshold = 0;
		m_dwMaxThreshold = (1 << m_lPixelDepth) - 1;
		m_dwColorNumber = 1 << m_lPixelDepth;

		m_svNormalizer.SetNormalRange(m_dwMinThreshold, m_dwMaxThreshold);
		m_svNormalizer.SetRealRange(m_dwMinThreshold, m_dwMaxThreshold);

		m_svShowAllEdgeAOverlays.SetObjectAttributesAllowed(SvPb::noAttributes, SvOi::SetAttributeType::OverwriteAttribute);
		m_svShowAllEdgeBOverlays.SetObjectAttributesAllowed(SvPb::noAttributes, SvOi::SetAttributeType::OverwriteAttribute);
		m_svShowAllEdgeAOverlays.setSaveDefaultValueFlag(false);
		m_svShowAllEdgeBOverlays.setSaveDefaultValueFlag(false);
		//Set the parameter to false, because in old configuration it is possible that by accident.
		m_svShowAllEdgeAOverlays.SetValue(false);
		m_svShowAllEdgeBOverlays.SetValue(false);
	}

	return bOk;
}

bool SVLinearAnalyzerClass::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = __super::ResetObject(pErrorMessages);

	SvOl::SVInObjectInfoStructPtrVector InputList
	{
		&m_InputImageObjectInfo,
		&m_InputProfileOrientation,
		&m_InputUseRotationAngle
	};

	SvOl::ValidateInputList(InputList);

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
		m_dwMinThreshold = 0;	
		m_dwMaxThreshold = (1 << m_lPixelDepth) - 1;
		m_dwColorNumber = 1 << m_lPixelDepth;

		m_svNormalizer.SetNormalRange( m_dwMinThreshold, m_dwMaxThreshold );
		m_svNormalizer.SetRealRange( m_dwMinThreshold, m_dwMaxThreshold );
	}

	return Result;
}

HRESULT SVLinearAnalyzerClass::GetSelectedEdgeOverlays( SVExtentMultiLineStruct &p_rsvMiltiLine )
{
	const SVImageExtentClass& rAnalyzerExtents = GetImageExtent();

	if(nullptr != GetEdgeA())
	{
		SVExtentLineStruct l_svLine;

		if( S_OK == GetEdgeA()->GetSelectedEdgeOverlay( l_svLine ) )
		{
			rAnalyzerExtents.TranslateFromOutputSpace( l_svLine, l_svLine );

			p_rsvMiltiLine.m_svLineArray.push_back( l_svLine );
		}
	}

	if(nullptr != GetEdgeB())
	{
		SVExtentLineStruct l_svLine;

		if( S_OK == GetEdgeB()->GetSelectedEdgeOverlay( l_svLine ) )
		{
			rAnalyzerExtents.TranslateFromOutputSpace( l_svLine, l_svLine );

			p_rsvMiltiLine.m_svLineArray.push_back( l_svLine );
		}
	}

	return S_OK;
}

HRESULT SVLinearAnalyzerClass::updateImageExtent()
{
	HRESULT result{S_OK};

	long Width( 0 );
	long Height( 0 );
	long Angle( 0 );
	bool bUseRotation( false );

	m_imageExtent.Initialize();

	SvTo::SVToolClass* pTool = dynamic_cast<SvTo::SVToolClass*> (GetTool());
	if( nullptr == pTool || 
	    ( S_OK != GetInputUseRotationAngle( bUseRotation ) ) ||
	    ( S_OK != GetInputProfileOrientation( Angle ) ) ||
			S_OK != pTool->GetImageExtent().GetExtentProperty( SvPb::SVExtentPropertyOutputWidth, Width ) ||
			S_OK != pTool->GetImageExtent().GetExtentProperty( SvPb::SVExtentPropertyOutputHeight, Height ) )
	{
		result = S_FALSE;
	}

	if( S_OK == result )
	{
		m_imageExtent.SetTranslation(SvPb::SVExtentTranslationLinear );

		SVPoint<double> position;
		if( !bUseRotation )
		{
			if( 90 == Angle )
			{
				position.m_x = Width - 1;
				std::swap( Width, Height );
			}
			else if( 0 != Angle )
			{
				return S_FALSE;
			}
		}
		m_imageExtent.SetExtentProperty( SvPb::SVExtentPropertyRotationAngle, static_cast<double> (Angle) );
		m_imageExtent.SetExtentProperty( SvPb::SVExtentPropertyPositionPoint, position );
		m_imageExtent.SetExtentProperty( SvPb::SVExtentPropertyWidth, static_cast<double> (Width) );
		m_imageExtent.SetExtentProperty( SvPb::SVExtentPropertyHeight, static_cast<double> (Height) );

		result = m_imageExtent.UpdateData();
	}

	return result;
}

void SVLinearAnalyzerClass::addParameterForMonitorList(SvStl::MessageContainerVector& rMessages, std::back_insert_iterator<SvOi::ParametersForML> inserter) const
{
	std::vector<std::string> nameList = getParameterNamesForML();

	bool isNoError = true;
	for (auto name : nameList)
	{
		std::string fullName = GetCompleteName() + "." + name;
		isNoError &= setParameterToList(fullName, inserter);
	}

	if (!isNoError)
	{
		SvDef::StringVector msgData;
		msgData.push_back("");
		for (auto name : nameList)
		{
			msgData[0] += "\n" + name;
		}
		SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_SetResultParameterToMonitorListFailed, msgData, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
		rMessages.push_back(Msg);
	}
}

SvOp::SVLinearEdgeProcessingClass *SVLinearAnalyzerClass::GetEdgeA()
{
	SvDef::SVObjectTypeInfoStruct  info {SvPb::SVLinearEdgeProcessingObjectType, SvPb::SVLinearEdgeAProcessingObjectType};
	SvOp::SVLinearEdgeProcessingClass* pEdge = dynamic_cast<SvOp::SVLinearEdgeProcessingClass *>(getFirstObject(info));
	return pEdge;
}

SvOp::SVLinearEdgeProcessingClass *SVLinearAnalyzerClass::GetEdgeB()
{

	SvDef::SVObjectTypeInfoStruct  info {SvPb::SVLinearEdgeProcessingObjectType, SvPb::SVLinearEdgeBProcessingObjectType};;
	SvOp::SVLinearEdgeProcessingClass* pEdge = dynamic_cast<SvOp::SVLinearEdgeProcessingClass *>(getFirstObject(info));
	return pEdge;
}

HRESULT SVLinearAnalyzerClass::GetPixelDepth()
{
	HRESULT l_hrOk = S_FALSE;

	SvIe::SVImageClass* pInputImage = SvOl::getInput<SvIe::SVImageClass>(m_InputImageObjectInfo);
	if(nullptr != pInputImage)
	{
		SVImageInfoClass ImageInfo = pInputImage->GetImageInfo();

		l_hrOk = ImageInfo.GetImageProperty( SvDef::SVImagePropertyEnum::SVImagePropertyPixelDepth, m_lPixelDepth );
	}

	return l_hrOk;
}

HRESULT SVLinearAnalyzerClass::GetInputProfileOrientation(long& rProfileOrientation)
{
	HRESULT Result(E_FAIL);
	if( m_InputProfileOrientation.IsConnected() && nullptr != m_InputProfileOrientation.GetInputObjectInfo().getObject() )
	{
		double Value(0.0);
		Result = m_InputProfileOrientation.GetInputObjectInfo().getObject()->getValue(Value);
		rProfileOrientation = static_cast<long> (Value);
	}

	return Result;
}

HRESULT SVLinearAnalyzerClass::GetInputUseRotationAngle(bool& rUseRotationAngle)
{
	HRESULT Result(E_FAIL);
	if( m_InputUseRotationAngle.IsConnected() && nullptr != m_InputUseRotationAngle.GetInputObjectInfo().getObject())
	{
		double Value(0.0);
		Result = m_InputUseRotationAngle.GetInputObjectInfo().getObject()->getValue(Value);
		rUseRotationAngle = 0.0 < Value ? true : false;
	}

	return Result;
}

HRESULT SVLinearAnalyzerClass::onCollectOverlays(SvIe::SVImageClass* pImage,SVExtentMultiLineStructVector& rMultiLineArray)
{
	HRESULT l_hrRet = S_OK;
	
	SVExtentMultiLineStruct l_svMultiLine;

	SvTo::SVToolClass* pTool = dynamic_cast<SvTo::SVToolClass*>(GetTool());
	bool bOk = nullptr != pTool;
	const SVImageExtentClass& rToolExtents = bOk ? pTool->GetImageExtent() : SVImageExtentClass{};
	const SVImageExtentClass& rAnalyzerExtents = GetImageExtent();
	
	if (rAnalyzerExtents.hasFigure())
	{
		COLORREF l_Color = 0;

		SVExtentFigureStruct figure = rAnalyzerExtents.GetFigure();

		m_statusColor.GetValue( l_Color );

		l_svMultiLine.m_Color = l_Color;

		rToolExtents.TranslateFromOutputSpace(figure,figure);
		l_svMultiLine.AssignExtentFigure( figure, l_Color );

		l_svMultiLine.m_csString = "";

		UpdateOverlayIDs( l_svMultiLine );

		rMultiLineArray.push_back( l_svMultiLine );
	
		l_svMultiLine.Initialize();

		BOOL l_bShow = false;
		if ( nullptr != GetEdgeA() )
		{ 
			if( nullptr != m_svShowAllEdgeAOverlays.GetParent() && 
				( S_OK == m_svShowAllEdgeAOverlays.GetValue(l_bShow) ) && l_bShow )
			{
				SVExtentLineStruct l_svLine;

				l_svLine.m_dwColor = l_Color;

				if ( S_OK == GetEdgeA()->GetHistogramOverlay(l_svLine) )
				{
					rAnalyzerExtents.TranslateFromOutputSpace(l_svLine,l_svLine);
					rToolExtents.TranslateFromOutputSpace(l_svLine,l_svLine);
					l_svMultiLine.m_svLineArray.push_back(l_svLine);
					rMultiLineArray.push_back(l_svMultiLine);
					l_svMultiLine.Initialize();
				}
				
				if ( S_OK == GetEdgeA()->GetThresholdBarsOverlay(l_svMultiLine) )
				{
					rAnalyzerExtents.TranslateFromOutputSpace(l_svMultiLine,l_svMultiLine);
					rToolExtents.TranslateFromOutputSpace(l_svMultiLine,l_svMultiLine);
					rMultiLineArray.push_back(l_svMultiLine);
					l_svMultiLine.Initialize();
				}
				if ( S_OK == GetEdgeA()->GetEdgesOverlay(l_svMultiLine) )
				{
					rAnalyzerExtents.TranslateFromOutputSpace(l_svMultiLine,l_svMultiLine);
					rToolExtents.TranslateFromOutputSpace(l_svMultiLine,l_svMultiLine);
					rMultiLineArray.push_back(l_svMultiLine);
					l_svMultiLine.Initialize();
				}

			}
		}
		if ( nullptr != GetEdgeB() )
		{ 
			if( nullptr != m_svShowAllEdgeBOverlays.GetParent() && 
				( S_OK == m_svShowAllEdgeBOverlays.GetValue(l_bShow) ) && l_bShow )
			{
				SVExtentLineStruct svLine;

				svLine.m_dwColor = l_Color;

				if ( S_OK == GetEdgeB()->GetHistogramOverlay(svLine) )
				{
					rAnalyzerExtents.TranslateFromOutputSpace(svLine,svLine);
					rToolExtents.TranslateFromOutputSpace(svLine,svLine);
					l_svMultiLine.m_svLineArray.push_back(svLine);
					rMultiLineArray.push_back(l_svMultiLine);
					l_svMultiLine.Initialize();
				}
				
				if ( S_OK == GetEdgeB()->GetThresholdBarsOverlay(l_svMultiLine) )
				{
					rAnalyzerExtents.TranslateFromOutputSpace(l_svMultiLine,l_svMultiLine);
					rToolExtents.TranslateFromOutputSpace(l_svMultiLine,l_svMultiLine);
					rMultiLineArray.push_back(l_svMultiLine);
					l_svMultiLine.Initialize();
				}
				if ( S_OK == GetEdgeB()->GetEdgesOverlay(l_svMultiLine) )
				{
					rAnalyzerExtents.TranslateFromOutputSpace(l_svMultiLine,l_svMultiLine);
					rToolExtents.TranslateFromOutputSpace(l_svMultiLine,l_svMultiLine);
					rMultiLineArray.push_back(l_svMultiLine);
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

	if ( bOk )
	{
		SVExtentMultiLineStruct l_svMultiLine;

		if ( S_OK == GetSelectedEdgeOverlays( l_svMultiLine ) )
		{
			rToolExtents.TranslateFromOutputSpace( l_svMultiLine, l_svMultiLine );

			m_statusColor.GetValue(l_svMultiLine.m_Color);

			UpdateOverlayIDs( l_svMultiLine );

			rMultiLineArray.push_back( l_svMultiLine );
		}
	}
	return l_hrRet;
}

void SVLinearAnalyzerClass::addOverlayGroups(const SvIe::SVImageClass* pImage, SvPb::Overlay& rOverlay) const
{
	const SVImageExtentClass& rAnalyzerExtents = GetImageExtent();

	if (rAnalyzerExtents.hasFigure())
	{
		SvTo::SVToolClass* pTool = dynamic_cast<SvTo::SVToolClass*>(GetTool());
		const SVImageExtentClass& rToolExtents = (nullptr != pTool) ? pTool->GetImageExtent() : SVImageExtentClass {};
		SVExtentFigureStruct figure = rAnalyzerExtents.GetFigure();
		rToolExtents.TranslateFromOutputSpace(figure, figure);
		if (SvDef::SVExtentShapeArrow == figure.m_eShape)
		{
			auto* pGroup = rOverlay.add_shapegroups();
			pGroup->set_name("LAnalyzer-Arrow");
			auto* pShape = pGroup->add_shapes();
			auto* pArrow = pShape->mutable_arrow();
			pArrow->mutable_x1()->set_value(figure.m_svCenterLeft.m_x);
			pArrow->mutable_x2()->set_value(figure.m_svCenterRight.m_x);
			pArrow->mutable_y1()->set_value(figure.m_svCenterLeft.m_y);
			pArrow->mutable_y2()->set_value(figure.m_svCenterRight.m_y);
		}
	}
	//@TODO[MZA][8.20][01.08.2019] add graph overlays
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

bool SVLinearAnalyzerClass::setParameterToList(const std::string& rName, std::back_insert_iterator<SvOi::ParametersForML> inserter) const
{
	SVObjectClass* pObject = nullptr;
	SVObjectManagerClass::Instance().GetObjectByDottedName(rName, pObject);
	//Value objects need to have the SV_VIEWABLE attribute to insert it to the monitor list
	if (nullptr != pObject && 0 != (pObject->ObjectAttributesAllowed() &  SvPb::viewable))
	{
		inserter = SvOi::ParameterPairForML(pObject->GetCompleteName(), pObject->GetUniqueObjectID());
		return true;
	}
	else
	{
		return false;
	}
}

std::vector<std::string> SVLinearAnalyzerClass::getParameterNamesForML() const
{
	return {};
}

} //namespace SvAo
