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
	RegisterEmbeddedObject( &m_cvoLabelValue, SVOCVCharacterValueGuid, IDS_OBJECTNAME_OCV_CHARACTER_VALUE, false, SVResetItemNone );

	RegisterEmbeddedObject( &m_dvoOverlayLeft, SVOCVExtentLeftGuid, IDS_OBJECTNAME_EXTENT_LEFT, false, SVResetItemNone, _T("Extent X") );
	RegisterEmbeddedObject( &m_dvoOverlayTop, SVOCVExtentTopGuid, IDS_OBJECTNAME_EXTENT_TOP, false, SVResetItemNone, _T("Extent Y") );
	RegisterEmbeddedObject( &m_dvoOverlayWidth, SVOCVExtentWidthGuid, IDS_OBJECTNAME_EXTENT_WIDTH, false, SVResetItemNone, _T("Extent Width") );
	RegisterEmbeddedObject( &m_dvoOverlayHeight, SVOCVExtentHeightGuid, IDS_OBJECTNAME_EXTENT_HEIGHT, false, SVResetItemNone, _T("Extent Height") );

	RegisterEmbeddedObject( &m_dvoMatchScore, SVOCVMatchScoreGuid, IDS_OBJECTNAME_OCV_MATCH_SCORE, false, SVResetItemNone );
}

void SVOCVCharacterResultClass::HideResults()
{
	m_isObjectValid.ObjectAttributesAllowedRef() = SV_EMBEDABLE;
	m_statusColor.ObjectAttributesAllowedRef() = SV_EMBEDABLE;
	m_statusTag.ObjectAttributesAllowedRef() = SV_EMBEDABLE;

	m_cvoLabelValue.ObjectAttributesAllowedRef() = SV_EMBEDABLE;

	m_dvoOverlayLeft.ObjectAttributesAllowedRef() = SV_EMBEDABLE;
	m_dvoOverlayTop.ObjectAttributesAllowedRef() = SV_EMBEDABLE;
	m_dvoOverlayWidth.ObjectAttributesAllowedRef() = SV_EMBEDABLE;
	m_dvoOverlayHeight.ObjectAttributesAllowedRef() = SV_EMBEDABLE;

	m_dvoMatchScore.ObjectAttributesAllowedRef() = SV_EMBEDABLE;
}

void SVOCVCharacterResultClass::UnhideResults()
{
	m_isObjectValid.ObjectAttributesAllowedRef() = ( SV_DEFAULT_VALUE_OBJECT_ATTRIBUTES & ~SV_PRINTABLE );
	m_statusColor.ObjectAttributesAllowedRef() = ( SV_DEFAULT_VALUE_OBJECT_ATTRIBUTES & ~SV_PRINTABLE );
	m_statusTag.ObjectAttributesAllowedRef() = ( SV_DEFAULT_VALUE_OBJECT_ATTRIBUTES & ~SV_PRINTABLE );

	m_cvoLabelValue.ObjectAttributesAllowedRef() = ( SV_DEFAULT_VALUE_OBJECT_ATTRIBUTES & ~SV_PRINTABLE );

	m_dvoOverlayLeft.ObjectAttributesAllowedRef() = ( SV_DEFAULT_VALUE_OBJECT_ATTRIBUTES & ~SV_PRINTABLE );
	m_dvoOverlayTop.ObjectAttributesAllowedRef() = ( SV_DEFAULT_VALUE_OBJECT_ATTRIBUTES & ~SV_PRINTABLE );
	m_dvoOverlayWidth.ObjectAttributesAllowedRef() = ( SV_DEFAULT_VALUE_OBJECT_ATTRIBUTES & ~SV_PRINTABLE );
	m_dvoOverlayHeight.ObjectAttributesAllowedRef() = ( SV_DEFAULT_VALUE_OBJECT_ATTRIBUTES & ~SV_PRINTABLE );

	m_dvoMatchScore.ObjectAttributesAllowedRef() = ( SV_DEFAULT_VALUE_OBJECT_ATTRIBUTES & ~SV_PRINTABLE );
}

BOOL SVOCVCharacterResultClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = SVTaskObjectClass::CreateObject( PCreateStructure );

	m_cvoLabelValue.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;

	m_dvoOverlayLeft.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	m_dvoOverlayTop.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	m_dvoOverlayWidth.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	m_dvoOverlayHeight.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;

	m_dvoMatchScore.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;

	m_isCreated = bOk;

	return bOk;
}// end CreateObject

BOOL SVOCVCharacterResultClass::CloseObject()
{
	BOOL bOk = SVTaskObjectClass::CloseObject();

	return bOk;
}// end CloseObject


BOOL SVOCVCharacterResultClass::onRun(SVRunStatusClass& RRunStatus)
{
	// Run base class
	BOOL bRetVal = SVTaskObjectClass::onRun(RRunStatus);
	
	return bRetVal;
}

