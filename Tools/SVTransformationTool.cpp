//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "Operators/SVEvaluateTranslationXClass.h"
#include "Operators/SVEvaluateTranslationYClass.h"
#include "Operators/SVEvaluateRotationXClass.h"
#include "Operators/SVEvaluateRotationYClass.h"
#include "Operators/SVEvaluateRotationAngleClass.h"
#include "Operators/SVImageTransform.h"
#pragma endregion Includes

namespace SvTo
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations


SV_IMPLEMENT_CLASS( SVTransformationToolClass, SvPb::TransformationToolClassId);

SVTransformationToolClass::SVTransformationToolClass( SVObjectClass* POwner, int StringResourceID )
						  :SVToolClass(ToolExtType::NoHeightWidth, POwner, StringResourceID )
{
	init();
}

void SVTransformationToolClass::init()
{
	// Set up your type...
	m_ObjectTypeInfo.m_ObjectType = SvPb::SVToolObjectType;
	m_ObjectTypeInfo.m_SubType    = SvPb::SVTransformationToolObjectType;

	// Identify our input type needs

	// Register Embedded Objects

	RegisterEmbeddedObject( &m_SourceImageNames, SvPb::SourceImageNamesEId, IDS_OBJECTNAME_SOURCE_IMAGE_NAMES, false, SvOi::SVResetItemTool, false);
		
	// Set Embedded defaults

	// Default taskObjectList items:

	// Setup the evaluation clases
	SvOp::SVEvaluate* pEvaluate = new SvOp::SVEvaluateTranslationXClass;
	if( pEvaluate ) Add( pEvaluate );

	pEvaluate = new SvOp::SVEvaluateTranslationYClass;
	if( pEvaluate )	Add( pEvaluate );

	pEvaluate = new SvOp::SVEvaluateRotationXClass;
	if( pEvaluate )	Add( pEvaluate );

	pEvaluate = new SvOp::SVEvaluateRotationYClass;
	if( pEvaluate )	Add( pEvaluate );

	pEvaluate = new SvOp::SVEvaluateRotationAngleClass;
	if( pEvaluate )	Add( pEvaluate );

	// Add the Image Transformation Class
	SvOp::SVImageTransform* pTransform = new SvOp::SVImageTransform;
	if( pTransform ) Add( pTransform );
	
	if (m_pEmbeddedExtents)
	{
		m_pEmbeddedExtents->SetAttributes();
	}

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

	m_SourceImageNames.setSaveValueFlag(false);
	m_SourceImageNames.SetObjectAttributesAllowed( SvPb::remotelySetable | SvPb::setableOnline, SvOi::SetAttributeType::RemoveAttribute );

	return m_isCreated;
}

bool SVTransformationToolClass::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = SVToolClass::ResetObject(pErrorMessages);

	UpdateImageWithExtent();

	return Result;
}


SVToolClass* SVTransformationToolClass::GetObjectAtPoint( const SVPoint<double>&)
{
	return nullptr;
}

bool SVTransformationToolClass::DoesObjectHaveExtents() const
{
	return true;
}

HRESULT SVTransformationToolClass::UpdateImageWithExtent()
{
	HRESULT l_Status = m_toolExtent.UpdateImageWithExtent(SVToolExtentClass::SVTransformationToolExtent);

	return l_Status;
}

SvVol::SVStringValueObjectClass* SVTransformationToolClass::GetInputImageNames()
{
	return &m_SourceImageNames;
}

std::vector<std::string> SVTransformationToolClass::getToolAdjustNameList() const
{
	constexpr std::array<LPCTSTR, 7> cToolAdjustNameList
	{
		_T("Image Source"),
		_T("Translation"),
		_T("Rotation"),
		_T("Learn"),
		_T("Conditional"),
		_T("General"),
		_T("Comment"),
	};
	return { cToolAdjustNameList.begin(), cToolAdjustNameList.end() };
}

} //namespace SvTo
