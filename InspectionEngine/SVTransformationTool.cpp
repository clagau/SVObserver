//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTransformationTool
//* .File Name       : $Workfile:   SVTransformationTool.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2013 12:27:00  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVTransformationTool.h"
#include "SVEvaluateTranslationXClass.h"
#include "SVEvaluateTranslationYClass.h"
#include "SVEvaluateRotationXClass.h"
#include "SVEvaluateRotationYClass.h"
#include "SVEvaluateRotationAngleClass.h"
#include "SVImageTransform.h"
#include "SVObjectLibrary/SVClsIds.h"
#pragma endregion Includes

SV_IMPLEMENT_CLASS( SVTransformationToolClass, SVTransformationToolClassGuid );

SVTransformationToolClass::SVTransformationToolClass( SVObjectClass* POwner, int StringResourceID )
						  :SVToolClass( POwner, StringResourceID )
{
	init();
}

void SVTransformationToolClass::init()
{
	// Set up your type...
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVToolObjectType;
	m_outObjectInfo.m_ObjectTypeInfo.SubType    = SvPb::SVTransformationToolObjectType;

	// Identify our input type needs

	// Register Embedded Objects

	RegisterEmbeddedObject( &m_SourceImageNames, SVSourceImageNamesGuid, IDS_OBJECTNAME_SOURCE_IMAGE_NAMES, false, SvOi::SVResetItemTool );
		
	// Set Embedded defaults

	// Default taskObjectList items:

	// Setup the evaluation clases
	SVEvaluateClass* pEvaluate = new SVEvaluateTranslationXClass;
	if( pEvaluate )
		Add( pEvaluate );

	pEvaluate = new SVEvaluateTranslationYClass;
	if( pEvaluate )
		Add( pEvaluate );

	pEvaluate = new SVEvaluateRotationXClass;
	if( pEvaluate )
		Add( pEvaluate );

	pEvaluate = new SVEvaluateRotationYClass;
	if( pEvaluate )
		Add( pEvaluate );

	pEvaluate = new SVEvaluateRotationAngleClass;
	if( pEvaluate )
		Add( pEvaluate );

	// Add the Image Transformation Class
	SVImageTransformClass* pTransform = new SVImageTransformClass;
	if( pTransform )
		Add( pTransform );

	// Set default inputs and outputs
	addDefaultInputObjects();
	
}

//******************************************************************************
// Destructor(s):
//******************************************************************************

SVTransformationToolClass::~SVTransformationToolClass()
{ 
}

//******************************************************************************
// Operator(s):
//******************************************************************************

bool SVTransformationToolClass::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	m_isCreated = SVToolClass::CreateObject(rCreateStructure);

	m_SourceImageNames.setStatic( true );
	m_SourceImageNames.setSaveValueFlag(false);
	m_SourceImageNames.SetObjectAttributesAllowed( SvDef::SV_REMOTELY_SETABLE | SvDef::SV_SETABLE_ONLINE, SvOi::SetAttributeType::RemoveAttribute );

	return m_isCreated;
}

bool SVTransformationToolClass::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = SVToolClass::ResetObject(pErrorMessages);

	UpdateImageWithExtent();

	return Result;
}


SVTaskObjectClass* SVTransformationToolClass::GetObjectAtPoint( const SVPoint<double>& rPoint )
{
	return nullptr;
}

bool SVTransformationToolClass::DoesObjectHaveExtents() const
{
	return true;
}

HRESULT SVTransformationToolClass::UpdateImageWithExtent()
{
	HRESULT l_Status = S_OK;

	l_Status = m_toolExtent.UpdateImageWithExtent( SVToolExtentClass::SVTransformationToolExtent );

	return l_Status;
}

SVStringValueObjectClass* SVTransformationToolClass::GetInputImageNames()
{
	return &m_SourceImageNames;
}

