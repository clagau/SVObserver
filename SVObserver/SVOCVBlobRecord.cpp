//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOCVCharacterResultClass
//* .File Name       : $Workfile:   SVOCVBlobRecord.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 11:07:32  $
//******************************************************************************

#include "stdafx.h"
#include "SVOCVBlobRecord.h"
#include "SVObjectLibrary/SVAnalyzerLevelCreateStruct.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SV_IMPLEMENT_CLASS( SVOCVCharacterResultClass, SVOCVCharacterResultClassGuid );

SVOCVCharacterResultClass::SVOCVCharacterResultClass(LPCSTR ObjectName )
					: SVTaskObjectClass( ObjectName )
{
	init();
}

SVOCVCharacterResultClass::SVOCVCharacterResultClass(SVObjectClass* POwner, int StringResourceID )
						: SVTaskObjectClass( POwner, StringResourceID )
{
	init();

	// Add Default Inputs and Outputs
	addDefaultInputObjects();
}

SVOCVCharacterResultClass::~SVOCVCharacterResultClass()
{
	CloseObject();
}

void SVOCVCharacterResultClass::init()
{
	//Special type names for extents
	m_dvoOverlayWidth.SetTypeName( _T("Extent Width") );
	m_dvoOverlayHeight.SetTypeName( _T("Extent Height") );
	m_dvoOverlayLeft.SetTypeName( _T("Extent X") );
	m_dvoOverlayTop.SetTypeName( _T("Extent Y") );

	RegisterEmbeddedObject( &m_cvoLabelValue, SVOCVCharacterValueGuid, IDS_OBJECTNAME_OCV_CHARACTER_VALUE, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_dvoOverlayLeft, SVOCVExtentLeftGuid, IDS_OBJECTNAME_EXTENT_LEFT, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_dvoOverlayTop, SVOCVExtentTopGuid, IDS_OBJECTNAME_EXTENT_TOP, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_dvoOverlayWidth, SVOCVExtentWidthGuid, IDS_OBJECTNAME_EXTENT_WIDTH, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_dvoOverlayHeight, SVOCVExtentHeightGuid, IDS_OBJECTNAME_EXTENT_HEIGHT, false, SvOi::SVResetItemNone );
	RegisterEmbeddedObject( &m_dvoMatchScore, SVOCVMatchScoreGuid, IDS_OBJECTNAME_OCV_MATCH_SCORE, false, SvOi::SVResetItemNone );
}

void SVOCVCharacterResultClass::HideResults()
{
	m_statusColor.SetObjectAttributesAllowed( SV_EMBEDABLE, SvOi::SetAttributeType::OverwriteAttribute );
	m_statusTag.SetObjectAttributesAllowed( SV_EMBEDABLE, SvOi::SetAttributeType::OverwriteAttribute );

	m_cvoLabelValue.SetObjectAttributesAllowed( SV_EMBEDABLE, SvOi::SetAttributeType::OverwriteAttribute );

	m_dvoOverlayLeft.SetObjectAttributesAllowed( SV_EMBEDABLE, SvOi::SetAttributeType::OverwriteAttribute );
	m_dvoOverlayTop.SetObjectAttributesAllowed( SV_EMBEDABLE, SvOi::SetAttributeType::OverwriteAttribute );
	m_dvoOverlayWidth.SetObjectAttributesAllowed( SV_EMBEDABLE, SvOi::SetAttributeType::OverwriteAttribute );
	m_dvoOverlayHeight.SetObjectAttributesAllowed( SV_EMBEDABLE, SvOi::SetAttributeType::OverwriteAttribute );

	m_dvoMatchScore.SetObjectAttributesAllowed( SV_EMBEDABLE, SvOi::SetAttributeType::OverwriteAttribute );
}

void SVOCVCharacterResultClass::UnhideResults()
{
	const UINT cAttributes = SV_DEFAULT_VALUE_OBJECT_ATTRIBUTES & ~SV_PRINTABLE;
	m_statusColor.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::OverwriteAttribute );
	m_statusTag.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::OverwriteAttribute );

	m_cvoLabelValue.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::OverwriteAttribute );

	m_dvoOverlayLeft.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::OverwriteAttribute );
	m_dvoOverlayTop.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::OverwriteAttribute );
	m_dvoOverlayWidth.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::OverwriteAttribute );
	m_dvoOverlayHeight.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::OverwriteAttribute );

	m_dvoMatchScore.SetObjectAttributesAllowed( cAttributes, SvOi::SetAttributeType::OverwriteAttribute );
}

BOOL SVOCVCharacterResultClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = SVTaskObjectClass::CreateObject( PCreateStructure );

	m_cvoLabelValue.SetObjectAttributesAllowed( SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );

	m_dvoOverlayLeft.SetObjectAttributesAllowed( SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );
	m_dvoOverlayTop.SetObjectAttributesAllowed( SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );
	m_dvoOverlayWidth.SetObjectAttributesAllowed( SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );
	m_dvoOverlayHeight.SetObjectAttributesAllowed( SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );

	m_dvoMatchScore.SetObjectAttributesAllowed( SV_PRINTABLE, SvOi::SetAttributeType::RemoveAttribute );

	m_isCreated = bOk;

	return bOk;
}// end CreateObject

BOOL SVOCVCharacterResultClass::CloseObject()
{
	BOOL bOk = SVTaskObjectClass::CloseObject();

	return bOk;
}// end CloseObject
