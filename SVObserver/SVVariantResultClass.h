//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVVariantResultClass
//* .File Name       : $Workfile:   SVVariantResultClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 16:19:34  $
//******************************************************************************

#if !defined(AFX_SVVARIANTRESULTCLASS_H__CA11DFAC_CF8C_4EAB_AC1B_52A03437666D__INCLUDED_)
#define AFX_SVVARIANTRESULTCLASS_H__CA11DFAC_CF8C_4EAB_AC1B_52A03437666D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SVResult.h"
#include "SVValueObject.h"

class SVVariantResultClass : public SVResultClass  
{
public:
	SVVariantResultClass( BOOL BCreateDefaultTaskList = FALSE, SVObjectClass* POwner = NULL , int StringResourceID = IDS_CLASSNAME_RESULT_VARIANT );
	virtual ~SVVariantResultClass();
	SV_DECLARE_CLASS( SVVariantResultClass );

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );
	virtual BOOL CloseObject();
	virtual BOOL OnValidate();

protected:
	SVValueObjectClass* GetInputValue();
	virtual BOOL onRun( SVRunStatusClass& RRunStatus );

protected:
	// Input
	SVInObjectInfoStruct		m_InputValueObjectInfo;

	// Output
	SVVariantValueObjectClass	m_Value;
};

#endif // !defined(AFX_SVVARIANTRESULTCLASS_H__CA11DFAC_CF8C_4EAB_AC1B_52A03437666D__INCLUDED_)

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVVariantResultClass.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 16:19:34   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   17 Feb 2005 14:36:36   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Modifications to support new reset and Create methodology.
 * Removed SetObjectDepth
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   08 Jul 2004 12:40:18   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  406
 * SCR Title:  Implement External Tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   first iteration
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/