//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOPPQObj
//* .File Name       : $Workfile:   SVOPPQObj.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:10:58  $
//******************************************************************************

#pragma once

#pragma region Includes
#include <deque>
#include <utility>
#include "SVUtilityLibrary/SVString.h"
#include "SVImportedInputList.h"
#include "SVUtilityLibrary\SVSharedPtr.h"
#pragma endregion Includes

typedef std::pair<SVString, GUID> SVNameGuidPair;
typedef std::deque<SVNameGuidPair> SVNameGuidPairList;

class SVOPPQObj  
{
public:
	SVOPPQObj();
	virtual ~SVOPPQObj();

	SVOPPQObj& operator=(const SVOPPQObj &source);

	void SetPPQName(CString sPPQName);
	CString GetPPQName();

	void AttachTriggerToPPQ(CString sTriggerName);
	void DetachTriggerFromPPQ();
	CString GetAttachedTriggerName() const;

	BOOL AttachCameraToPPQ(CString sCameraName);
	BOOL DetachCameraFromPPQ(CString sCameraName);
	int GetAttachedCameraCount() const;
	CString GetAttachedCamera(int ipos) const;

	BOOL AttachInspectionToPPQ(CString sInspectName);
	BOOL DetachInspectionFromPPQ(CString sInspectName);
	int GetAttachedInspectionCount();
	CString GetAttachedInspection(int iPos);

	//also have methods for properties
	void SetPPQMode(int iMode);
	void SetPPQLength(long lLength);
	void SetPPQOutputResetDelay(long lResetDelay);
	void SetPPQOutputDelayTime(long lDelayTime);
	void SetMaintainSourceImageProperty(bool bValue);
	void SetInspectionTimeout( long lTimeoutMillisec );

	int GetPPQMode() const;
	long GetPPQLength() const;
	long GetPPQOutputResetDelay() const;
	long GetPPQOutputDelayTime() const;
	bool GetMaintainSourceImageProperty() const;
	long GetInspectionTimeout() const;

	void SetAvailableInputsForConditionalOutput(const SVNameGuidPairList& list);
	const SVNameGuidPairList& GetAvailableInputsForConditionalOutput() const;

	void SetConditionalOutputName(const SVString& name);
	const SVString& GetConditionalOutputName() const;
	bool IsConditionalOutputCameraInput() const;
	void RemoveCameraInputConditionalOutput();

	void SetImportedInputList(const SVImportedInputList& list);
	const SVImportedInputList& GetImportedInputList() const;
	void ClearImportedInputList();

private:
	CString m_sPPQName;
	CString m_sAttachedTrigger;
	CStringList m_slAttachedCameraList;
	CStringList m_slAttachedInspectList;

	//properties
	int m_iPPQMode;
	long m_lPPQLength;
	long m_lPPQOutputResetDelay;
	long m_lPPQOutputDelayTime;
	bool m_bMaintainSrcImg;
	long m_lInspectionTimeout;
	SVString m_conditionalOutputName;
	SVNameGuidPairList m_availableInputs;
	SVImportedInputList m_importedInputList;
};

typedef SVSharedPtr< SVOPPQObj > SVOPPQObjPtr;


//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVOPPQObj.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 13:10:58   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   05 Mar 2013 16:41:40   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  817
 * SCR Title:  Import/Export of Inspections
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added Inspection Import list
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   21 Jan 2013 11:34:48   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  809
 * SCR Title:  Non I/O SVIM
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added support for Conditional outputs
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   16 Jan 2013 16:35:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  802
 * SCR Title:  Add new product type GD1A
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Migrating branch code into primary code base.  
 * 
 * Updated method with const correctness.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   05 Nov 2010 11:08:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  703
 * SCR Title:  Allow SVObserver to Change Remote Input Values
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove redundent data objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   10 Sep 2008 16:43:56   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised GetAttachedTriggerName, GetAttachedCameraCount, and GetAttachedCamera to be const
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   22 Jan 2007 09:29:04   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  587
 * SCR Title:  Add Timeout Threshold to Extended Time Delay Mode
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added code for the new Inspection Timeout Threshold for the Extended Time Delay Mode.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   22 Apr 2003 13:17:26   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   10 Feb 2003 13:27:16   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  316
 * SCR Title:  Add option to extent acquisition image source buffers to the length of the PPQ
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added code for the new PPQ property. 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Nov 2002 13:39:18   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   This is the first revision of this file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
