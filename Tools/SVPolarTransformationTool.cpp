//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "SVProtoBuf/ConverterHelper.h"
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

SV_IMPLEMENT_CLASS( SVPolarTransformationToolClass, SvPb::PolarTransformationToolClassId );

SVPolarTransformationToolClass::SVPolarTransformationToolClass( SVObjectClass* POwner, int StringResourceID )
							   :SVToolClass(ToolExtType::None,POwner, StringResourceID )
{
	// BoolValueObject to use new method
	RegisterEmbeddedObject( &m_svAngularMethod, SvPb::OutputAngularMethodEId, IDS_OBJECTNAME_ANGULAR_METHOD, false, SvOi::SVResetItemNone, true);

	init();
}

void SVPolarTransformationToolClass::init()
{
	// Set up your type...
	m_ObjectTypeInfo.m_ObjectType = SvPb::SVToolObjectType;
	m_ObjectTypeInfo.m_SubType    = SvPb::SVPolarTransformationToolObjectType;

	// Identify our input type needs

	m_toolExtent.SetTool( this );
	m_toolExtent.SetTranslation(SvPb::SVExtentTranslationPolarUnwrap );
	m_toolExtent.SetAlwaysUpdate( true );

	

	// Register Embedded Objects
	RegisterEmbeddedObject( &m_SourceImageNames, SvPb::SourceImageNamesEId, IDS_OBJECTNAME_SOURCE_IMAGE_NAMES, false, SvOi::SVResetItemToolAndDependent, false);
		

	// Default taskObjectList items:

	// Setup the evaluation clases
	SvOp::SVEvaluate* pEvaluate = new SvOp::SVEvaluateCenterXClass;
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
	m_svAngularMethod.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::AddAttribute );

	// Add the Image Transformation Class
	SvOp::SVImagePolarTransform* pTransform = new SvOp::SVImagePolarTransform;
	if( nullptr != pTransform )
	{
		Add( pTransform );
	}
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
	m_SourceImageNames.SetObjectAttributesAllowed( SvPb::remotelySetable, SvOi::SetAttributeType::RemoveAttribute );

	return m_isCreated;
}

SVToolClass* SVPolarTransformationToolClass::GetObjectAtPoint(const SVPoint<double>& rPoint)
{
	if (SvPb::SVExtentLocationPropertyUnknown != m_toolExtent.GetLocationPropertyAt(rPoint))
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
	objectInfo.m_SubType = SvPb::SVImagePolarTransformObjectType;
	SvOp::SVImagePolarTransform* pImagePolarTransform = dynamic_cast<SvOp::SVImagePolarTransform*>(getFirstObject(objectInfo));
	if(nullptr !=  pImagePolarTransform)
	{
		return pImagePolarTransform->SetDefaultFormulas(pErrorMessages);
	}
	return false;
}

HRESULT SVPolarTransformationToolClass::SetImageExtent( const SVImageExtentClass& rImageExtent)
{
	HRESULT l_hrOk = S_FALSE;
	double l_dInnerRadius{0.0};
	double l_dOuterRadius{0.0};

	if( S_OK == (l_hrOk = rImageExtent.GetExtentProperty( SvPb::SVExtentPropertyOuterRadius, l_dOuterRadius )) )
	{
		l_hrOk = rImageExtent.GetExtentProperty( SvPb::SVExtentPropertyInnerRadius, l_dInnerRadius );
	}
	// Validate that at least one radius is greater than or equal to 1.
	if( S_OK == l_hrOk && (l_dOuterRadius >= 1 || l_dInnerRadius >= 1) )
	{
		l_hrOk = SVToolClass::SetImageExtent(rImageExtent);
	}

	return l_hrOk;
}


SvVol::SVStringValueObjectClass* SVPolarTransformationToolClass::GetInputImageNames()
{
	return &m_SourceImageNames;
}

void SVPolarTransformationToolClass::addOverlays(SvPb::OverlayDesc& rOverlay) const
{
	SvDef::SVObjectTypeInfoStruct objectInfo;
	objectInfo.m_SubType = SvPb::SVImagePolarTransformObjectType;
	SvOp::SVImagePolarTransform* pImagePolarTransform = dynamic_cast<SvOp::SVImagePolarTransform*>(getFirstObject(objectInfo));
	if (nullptr != pImagePolarTransform)
	{
		auto* pOverlay = rOverlay.add_overlays();
		pOverlay->set_name(GetName());
		pOverlay->set_objectid(getObjectId());
		pOverlay->set_displaybounding(true);
		auto* pBoundingBox = pOverlay->mutable_boundingshape();
		auto* pSlice = pBoundingBox->mutable_slice();
		pImagePolarTransform->setSliceOverlay(*pSlice);
		setStateValueToOverlay(*pOverlay);
	}
}

std::vector<std::string> SVPolarTransformationToolClass::getToolAdjustNameList() const
{
	constexpr std::array<LPCTSTR, 5> cToolAdjustNameList
	{
		_T("Image Source"),
		_T("Polar Unwrap"),
		_T("Conditional"),
		_T("General"),
		_T("Comment"),
	};
	return { cToolAdjustNameList.begin(), cToolAdjustNameList.end() };
}

} //namespace SvTo
