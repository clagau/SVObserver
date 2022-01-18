//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolImage
//* .File Name       : $Workfile:   SVToolImage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.4  $
//* .Check In Date   : $Date:   15 May 2014 14:47:34  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVImageTool.h"
#include "Definitions/ImageOperatorEnums.h"
#include "SVMatroxLibrary\SVMatroxSimpleEnums.h"
#include "Operators/SVImageArithmetic.h"
#include "Operators/SVInplaceImageOperatorListClass.h"
#include "Operators/SVLUTEquation.h"
#include "Operators/SVLUTOperator.h"
#include "Operators/SVThresholdClass.h"
#include "Operators/SVUserMaskOperatorClass.h"
#include "Operators/ToolSizeAdjustTask.h"
#include "SVProtoBuf/ConverterHelper.h"
#include "SVProtoBuf/Overlay.h"
#include "SVMatroxLibrary/SVMatroxImageInterface.h"
#pragma endregion Includes

namespace SvTo
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS(SVImageToolClass, SvPb::ImageToolClassId);

SVImageToolClass::SVImageToolClass(SVObjectClass* POwner, int StringResourceID)
	:SVToolClass(true,POwner, StringResourceID)
{
	init();
}

void SVImageToolClass::init()
{
	m_canResizeToParent = true;
	// Set up your type...
	m_ObjectTypeInfo.m_ObjectType = SvPb::SVToolObjectType;
	m_ObjectTypeInfo.m_SubType = SvPb::SVToolImageObjectType;

	// Register Embedded Objects
	BuildEmbeddedObjectList();

	m_toolExtent.SetTranslation(SvPb::SVExtentTranslationFigureShift);

	// Default taskObjectList items:
	SvOp::SVImageArithmetic* pArithmetic = new SvOp::SVImageArithmetic;
	if (nullptr != pArithmetic)
	{
		Add(pArithmetic);
	}

	// Build an operator list...
	// ...use In Place Image Operator list, 
	//	because we DON'T want another output image in the image tool!!!
	//
	//	We are using image output of SVImageArithmetic and do an
	//	in place processing.
	//	RO_20Mar2000
	SvOp::SVUnaryImageOperatorList* pOperatorList = new SvOp::SVInPlaceImageOperatorListClass;

	// Operator list defaults:
	if (pOperatorList)
	{
		// Requires a SVThresholdClass Object
		SvOp::SVThresholdClass* pThresholdOperator = new SvOp::SVThresholdClass(pOperatorList);
		if (pThresholdOperator)
		{
			// Activated by default!
			// We don't want to use this operator by default.
			// So deactivate it...
			pThresholdOperator->GetThresholdActivateAttribute().SetDefaultValue(BOOL(false));
			pOperatorList->Add(pThresholdOperator);
		}

		// and Requires a SVUsermaskOperatorClass Object
		// Deactivated by default!
		pOperatorList->Add(new SvOp::SVUserMaskOperatorClass(pOperatorList));

		// and Requires a SVLUTOperator Object
		// Deactivated by default!
		pOperatorList->Add(new SvOp::SVLUTOperator(pOperatorList));

		// Add the UnaryImageOperatorList to the Tool's List
		Add(pOperatorList);

		// Ensure input image gets connected to preceeding image output 
		// ( SVImageArithmetic image output !!! )
		pOperatorList->connectAllInputs();
	}

	SvOp::ToolSizeAdjustTask::AddToFriendlist(this, true, true, false);
}

SVImageToolClass::~SVImageToolClass()
{
	SVImageToolClass::CloseObject();
}


void SVImageToolClass::BuildEmbeddedObjectList()
{
	RegisterEmbeddedObject(&m_outputOperator, SvPb::ArithmeticOperatorEId, IDS_OBJECTNAME_ARITHMETICOPERATOR, false, SvOi::SVResetItemTool);
	m_outputOperator.SetDefaultValue(SVImageSubSaturation);

	RegisterEmbeddedObject(&m_SourceImageNames, SvPb::SourceImageNamesEId, IDS_OBJECTNAME_SOURCE_IMAGE_NAMES, false, SvOi::SVResetItemTool);
	m_SourceImageNames.SetArraySize(2);

	RegisterEmbeddedObject(&m_IsGainOffsetEnabled, SvPb::ImageToolEnabledGainId, IDS_OBJECTNAME_IMAGETOOL_ENABLE_GAIN, false, SvOi::SVResetItemTool);
	m_IsGainOffsetEnabled.SetDefaultValue(FALSE);

	RegisterEmbeddedObject(&m_UseLut, SvPb::ImageToolReplaceOneId, IDS_OBJECTNAME_RECALCULATE_ONCE, false, SvOi::SVResetItemTool);
	m_UseLut.SetDefaultValue(FALSE);

	RegisterEmbeddedObject(&m_LinkedGain, SvPb::ImageToolGainId, IDS_OBJECTNAME_IMAGETOOL_GAIN, false, SvOi::SVResetItemTool);

	m_LinkedGain.SetDefaultValue(_variant_t(1.0), true);
	
	RegisterEmbeddedObject(&m_LinkedOffset, SvPb::ImageToolOffsetId, IDS_OBJECTNAME_IMAGETOOL_OFFSET, false, SvOi::SVResetItemTool);
	m_LinkedOffset.SetDefaultValue(_variant_t(0.0), true);
}

