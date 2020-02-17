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
#include "SVObjectLibrary/SVClsIds.h"
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
#pragma endregion Includes

namespace SvTo
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS( SVImageToolClass, SvPb::ImageToolClassId );

SVImageToolClass::SVImageToolClass( SVObjectClass* POwner, int StringResourceID )
				 :SVToolClass( POwner, StringResourceID )
{
	init();
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : init
// -----------------------------------------------------------------------------
// .Description : Initialization of newly Instantiated Object
////////////////////////////////////////////////////////////////////////////////
void SVImageToolClass::init()
{
	m_canResizeToParent = true;
	// Set up your type...
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVToolObjectType;
	m_outObjectInfo.m_ObjectTypeInfo.SubType    = SvPb::SVToolImageObjectType;

	// Register Embedded Objects
	RegisterEmbeddedObject( &m_outputEnableOffsetA, SVEnableOffsetAObjectGuid, IDS_OBJECTNAME_ENABLEOFFSETA, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_outputOffsetAPoint, SVOffsetAPointObjectGuid, IDS_OBJECTNAME_OFFSETAPOINT, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_outputEnableOffsetB, SVEnableOffsetBObjectGuid, IDS_OBJECTNAME_ENABLEOFFSETB, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_outputOffsetBPoint, SVOffsetBPointObjectGuid, IDS_OBJECTNAME_OFFSETBPOINT, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_outputOperator, SVArithmeticOperatorObjectGuid, IDS_OBJECTNAME_ARITHMETICOPERATOR, false, SvOi::SVResetItemOwner);

	m_toolExtent.SetTranslation(SvPb::SVExtentTranslationFigureShift);

	// Register SourceImageNames Value Object
	RegisterEmbeddedObject( &m_SourceImageNames, SVSourceImageNamesGuid, IDS_OBJECTNAME_SOURCE_IMAGE_NAMES, false, SvOi::SVResetItemTool );
	m_SourceImageNames.SetArraySize(2);

	// Set Embedded Defaults...
	SVPoint<long> defaultPoint(0,0);
	m_outputEnableOffsetA.SetDefaultValue( BOOL(false) );
	m_outputOffsetAPoint.SetDefaultValue( defaultPoint );
	m_outputEnableOffsetB.SetDefaultValue(BOOL(false) );
	m_outputOffsetBPoint.SetDefaultValue( defaultPoint );
	m_outputOperator.SetDefaultValue( SVImageSubSaturation );

	// Default taskObjectList items:

	SvOp::SVImageArithmeticClass* pArithmetic = new SvOp::SVImageArithmeticClass;
	if(nullptr != pArithmetic)
	{
		Add( pArithmetic );
	}

	// Build an operator list...
	// ...use In Place Image Operator list, 
	//	because we DON'T want another output image in the image tool!!!
	//
	//	We are using image output of SVImageArithmeticClass and do an
	//	in place processing.
	//	RO_20Mar2000
	SvOp::SVUnaryImageOperatorListClass* pOperatorList = new SvOp::SVInPlaceImageOperatorListClass;

	// Operator list defaults:
	if( pOperatorList )
	{
		// Requires a SVThresholdClass Object
		SvOp::SVThresholdClass* pThresholdOperator = new SvOp::SVThresholdClass( pOperatorList );
		if( pThresholdOperator )
		{
			// Activated by default!
			// We don't want to use this operator by default.
			// So deactivate it...
			pThresholdOperator->GetThresholdActivateAttribute().SetDefaultValue(BOOL(false) );
			pOperatorList->Add( pThresholdOperator );
		}

		// and Requires a SVUsermaskOperatorClass Object
		// Deactivated by default!
		pOperatorList->Add( new SvOp::SVUserMaskOperatorClass(pOperatorList) );

		// and Requires a SVLUTOperatorClass Object
		// Deactivated by default!
		pOperatorList->Add( new SvOp::SVLUTOperatorClass(pOperatorList) );

		// Add the UnaryImageOperatorList to the Tool's List
		Add( pOperatorList );

		// Ensure input image gets connected to preceeding image output 
		// ( SVImageArithmeticClass image output !!! )
		pOperatorList->ConnectAllInputs();
	}

	SvOp::ToolSizeAdjustTask::AddToFriendlist(this, true,true,false);

	// Add Default Inputs and Outputs
	addDefaultInputObjects();
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : Standard Destructor of class SVImageToolClass
// -----------------------------------------------------------------------------
// .Description : Standard destructor
////////////////////////////////////////////////////////////////////////////////
SVImageToolClass::~SVImageToolClass()
{ 
	SVImageToolClass::CloseObject();
}

bool SVImageToolClass::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	bool bOk = SVToolClass::CreateObject(rCreateStructure);

	// Set / Reset Printable Flags
	m_outputEnableOffsetA.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::AddAttribute );
	m_outputOffsetAPoint.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::AddAttribute );
	m_outputEnableOffsetB.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::AddAttribute );
	m_outputOffsetBPoint.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::AddAttribute );
	m_outputOperator.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::AddAttribute );

	m_SourceImageNames.setSaveValueFlag(false);
	m_SourceImageNames.SetObjectAttributesAllowed( SvPb::remotelySetable | SvPb::setableOnline, SvOi::SetAttributeType::RemoveAttribute );

	bOk &= S_OK == UpdateTranslation();
	
	if(bOk)
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

	if ( (rFigure.m_svTopLeft.m_x >= 0) && (rFigure.m_svTopLeft.m_y >= 0) )
	{
		l_hrOk = SVToolClass::SetImageExtent( rImageExtent );
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
bool SVImageToolClass::SetDefaultFormulas(SvStl::MessageContainerVector *pErrorMessages)
{
	bool bRetVal = true;

	// Set Default Formula of LUTEquation, if any...
	SvDef::SVObjectTypeInfoStruct lutEquationInfo;
	lutEquationInfo.ObjectType	= SvPb::SVEquationObjectType;
	lutEquationInfo.SubType		= SvPb::SVLUTEquationObjectType;
	SvOp::SVLUTEquationClass* pLUTEquation = dynamic_cast<SvOp::SVLUTEquationClass*>(getFirstObject(lutEquationInfo));
	if( pLUTEquation )
	{
		bRetVal = pLUTEquation->SetDefaultFormula(pErrorMessages) && bRetVal;
	}

	return bRetVal;
}

bool SVImageToolClass::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = true;

	if( S_OK != UpdateTranslation() )
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_UpdateTranslationFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID() );
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
	long	l_lValue =		0;
	bool	extentChanged = false;

	HRESULT result{S_OK};

	SVImageExtentClass toolImageExtents = GetImageExtent();

	if( S_OK == m_outputOperator.GetValue(l_lValue))
	{
		//change translation type on extents to match operator if:
		//			Height Double, Flip Horizontal of Flip Vertical
		if ( l_lValue == SvDef::SVImageOperatorDoubleHeight )
		{
			toolImageExtents.SetTranslation(SvPb::SVExtentTranslationDoubleHeight);
			double heightScaleFactor = 2.0;
			toolImageExtents.SetExtentProperty(SvPb::SVExtentPropertyHeightScaleFactor, heightScaleFactor);
			extentChanged = true;
		}
		else if ( l_lValue == SvDef::SVImageOperatorFlipVertical )
		{
			toolImageExtents.SetTranslation(SvPb::SVExtentTranslationFlipVertical);
			extentChanged = true;
		}
		else if ( l_lValue == SvDef::SVImageOperatorFlipHorizontal )
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
	SvPb::SetGuidInProtoBytes(pOverlay->mutable_guid(), GetUniqueObjectID());
	pOverlay->mutable_color()->set_trpos(m_statusColor.getTrPos() + 1);
	pOverlay->set_displaybounding(true);
	auto* pBoundingBox = pOverlay->mutable_boundingshape();
	auto* pRect = pBoundingBox->mutable_rect();
	pRect->mutable_x()->set_value(0);
	pRect->mutable_y()->set_value(0);
	SvPb::setValueObject(m_ExtentWidth, *pRect->mutable_w());
	SvPb::setValueObject(m_ExtentHeight, *pRect->mutable_h());
	setStateValueToOverlay(*pOverlay);
	collectOverlays(pImage, *pOverlay);
}

HRESULT SVImageToolClass::SetImageExtentToParent()
{
	SVImageExtentClass NewExtent;
	HRESULT l_hrOk = m_toolExtent.UpdateExtentToParentExtents( NewExtent );

	if( S_OK == l_hrOk )
	{
		l_hrOk = SVToolClass::SetImageExtent(NewExtent);
	}
	return l_hrOk;
}

} //namespace SvTo
