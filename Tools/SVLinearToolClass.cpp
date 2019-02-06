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
#include "SVObjectLibrary/SVClsIds.h"
#include "AnalyzerOperators/SVAnalyzer.h"
#include "Operators/SVConditional.h"
#include "Operators/SVLinearImageOperatorList.h"
#include "Operators/SVThresholdClass.h"
#include "Operators/SVUserMaskOperatorClass.h"
#include "Operators/ToolSizeAdjustTask.h"
#include "Definitions/GlobalConst.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

namespace SvTo
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS( SVLinearToolClass, SVLinearToolClassGuid );

#pragma region Constructor
SVLinearToolClass::SVLinearToolClass( SVObjectClass* POwner, int StringResourceID )
: SVToolClass( POwner, StringResourceID )
{
	init();
}

SVLinearToolClass::~SVLinearToolClass()
{
	CloseObject();
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
			m_toolExtent.SetTranslation(SvDef::SVExtentTranslationProfile);
		}
		else
		{
			m_toolExtent.SetTranslation(SvDef::SVExtentTranslationProfileShift);
		}
	}
	UINT Attributes = SvPb::remotelySetable | SvPb::setableOnline | SvPb::printable;
	m_svRotationAngle.SetObjectAttributesAllowed( Attributes, SvOi::SetAttributeType::AddAttribute );
	m_svRotationPointX.SetObjectAttributesAllowed( Attributes, SvOi::SetAttributeType::AddAttribute );
	m_svRotationPointY.SetObjectAttributesAllowed( Attributes, SvOi::SetAttributeType::AddAttribute );
	m_voProfileOrientation.SetObjectAttributesAllowed( Attributes, SvOi::SetAttributeType::AddAttribute );
	m_voUseProfileRotation.SetObjectAttributesAllowed( Attributes, SvOi::SetAttributeType::AddAttribute );

	Attributes = SvPb::remotelySetable | SvPb::setableOnline;
	m_SourceImageNames.setSaveValueFlag(false);
	m_SourceImageNames.SetObjectAttributesAllowed( Attributes, SvOi::SetAttributeType::RemoveAttribute );
	m_ExtentLeft.SetObjectAttributesAllowed( Attributes, SvOi::SetAttributeType::RemoveAttribute );
	m_ExtentTop.SetObjectAttributesAllowed( Attributes, SvOi::SetAttributeType::RemoveAttribute );


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
			if( m_toolExtent.GetTranslation() != SvDef::SVExtentTranslationProfile )
			{
				m_voProfileOrientation.setValue( std::string( _T("Horizontal") ) );

				hrOk = m_toolExtent.SetLinearTranslation(SvDef::SVExtentTranslationProfile);
			}
		}
		else
		{
			if( m_toolExtent.GetTranslation() != SvDef::SVExtentTranslationProfileShift )
			{
				m_svRotationAngle.SetValue( 0.0 );

				hrOk = m_toolExtent.SetLinearTranslation(SvDef::SVExtentTranslationProfileShift);
			}
		}

		SvIe::SVExtentPropertyInfoStruct info;

		if( S_OK == m_toolExtent.GetExtentPropertyInfo( SvDef::SVExtentPropertyRotationAngle, info ) )
		{
			info.bHidden = ! UseProfileRotation;

			if( S_OK != m_toolExtent.SetExtentPropertyInfo( SvDef::SVExtentPropertyRotationAngle, info ) )
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
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_UpdateLinearToolDataFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
			pErrorMessages->push_back(Msg);
		}
	}

	Result = SVToolClass::ResetObject(pErrorMessages) && Result;

	if ( Result )
	{
		//This update call is required for the image extents which may have changed above
		updateImageExtent();

		if(GetImageExtent().hasFigure())
		{
			SVExtentFigureStruct rFigure = GetImageExtent().GetFigure();

			if( m_ExtentLeft.ObjectAttributesAllowed() != SvPb::noAttributes )
			{
				m_ExtentLeft.SetValue(rFigure.m_svTopLeft.m_x);
			}
				
			if( m_ExtentTop.ObjectAttributesAllowed() != SvPb::noAttributes )
			{
				m_ExtentTop.SetValue(rFigure.m_svTopLeft.m_y);
			}
		}
		else
		{
			Result = false;
			if (nullptr != pErrorMessages)
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_GetImageExtentFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
				pErrorMessages->push_back(Msg);
			}
		}
	}

	return Result;
}

