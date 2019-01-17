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
#include "SVToolImage.h"
#include "InspectionEngine/SVAnalyzer.h"
#include "SVImageArithmetic.h"
#include "SVInspectionProcess.h"
#include "InspectionEngine/SVInplaceImageOperatorListClass.h"
#include "SVLUTEquation.h"
#include "SVLUTOperator.h"
#include "InspectionEngine/SVThresholdClass.h"
#include "SVUserMaskOperatorClass.h"
#include "ToolSizeAdjustTask.h"
#pragma endregion Includes

SV_IMPLEMENT_CLASS( SVImageToolClass, SVImageToolClassGuid );

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
	RegisterEmbeddedObject( &outputEnableOffsetA, SVEnableOffsetAObjectGuid, IDS_OBJECTNAME_ENABLEOFFSETA, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &outputOffsetAPoint, SVOffsetAPointObjectGuid, IDS_OBJECTNAME_OFFSETAPOINT, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &outputEnableOffsetB, SVEnableOffsetBObjectGuid, IDS_OBJECTNAME_ENABLEOFFSETB, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &outputOffsetBPoint, SVOffsetBPointObjectGuid, IDS_OBJECTNAME_OFFSETBPOINT, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &outputOperator, SVArithmeticOperatorObjectGuid, IDS_OBJECTNAME_ARITHMETICOPERATOR, false, SvOi::SVResetItemOwner);

	m_toolExtent.SetTranslation(SvDef::SVExtentTranslationFigureShift);

	// Register SourceImageNames Value Object
	RegisterEmbeddedObject( &m_SourceImageNames, SVSourceImageNamesGuid, IDS_OBJECTNAME_SOURCE_IMAGE_NAMES, false, SvOi::SVResetItemTool );
	m_SourceImageNames.SetArraySize(2);

	// Set Embedded Defaults...
	POINT defaultPoint;
	defaultPoint.x = 0;
	defaultPoint.y = 0;
	outputEnableOffsetA.SetDefaultValue( BOOL(false) );
	outputOffsetAPoint.SetDefaultValue( defaultPoint );
	outputEnableOffsetB.SetDefaultValue(BOOL(false) );
	outputOffsetBPoint.SetDefaultValue( defaultPoint );
	outputOperator.SetDefaultValue( SVImageSubSaturation );

	// Default taskObjectList items:

	SVImageArithmeticClass* pArithmetic = new SVImageArithmeticClass; //( this );
	if( pArithmetic )
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
	SVUnaryImageOperatorListClass* pOperatorList = new SVInPlaceImageOperatorListClass;

	// Operator list defaults:
	if( pOperatorList )
	{
		// Requires a SVThresholdClass Object
		SVThresholdClass* pThresholdOperator = new SVThresholdClass( pOperatorList );
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
		pOperatorList->Add( new SVUserMaskOperatorClass( pOperatorList ) );

		// and Requires a SVLUTOperatorClass Object
		// Deactivated by default!
		pOperatorList->Add( new SVLUTOperatorClass( pOperatorList ) );

		// Add the UnaryImageOperatorList to the Tool's List
		Add( pOperatorList );

		// Ensure input image gets connected to preceeding image output 
		// ( SVImageArithmeticClass image output !!! )
		pOperatorList->ConnectAllInputs();
	}

	ToolSizeAdjustTask::AddToFriendlist(this, true,true,false);

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
	CloseObject();
}

bool SVImageToolClass::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	bool bOk = SVToolClass::CreateObject(rCreateStructure);

	// Set / Reset Printable Flags
	outputEnableOffsetA.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::AddAttribute );
	outputOffsetAPoint.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::AddAttribute );
	outputEnableOffsetB.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::AddAttribute );
	outputOffsetBPoint.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::AddAttribute );
	outputOperator.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::AddAttribute );

	m_SourceImageNames.setSaveValueFlag(false);
	m_SourceImageNames.SetObjectAttributesAllowed( SvPb::remotelySetable | SvPb::setableOnline, SvOi::SetAttributeType::RemoveAttribute );

	bOk &= S_OK == UpdateTranslation();
	
	if(bOk)
	{
		bOk = (S_OK == ToolSizeAdjustTask::EnsureInFriendList(this, true, true, false)); 
	}
	
	m_isCreated = bOk;

	return bOk;
}

SVTaskObjectClass *SVImageToolClass::GetObjectAtPoint(const SVPoint<double>& rPoint)
{
	SVTaskObjectClass *pObject {nullptr};

	if (SvDef::SVExtentLocationPropertyUnknown != GetImageExtent().GetLocationPropertyAt(rPoint))
	{
		pObject = this;
	}

	return pObject;
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
	SVLUTEquationClass* pLUTEquation = dynamic_cast<SVLUTEquationClass*>(getFirstObject(lutEquationInfo));
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

	if( S_OK == outputOperator.GetValue(l_lValue))
	{
		//change translation type on extents to match operator if:
		//			Height Double, Flip Horizontal of Flip Vertical
		double	heightScaleFactor = 1.0;
		
		if ( l_lValue == SvDef::SVImageOperatorDoubleHeight )
		{
			toolImageExtents.SetTranslation(SvDef::SVExtentTranslationDoubleHeight);
			heightScaleFactor = 2.0;
			toolImageExtents.SetExtentProperty(SvDef::SVExtentPropertyHeightScaleFactor, heightScaleFactor);
			extentChanged = true;
		}
		else if ( l_lValue == SvDef::SVImageOperatorFlipVertical )
		{
			toolImageExtents.SetTranslation(SvDef::SVExtentTranslationFlipVertical);
			extentChanged = true;
		}
		else if ( l_lValue == SvDef::SVImageOperatorFlipHorizontal )
		{
			toolImageExtents.SetTranslation(SvDef::SVExtentTranslationFlipHorizontal);
			extentChanged = true;
		}
		else
		{
			toolImageExtents.SetTranslation(SvDef::SVExtentTranslationFigureShift);
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

SVStringValueObjectClass* SVImageToolClass::GetInputImageNames()
{
	return &m_SourceImageNames;
}

HRESULT SVImageToolClass::SetImageExtentToParent()
{
	HRESULT l_hrOk = S_OK;
	SVImageExtentClass NewExtent;

	l_hrOk = m_toolExtent.UpdateExtentToParentExtents( NewExtent );

	if( S_OK == l_hrOk )
	{
		l_hrOk = SVToolClass::SetImageExtent(NewExtent);
	}
	return l_hrOk;
}
