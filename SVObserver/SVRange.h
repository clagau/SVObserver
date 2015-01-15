//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVRangeClass
//* .File Name       : $Workfile:   SVRange.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.5  $
//* .Check In Date   : $Date:   15 Jan 2015 08:28:58  $
//******************************************************************************

#ifndef SVRANGE_H
#define SVRANGE_H

#include "SVTaskObject.h"
#include "ISVCancel.h"

enum ERange { ER_FailHigh = 0, ER_WarnHigh, ER_FailLow, ER_WarnLow, ER_COUNT = 4 };

////////////////////////////////////////////////////////////////////////////////
// .Title       : Class SVRangeClass
// -----------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
class SVRangeClass : public SVTaskObjectClass, public ISVCancel
{
	SV_DECLARE_CLASS( SVRangeClass );

public:
	SVRangeClass( SVObjectClass* POwner = NULL, int StringResourceID = IDS_CLASSNAME_SVRANGE );
	virtual ~SVRangeClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );

	//************************************
	// Description:  call InitReferencesAndInputs and base class
	// Returns:  HRESULT:  S_OK if successful
	//************************************
	virtual HRESULT ResetObject();

	//************************************
	// Description:  Disconnect Inputs and call base class 
	// Returns:  BOOL:  TRUE if succesful
	//************************************
	virtual BOOL CloseObject();

	BOOL OnValidate();

	// ISVCancel interface
	virtual bool CanCancel();
	virtual HRESULT GetCancelData(SVCancelData*& rpData);
	virtual HRESULT SetCancelData(SVCancelData* pData);

	//************************************
	// Description:  Recalculate Reference Object for indirect range Variables.
	//               Mark reference object as Input.
	// Returns:  S_OK if references are valid
	//************************************
	HRESULT InitReferencesAndInputs();

	//************************************
	// Description:  Calculate Reference
	// Parameter: IN LPCTSTR dottedName
	// Parameter: OUT SVValueObjectReference & ValueObjectReference
	// Returns:  bool:  true if valid reference
	//************************************
	static bool SetReference(LPCTSTR dottedName, SVValueObjectReference &ValueObjectReference);

	//************************************
	// Description:  Connect all references as inputs.
	//************************************
	void ConnectAllInputObjects();

	//************************************
	// Description:  Disconnect all input references.
	//************************************
	void DisconnectAllInputObjects();

	//************************************
	// Description:  Retrieve the indirect value string for the specified ERange object
	// Parameter:  ra <in>:  specifies which range object to retrieve
	// Parameter:  ref <out>:  the returned indirect value string
	// Returns:  HRESULT:  S_OK if sucessful
	//************************************
	HRESULT GetIndirectValue(enum ERange ra, CString& ref);

	//************************************
	// Description:  True if an indirect value exist.
	// Parameter: enum ERange ra
	// Returns:   bool
	//************************************
	bool HasIndirectValue(enum ERange ra);
	
	//************************************
	// Description:  retrieve the direct value string for Erange
	// Parameter: enum ERange
	// Parameter: double & ref
	// Returns:   HRESULT
	//************************************
	HRESULT GetValue(enum ERange, double &ref);
	
	//************************************
	// Description:  retrieve the indirect object for ERange
	// Parameter: enum ERange
	// Returns:   SVStringValueObjectClass*
	//************************************
	SVStringValueObjectClass* GetIndirectObject(enum ERange);

	//************************************
	// Description:  Set m_isValidRange to false.
	// Returns:   void
	//************************************
	void InvalidateRange();

protected:
	virtual void init();
	BOOL getInputValue( double& RVal );
	BOOL onRun(SVRunStatusClass& RRunStatus);
	virtual DWORD_PTR processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext );
	//************************************
	//! function is called when an object was renamed
	//! \param pObject [in] object name 
	//! \param orgName [in] previous name 
	//! \returns BOOL
	//************************************
	virtual BOOL renameToolSetSymbol(SVObjectClass* pObject, LPCTSTR orgName);
	public:
	SVDoubleValueObjectClass FailLow;
	SVDoubleValueObjectClass FailHigh;
	SVDoubleValueObjectClass WarnLow;
	SVDoubleValueObjectClass WarnHigh;

protected:
	// BRW - It seems to me like it be easier to keep these 3 items together in a struct than in 3 different arrays that must be synchronized.
	SVStringValueObjectClass m_ValueIndirect[ER_COUNT];
	bool m_IsConnectedInput[ER_COUNT];
	SVValueObjectReference m_ValueObjectReferences[ER_COUNT];
	bool m_isValidRange;
	SVInObjectInfoStruct m_inputObjectInfo;
};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVRange.h_v  $
 * 
 *    Rev 1.5   15 Jan 2015 08:28:58   mEichengruen
 * Project:  SVObserver
 * Change Request (SCR) nbr:  979
 * SCR Title:  Provide additional options to input the feature range for the blob analyzer.
 * Checked in by:  mEichengruen;  Marcus Eichengruen
 * Change Description:  
 *   Change Returnvalue for InitReferencesAndInput to HRESULT
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   13 Jan 2015 13:10:52   mEichengruen
 * Project:  SVObserver
 * Change Request (SCR) nbr:  979
 * SCR Title:  Provide additional options to input the feature range for the blob analyzer.
 * Checked in by:  mEichengruen;  Marcus Eichengruen
 * Change Description:  
 *   Range Indirect name String without inspection Name 
 * Rename Range Indirect name String when a Toolname  is renamed  
 * add function to rename in direct values 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   19 Dec 2014 14:22:02   mEichengruen
 * Project:  SVObserver
 * Change Request (SCR) nbr:  979
 * SCR Title:  Provide additional options to input the feature range for the blob analyzer.
 * Checked in by:  mEichengruen;  Marcus Eichengruen
 * Change Description:  
 *   add SVStringValueObjectClass and SVValueObjectReference to SVRange 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   15 May 2014 11:56:10   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed processMessage signature to use DWORD_PTR.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   01 Feb 2014 12:03:56   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SVSendmessage and processmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 14:39:00   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.5   19 Jun 2012 14:12:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Protected Object Information structure attributes.  This change will add accessor methods to get and set attributes.
 * Converted GUID to SVGUID.   This allows the use of standard operators for comparison and methods for conversion.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   02 Sep 2005 14:33:40   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  492
 * SCR Title:  Upgrade SVObserver to version 4.60
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   made speed optimizations by making functions inline
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   12 Jul 2004 11:59:28   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  406
 * SCR Title:  Implement External Tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   made SVRangeClass implement ISVCancel
 * cleaned up code
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   22 Apr 2003 14:20:00   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   04 Apr 2002 10:21:12   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  286
 * SCR Title:  Upgrade SVObserver to Version 3.37 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code syntax to allow rational rose to reverse engineer properly.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 13:28:58   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   27 Sep 1999 15:57:26   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin. Moved SVRangeClass from SVResult.cpp
 *  and SVResult.h
 * Revised onRun Method to correct problem with testing
 * FailHigh/WarnHigh/WarnLow/FailLow values.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/