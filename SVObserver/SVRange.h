//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVRangeClass
//* .File Name       : $Workfile:   SVRange.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.7  $
//* .Check In Date   : $Date:   17 Feb 2015 18:14:06  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVTaskObject.h"
#include "ISVCancel.h"
#include "ObjectInterfaces\RangeEnum.h"
#pragma endregion Includes

////////////////////////////////////////////////////////////////////////////////
// .Title       : Class SVRangeClass
// -----------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
class SVRangeClass : public SVTaskObjectClass, public ISVCancel
{
	SV_DECLARE_CLASS( SVRangeClass );

public:
	SVRangeClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVRANGE );
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
	// Parameter: OUT SVObjectReference & ValueObjectReference
	// Returns:  bool:  true if valid reference
	//************************************
	static bool SetReference(LPCTSTR dottedName, SVObjectReference &ObjectReference);

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
	// Returns:  HRESULT:  S_OK if successful
	//************************************
	HRESULT GetIndirectValue(RangeEnum::ERange ra, CString& ref);

	//************************************
	// Description:  True if an indirect value exist.
	// Parameter: enum ERange ra
	// Returns:   bool
	//************************************
	bool HasIndirectValue(RangeEnum::ERange ra);
	
	//************************************
	// Description:  retrieve the direct value string for Erange
	// Parameter: enum ERange
	// Parameter: double & ref
	// Returns:  HRESULT:  S_OK if successful
	//************************************
	HRESULT GetValue(RangeEnum::ERange, double &ref);
	
	//************************************
	// Description:  retrieve the indirect object for ERange
	// Parameter: enum ERange
	// Returns:   SVStringValueObjectClass*
	//************************************
	SVStringValueObjectClass* GetIndirectObject(RangeEnum::ERange ra);

	//************************************
	// Description:  Set m_isValidRange to false.
	// Returns:   void
	//************************************
	void InvalidateRange();

	//************************************
	// Description:  Gets the updated Fail Low object
	// Parameter:  bucket <in>:  the bucket index
	// Returns:  const SVDoubleValueObjectClass&:  const reference to the Fail Low object
	//************************************
	const SVDoubleValueObjectClass& getUpdatedFailLow( int bucket );

	//************************************
	// Description:  Gets the value of the updated Fail High object
	// Parameter:  bucket <in>:  the bucket index
	// Returns:  const SVDoubleValueObjectClass&:  const reference to the Fail High object
	//************************************
	const SVDoubleValueObjectClass& getUpdatedFailHigh( int bucket );


	//************************************
	//! Updates the range value with the value from the indirect value if an indirect value exist.
	//! \param bucket [in] the bucket index
	//! \param range [in ]  enum ERange
	//! \returns void
	//************************************
	void   UpdateRange(int bucket, RangeEnum::ERange  range );
	
protected:
	virtual void init();
	BOOL getInputValue( double& RVal );
	BOOL onRun(SVRunStatusClass& RRunStatus);
	virtual DWORD_PTR processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext );

	//************************************
	//! function is called when an object was renamed
	//! \param pObject [in] object name 
	//! \param originalName [in] previous name 
	//! \returns BOOL
	//************************************
	virtual BOOL renameToolSetSymbol(const SVObjectClass* pObject, LPCTSTR originalName);


	//************************************
	//! return a reference to the range value 
	//! \param range [in] enum ERange
	//! \returns SVDoubleValueObjectClass&
	//************************************
	SVDoubleValueObjectClass&  GetRange(RangeEnum::ERange range);

public: // Bad
	SVDoubleValueObjectClass FailLow;
	SVDoubleValueObjectClass FailHigh;
	SVDoubleValueObjectClass WarnLow;
	SVDoubleValueObjectClass WarnHigh;

protected:
	
	SVStringValueObjectClass m_ValueIndirect[RangeEnum::ER_COUNT];
	bool m_IsConnectedInput[RangeEnum::ER_COUNT];
	SVObjectReference m_ValueObjectReferences[RangeEnum::ER_COUNT];
	bool m_isValidRange;
	SVInObjectInfoStruct m_inputObjectInfo;
};

