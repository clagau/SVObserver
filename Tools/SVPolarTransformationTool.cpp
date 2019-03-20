//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPolarTransformationTool
//* .File Name       : $Workfile:   SVPolarTransformationTool.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 12:31:46  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVPolarTransformationTool.h"
#include "SVObjectLibrary/SVClsIds.h"
#include "Operators/SVImagePolarTransform.h"
#include "Operators/SVEvaluateCenterXClass.h"
#include "Operators/SVEvaluateCenterYClass.h"
#include "Operators/SVEvaluateStartRadiusClass.h"
#include "Operators/SVEvaluateEndRadiusClass.h"
#include "Operators/SVEvaluateStartAngleClass.h"
#include "Operators/SVEvaluateEndAngleClass.h"
#pragma endregion Includes

namespace SvTo
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS( SVPolarTransformationToolClass, SVPolarTransformationToolClassGuid );

SVPolarTransformationToolClass::SVPolarTransformationToolClass( SVObjectClass* POwner, int StringResourceID )
							   :SVToolClass( POwner, StringResourceID )
{
	// BoolValueObject to use new method
	RegisterEmbeddedObject( &m_svAngularMethod, SVOutputAngularMethodObjectGuid, IDS_OBJECTNAME_ANGULAR_METHOD,	false, SvOi::SVResetItemNone );

	init();
}

void SVPolarTransformationToolClass::init()
{
	// Set up your type...
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVToolObjectType;
	m_outObjectInfo.m_ObjectTypeInfo.SubType    = SvPb::SVPolarTransformationToolObjectType;

	// Identify our input type needs

	m_toolExtent.SetTool( this );
	m_toolExtent.SetTranslation(SvPb::SVExtentTranslationPolarUnwrap );
	m_toolExtent.SetAlwaysUpdate( true );

	// Hide and Remove Embedded Extents
	removeEmbeddedExtents();

	// Register Embedded Objects
	RegisterEmbeddedObject( &m_SourceImageNames, SVSourceImageNamesGuid, IDS_OBJECTNAME_SOURCE_IMAGE_NAMES, false, SvOi::SVResetItemTool );
		

	// Default taskObjectList items:

	// Setup the evaluation clases
	SvOp::SVEvaluateClass* pEvaluate = new SvOp::SVEvaluateCenterXClass;
	if( pEvaluate )
		Add( pEvaluate );

	pEvaluate = new SvOp::SVEvaluateCenterYClass;
	if( pEvaluate )
		Add( pEvaluate );

	pEvaluate = new SvOp::SVEvaluateStartRadiusClass;
	if( pEvaluate )
		Add( pEvaluate );

	pEvaluate = new SvOp::SVEvaluateEndRadiusClass;
	if( pEvaluate )
		Add( pEvaluate );

	pEvaluate = new SvOp::SVEvaluateStartAngleClass;
	if( pEvaluate )
		Add( pEvaluate );

	pEvaluate = new SvOp::SVEvaluateEndAngleClass;
	if( pEvaluate )
		Add( pEvaluate );

	// Set Default Value to Use Angular Method1
	m_svAngularMethod.SetEnumTypes( _T("Angular Method1=0,Angular Method2=1") );

	m_svAngularMethod.SetDefaultValue( _T("Angular Method2"), true );
	m_svAngularMethod.SetObjectAttributesAllowed( SvPb::printable, SvOi::SetAttributeType::AddAttribute );

	// Add the Image Transformation Class
	SvOp::SVImagePolarTransformClass* pTransform = new SvOp::SVImagePolarTransformClass;
	if( nullptr != pTransform )
	{
		Add( pTransform );
	}

	// Set default inputs and outputs
	addDefaultInputObjects();
}

//******************************************************************************
// Destructor(s):
//******************************************************************************

SVPolarTransformationToolClass::~SVPolarTransformationToolClass()
{ 
}

//******************************************************************************
// Operator(s):
//******************************************************************************

bool SVPolarTransformationToolClass::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	m_isCreated = SVToolClass::CreateObject(rCreateStructure);

	m_SourceImageNames.setSaveValueFlag(false);
	m_SourceImageNames.SetObjectAttributesAllowed( SvPb::remotelySetable | SvPb::setableOnline, SvOi::SetAttributeType::RemoveAttribute );

	return m_isCreated;
}

SVToolClass* SVPolarTransformationToolClass::GetObjectAtPoint(const SVPoint<double>& rPoint)
{
	if (SvPb::SVExtentLocationPropertyUnknown != GetImageExtent().GetLocationPropertyAt(rPoint))
	{
		return this;
	}

	return nullptr;
}

bool SVPolarTransformationToolClass::DoesObjectHaveExtents() const
{
	return true;
}

// Sets the formulas of the evaluation inputs to the standard
// outputs of this image polar transform object. 
// This is very useful, if you want that mouse movings will 
// work also if use formula is selected.
// So this should be called if the user adds a new blank polar
// unwrap tool for easier handling. If he wants he can then  
// change the formulas.
// Another possibilty to call this function is in the setup
// dialog, if a kind of reset formula button exists.
bool SVPolarTransformationToolClass::SetDefaultFormulas(SvStl::MessageContainerVector *pErrorMessages)
{
	// Find image polar transform child...
	SvDef::SVObjectTypeInfoStruct objectInfo;
	objectInfo.SubType = SvPb::SVImagePolarTransformObjectType;
	SvOp::SVImagePolarTransformClass* pImagePolarTransform = dynamic_cast<SvOp::SVImagePolarTransformClass*>(getFirstObject(objectInfo));
	if(nullptr !=  pImagePolarTransform)
	{
		return pImagePolarTransform->SetDefaultFormulas(pErrorMessages);
	}
	return false;
}

HRESULT SVPolarTransformationToolClass::SetImageExtent( const SVImageExtentClass& rImageExtent )
{
	HRESULT l_hrOk = S_FALSE;
	double l_dInnerRadius;
	double l_dOuterRadius;

	if( S_OK == (l_hrOk = rImageExtent.GetExtentProperty( SvPb::SVExtentPropertyOuterRadius, l_dOuterRadius )) )
	{
		l_hrOk = rImageExtent.GetExtentProperty( SvPb::SVExtentPropertyOuterRadius, l_dInnerRadius );
	}
	// Validate that at least one radius is greater than or equal to 1.
	if( S_OK == l_hrOk && (l_dOuterRadius >= 1 || l_dInnerRadius >= 1) )
	{
		l_hrOk = SVToolClass::SetImageExtent( rImageExtent );
	}

	return l_hrOk;
}


SvVol::SVStringValueObjectClass* SVPolarTransformationToolClass::GetInputImageNames()
{
	return &m_SourceImageNames;
}

} //namespace SvTo
