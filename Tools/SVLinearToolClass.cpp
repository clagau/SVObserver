// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVLinearToolClass
// * .File Name       : $Workfile:   SVLinearToolClass.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.4  $
// * .Check In Date   : $Date:   15 May 2014 13:12:48  $
// ******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVLinearToolClass.h"
#include "AnalyzerOperators/Analyzer.h"
#include "Operators/SVConditional.h"
#include "Operators/SVLinearImageOperatorList.h"
#include "Operators/SVThresholdClass.h"
#include "Operators/SVUserMaskOperatorClass.h"
#include "Operators/ToolSizeAdjustTask.h"
#include "Definitions/GlobalConst.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "SVProtoBuf/ConverterHelper.h"
#include "SVProtoBuf/Overlay.h"
#pragma endregion Includes

namespace SvTo
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS( SVLinearToolClass, SvPb::LinearToolClassId );

#pragma region Constructor
SVLinearToolClass::SVLinearToolClass( SVObjectClass* POwner, int StringResourceID )
:SVToolClass(ToolExtType::All, POwner, StringResourceID )
{
	init();
}

SVLinearToolClass::~SVLinearToolClass()
{
	SVLinearToolClass::CloseObject();
}
#pragma endregion Constructor

#pragma region Public Methods
bool SVLinearToolClass::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	bool bOk = SVToolClass::CreateObject(rCreateStructure);

	BOOL l_bValue = false;

	if( S_OK == m_voUseProfileRotation.GetValue( l_bValue ) )
	{
		if( l_bValue )
		{
			m_toolExtent.SetTranslation(SvPb::SVExtentTranslationProfile);
		}
		else
		{
			m_toolExtent.SetTranslation(SvPb::SVExtentTranslationProfileShift);
		}
	}
	UINT Attributes = SvPb::remotelySetable | SvPb::setableOnline | SvPb::audittrail;
	m_svRotationAngle.SetObjectAttributesAllowed( Attributes, SvOi::SetAttributeType::AddAttribute );
	m_svRotationPointX.SetObjectAttributesAllowed( Attributes, SvOi::SetAttributeType::AddAttribute );
	m_svRotationPointY.SetObjectAttributesAllowed( Attributes, SvOi::SetAttributeType::AddAttribute );
	m_voProfileOrientation.SetObjectAttributesAllowed( Attributes, SvOi::SetAttributeType::AddAttribute );
	m_voUseProfileRotation.SetObjectAttributesAllowed( Attributes, SvOi::SetAttributeType::AddAttribute );

	Attributes = SvPb::remotelySetable | SvPb::setableOnline;
	m_SourceImageNames.setSaveValueFlag(false);
	m_SourceImageNames.SetObjectAttributesAllowed( Attributes, SvOi::SetAttributeType::RemoveAttribute );
	assert(m_pEmbeddedExtents);
	if (m_pEmbeddedExtents)
	{
		m_pEmbeddedExtents->m_ExtentLeft.SetObjectAttributesAllowed(Attributes, SvOi::SetAttributeType::RemoveAttribute);
		m_pEmbeddedExtents->m_ExtentTop.SetObjectAttributesAllowed(Attributes, SvOi::SetAttributeType::RemoveAttribute);
	}

	if(bOk)
	{
		bOk  = (S_OK == SvOp::ToolSizeAdjustTask::EnsureInFriendList(this, true, true, true));
	}

	m_isCreated = bOk;

	return bOk;
}

