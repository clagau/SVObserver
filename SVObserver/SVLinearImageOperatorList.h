//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLinearImageOperatorList
//* .File Name       : $Workfile:   SVLinearImageOperatorList.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 11:54:06  $
//******************************************************************************

//******************************************************************************
//* INCLUDE CONTROL:
//******************************************************************************
#ifndef SVLINEARIMAGEOPERATORLIST_H
#define SVLINEARIMAGEOPERATORLIST_H

//******************************************************************************
//* INCLUDE(S):
//******************************************************************************

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

////////////////////////////////////////////////////////////////////////////////
// Includes which are necessary to declare or define types, constants and macros
////////////////////////////////////////////////////////////////////////////////

#include "SVUnaryImageOperatorList.h"

class SVLinearImageOperatorListClass : public SVStdImageOperatorListClass 
{
	SV_DECLARE_CLASS( SVLinearImageOperatorListClass )

public:
	SVLinearImageOperatorListClass( BOOL BCreateDefaultTaskList = FALSE, SVObjectClass* POwner = NULL , int StringResourceID = IDS_CLASSNAME_SVLINEARIMAGEOPERATORLIST );
	virtual ~SVLinearImageOperatorListClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );
	virtual BOOL CloseObject();

	virtual HRESULT ResetObject();

	virtual BOOL OnValidate();
	virtual BOOL Run( SVRunStatusClass& RRunStatus );

protected:
	SVBoolValueObjectClass *getUseRotationAngle();
	SVEnumerateValueObjectClass *getInputProfileOrientation();

	SVInObjectInfoStruct inputUseRotationAngle;
	SVInObjectInfoStruct inputProfileOrientation;

	SVDoubleValueObjectClass m_svLinearData;

	SVDoubleValueObjectClass m_svMinThreshold;
	SVDoubleValueObjectClass m_svMaxThreshold;

	SVDataBufferClass m_svProfileResultData;

	unsigned long m_ulLineLength;
	SVMatroxLongArray m_aulLineData;

	std::vector<double> m_svArray;

private:
	void init();

	HRESULT UpdateLineExtentData();
};

//******************************************************************************
//* INCLUDE CONTROL:
//******************************************************************************

#endif	//	SVLINEARIMAGEOPERATORLIST_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVLinearImageOperatorList.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 11:54:06   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   24 Jul 2007 14:27:08   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Update to MIL 8.0
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   01 Aug 2005 11:59:10   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed VectorObject to ValueObject
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Feb 2005 10:43:20   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  463
 * SCR Title:  Add new Linear Measurement Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Adding new files for the new linear tool.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/