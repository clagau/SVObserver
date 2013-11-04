//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOCVCharacterResultClass
//* .File Name       : $Workfile:   SVOCVBlobRecord.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:01:14  $
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
	isObjectValid.ObjectAttributesAllowedRef() = SV_EMBEDABLE;
	statusColor.ObjectAttributesAllowedRef() = SV_EMBEDABLE;
	statusTag.ObjectAttributesAllowedRef() = SV_EMBEDABLE;

	m_cvoLabelValue.ObjectAttributesAllowedRef() = SV_EMBEDABLE;

	m_dvoOverlayLeft.ObjectAttributesAllowedRef() = SV_EMBEDABLE;
	m_dvoOverlayTop.ObjectAttributesAllowedRef() = SV_EMBEDABLE;
	m_dvoOverlayWidth.ObjectAttributesAllowedRef() = SV_EMBEDABLE;
	m_dvoOverlayHeight.ObjectAttributesAllowedRef() = SV_EMBEDABLE;

	m_dvoMatchScore.ObjectAttributesAllowedRef() = SV_EMBEDABLE;
}

void SVOCVCharacterResultClass::UnhideResults()
{
	isObjectValid.ObjectAttributesAllowedRef() = ( SV_DEFAULT_VALUE_OBJECT_ATTRIBUTES & ~SV_PRINTABLE );
	statusColor.ObjectAttributesAllowedRef() = ( SV_DEFAULT_VALUE_OBJECT_ATTRIBUTES & ~SV_PRINTABLE );
	statusTag.ObjectAttributesAllowedRef() = ( SV_DEFAULT_VALUE_OBJECT_ATTRIBUTES & ~SV_PRINTABLE );

	m_cvoLabelValue.ObjectAttributesAllowedRef() = ( SV_DEFAULT_VALUE_OBJECT_ATTRIBUTES & ~(SV_PRINTABLE | SV_CH_CONDITIONAL) );

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

	isCreated = bOk;

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

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
DWORD SVOCVCharacterResultClass::processMessage( DWORD DwMessageID, DWORD DwMessageValue, DWORD DwMessageContext )
{
	DWORD DwResult = NULL;

	return( SVTaskObjectClass::processMessage( DwMessageID, DwMessageValue, DwMessageContext ) | DwResult );
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVOCVBlobRecord.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 13:01:14   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   01 Jun 2010 14:58:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to improve ability to track performance.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   16 Dec 2009 12:16:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   07 Mar 2008 07:33:22   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  616
 * SCR Title:  Add Server-side Configuration Data Def Lists to the SVObserver program
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add Extent Type Names to Value Objects for Data Definition Lists
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   20 Jan 2006 15:07:18   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  529
 * SCR Title:  Add Conditional Product History
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   SV_ALL_ATTRIBUTES was renamed to SV_DEFAULT_ATTRIBUTES
 * added SV_DEFAULT_VALUE_OBJECT_ATTRIBUTES
 * added SV_DEFAULT_IMAGE_OBJECT_ATTRIBUTES
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   29 Jul 2005 13:08:06   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated object initialization to change the parameters for RegisterEmbeddedObject for a SVValueObjectClass.  The two additional parameters will inform the preperation process before toolset execution.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   21 Jun 2005 08:21:20   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   object attributes now use accessor methods
 * value object functions now use HRESULT
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   18 Feb 2005 07:39:18   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   18 Feb 2005 07:37:50   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   16 Feb 2005 14:53:48   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  440
 * SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   updated header files
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   17 Sep 2003 13:07:50   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  384
 * SCR Title:  Add new Matrox OCR/OCV analyzer to SVObserver
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Created and integrated a new Matrox based OCR/OCV analyzer to SVObserver.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