bool SVLinearToolClass::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result( true );
	BOOL UseProfileRotation( false );

	HRESULT hrOk = m_voUseProfileRotation.GetValue( UseProfileRotation );
	if( S_OK == hrOk )
	{
		if( UseProfileRotation )
		{
			if( m_toolExtent.GetTranslation() != SvPb::SVExtentTranslationProfile )
			{
				m_voProfileOrientation.setValue( std::string( _T("Horizontal") ) );

				hrOk = m_toolExtent.SetLinearTranslation(SvPb::SVExtentTranslationProfile);
			}
		}
		else
		{
			if( m_toolExtent.GetTranslation() != SvPb::SVExtentTranslationProfileShift )
			{
				m_svRotationAngle.SetValue( 0.0 );

				hrOk = m_toolExtent.SetLinearTranslation(SvPb::SVExtentTranslationProfileShift);
			}
		}

		SvIe::SVExtentPropertyInfoStruct info;

		if( S_OK == m_toolExtent.GetExtentPropertyInfo( SvPb::SVExtentPropertyRotationAngle, info ) )
		{
			info.bHidden = ! UseProfileRotation;

			if( S_OK != m_toolExtent.SetExtentPropertyInfo( SvPb::SVExtentPropertyRotationAngle, info ) )
			{
				hrOk = S_FALSE;
			}
		}
		else
		{
			hrOk = S_FALSE;
		}
	}

	if ( S_OK != hrOk )
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_UpdateLinearToolDataFailed, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId() );
			pErrorMessages->push_back(Msg);
		}
	}

	Result = SVToolClass::ResetObject(pErrorMessages) && Result;

	if ( Result )
	{
		//This update call is required for the image extents which may have changed above
		updateImageExtent(true);

		if(GetImageExtent().hasFigure() && m_pEmbeddedExtents)
		{
			SVExtentFigureStruct rFigure = GetImageExtent().GetFigure();

			if(m_pEmbeddedExtents->m_ExtentLeft.ObjectAttributesAllowed() != SvPb::noAttributes )
			{
				m_pEmbeddedExtents->m_ExtentLeft.SetValue(rFigure.m_svTopLeft.m_x);
			}
				
			if(m_pEmbeddedExtents->m_ExtentTop.ObjectAttributesAllowed() != SvPb::noAttributes )
			{
				m_pEmbeddedExtents->m_ExtentTop.SetValue(rFigure.m_svTopLeft.m_y);
			}
		}
		else
		{
			Result = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_GetImageExtentFailed, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId() );
				pErrorMessages->push_back(Msg);
			}
		}
	}

	return Result;
}

HRESULT SVLinearToolClass::SetImageExtentToParent()
{
	SVImageExtentClass NewExtent;
	HRESULT l_hrOk = m_toolExtent.UpdateExtentToParentExtents( NewExtent );

	if( S_OK == l_hrOk )
	{
		l_hrOk = SVToolClass::SetImageExtent( NewExtent );
	}
	return l_hrOk;
}

SVToolClass* SVLinearToolClass::GetObjectAtPoint(const SVPoint<double>& rPoint)
{
	if (SvPb::SVExtentLocationPropertyUnknown != GetImageExtent().GetLocationPropertyAt(rPoint))
	{
		return this;
	}

	return nullptr;
}

bool SVLinearToolClass::DoesObjectHaveExtents() const
{
	return true;
}

std::vector<std::string> SVLinearToolClass::getToolAdjustNameList() const
{
	constexpr std::array<LPCTSTR, 10> cToolAdjustNameList
	{
		_T("Image Source"),
		_T("Size and Position"),
		_T("Filter"),
		_T("Threshold"),
		_T("Analyzer"),
		_T("Special"),
		_T("Profile Values Ranges"),
		_T("Conditional"),
		_T("General"),
		_T("Comment"),
	};
	return { cToolAdjustNameList.begin(), cToolAdjustNameList.end() };
}

bool SVLinearToolClass::GetRotation() const
{
	BOOL bVal = false;
	
	m_voUseProfileRotation.GetValue(bVal);

	return (TRUE == bVal);
}


SvPb::EAutoSize SVLinearToolClass::GetAutoSizeEnabled() const
{
	if (GetRotation())
	{
		return SvPb::EnableNone;
	}
	else
	{
		return SvPb::EnableSizeAndPosition;
	}
}

SvVol::SVStringValueObjectClass* SVLinearToolClass::GetInputImageNames()
{
	return &m_SourceImageNames;
}

