//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVRangeClassHelper
//* .File Name       : $Workfile:   RangeClassHelper.h  $
//* .Description     : Helper class for the SVRangeClass.  Implements the interface SVTaskObjectInterfaceClass.
//*                    Contains variables for range values.
//*                    Allows the RangeClass to get, set, and check the values.
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   14 Jan 2015 16:41:56  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVTaskObjectInterfaceClass.h"
#include "SVTaskObjectList.h"
#include "RangeEnum.h"
#pragma endregion Includes

#pragma region Declarations
class SVRangeClass;
class SVIPDoc;
#pragma endregion Declarations

class RangeClassHelper : public SVTaskObjectInterfaceClass
{
#pragma region Constructor
public:
	RangeClassHelper(SVRangeClass* PRange = nullptr);
	virtual ~RangeClassHelper();
#pragma endregion Constructor

#pragma region Public Methods
	//************************************
	/// Set the range object to this class.
	/// \param PRange [in] Range Object.
	//************************************
	void setRangeObject(SVRangeClass* PRange);

	//************************************
	// Description:  implements interface of SVTaskObjectInterfaceClass Set internal value to m_pRange
	// Returns:  HRESULT:  S_OK if successful
	//************************************
	virtual HRESULT SetInspectionData();

	//************************************
	// Description:  Get internal value Data From m_pRange 
	// Returns:  HRESULT:   S_OK if successful
	//************************************
	HRESULT GetAllInspectionData();

	//************************************
	// Description:  Get internal value Data From m_pRange for ra 
	// Parameter:  ra <in>:  what  limit 
	// Returns:  HRESULT  S_OK if successful
	//************************************
	HRESULT GetInspectionData(RangeEnum::ERange ra);

	//************************************
	// Description: Check if data are plausible
	// Parameter: CString& csmsg // @TODO:  Change to SVString
	// Returns:   HRESULT  S_OK if successful
	//************************************
	HRESULT CheckInternalData(CString &Errorsmsg) const;

	//************************************
	// Description:  Set the data
	// Parameter:  er <in>:  which range variable to set
	// Parameter:  lp <in>:  string what to set
	// Returns:  void, but throw an Exception from type ExceptionMgr1 if set failed
	//************************************
	 void SetInternalData(RangeEnum::ERange er, LPCTSTR lp);

	//************************************
	// Description:  set m_pRange as TaskObject
	// Returns:  void
	//************************************
	void SetRangeTaskObject();

	//************************************
	// Description: Get the range value string from the RangeObject could be a numeric value or an indirect object
	// Parameter:  ra <in>:  the enum value of the object
	// Returns:  CString   with the value 
	//************************************
	CString GetStringFromRange( RangeEnum::ERange ra ) const;

	//************************************
	// Description:  get the Owner name of the Range Object 
	// Returns:  LPCTSTR: the Owner name of tghe range object or Null.
	//************************************
	LPCTSTR GetOwnerName() const;

	//************************************
	// Description:  Change the indirect names, if necessary
	// Parameter:  oldPefix <in>:  old tool name
	// Parameter:  newPrefix <in>:  new tool name
	// Returns:  bool:  true if an indirect name was changed
	//************************************
	bool RenameIndirectValues(LPCTSTR oldPefix, LPCTSTR newPrefix);

	//************************************
	/// \returns SVString the Fail High value as string. 
	/// If it is a indirect value it return the name of the linked value.
	//************************************
	SVString GetFailHighString();
		
	//************************************
	/// \returns SVString the Warn High value as string. 
	/// If it is a indirect value it return the name of the linked value.
	//************************************
	SVString GetWarnHighString();

	//************************************
	/// \returns SVString the Fail Low value as string. 
	/// If it is a indirect value it return the name of the linked value.
	//************************************
	SVString GetFailLowString();

	//************************************
	/// \returns SVString the Warn Low value as string. 
	/// If it is a indirect value it return the name of the linked value.
	//************************************
	SVString GetWarnLowString();

	//************************************
	/// Fill the Object selector with filter and object, 
	/// so that the caller can open the selector with showDialog.
	/// \returns bool true if fill was succeeded.
	//************************************
	bool FillObjectSelector();


	//************************************
	// Description:  Checks if the owner of ref is a SVRangeObjectType
	// Parameter:  ref:  const reference to the object to check
	// Returns:  bool:  true if the object is owned by a range object else false
	//************************************
	static bool IsOwnedByRangeObject(const SVObjectClass& ref);

	//************************************
	// Description:  Checks if the value can be set to the  object is allowed to be set via remote interface
	// if the object is a reference object it is only allowed when we are not in run mode and it is a valid reference
	// Parameter:  ref <in>: the object
	// Parameter:  value<in?>:   
	// Parameter:  bOnline <in>:  true if we are in run mode
	// Parameter:  status <out>:  the reason why it is not allowed
	// Returns:  bool:  if true the value can be set
	//************************************
	static bool IsAllowedToSet(const SVObjectClass& ref, const CString& value, bool bOnline, HRESULT& hres);
#pragma endregion Public Methods

#pragma region Private Methods
	//************************************
	/// If indirect value is not empty it will return this, else it will return the direct value
	/// \param indirectString [in] The indirect string
	/// \param directValue [in,out] The direct value
	/// \returns SVString the value as string.
	//************************************
	SVString GetValueString(const CString& indirectString, double directValue);

	//************************************
	/// Checks the indirect reference string if it is valid
	/// \param rInspectionName [in] reference to the inspection name
	/// \param rIndirectString [in] reference top the indirect string
	/// \returns true if valid
	//************************************
	bool isValidReference( const CString& rInspectionName, const CString& rIndirectString ) const;
#pragma endregion Private Methods

#pragma region Member variables
private:
	SVRangeClass* m_pRange;
	double m_FailHigh;
	double m_WarnHigh;
	double m_WarnLow;
	double m_FailLow;
	CString m_FailHighIndirect;
	CString m_WarnHighIndirect;
	CString m_WarnLowIndirect;
	CString m_FailLowIndirect;
#pragma endregion Member variables
};

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\RangeClassHelper.h_v  $
 * 
 *    Rev 1.2   14 Jan 2015 16:41:56   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  979
 * SCR Title:  Provide additional options to input the feature range for the blob analyzer.
 * Checked in by:  mEichengruen;  Marcus Eichengruen
 * Change Description:  
 *   Removed Min/Max member variables, which were only used in one method.
 *   Cleaned up comment and spacing.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   13 Jan 2015 13:10:52   mEichengruen
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
 *    Rev 1.0   19 Dec 2014 13:53:42   mEichengruen
 * Project:  SVObserver
 * Change Request (SCR) nbr:  979
 * SCR Title:  Provide additional options to input the feature range for the blob analyzer.
 * Checked in by:  mEichengruen;  Marcus Eichengruen
 * Change Description:  
 *   Initial Check In for SCR 979 Indirect Range Objects 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/