// ******************************************************************************
// * COPYRIGHT (c) 2005 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVLinearEdgeBProcessingClass
// * .File Name       : $Workfile:   SVLinearEdgeBProcessingClass.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   23 Apr 2013 11:53:16  $
// ******************************************************************************

#include "stdafx.h"
#include "SVLinearEdgeBProcessingClass.h"
#include "SVGlobal.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SV_IMPLEMENT_CLASS( SVLinearEdgeBProcessingClass, SVLinearEdgeBProcessingClassGuid );


SVLinearEdgeBProcessingClass::SVLinearEdgeBProcessingClass( SVObjectClass* POwner, int StringResourceID )
					                   :SVLinearEdgeProcessingClass( POwner, StringResourceID )
{
	outObjectInfo.ObjectTypeInfo.SubType = SVLinearEdgeBProcessingObjectType;

	RegisterEmbeddedObject( &m_svDirection, SVEdgeBDirectionGuid, IDS_OBJECTNAME_EDGE_B_DIRECTION, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_svPolarisation, SVEdgeBPolarisationGuid, IDS_OBJECTNAME_EDGE_B_POLARISATION, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_svEdgeSelect, SVEdgeBEdgeSelectGuid, IDS_OBJECTNAME_EDGE_B_EDGE_SELECT, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_svEdgeSelectThisValue, SVEdgeBEdgeSelectThisValueGuid, IDS_OBJECTNAME_EDGE_B_EDGE_SELECT_THIS_VALUE, false, SVResetItemNone );

	RegisterEmbeddedObject( &m_svIsFixedEdgeMarker, SVEdgeBIsFixedEdgeMarkerGuid, IDS_OBJECTNAME_EDGE_B_IS_FIXED_EDGE_MARKER, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_svPosition, SVEdgeBPositionGuid, IDS_OBJECTNAME_EDGE_B_POSITION, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_svPositionOffsetValue, SVEdgeBPositionOffsetGuid, IDS_OBJECTNAME_EDGE_B_POSITION_OFFSET, false, SVResetItemNone );

	RegisterEmbeddedObject( &m_svUseLowerThresholdSelectable, SVUseLowerThresholdSelectableBObjectGuid, IDS_OBJECTNAME_USELOWERTHRESHOLD_SELECTABLE_B, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_svUseLowerThresholdMaxMinusPercentDiff, SVUseLowerThresholdMaxMinusPercentDiffBObjectGuid, IDS_OBJECTNAME_USELOWERTHRESHOLD_MAXMINUSPERCENTDIFF_B, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_svUseLowerThresholdMaxMinusOffset, SVUseLowerThresholdMaxMinusOffsetBObjectGuid, IDS_OBJECTNAME_USELOWERTHRESHOLD_MAXMINUSOFFSET_B, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_svUseLowerThresholdMinPlusOffset, SVUseLowerThresholdMinPlusOffsetBObjectGuid, IDS_OBJECTNAME_USELOWERTHRESHOLD_MINPLUSOFFSET_B, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_svLowerThresholdValue, SVEdgeBLowerThresholdValueGuid, IDS_OBJECTNAME_EDGE_B_LOWER_THRESHOLD_VALUE, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_svLowerMaxMinusPercentDiffValue, SVLowerThresholdMaxMinusPercentDiffBObjectGuid, IDS_OBJECTNAME_LOWERTHRESHOLD_PERCENTDIFF_B, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_svLowerMaxMinusOffsetValue, SVLowerThresholdMaxMinusOffsetBObjectGuid, IDS_OBJECTNAME_LOWERTHRESHOLD_MAXOFFSET_B, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_svLowerMinPlusOffsetValue, SVLowerThresholdMinPlusOffsetBObjectGuid, IDS_OBJECTNAME_LOWERTHRESHOLD_MINOFFSET_B, false, SVResetItemNone );

	RegisterEmbeddedObject( &m_svUseUpperThresholdSelectable, SVUseUpperThresholdSelectableBObjectGuid, IDS_OBJECTNAME_USEUPPERTHRESHOLD_SELECTABLE_B, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_svUseUpperThresholdMaxMinusPercentDiff, SVUseUpperThresholdMaxMinusPercentDiffBObjectGuid, IDS_OBJECTNAME_USEUPPERTHRESHOLD_MAXMINUSPERCENTDIFF_B, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_svUseUpperThresholdMaxMinusOffset, SVUseUpperThresholdMaxMinusOffsetBObjectGuid, IDS_OBJECTNAME_USEUPPERTHRESHOLD_MAXMINUSOFFSET_B, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_svUseUpperThresholdMinPlusOffset, SVUseUpperThresholdMinPlusOffsetBObjectGuid, IDS_OBJECTNAME_USEUPPERTHRESHOLD_MINPLUSOFFSET_B, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_svUpperThresholdValue, SVEdgeBUpperThresholdValueGuid, IDS_OBJECTNAME_EDGE_B_UPPER_THRESHOLD_VALUE, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_svUpperMaxMinusPercentDiffValue, SVUpperThresholdMaxMinusPercentDiffBObjectGuid, IDS_OBJECTNAME_UPPERTHRESHOLD_PERCENTDIFF_B, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_svUpperMaxMinusOffsetValue, SVUpperThresholdMaxMinusOffsetBObjectGuid, IDS_OBJECTNAME_UPPERTHRESHOLD_MAXOFFSET_B, false, SVResetItemNone );
	RegisterEmbeddedObject( &m_svUpperMinPlusOffsetValue, SVUpperThresholdMinPlusOffsetBObjectGuid, IDS_OBJECTNAME_UPPERTHRESHOLD_MINOFFSET_B, false, SVResetItemNone );

	RegisterEmbeddedObject(&m_svLinearEdges, SVLinearEdgesClassGuid, IDS_CLASSNAME_SVLINEAREDGESCLASS, false, SVResetItemNone );

	m_svDirection.SetEnumTypes( g_strDirectionEnums );
	m_svPolarisation.SetEnumTypes( g_strPolarisationEnums );
	m_svEdgeSelect.SetEnumTypes( g_strEdgeSelectEnums );
	m_svPosition.SetEnumTypes( g_strPositionEnums );

	// Set default inputs and outputs
	addDefaultInputObjects();
}

SVLinearEdgeBProcessingClass::~SVLinearEdgeBProcessingClass()
{
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVLinearEdgeBProcessingClass.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 11:53:16   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   27 Jan 2011 11:28:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   01 Jun 2010 14:41:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to improve ability to track performance.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   01 Aug 2005 11:59:08   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed VectorObject to ValueObject
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   29 Jul 2005 12:39:52   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated object initialization to change the parameters for RegisterEmbeddedObject for a SVValueObjectClass.  The two additional parameters will inform the preperation process before toolset execution.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Feb 2005 10:43:22   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  463
 * SCR Title:  Add new Linear Measurement Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Adding new files for the new linear tool.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