HRESULT SVLinearToolClass::SetImageExtentToParent()
{
	HRESULT l_hrOk = S_OK;
	SVImageExtentClass NewExtent;

	l_hrOk = m_toolExtent.UpdateExtentToParentExtents( NewExtent );

	if( S_OK == l_hrOk )
	{
		l_hrOk = SVToolClass::SetImageExtent( NewExtent );
	}
	return l_hrOk;
}

SvIe::SVTaskObjectClass* SVLinearToolClass::GetObjectAtPoint(const SVPoint<double>& rPoint)
{
	SVTaskObjectClass *pObject {nullptr};

	if (SvDef::SVExtentLocationPropertyUnknown != GetImageExtent().GetLocationPropertyAt(rPoint))
	{
		pObject = this;
	}

	return pObject;
}

bool SVLinearToolClass::DoesObjectHaveExtents() const
{
	return true;
}

bool SVLinearToolClass::GetRotation()
{
	BOOL bVal = false;
	
	m_voUseProfileRotation.GetValue(bVal);

	return (TRUE == bVal);
}

EAutoSize SVLinearToolClass::GetAutoSizeEnabled()
{
	if (GetRotation())
	{
		return EnableNone;
	}
	else
	{
		return EnableSizeAndPosition;
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
	retList.push_back(SvOi::ParameterPairForML(m_statusColor.GetCompleteName(), m_statusColor.GetUniqueObjectID()));
	if (nullptr != m_pToolConditional)
	{
		retList.push_back(m_pToolConditional->getResultData());
	}
	else
	{
		isNoError = false;
	}
	isNoError = addEntryToMonitorList(retList, SVExtentWidthObjectGuid) && isNoError;
	isNoError = addEntryToMonitorList(retList, SVExtentHeightObjectGuid) && isNoError;
	isAuxNoError = addEntryToMonitorList(retList, SVAuxiliarySourceXObjectGuid);
	isAuxNoError = addEntryToMonitorList(retList, SVAuxiliarySourceYObjectGuid) && isAuxNoError;

	SvOl::SVInObjectInfoStruct* pImageInfo = nullptr;
	if (S_OK == FindNextInputImageInfo(pImageInfo))
	{
		if (nullptr != pImageInfo && pImageInfo->IsConnected())
		{
			const SvIe::SVImageClass* pSourceImage = dynamic_cast<SvIe::SVImageClass*> (pImageInfo->GetInputObjectInfo().getObject());
			if (nullptr != pSourceImage)
			{
				auto* pSourceTool = dynamic_cast<SVToolClass*>(pSourceImage->GetAncestor(SvPb::SVToolObjectType));
				if (nullptr != pSourceTool)
				{
					isAuxNoError = pSourceTool->addEntryToMonitorList(retList, SVAuxiliarySourceXObjectGuid) && isAuxNoError;
					isAuxNoError = pSourceTool->addEntryToMonitorList(retList, SVAuxiliarySourceYObjectGuid) && isAuxNoError;
				}
			}
		}
	}

	if (!isAuxNoError)
	{
		SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_SetAuxiliaryParameterToMonitorListFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
		rMessages.push_back(Msg);
	}
	else if (!isNoError)
	{
		SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_SetParameterToMonitorListFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
		rMessages.push_back(Msg);
	}

	SvAo::SVAnalyzerClass* pCurrentAnalyzer = dynamic_cast<SvAo::SVAnalyzerClass *>(getFirstObject(SvDef::SVObjectTypeInfoStruct(SvPb::SVAnalyzerObjectType)));
	if (nullptr != pCurrentAnalyzer)
	{
		pCurrentAnalyzer->addParameterForMonitorList(rMessages, std::back_inserter(retList));
	}

	return retList;
}
#pragma endregion Public Methods

#pragma region Protected Methods
#pragma endregion Protected Methods

#pragma region Private Methods
void SVLinearToolClass::init()
{
	m_canResizeToParent = true;
	// Set up your type...
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVToolObjectType;
	m_outObjectInfo.m_ObjectTypeInfo.SubType    = SvPb::SVLinearToolObjectType;

	//Special type names for extents
	m_svRotationPointX.SetTypeName( _T("Extent X") );
	m_svRotationPointY.SetTypeName( _T("Extent Y") );
	m_svRotationAngle.SetTypeName( _T("Extent Angle") );
	m_voUseProfileRotation.SetTypeName( _T("Extent Angle") );
	// Register Embedded Objects
	RegisterEmbeddedObject( &m_svRotationAngle, SVRotationAngleObjectGuid, IDS_OBJECTNAME_ROTATION_ANGLE, false, SvOi::SVResetItemTool );
	RegisterEmbeddedObject( &m_svRotationPointX, SVRotationPointXObjectGuid, IDS_OBJECTNAME_ROTATION_POINT_X, false, SvOi::SVResetItemTool );
	RegisterEmbeddedObject( &m_svRotationPointY, SVRotationPointYObjectGuid, IDS_OBJECTNAME_ROTATION_POINT_Y, false, SvOi::SVResetItemTool );
	RegisterEmbeddedObject( &m_voProfileOrientation, SVProfileOrientationGuid,IDS_OBJECTNAME_LINEARTOOL_ORIENTATION, false, SvOi::SVResetItemTool );
	RegisterEmbeddedObject( &m_voUseProfileRotation, SVLinearToolUseRotationGuid, IDS_OBJECTNAME_LINEAR_TOOL_USE_ROTATION, false, SvOi::SVResetItemTool );
	RegisterEmbeddedObject( &m_SourceImageNames, SVSourceImageNamesGuid, IDS_OBJECTNAME_SOURCE_IMAGE_NAMES, false, SvOi::SVResetItemTool );

	// Set Embedded defaults
	m_svRotationAngle.SetDefaultValue( 0.0, true );
	m_svRotationPointX.SetDefaultValue( SvDef::cDefaultWindowToolLeft, true);
	m_svRotationPointY.SetDefaultValue( ( SvDef::cDefaultWindowToolTop + ( SvDef::cDefaultWindowToolHeight / 2 ) ), true);
	m_voProfileOrientation.SetEnumTypes(cOrientationEnums);
	m_voProfileOrientation.SetDefaultValue( "Horizontal", true);
	m_voUseProfileRotation.SetDefaultValue( BOOL(true), true);

	m_toolExtent.SetTranslation( SvDef::SVExtentTranslationProfile );
	m_toolExtent.SetExtentObject( SvDef::SVExtentPropertyPositionPointX, &m_svRotationPointX );
	m_toolExtent.SetExtentObject( SvDef::SVExtentPropertyPositionPointY, &m_svRotationPointY );
	m_toolExtent.SetExtentObject( SvDef::SVExtentPropertyRotationAngle, &m_svRotationAngle );

	// Populate the available analyzer list
	SvIe::SVClassInfoStruct analyzerClassInfo;

	// Add the Line Pixel Count Analyzer...
	analyzerClassInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVAnalyzerObjectType;
	analyzerClassInfo.m_ObjectTypeInfo.SubType	= SvPb::SVLinearPixelCountingAnalyzerObjectType;
	analyzerClassInfo.m_ClassId = SVLinearPixelCountingLineAnalyzerClassGuid;
	analyzerClassInfo.m_ClassName = SvUl::LoadStdString( IDS_CLASSNAME_SVLINEARPIXELCOUNTINGLINEANALYZER );
	m_availableChildren.push_back( analyzerClassInfo );

	// Add the Line Edge Count Analyzer...
	analyzerClassInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVAnalyzerObjectType;
	analyzerClassInfo.m_ObjectTypeInfo.SubType	= SvPb::SVLinearEdgeCountingAnalyzerObjectType;
	analyzerClassInfo.m_ClassId = SVLinearEdgeCountingLineAnalyzerClassGuid;
	analyzerClassInfo.m_ClassName = SvUl::LoadStdString( IDS_CLASSNAME_SVLINEAREDGECOUNTINGLINEANALYZER );
	m_availableChildren.push_back(analyzerClassInfo);
	
	// Add the Line Edge Position Analyzer...
	analyzerClassInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVAnalyzerObjectType;
	analyzerClassInfo.m_ObjectTypeInfo.SubType	= SvPb::SVLinearEdgePositionAnalyzerObjectType;
	analyzerClassInfo.m_ClassId = SVLinearEdgePositionLineAnalyzerClassGuid;
	analyzerClassInfo.m_ClassName = SvUl::LoadStdString( IDS_CLASSNAME_SVLINEAREDGEPOSITIONLINEANALYZER );
	m_availableChildren.push_back(analyzerClassInfo);

	// Add the Line Linear Measurement Analyzer...
	analyzerClassInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVAnalyzerObjectType;
	analyzerClassInfo.m_ObjectTypeInfo.SubType	= SvPb::SVLinearMeasurementAnalyzerObjectType;
	analyzerClassInfo.m_ClassId = SVLinearMeasurementAnalyzerClassGuid;
	analyzerClassInfo.m_ClassName = SvUl::LoadStdString( IDS_CLASSNAME_SVLINEARMEASUREMENTLINEANALYZER );
	m_availableChildren.push_back(analyzerClassInfo);

	// Add the Line Maximum Foreground Object Analyzer...
	analyzerClassInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVAnalyzerObjectType;
	analyzerClassInfo.m_ObjectTypeInfo.SubType	= SvPb::SVLinearMaximumForegroundObjectAnalyzerObjectType;
	analyzerClassInfo.m_ClassId = SVLinearMaximumForegroundObjectLineAnalyzerClassGuid;
	analyzerClassInfo.m_ClassName = SvUl::LoadStdString( IDS_CLASSNAME_SVLINEARMAXIMUMFOREGROUNDOBJECTLINEANALYZER );
	m_availableChildren.push_back(analyzerClassInfo);

	// Add the Line Maximum Background Object Analyzer...
	analyzerClassInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVAnalyzerObjectType;
	analyzerClassInfo.m_ObjectTypeInfo.SubType	= SvPb::SVLinearMaximumBackgroundObjectAnalyzerObjectType;
	analyzerClassInfo.m_ClassId = SVLinearMaximumBackgroundObjectLineAnalyzerClassGuid;
	analyzerClassInfo.m_ClassName = SvUl::LoadStdString( IDS_CLASSNAME_SVLINEARMAXIMUMBACKGROUNDOBJECTLINEANALYZER );
	m_availableChildren.push_back(analyzerClassInfo);

	// Add the Line Maximum Object Analyzer...
	analyzerClassInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVAnalyzerObjectType;
	analyzerClassInfo.m_ObjectTypeInfo.SubType	= SvPb::SVLinearMaximumObjectAnalyzerObjectType;
	analyzerClassInfo.m_ClassId = SVLinearMaximumObjectLineAnalyzerClassGuid;
	analyzerClassInfo.m_ClassName = SvUl::LoadStdString( IDS_CLASSNAME_SVLINEARMAXIMUMOBJECTLINEANALYZER );
	m_availableChildren.push_back(analyzerClassInfo);

	// Build an operator list...

	// ...use In Place image operator list, because we already have an output image!
	SvOp::SVUnaryImageOperatorListClass* pOperatorList = new SvOp::SVLinearImageOperatorListClass;

	// Operator list defaults:
	if(nullptr != pOperatorList)
	{
		// Requires a SVThresholdClass Object
		pOperatorList->Add( new SvOp::SVThresholdClass( pOperatorList ) );

		// and Requires a SVUsermaskOperatorClass Object
		pOperatorList->Add( new SvOp::SVUserMaskOperatorClass( pOperatorList ) );

		// Add the UnaruyImageOperatorList to the Tool's List
		Add( pOperatorList );
	}
	SvOp::ToolSizeAdjustTask::AddToFriendlist(this, true,true,true);

	addDefaultInputObjects();
}
#pragma endregion Private Methods

} //namespace SvTo