SvOi::ParametersForML SVLinearToolClass::getParameterForMonitorList(SvStl::MessageContainerVector& rMessages) const
{
	bool isNoError = true;
	bool isAuxNoError = true;
	SvOi::ParametersForML retList;
	retList.push_back(SvOi::ParameterPairForML(m_statusColor.GetCompleteName(), m_statusColor.getObjectId()));
	if (nullptr != m_pToolConditional)
	{
		retList.push_back(m_pToolConditional->getResultData());
	}
	else
	{
		isNoError = false;
	}
	isNoError = addEntryToMonitorList(retList, SvPb::ExtentWidthEId) && isNoError;
	isNoError = addEntryToMonitorList(retList, SvPb::ExtentHeightEId) && isNoError;
	isAuxNoError = addEntryToMonitorList(retList, SvPb::AuxiliarySourceXEId);
	isAuxNoError = addEntryToMonitorList(retList, SvPb::AuxiliarySourceYEId) && isAuxNoError;

	std::vector<SvOl::InputObject*> inputList;
	getInputs(std::back_inserter(inputList));
	auto iter = std::find_if(inputList.begin(), inputList.end(), [](const auto* pInput) {
		return (nullptr != pInput && SvPb::SVImageObjectType == pInput->GetInputObjectInfo().m_ObjectTypeInfo.m_ObjectType && SvPb::AuxImageInputEId != pInput->GetEmbeddedID());
		});
	if (inputList.end() != iter && (*iter)->IsConnected())
	{
		const SvIe::SVImageClass* pSourceImage = dynamic_cast<SvIe::SVImageClass*>((*iter)->GetInputObjectInfo().getFinalObject());
		if (nullptr != pSourceImage)
		{
			auto* pSourceTool = dynamic_cast<SVToolClass*>(pSourceImage->GetAncestor(SvPb::SVToolObjectType));
			if (nullptr != pSourceTool)
			{
				isAuxNoError = pSourceTool->addEntryToMonitorList(retList, SvPb::AuxiliarySourceXEId) && isAuxNoError;
				isAuxNoError = pSourceTool->addEntryToMonitorList(retList, SvPb::AuxiliarySourceYEId) && isAuxNoError;
			}
		}
	}

	if (!isAuxNoError)
	{
		SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_SetAuxiliaryParameterToMonitorListFailed, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
		rMessages.push_back(Msg);
	}
	else if (!isNoError)
	{
		SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_SetParameterToMonitorListFailed, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
		rMessages.push_back(Msg);
	}

	SvAo::Analyzer* pCurrentAnalyzer = dynamic_cast<SvAo::Analyzer *>(getFirstObject(SvDef::SVObjectTypeInfoStruct(SvPb::SVAnalyzerObjectType)));
	if (nullptr != pCurrentAnalyzer)
	{
		pCurrentAnalyzer->addParameterForMonitorList(rMessages, std::back_inserter(retList));
	}

	return retList;
}

void SVLinearToolClass::addOverlays(const SvIe::SVImageClass* pImage, SvPb::OverlayDesc& rOverlay) const
{
	BOOL isRotation = false;
	m_voUseProfileRotation.GetValue(isRotation);

	auto* pOverlay = rOverlay.add_overlays();
	pOverlay->set_name(GetName());
	pOverlay->set_objectid(getObjectId());
	pOverlay->set_displaybounding(true);
	auto* pBoundingBox = pOverlay->mutable_boundingshape();
	auto* pRect = pBoundingBox->mutable_rect();
	if (m_pEmbeddedExtents)
	{
		SvPb::setValueObject(m_pEmbeddedExtents->m_ExtentLeft, *pRect->mutable_x());
		SvPb::setValueObject(m_pEmbeddedExtents->m_ExtentTop, *pRect->mutable_y());
		SvPb::setValueObject(m_pEmbeddedExtents->m_ExtentWidth, *pRect->mutable_w());
		SvPb::setValueObject(m_pEmbeddedExtents->m_ExtentHeight, *pRect->mutable_h());
	}
	setStateValueToOverlay(*pOverlay);
	if (isRotation)
	{
		SvPb::setValueObject(m_svRotationAngle, *pRect->mutable_angle());
	}
	collectOverlays(pImage, *pOverlay);
}

void SVLinearToolClass::overwriteInputSource(SvOi::SVImageBufferHandlePtr imageHandlePtr)
{
	SvDef::SVObjectTypeInfoStruct info{ SvPb::SVUnaryImageOperatorListObjectType, SvPb::SVLinearImageOperatorListObjectType };
	SvOp::SVStdImageOperatorListClass* pObject = dynamic_cast<SvOp::SVStdImageOperatorListClass*>(getFirstObject(info));
	if (nullptr != pObject)
	{
		pObject->setTempSourceImageHandle(imageHandlePtr);
	}
}

void SVLinearToolClass::getToolsWithReplaceableSourceImage(SvPb::GetToolsWithReplaceableSourceImageResponse& rResponse) const
{
	auto* rData = rResponse.add_list();
	rData->set_objectname(GetObjectNameBeforeObjectType(SvPb::SVToolSetObjectType));
	rData->set_objectid(getObjectId());
}
#pragma endregion Public Methods

