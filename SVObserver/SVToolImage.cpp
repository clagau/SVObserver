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
#include "SVAnalyzer.h"
#include "SVImageArithmetic.h"
#include "SVInspectionProcess.h"
#include "SVInplaceImageOperatorListClass.h"
#include "SVLUTEquation.h"
#include "SVLUTOperator.h"
#include "SVThresholdClass.h"
#include "SVUserMaskOperatorClass.h"
#include "ToolSizeAdjustTask.h"
#pragma endregion Includes

SV_IMPLEMENT_CLASS( SVImageToolClass, SVImageToolClassGuid );

SVImageToolClass::SVImageToolClass( BOOL BCreateDefaultTaskList, SVObjectClass* POwner, int StringResourceID )
				 :SVToolClass( BCreateDefaultTaskList, POwner, StringResourceID )
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
	// Set up your type...
	m_outObjectInfo.ObjectTypeInfo.ObjectType = SVToolObjectType;
	m_outObjectInfo.ObjectTypeInfo.SubType    = SVToolImageObjectType;

	// Register Embedded Objects
	RegisterEmbeddedObject( &outputEnableOffsetA, SVEnableOffsetAObjectGuid, IDS_OBJECTNAME_ENABLEOFFSETA, false, SVResetItemNone );
	RegisterEmbeddedObject( &outputOffsetAPoint, SVOffsetAPointObjectGuid, IDS_OBJECTNAME_OFFSETAPOINT, false, SVResetItemNone );
	RegisterEmbeddedObject( &outputEnableOffsetB, SVEnableOffsetBObjectGuid, IDS_OBJECTNAME_ENABLEOFFSETB, false, SVResetItemNone );
	RegisterEmbeddedObject( &outputOffsetBPoint, SVOffsetBPointObjectGuid, IDS_OBJECTNAME_OFFSETBPOINT, false, SVResetItemNone );
	RegisterEmbeddedObject( &outputOperator, SVArithmeticOperatorObjectGuid, IDS_OBJECTNAME_ARITHMETICOPERATOR, false, SVResetItemOwner);

	m_svToolExtent.SetTranslation(SVExtentTranslationFigureShift);

	// Register SourceImageNames Value Object
	RegisterEmbeddedObject( &m_svSourceImageNames, SVSourceImageNamesGuid, IDS_OBJECTNAME_SOURCE_IMAGE_NAMES, false, SVResetItemTool );
	m_svSourceImageNames.SetArraySize(2);

	// Set Embedded Defaults...
	POINT defaultPoint;
	defaultPoint.x = 0;
	defaultPoint.y = 0;
	outputEnableOffsetA.SetDefaultValue( FALSE, TRUE );
	outputOffsetAPoint.SetDefaultValue( defaultPoint, TRUE );
	outputEnableOffsetB.SetDefaultValue( FALSE, TRUE );
	outputOffsetBPoint.SetDefaultValue( defaultPoint, TRUE );
	outputOperator.SetDefaultValue( SVImageSubSaturation, TRUE );

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
			pThresholdOperator->GetThresholdActivateAttribute().SetDefaultValue( FALSE, TRUE );
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
		::SVSendMessage( pOperatorList, SVM_CONNECT_ALL_INPUTS, 0, 0 );
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

BOOL SVImageToolClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = SVToolClass::CreateObject( PCreateStructure );

	// Set / Reset Printable Flags
	outputEnableOffsetA.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	outputOffsetAPoint.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	outputEnableOffsetB.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	outputOffsetBPoint.ObjectAttributesAllowedRef() |= SV_PRINTABLE;
	outputOperator.ObjectAttributesAllowedRef() |= SV_PRINTABLE;

	m_svSourceImageNames.ObjectAttributesAllowedRef() &=~SV_REMOTELY_SETABLE & ~SV_SETABLE_ONLINE;

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

HRESULT SVImageToolClass::SetImageExtent( unsigned long p_ulIndex, SVImageExtentClass p_svImageExtent )
{
	HRESULT l_hrOk = S_FALSE;

	SVExtentFigureStruct l_svFigure;

	p_svImageExtent.GetFigure(l_svFigure);

	if ( (l_svFigure.m_svTopLeft.m_dPositionX >= 0) && (l_svFigure.m_svTopLeft.m_dPositionY >= 0) )
	{
		l_hrOk = SVToolClass::SetImageExtent( p_ulIndex, p_svImageExtent );
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
BOOL SVImageToolClass::SetDefaultFormulas()
{
	BOOL bRetVal = TRUE;

	// Set Default Formula of LUTEquation, if any...
	SVObjectTypeInfoStruct lutEquationInfo;
	lutEquationInfo.ObjectType	= SVEquationObjectType;
	lutEquationInfo.SubType		= SVLUTEquationObjectType;
	SVLUTEquationClass* pLUTEquation = reinterpret_cast<SVLUTEquationClass*>(::SVSendMessage( this, SVM_GETFIRST_OBJECT, 0, reinterpret_cast<DWORD_PTR>(&lutEquationInfo) ));
	if( pLUTEquation )
	{
		bRetVal = pLUTEquation->SetDefaultFormula() && bRetVal;
	}

	return bRetVal;
}

HRESULT SVImageToolClass::ResetObject()
{
	HRESULT l_hrOk = S_OK;

	if( S_OK != UpdateTranslation() )
	{
		l_hrOk = S_FALSE;
	}

	if( S_OK != SVToolClass::ResetObject() )
	{
		l_hrOk = S_FALSE;
	}

	UpdateImageWithExtent( 1 );

	return l_hrOk;
} 

BOOL SVImageToolClass::onRun( SVRunStatusClass& RRunStatus )
{
	return SVToolClass::onRun( RRunStatus );
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
		
		if ( l_lValue == SV_IMGOP_DOUBLE_HEIGHT )
		{
			l_hrOK = toolImageExtents.SetTranslation (SVExtentTranslationDoubleHeight);
			heightScaleFactor = 2.0;
			toolImageExtents.SetExtentProperty (SVExtentPropertyHeightScaleFactor, heightScaleFactor);
			extentChanged = true;
		}
		else if ( l_lValue == SV_IMGOP_FLIP_VERTICAL )
		{
			l_hrOK = toolImageExtents.SetTranslation(SVExtentTranslationFlipVertical);
			extentChanged = true;
		}
		else if ( l_lValue == SV_IMGOP_FLIP_HORIZONTAL )
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
		SetImageExtent(1, toolImageExtents);
	}

	return l_hrOK;
}

SVStaticStringValueObjectClass* SVImageToolClass::GetInputImageNames()
{
	return &m_svSourceImageNames;
}

HRESULT SVImageToolClass::SetImageExtentToParent(unsigned long p_ulIndex )
{
	HRESULT l_hrOk = S_OK;
	SVImageExtentClass l_NewExtent;

	l_hrOk = m_svToolExtent.UpdateExtentToParentExtents( p_ulIndex, l_NewExtent );

	if( S_OK == l_hrOk )
	{
		l_hrOk = SVToolClass::SetImageExtent( p_ulIndex, l_NewExtent );
	}
	return l_hrOk;
}

BOOL SVImageToolClass::IsValid()
{
	BOOL bValid = TRUE;

	ToolSizeAdjustTask* pToolSizeAdjustTask = nullptr;
	pToolSizeAdjustTask = ToolSizeAdjustTask::GetToolSizeAdjustTask(this);
	if(nullptr != pToolSizeAdjustTask)
	{
			bValid =  pToolSizeAdjustTask->OnValidate();
	}

	return SVToolClass::IsValid() & bValid ;
}

