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
#include "SVStaticStringValueObjectClass.h"
#include "SVVariantValueObjectClass.h"
#include "SVTaskObject.h"
#pragma endregion Includes


#pragma region Declarations
#pragma endregion Declarations

class LinkedValue : public SVVariantValueObjectClass
{
#pragma region Constructor
public:
	LinkedValue();

	~LinkedValue();
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
	virtual HRESULT GetValueAt( int Bucket, int Index, CString& rValue ) const override;
	virtual HRESULT GetValueAt( int Bucket, int Index, BOOL& rValue ) const override;
	virtual HRESULT GetValueAt( int Bucket, int Index, double& rValue ) const override;
	virtual HRESULT GetValueAt( int Bucket, int Index, long& rValue ) const override;
	virtual HRESULT GetValueAt( int Bucket, int Index, DWORD& rValue ) const override;
	virtual HRESULT GetValueAt( int Bucket, int Index, BYTE& rValue ) const override;

	//************************************
	/// Set the value. If string a valid dotted name of a value object, it connect it to the linked object.
	/// \returns HRESULT S_OK, if set was OK.
	//************************************
	virtual HRESULT SetValueAt( int Bucket, int Index, CString Value ) override;

	//************************************
	/// Disconnect the object if it is connected to this value.
	/// \param pObject [in] Pointer to the object to disconnect.
	//************************************
	void DisconnectObject( const SVObjectClass* const pObject );

	//************************************
	/// Update the linked name 
	/// \returns HRESULT
	//************************************
	void UpdateLinkedName();

	SVStaticStringValueObjectClass& getLinkedName() { return m_LinkedName; };
#pragma endregion Public Methods

#pragma region Protected Methods
protected:

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
	/// \returns DWORD_PTR
	//************************************
	DWORD_PTR ConnectInput();

	virtual DWORD_PTR processMessage( DWORD MessageID, DWORD_PTR MessageValue, DWORD_PTR MessageContext ) override;
	virtual HRESULT ResetObject() override;

#pragma endregion Private Methods

#pragma region Member Variables
private:
	SVStaticStringValueObjectClass m_LinkedName;
	SVObjectClass* m_pLinkedObject;
	SVGUID m_LinkedUid;
#pragma endregion Member Variables
};
