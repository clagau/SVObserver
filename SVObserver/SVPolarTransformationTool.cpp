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
#include "SVImagePolarTransform.h"
#include "SVEvaluateCenterXClass.h"
#include "SVEvaluateCenterYClass.h"
#include "SVEvaluateStartRadiusClass.h"
#include "SVEvaluateEndRadiusClass.h"
#include "SVEvaluateStartAngleClass.h"
#include "SVEvaluateEndAngleClass.h"
#pragma endregion Includes

SV_IMPLEMENT_CLASS( SVPolarTransformationToolClass, SVPolarTransformationToolClassGuid );

SVPolarTransformationToolClass::SVPolarTransformationToolClass( BOOL BCreateDefaultTaskList, SVObjectClass* POwner, int StringResourceID )
							   :SVToolClass( BCreateDefaultTaskList, POwner, StringResourceID )
{
	// BoolValueObject to use new method
	RegisterEmbeddedObject( &m_svAngularMethod, SVOutputAngularMethodObjectGuid, IDS_OBJECTNAME_ANGULAR_METHOD,	false, SVResetItemNone );

	init();
}

void SVPolarTransformationToolClass::init()
{
	// Set up your type...
	m_outObjectInfo.ObjectTypeInfo.ObjectType = SVToolObjectType;
	m_outObjectInfo.ObjectTypeInfo.SubType    = SVPolarTransformationToolObjectType;

	// Identify our input type needs

	m_svToolExtent.SetTool( this );
	m_svToolExtent.SetImageType( SVImageTypePhysical );
	m_svToolExtent.SetTranslation( SVExtentTranslationPolarUnwrap );
	m_svToolExtent.SetAlwaysUpdate( true );

	// Hide and Remove Embedded Extents
	removeEmbeddedExtents();

	// Register Embedded Objects
	RegisterEmbeddedObject( &m_svSourceImageNames, SVSourceImageNamesGuid, IDS_OBJECTNAME_SOURCE_IMAGE_NAMES, false, SVResetItemTool );
		

	// Default taskObjectList items:

	// Setup the evaluation clases
	SVEvaluateClass* pEvaluate = new SVEvaluateCenterXClass;
	if( pEvaluate )
		Add( pEvaluate );

	pEvaluate = new SVEvaluateCenterYClass;
	if( pEvaluate )
		Add( pEvaluate );

	pEvaluate = new SVEvaluateStartRadiusClass;
	if( pEvaluate )
		Add( pEvaluate );

	pEvaluate = new SVEvaluateEndRadiusClass;
	if( pEvaluate )
		Add( pEvaluate );

	pEvaluate = new SVEvaluateStartAngleClass;
	if( pEvaluate )
		Add( pEvaluate );

	pEvaluate = new SVEvaluateEndAngleClass;
	if( pEvaluate )
		Add( pEvaluate );

	// Set Default Value to Use Angular Method1
	m_svAngularMethod.SetEnumTypes( _T("Angular Method1=0,Angular Method2=1") );

	m_svAngularMethod.SetDefaultValue( _T("Angular Method1"), TRUE );
	m_svAngularMethod.ObjectAttributesAllowedRef() |= SV_PRINTABLE;

	// Add the Image Transformation Class
	SVImagePolarTransformClass* pTransform = new SVImagePolarTransformClass;
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

BOOL SVPolarTransformationToolClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = SVToolClass::CreateObject( PCreateStructure );

	m_svSourceImageNames.ObjectAttributesAllowedRef() &=~SV_REMOTELY_SETABLE & ~SV_SETABLE_ONLINE;

	m_isCreated = bOk;

	return bOk;
}

SVTaskObjectClass *SVPolarTransformationToolClass::GetObjectAtPoint( const SVExtentPointStruct &p_rsvPoint )
{
	SVImageExtentClass l_svExtents;

	SVTaskObjectClass* l_psvObject = nullptr;

	if( S_OK == m_svToolExtent.GetImageExtent( l_svExtents ) &&
	    l_svExtents.GetLocationPropertyAt( p_rsvPoint ) != SVExtentLocationPropertyUnknown )
	{
		l_psvObject = this;
	}

	return l_psvObject;
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
BOOL SVPolarTransformationToolClass::SetDefaultFormulas()
{
	// Find image polar transform child...
	SVObjectTypeInfoStruct objectInfo;
	objectInfo.SubType = SVImagePolarTransformObjectType;
	SVImagePolarTransformClass* pImagePolarTransform = dynamic_cast<SVImagePolarTransformClass*>(getFirstObject(objectInfo));
	if( pImagePolarTransform )
	{
		return pImagePolarTransform->SetDefaultFormulas();
	}
	return FALSE;
}

HRESULT SVPolarTransformationToolClass::SetImageExtent( unsigned long p_ulIndex, SVImageExtentClass p_svImageExtent )
{
	HRESULT l_hrOk = S_FALSE;
	double l_dInnerRadius;
	double l_dOuterRadius;

	if( S_OK == (l_hrOk = p_svImageExtent.GetExtentProperty( SVExtentPropertyOuterRadius, l_dOuterRadius )) )
	{
		l_hrOk = p_svImageExtent.GetExtentProperty( SVExtentPropertyOuterRadius, l_dInnerRadius );
	}
	// Validate that at least one radius is greater than or equal to 1.
	if( S_OK == l_hrOk && (l_dOuterRadius >= 1 || l_dInnerRadius >= 1) )
	{
		l_hrOk = SVToolClass::SetImageExtent( p_ulIndex, p_svImageExtent );
	}

	return l_hrOk;
}


SVStaticStringValueObjectClass* SVPolarTransformationToolClass::GetInputImageNames()
{
	return &m_svSourceImageNames;
}

