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
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SvDef::SVToolObjectType;
	m_outObjectInfo.m_ObjectTypeInfo.SubType    = SvDef::SVToolImageObjectType;

	// Register Embedded Objects
	RegisterEmbeddedObject( &outputEnableOffsetA, SVEnableOffsetAObjectGuid, IDS_OBJECTNAME_ENABLEOFFSETA, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &outputOffsetAPoint, SVOffsetAPointObjectGuid, IDS_OBJECTNAME_OFFSETAPOINT, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &outputEnableOffsetB, SVEnableOffsetBObjectGuid, IDS_OBJECTNAME_ENABLEOFFSETB, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &outputOffsetBPoint, SVOffsetBPointObjectGuid, IDS_OBJECTNAME_OFFSETBPOINT, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &outputOperator, SVArithmeticOperatorObjectGuid, IDS_OBJECTNAME_ARITHMETICOPERATOR, false, SvOi::SVResetItemOwner);

	m_svToolExtent.SetTranslation(SVExtentTranslationFigureShift);

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
	outputEnableOffsetA.SetObjectAttributesAllowed( SvDef::SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute );
	outputOffsetAPoint.SetObjectAttributesAllowed( SvDef::SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute );
	outputEnableOffsetB.SetObjectAttributesAllowed( SvDef::SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute );
	outputOffsetBPoint.SetObjectAttributesAllowed( SvDef::SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute );
	outputOperator.SetObjectAttributesAllowed( SvDef::SV_PRINTABLE, SvOi::SetAttributeType::AddAttribute );

	m_SourceImageNames.setStatic( true );
	m_SourceImageNames.setSaveValueFlag(false);
	m_SourceImageNames.SetObjectAttributesAllowed( SvDef::SV_REMOTELY_SETABLE | SvDef::SV_SETABLE_ONLINE, SvOi::SetAttributeType::RemoveAttribute );

	bOk &= S_OK == UpdateTranslation();
	
	if(bOk)
	{
		bOk = (S_OK == ToolSizeAdjustTask::EnsureInFriendList(this, true, true, false)); 
	}
	
	m_isCreated = bOk;

	return bOk;
}

SVTaskObjectClass *SVImageToolClass::GetObjectAtPoint( const SVExtentPointStruct &p_rsvPoint )
{
	SVImageExtentClass l_svExtents;

	SVTaskObjectClass *l_psvObject = nullptr;

	if( S_OK == m_svToolExtent.GetImageExtent( l_svExtents ) &&
	    SVExtentLocationPropertyUnknown != l_svExtents.GetLocationPropertyAt( p_rsvPoint ) )
	{
		l_psvObject = this;
	}

	return l_psvObject;
}

bool SVImageToolClass::DoesObjectHaveExtents() const
{
	return true;
}

HRESULT SVImageToolClass::SetImageExtent(const SVImageExtentClass& rImageExtent)
{
	HRESULT l_hrOk = S_FALSE;

	SVExtentFigureStruct l_svFigure;

	rImageExtent.GetFigure(l_svFigure);

	if ( (l_svFigure.m_svTopLeft.m_dPositionX >= 0) && (l_svFigure.m_svTopLeft.m_dPositionY >= 0) )
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
	lutEquationInfo.ObjectType	= SvDef::SVEquationObjectType;
	lutEquationInfo.SubType		= SvDef::SVLUTEquationObjectType;
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

	HRESULT l_hrOK =		S_FALSE;

	SVImageExtentClass toolImageExtents;

	if( S_OK == outputOperator.GetValue(l_lValue) )
	{
		//change translation type on extents to match operator if:
		//			Height Double, Flip Horizontal of Flip Vertical
		GetImageExtent	(toolImageExtents);
		double	heightScaleFactor = 1.0;
		
		if ( l_lValue == SvDef::SVImageOperatorDoubleHeight )
		{
			l_hrOK = toolImageExtents.SetTranslation (SVExtentTranslationDoubleHeight);
			heightScaleFactor = 2.0;
			toolImageExtents.SetExtentProperty (SVExtentPropertyHeightScaleFactor, heightScaleFactor);
			extentChanged = true;
		}
		else if ( l_lValue == SvDef::SVImageOperatorFlipVertical )
		{
			l_hrOK = toolImageExtents.SetTranslation(SVExtentTranslationFlipVertical);
			extentChanged = true;
		}
		else if ( l_lValue == SvDef::SVImageOperatorFlipHorizontal )
		{
			l_hrOK = toolImageExtents.SetTranslation(SVExtentTranslationFlipHorizontal);
			extentChanged = true;
		}
		else
		{
			l_hrOK = toolImageExtents.SetTranslation(SVExtentTranslationFigureShift);
			extentChanged = true;
		}
	} // if( S_OK == outputOperator.GetValue(l_lValue)  )

	if ((true == extentChanged) && (S_OK == l_hrOK))
	{
		SetImageExtent(toolImageExtents);
	}

	return l_hrOK;
}

SVStringValueObjectClass* SVImageToolClass::GetInputImageNames()
{
	return &m_SourceImageNames;
}

HRESULT SVImageToolClass::SetImageExtentToParent()
{
	HRESULT l_hrOk = S_OK;
	SVImageExtentClass NewExtent;

	l_hrOk = m_svToolExtent.UpdateExtentToParentExtents( NewExtent );

	if( S_OK == l_hrOk )
	{
		l_hrOk = SVToolClass::SetImageExtent(NewExtent);
	}
	return l_hrOk;
}
