//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMathTool
//* .File Name       : $Workfile:   SVMathTool.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   17 Feb 2015 18:21:56  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVMathTool.h"
#include "SVGlobal.h"
#include "SVMathEquation.h"
#include "SVResultDouble.h"
#include "SVInspectionProcess.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SV_IMPLEMENT_CLASS( SVMathToolClass, SVMathToolClassGuid );

////////////////////////////////////////////////////////////////////////////////
// .Title       : Constructor of class SVMathToolClass
// -----------------------------------------------------------------------------
// .Description : This constructor inits all own members and the members of
//				: the parent class SVToolClass
// -----------------------------------------------------------------------------
// .Input(s)
//	 Type				Name				Description
//	:SVImageClass*		Source				Pointer to an image object which is
//											the complete or partly source of
//											this tool
//	:LPCSTR				ObjectName			Pointer to a null terminated const
//	                                        string that contents an optional
//                                          class name.
//                                          Initiated in SVObjectClass.  
////////////////////////////////////////////////////////////////////////////////
SVMathToolClass::SVMathToolClass( BOOL BCreateDefaultTaskList, SVObjectClass* POwner, int StringResourceID )
: SVToolClass( BCreateDefaultTaskList, POwner, StringResourceID )
{
	init();
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
void SVMathToolClass::init(void)
{
	// Set up your type...
	m_outObjectInfo.ObjectTypeInfo.ObjectType = SVToolObjectType;
	m_outObjectInfo.ObjectTypeInfo.SubType    = SVMathToolObjectType;

	// Hide and Remove Embedded Extents
	removeEmbeddedExtents();

	// Set up the friend
	pMathEquation = new SVMathEquationClass( this );
	AddFriend( pMathEquation->GetUniqueObjectID() );

	// Setup the result
	SVClassInfoStruct resultClassInfo;
	CString strTitle;
	SVObjectTypeInfoStruct interfaceInfo;

	// Declare Input Interface of Math Equation Result...
	interfaceInfo.EmbeddedID = SVMathEquationResultObjectGuid;
	resultClassInfo.DesiredInputInterface.Add( interfaceInfo );

	resultClassInfo.ObjectTypeInfo.ObjectType = SVResultObjectType;
	resultClassInfo.ObjectTypeInfo.SubType	= SVResultDoubleObjectType;
	resultClassInfo.ClassId = SVDoubleResultClassGuid;
	resultClassInfo.ClassName.LoadString( IDS_OBJECTNAME_RESULT );
	strTitle.LoadString( IDS_CLASSNAME_RESULT_DOUBLE );
	resultClassInfo.ClassName += SV_TSTR_SPACE + strTitle;

	// Construct the result
	SVDoubleResultClass* pResult = ( SVDoubleResultClass * )resultClassInfo.Construct();
	Add( pResult );
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : Standard Destructor of class SVMathToolClass
// -----------------------------------------------------------------------------
// .Description : Standard destructor
////////////////////////////////////////////////////////////////////////////////
SVMathToolClass::~SVMathToolClass()
{ 
}

///////////////////////////////////////////////////////////////////////////////
// Create Operator
////////////////////////////////////////////////////////////////////////////////
BOOL SVMathToolClass::CreateObject(SVObjectLevelCreateStruct* PCreateStruct )
{
	m_isCreated = SVToolClass::CreateObject( PCreateStruct );

	// Override base class exposure of the drawflag
	// This value will not be exposed for the Math Tool.
	drawToolFlag.ObjectAttributesAllowedRef() = SV_HIDDEN;

	// Override base class exposure of the auxillaryextent variables
	// These values will not be exposed for the Math Tool.
	m_svUpdateAuxiliaryExtents.ObjectAttributesAllowedRef() = SV_HIDDEN;
	m_svAuxiliarySourceX.ObjectAttributesAllowedRef() = SV_HIDDEN;
	m_svAuxiliarySourceY.ObjectAttributesAllowedRef() = SV_HIDDEN;
	m_svAuxiliarySourceAngle.ObjectAttributesAllowedRef() = SV_HIDDEN;
	m_svAuxiliarySourceImageName.ObjectAttributesAllowedRef() = SV_HIDDEN;
	m_svAuxiliaryDrawType.ObjectAttributesAllowedRef() = SV_HIDDEN;

	return m_isCreated;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : OnValidate
// -----------------------------------------------------------------------------
// .Description : 
////////////////////////////////////////////////////////////////////////////////
BOOL SVMathToolClass::OnValidate()
{
	if( SVToolClass::OnValidate() )
	{
		return TRUE;
	}
	SetInvalid();
	return FALSE;
}

