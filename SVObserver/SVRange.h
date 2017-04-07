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
#include "SVOCore/SVTaskObject.h"
#include "SVLibrary/ISVCancel.h"
#include "ObjectInterfaces\RangeEnum.h"
#include "SVUtilityLibrary/SVString.h"
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

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure ) override;

	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

	//************************************
	// Description:  Disconnect Inputs and call base class 
	// Returns:  BOOL:  TRUE if succesful
	//************************************
	virtual BOOL CloseObject() override;

	// ISVCancel interface
	virtual bool CanCancel() override;
	virtual HRESULT GetCancelData(SVCancelData*& rpData) override;
	virtual HRESULT SetCancelData(SVCancelData* pData) override;

	//************************************
	// Description:  Recalculate Reference Object for indirect range Variables.
	//               Mark reference object as Input.
	// Returns:  true if references are valid
	//************************************
	bool InitReferencesAndInputs(SvStl::MessageContainerVector *pErrorMessages=nullptr);

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
	HRESULT GetIndirectValue( RangeEnum::ERange ra, SVString& rValue );

	//************************************
	// Description:  True if an indirect value exist.
	// Parameter: enum ERange ra
	// Returns:   bool
	//************************************
	bool HasIndirectValue(RangeEnum::ERange ra);
	
	//************************************
	// Description:  retrieve the direct value string for Erange
	// Parameter: enum ERange
	// Parameter: double rValue reference to the value 
	// Returns:  HRESULT:  S_OK if successful
	//************************************
	HRESULT GetValue(RangeEnum::ERange, double& rValue);
	
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
	// Description:  Gets the value of the selected Range  object
	// Parameter:  bucket <in>:  the bucket index
	// Parameter RangeEnum::ERange ra the selected range object 
	// Returns:  const SVDoubleValueObjectClass&:  const reference to the Fail High object
	//************************************
	const SVDoubleValueObjectClass& SVRangeClass::getUpdatedRange( RangeEnum::ERange ra  ,int bucket );

	//************************************
	//! Updates the range value with the value from the indirect value if an indirect value exist.
	//! \param bucket [in] the bucket index
	//! \param range [in ]  enum ERange
	//! \returns void
	//************************************
	void   UpdateRange(int bucket, RangeEnum::ERange  range );

#pragma region Methods to replace processMessage
	virtual bool DisconnectObjectInput( SVInObjectInfoStruct* pObjectInInfo ) override;
	virtual void OnObjectRenamed(const SVObjectClass& rRenamedObject, const SVString& rOldName) override;
#pragma endregion Methods to replace processMessage
	
protected:
	void init();
	BOOL getInputValue( double& RVal );
	virtual bool onRun(SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

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

