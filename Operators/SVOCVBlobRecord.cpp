//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOCVCharacterResult
//* .File Name       : $Workfile:   SVOCVBlobRecord.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 11:07:32  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVOCVBlobRecord.h"
#include "SVObjectLibrary/SVObjectLevelCreateStruct.h"
#pragma endregion Includes

namespace SvOp
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS( SVOCVCharacterResult, SvPb::OCVCharacterResultClassId);

SVOCVCharacterResult::SVOCVCharacterResult(LPCSTR ObjectName )
					: SVTaskObjectClass( ObjectName )
{
	init();
}

SVOCVCharacterResult::SVOCVCharacterResult(SVObjectClass* POwner, int StringResourceID )
						: SVTaskObjectClass( POwner, StringResourceID )
{
	init();

	// Add Default Inputs and Outputs
	addDefaultInputObjects();
}

SVOCVCharacterResult::~SVOCVCharacterResult()
{
	SVOCVCharacterResult::CloseObject();
}

void SVOCVCharacterResult::init()
{
	//Special type names for extents
	m_dvoOverlayWidth.SetTypeName( _T("Extent Width") );
	m_dvoOverlayHeight.SetTypeName( _T("Extent Height") );
	m_dvoOverlayLeft.SetTypeName( _T("Extent X") );
	m_dvoOverlayTop.SetTypeName( _T("Extent Y") );

	RegisterEmbeddedObject( &m_cvoLabelValue, SvPb::OCVCharacterValueEId, IDS_OBJECTNAME_OCV_CHARACTER_VALUE, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_dvoOverlayLeft, SvPb::OCVExtentLeftEId, IDS_OBJECTNAME_EXTENT_LEFT, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_dvoOverlayTop, SvPb::OCVExtentTopEId, IDS_OBJECTNAME_EXTENT_TOP, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_dvoOverlayWidth, SvPb::OCVExtentWidthEId, IDS_OBJECTNAME_EXTENT_WIDTH, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_dvoOverlayHeight, SvPb::OCVExtentHeightEId, IDS_OBJECTNAME_EXTENT_HEIGHT, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_dvoMatchScore, SvPb::OCVMatchScoreEId, IDS_OBJECTNAME_OCV_MATCH_SCORE, false, SvOi::SVResetItemNone );
	m_cvoLabelValue.setSaveValueFlag(false);
	m_dvoOverlayLeft.setSaveValueFlag(false);
	m_dvoOverlayTop.setSaveValueFlag(false);
	m_dvoOverlayWidth.setSaveValueFlag(false);
	m_dvoOverlayHeight.setSaveValueFlag(false);
	m_dvoMatchScore.setSaveValueFlag(false);
}

void SVOCVCharacterResult::HideResults()
{
	m_statusColor.SetObjectAttributesAllowed( SvPb::embedable, SvOi::SetAttributeType::OverwriteAttribute );
	m_statusTag.SetObjectAttributesAllowed( SvPb::embedable, SvOi::SetAttributeType::OverwriteAttribute );

	m_cvoLabelValue.SetObjectAttributesAllowed( SvPb::embedable, SvOi::SetAttributeType::OverwriteAttribute );

	m_dvoOverlayLeft.SetObjectAttributesAllowed( SvPb::embedable, SvOi::SetAttributeType::OverwriteAttribute );
	m_dvoOverlayTop.SetObjectAttributesAllowed( SvPb::embedable, SvOi::SetAttributeType::OverwriteAttribute );
	m_dvoOverlayWidth.SetObjectAttributesAllowed( SvPb::embedable, SvOi::SetAttributeType::OverwriteAttribute );
	m_dvoOverlayHeight.SetObjectAttributesAllowed( SvPb::embedable, SvOi::SetAttributeType::OverwriteAttribute );

	m_dvoMatchScore.SetObjectAttributesAllowed( SvPb::embedable, SvOi::SetAttributeType::OverwriteAttribute );
}

void SVOCVCharacterResult::UnhideResults()
{
	const UINT cAttributes = SvDef::defaultValueObjectAttributes & ~SvPb::audittrail;
	m_statusColor.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::OverwriteAttribute );
	m_statusTag.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::OverwriteAttribute );

	m_cvoLabelValue.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::OverwriteAttribute );

	m_dvoOverlayLeft.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::OverwriteAttribute );
	m_dvoOverlayTop.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::OverwriteAttribute );
	m_dvoOverlayWidth.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::OverwriteAttribute );
	m_dvoOverlayHeight.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::OverwriteAttribute );

	m_dvoMatchScore.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::OverwriteAttribute );
}

bool SVOCVCharacterResult::CreateObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	bool bOk = SVTaskObjectClass::CreateObject(rCreateStructure);

	m_cvoLabelValue.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute );

	m_dvoOverlayLeft.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute );
	m_dvoOverlayTop.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute );
	m_dvoOverlayWidth.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute );
	m_dvoOverlayHeight.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute );

	m_dvoMatchScore.SetObjectAttributesAllowed( SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute );

	m_isCreated = bOk;

	return bOk;
}// end CreateObject

} //namespace SvOp