bool SVImageToolClass::CreateObject(const SVObjectLevelCreateStruct& rCreateStructure)
{
	bool bOk = SVToolClass::CreateObject(rCreateStructure);

	// Set / Reset Printable Flags
	m_outputOperator.SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::AddAttribute);

	m_SourceImageNames.setSaveValueFlag(false);
	m_SourceImageNames.SetObjectAttributesAllowed(SvPb::remotelySetable | SvPb::setableOnline, SvOi::SetAttributeType::RemoveAttribute);

	bOk &= S_OK == UpdateTranslation();

	if (bOk)
	{
		bOk = (S_OK == SvOp::ToolSizeAdjustTask::EnsureInFriendList(this, true, true, false));
	}

	m_isCreated = bOk;

	return bOk;
}

SVToolClass* SVImageToolClass::GetObjectAtPoint(const SVPoint<double>& rPoint)
{
	if (SvPb::SVExtentLocationPropertyUnknown != GetImageExtent().GetLocationPropertyAt(rPoint))
	{
		return this;
	}

	return nullptr;
}

bool SVImageToolClass::DoesObjectHaveExtents() const
{
	return true;
}

HRESULT SVImageToolClass::SetImageExtent(const SVImageExtentClass& rImageExtent)
{
	HRESULT l_hrOk = S_FALSE;

	const SVExtentFigureStruct& rFigure = rImageExtent.GetFigure();

	if ((rFigure.m_svTopLeft.m_x >= 0) && (rFigure.m_svTopLeft.m_y >= 0))
	{
		l_hrOk = SVToolClass::SetImageExtent(rImageExtent);
	}
	else
	{
		l_hrOk = S_FALSE;
	}

	return l_hrOk;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : SetDefaultFormulas
// -----------------------------------------------------------------------------
// .Description : Sets the default Formulas of the embbeded equation objects.
//				:
//				:	This should be called in SVIPDoc::OnAddxxxTool(...),
//				:	after the tool is added to the tool set, if you need
//				:	to have default equations in your tool and/or 
//				:	friends, children, embeddeds, etc.
////////////////////////////////////////////////////////////////////////////////
bool SVImageToolClass::SetDefaultFormulas(SvStl::MessageContainerVector* pErrorMessages)
{
	bool bRetVal = true;

	// Set Default Formula of LUTEquation, if any...
	SvDef::SVObjectTypeInfoStruct lutEquationInfo;
	lutEquationInfo.m_ObjectType = SvPb::SVEquationObjectType;
	lutEquationInfo.m_SubType = SvPb::SVLUTEquationObjectType;
	SvOp::SVLUTEquation* pLUTEquation = dynamic_cast<SvOp::SVLUTEquation*>(getFirstObject(lutEquationInfo));
	if (pLUTEquation)
	{
		bRetVal = pLUTEquation->SetDefaultFormula(pErrorMessages) && bRetVal;
	}

	return bRetVal;
}

bool SVImageToolClass::ResetObject(SvStl::MessageContainerVector* pErrorMessages)
{
	bool Result = true;

	long op;
	BOOL isEnabled;
	m_outputOperator.GetValue(op);
	m_IsGainOffsetEnabled.GetValue(isEnabled);

	if (SVMatroxImageInterface::isFloatBufferAllowed(op) == false && isEnabled)
	{
		m_IsGainOffsetEnabled.SetValue(FALSE);
	}

	SvIe::SVExtentPropertyInfoStruct info;
	getToolExtent().GetExtentPropertyInfo(SvPb::SVExtentPropertyPositionPointX, info);
	info.bSetByReset = true;
	getToolExtent().SetExtentPropertyInfo(SvPb::SVExtentPropertyPositionPointX, info);
	getToolExtent().getImageExtent().SetExtentProperty(SvPb::SVExtentPropertyPositionPointX, 0.);
	getToolExtent().GetExtentPropertyInfo(SvPb::SVExtentPropertyPositionPointY, info);
	info.bSetByReset = true;
	getToolExtent().SetExtentPropertyInfo(SvPb::SVExtentPropertyPositionPointY, info);
	getToolExtent().getImageExtent().SetExtentProperty(SvPb::SVExtentPropertyPositionPointY, 0.);

	if (S_OK != UpdateTranslation())
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_UpdateTranslationFailed, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
			pErrorMessages->push_back(Msg);
		}
	}

	Result = SVToolClass::ResetObject(pErrorMessages) && Result;

	UpdateImageWithExtent();

	return Result;
}

