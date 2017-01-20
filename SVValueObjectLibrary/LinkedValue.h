//*****************************************************************************
/// \copyright (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
/// \Author	Marc Ziegler
//*****************************************************************************
/// This class should managed value object which are linked to other value objects (or only a valid value).
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVObjectLibrary\SVInObjectInfoStruct.h"
#include "SVUtilityLibrary\SVString.h"
#include "SVStaticStringValueObjectClass.h"
#include "SVVariantValueObjectClass.h"
#pragma endregion Includes

#pragma region Declarations
#pragma endregion Declarations

class LinkedValue : public SVVariantValueObjectClass
{
#pragma region Constructor
public:
	LinkedValue();

	virtual ~LinkedValue();
#pragma endregion Constructor

#pragma region Public Methods
public:

	//************************************
	/// Return the current value. If a valid linked value this will be returned otherwise it will return the the variant value.
	/// \param Bucket [in] bucket to get
	/// \param Index [in]  index of array to get
	/// \param rValue [out] reference to the value to write to
	/// \returns S_OK if successful
	//************************************
	virtual HRESULT GetValueAt( int Bucket, int Index, VARIANT& rValue ) const override;
	virtual HRESULT GetValueAt( int Bucket, int Index, SVString& rValue ) const override;
	virtual HRESULT GetValueAt( int Bucket, int Index, BOOL& rValue ) const override;
	virtual HRESULT GetValueAt( int Bucket, int Index, double& rValue ) const override;
	virtual HRESULT GetValueAt( int Bucket, int Index, long& rValue ) const override;
	virtual HRESULT GetValueAt( int Bucket, int Index, DWORD& rValue ) const override;
	virtual HRESULT GetValueAt( int Bucket, int Index, BYTE& rValue ) const override;

	//************************************
	/// Set the value. If string a valid dotted name of a value object, it connect it to the linked object.
	/// \returns HRESULT S_OK, if set was OK.
	//************************************
	virtual HRESULT SetValueAt( int Bucket, int Index, const SVString& rValue ) override;

	virtual bool DisconnectObjectInput( SVInObjectInfoStruct* pObjectInInfo ) override;

	//************************************
	/// Update the linked name 
	/// \returns HRESULT
	//************************************
	void UpdateLinkedName();

	SVStaticStringValueObjectClass& getLinkedName() { return m_LinkedName; };

	bool isIndirectValue() { return ( nullptr != m_pLinkedObject ); };

#pragma region Methods to replace processMessage
	virtual void OnObjectRenamed(const SVObjectClass& rRenamedObject, const SVString& rOldName) override { UpdateLinkedName(); };
#pragma endregion Methods to replace processMessage
#pragma endregion Public Methods

#pragma region Protected Methods
protected:
	virtual void ValidateValue( int iBucket, int iIndex, const SVString& rValue ) const override;
#pragma endregion Protected Methods

#pragma region Private Methods
private:
	//************************************
	/// Update the input connection, dependent of the variant value.
	/// \returns HRESULT
	//************************************
	HRESULT UpdateConnection();

	//************************************
	/// Disconnected the input connection and set it to nullptr.
	//************************************
	void DisconnectInput();

	//************************************
	/// Connect the input connection with a new object
	/// \returns bool
	//************************************
	bool ConnectInput();

	virtual HRESULT ResetObject() override;

	/// Convert a string (dotted name) to an object.
	/// \param rValue [in] Input string
	/// \returns SVObjectClass* The founded object. If object not found, return a nullptr.
	SVObjectClass* ConvertStringInObject( const SVString& rValue ) const;

	//! Checks if the linked object is valid.
	//! \param pLinkedObject [in] Pointer to the indirect object
	//! \returns S_OK if linked object is valid
	HRESULT CheckLinkedObject( const SVObjectClass* const pLinkedObject ) const;
#pragma endregion Private Methods

#pragma region Member Variables
private:
	SVStaticStringValueObjectClass m_LinkedName;
	SVObjectClass* m_pLinkedObject;
	SVGUID m_LinkedUid;
	mutable bool m_CircularReference;					//! Use this flag during GetValue to make sure no circular references are present
#pragma endregion Member Variables
};