#pragma region Protected Methods
#pragma endregion Protected Methods

#pragma region Private Methods
void SVLinearToolClass::init()
{
	m_canResizeToParent = true;
	// Set up your type...
	m_ObjectTypeInfo.m_ObjectType = SvPb::SVToolObjectType;
	m_ObjectTypeInfo.m_SubType    = SvPb::SVLinearToolObjectType;

	//Special type names for extents
	m_svRotationPointX.SetTypeName( _T("Extent X") );
	m_svRotationPointY.SetTypeName( _T("Extent Y") );
	m_svRotationAngle.SetTypeName( _T("Extent Angle") );
	m_voUseProfileRotation.SetTypeName( _T("Extent Angle") );
	// Register Embedded Objects
	RegisterEmbeddedObject( &m_svRotationAngle, SvPb::RotationAngleEId, IDS_OBJECTNAME_ROTATION_ANGLE, false, SvOi::SVResetItemTool );
	RegisterEmbeddedObject( &m_svRotationPointX, SvPb::RotationPointXEId, IDS_OBJECTNAME_ROTATION_POINT_X, false, SvOi::SVResetItemTool );
	RegisterEmbeddedObject( &m_svRotationPointY, SvPb::RotationPointYEId, IDS_OBJECTNAME_ROTATION_POINT_Y, false, SvOi::SVResetItemTool );
	RegisterEmbeddedObject( &m_voProfileOrientation, SvPb::ProfileOrientationEId,IDS_OBJECTNAME_LINEARTOOL_ORIENTATION, false, SvOi::SVResetItemTool );
	RegisterEmbeddedObject( &m_voUseProfileRotation, SvPb::LinearToolUseRotationEId, IDS_OBJECTNAME_LINEAR_TOOL_USE_ROTATION, false, SvOi::SVResetItemTool );
	RegisterEmbeddedObject( &m_SourceImageNames, SvPb::SourceImageNamesEId, IDS_OBJECTNAME_SOURCE_IMAGE_NAMES, false, SvOi::SVResetItemTool );

	// Set Embedded defaults
	m_svRotationAngle.SetDefaultValue( 0.0, true );
	m_svRotationPointX.SetDefaultValue( SvDef::cDefaultWindowToolLeft, true);
	m_svRotationPointY.SetDefaultValue( ( SvDef::cDefaultWindowToolTop + ( SvDef::cDefaultWindowToolHeight / 2 ) ), true);
	m_voProfileOrientation.SetEnumTypes(cOrientationEnums);
	m_voProfileOrientation.SetDefaultValue( "Horizontal", true);
	m_voUseProfileRotation.SetDefaultValue( BOOL(true), true);

	m_toolExtent.SetTranslation(SvPb::SVExtentTranslationProfile );
	m_toolExtent.SetExtentObject( SvPb::SVExtentPropertyPositionPointX, &m_svRotationPointX );
	m_toolExtent.SetExtentObject( SvPb::SVExtentPropertyPositionPointY, &m_svRotationPointY );
	m_toolExtent.SetExtentObject( SvPb::SVExtentPropertyRotationAngle, &m_svRotationAngle );

	// Populate the available analyzer list
	SvIe::SVClassInfoStruct analyzerClassInfo;

	// Add the Line Pixel Count Analyzer...
	analyzerClassInfo.m_ObjectTypeInfo.m_ObjectType = SvPb::SVAnalyzerObjectType;
	analyzerClassInfo.m_ObjectTypeInfo.m_SubType	= SvPb::SVLinearPixelCountingAnalyzerObjectType;
	analyzerClassInfo.m_ClassId = SvPb::LinearPixelCountingLineAnalyzerClassId;
	analyzerClassInfo.m_ClassName = SvUl::LoadStdString( IDS_CLASSNAME_LINEARPIXELCOUNTINGANALYZER );
	m_availableChildren.push_back( analyzerClassInfo );

	// Add the Line Edge Count Analyzer...
	analyzerClassInfo.m_ObjectTypeInfo.m_ObjectType = SvPb::SVAnalyzerObjectType;
	analyzerClassInfo.m_ObjectTypeInfo.m_SubType	= SvPb::SVLinearEdgeCountingAnalyzerObjectType;
	analyzerClassInfo.m_ClassId = SvPb::LinearEdgeCountingLineAnalyzerClassId;
	analyzerClassInfo.m_ClassName = SvUl::LoadStdString( IDS_CLASSNAME_LINEAREDGECOUNTINGANALYZER );
	m_availableChildren.push_back(analyzerClassInfo);
	
	// Add the Line Edge Position Analyzer...
	analyzerClassInfo.m_ObjectTypeInfo.m_ObjectType = SvPb::SVAnalyzerObjectType;
	analyzerClassInfo.m_ObjectTypeInfo.m_SubType	= SvPb::SVLinearEdgePositionAnalyzerObjectType;
	analyzerClassInfo.m_ClassId = SvPb::LinearEdgePositionLineAnalyzerClassId;
	analyzerClassInfo.m_ClassName = SvUl::LoadStdString( IDS_CLASSNAME_LINEAREDGEPOSITIONANALYZER );
	m_availableChildren.push_back(analyzerClassInfo);

	// Add the Line Linear Measurement Analyzer...
	analyzerClassInfo.m_ObjectTypeInfo.m_ObjectType = SvPb::SVAnalyzerObjectType;
	analyzerClassInfo.m_ObjectTypeInfo.m_SubType	= SvPb::SVLinearMeasurementAnalyzerObjectType;
	analyzerClassInfo.m_ClassId = SvPb::LinearMeasurementAnalyzerClassId;
	analyzerClassInfo.m_ClassName = SvUl::LoadStdString( IDS_CLASSNAME_SVLINEARMEASUREMENTLINEANALYZER );
	m_availableChildren.push_back(analyzerClassInfo);

	// Add the Line Maximum Foreground Object Analyzer...
	analyzerClassInfo.m_ObjectTypeInfo.m_ObjectType = SvPb::SVAnalyzerObjectType;
	analyzerClassInfo.m_ObjectTypeInfo.m_SubType	= SvPb::SVLinearMaximumForegroundObjectAnalyzerObjectType;
	analyzerClassInfo.m_ClassId = SvPb::LinearMaximumForegroundObjectLineAnalyzerClassId;
	analyzerClassInfo.m_ClassName = SvUl::LoadStdString( IDS_CLASSNAME_LINEARMAXIMUMFOREGROUNDOBJECTANALYZER );
	m_availableChildren.push_back(analyzerClassInfo);

	// Add the Line Maximum Background Object Analyzer...
	analyzerClassInfo.m_ObjectTypeInfo.m_ObjectType = SvPb::SVAnalyzerObjectType;
	analyzerClassInfo.m_ObjectTypeInfo.m_SubType	= SvPb::SVLinearMaximumBackgroundObjectAnalyzerObjectType;
	analyzerClassInfo.m_ClassId = SvPb::LinearMaximumBackgroundObjectLineAnalyzerClassId;
	analyzerClassInfo.m_ClassName = SvUl::LoadStdString( IDS_CLASSNAME_LINEARMAXIMUMBACKGROUNDOBJECTANALYZER );
	m_availableChildren.push_back(analyzerClassInfo);

	// Add the Line Maximum Object Analyzer...
	analyzerClassInfo.m_ObjectTypeInfo.m_ObjectType = SvPb::SVAnalyzerObjectType;
	analyzerClassInfo.m_ObjectTypeInfo.m_SubType	= SvPb::SVLinearMaximumObjectAnalyzerObjectType;
	analyzerClassInfo.m_ClassId = SvPb::LinearMaximumObjectLineAnalyzerClassId;
	analyzerClassInfo.m_ClassName = SvUl::LoadStdString( IDS_CLASSNAME_LINEARMAXIMUMOBJECTANALYZER );
	m_availableChildren.push_back(analyzerClassInfo);

	// Build an operator list...

	// ...use in-place image operator list, because we already have an output image!
	SvOp::SVUnaryImageOperatorList* pOperatorList = new SvOp::SVLinearImageOperatorList;

	// Operator list defaults:
	if(nullptr != pOperatorList)
	{
		// Requires a SVThresholdClass object
		pOperatorList->Add( new SvOp::SVThresholdClass( pOperatorList ) );

		// and Requires a SVUsermaskOperatorClass object
		pOperatorList->Add( new SvOp::SVUserMaskOperatorClass( pOperatorList ) );

		// Add the UnaryImageOperatorList to the tool's list
		Add( pOperatorList );
	}
	SvOp::ToolSizeAdjustTask::AddToFriendlist(this, true, true, true);
}
#pragma endregion Private Methods

} //namespace SvTo