HRESULT SVImageToolClass::UpdateTranslation()
{
	//get operator type
	long	l_lValue = 0;
	bool	extentChanged = false;

	HRESULT result {S_OK};

	SVImageExtentClass toolImageExtents = GetImageExtent();

	if (S_OK == m_outputOperator.GetValue(l_lValue))
	{
		//change translation type on extents to match operator if:
		//			Height Double, Flip Horizontal of Flip Vertical
		if (l_lValue == SvDef::SVImageOperatorDoubleHeight)
		{
			toolImageExtents.SetTranslation(SvPb::SVExtentTranslationDoubleHeight);
			double heightScaleFactor = 2.0;
			toolImageExtents.SetExtentProperty(SvPb::SVExtentPropertyHeightFactorContent, heightScaleFactor);
			toolImageExtents.SetExtentProperty(SvPb::SVExtentPropertyHeightFactorFormat, heightScaleFactor);
			double widthScaleFactor = 1.0;
			toolImageExtents.SetExtentProperty(SvPb::SVExtentPropertyWidthFactorContent, widthScaleFactor);
			toolImageExtents.SetExtentProperty(SvPb::SVExtentPropertyWidthFactorFormat, widthScaleFactor);
			extentChanged = true;
		}
		else if (l_lValue == SvDef::SVImageOperatorFlipVertical)
		{
			toolImageExtents.SetTranslation(SvPb::SVExtentTranslationFlipVertical);
			extentChanged = true;
		}
		else if (l_lValue == SvDef::SVImageOperatorFlipHorizontal)
		{
			toolImageExtents.SetTranslation(SvPb::SVExtentTranslationFlipHorizontal);
			extentChanged = true;
		}
		else
		{
			toolImageExtents.SetTranslation(SvPb::SVExtentTranslationFigureShift);
			extentChanged = true;
		}
	}
	else
	{
		result = E_FAIL;
	}

	if ((true == extentChanged) && (S_OK == result))
	{
		result = SetImageExtent(toolImageExtents);
	}

	return result;
}

SvVol::SVStringValueObjectClass* SVImageToolClass::GetInputImageNames()
{
	return &m_SourceImageNames;
}

void SVImageToolClass::addOverlays(const SvIe::SVImageClass* pImage, SvPb::OverlayDesc& rOverlay) const
{
	auto* pOverlay = rOverlay.add_overlays();
	pOverlay->set_name(GetName());
	pOverlay->set_objectid(getObjectId());
	pOverlay->set_displaybounding(true);
	auto* pBoundingBox = pOverlay->mutable_boundingshape();
	auto* pRect = pBoundingBox->mutable_rect();
	pRect->mutable_x()->set_value(0);
	pRect->mutable_y()->set_value(0);
	if (m_pEmbeddedExtents)
	{
		SvPb::setValueObject(m_pEmbeddedExtents->m_ExtentWidth, *pRect->mutable_w());
		SvPb::setValueObject(m_pEmbeddedExtents->m_ExtentHeight, *pRect->mutable_h());
	}
	setStateValueToOverlay(*pOverlay);
	collectOverlays(pImage, *pOverlay);
}

std::vector<std::string> SVImageToolClass::getToolAdjustNameList() const
{
	constexpr std::array<LPCTSTR, 9> cToolAdjustNameList
	{
		_T("Image Source"),
		_T("Size and Position"),
		_T("Filter"),
		_T("Threshold"),
		_T("Mask"),
		_T("LUT"),
		_T("Conditional"),
		_T("General"),
		_T("Comment"),
	};
	return {cToolAdjustNameList.begin(), cToolAdjustNameList.end()};
}

HRESULT SVImageToolClass::SetImageExtentToParent()
{
	SVImageExtentClass NewExtent;
	HRESULT l_hrOk = m_toolExtent.UpdateExtentToParentExtents(NewExtent);

	if (S_OK == l_hrOk)
	{
		l_hrOk = SVToolClass::SetImageExtent(NewExtent);
	}
	return l_hrOk;
}


} //namespace SvTo
