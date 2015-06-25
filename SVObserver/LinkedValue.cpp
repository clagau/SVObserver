//*****************************************************************************
/// \copyright (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
/// \Author	Jane Doe
//*****************************************************************************
/// PLEASE 
/// ENTER 
/// A DESCRIPTION
//*****************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "LinkedValue.h"
#include "BasicValueObject.h"
#include "SVInspectionProcess.h"
#include "SVObjectLibrary\SVObjectManagerClass.h"
#include "ObjectInterfaces\ErrorNumbers.h"
#include "SVObjectLibrary\GlobalConst.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

#pragma region Constructor
LinkedValue::LinkedValue()
	: m_isValid(false)
	,m_isInvalidStringSetToDefault(true)
	,m_pInspection(nullptr)
{

}

LinkedValue::~LinkedValue()
{

}
#pragma endregion Constructor

#pragma region Public Methods
void LinkedValue::init(SVInspectionProcess *pInspection, bool isInvalidStringSetToDefault)
{
	m_pInspection = pInspection;
	m_isInvalidStringSetToDefault = isInvalidStringSetToDefault;
	m_isValid = (m_pInspection != nullptr);
}

void LinkedValue::RegisterObjects(SVTaskObjectClass &taskObject, const SVString& inputConnectionName, const GUID& rguidEmbeddedID, int iStringResourceID, bool bResetAlways, SVResetItemEnum eRequiredReset)
{
	taskObject.RegisterEmbeddedObject( &m_variantObject, rguidEmbeddedID, iStringResourceID, bResetAlways, eRequiredReset );
	m_inputConnectionInfo.SetObject( taskObject.GetObjectInfo() );
	m_inputConnectionInfo.SetInputObject(nullptr);	
}

void LinkedValue::setDefaultValue(const VARIANT value, bool isResetAll)
{
	if (isResetAll)
	{
		//try to disconnect the last connected object, but no check if it work, because if it is not anymore connected it is also fine.
		DisconnectInput();
	}
	m_variantObject.SetDefaultValue(value, isResetAll);
}

_variant_t LinkedValue::GetValue() const
{
	_variant_t value;
	value.Clear();

	SVObjectClass *pObject = m_inputConnectionInfo.GetInputObjectInfo().PObject;
	if( SV_IS_KIND_OF( pObject, SVValueObjectClass ) )
	{
		(static_cast <SVValueObjectClass*> (pObject) )->GetValue( value );
	}
	else if( SV_IS_KIND_OF( pObject, BasicValueObject ) )
	{
		static_cast <BasicValueObject*> (pObject)->getValue( value );
	}
	else
	{
		m_variantObject.GetValue( value );
	}

	if (S_OK != ::VariantChangeType( &value, &value, VARIANT_ALPHABOOL, m_variantObject.GetDefaultType() ) )
	{
		value.Clear(); //empty value if variant can not be converted in the right type
	}

	return value;
}

SVString LinkedValue::GetInputValue() const
{
	SVString retValue = "";
	_variant_t value;
	m_variantObject.GetValue( value );

	if( (value.vt & VT_ARRAY) != VT_ARRAY)
	{
		VARTYPE oldType = value.vt;
		HRESULT hr = ::VariantChangeTypeEx(&value, &value, SvOl::LCID_USA, VARIANT_ALPHABOOL, VT_BSTR);	// use United States locale
		if ( hr == S_OK )
		{
			retValue = value.bstrVal;
			if( oldType == VT_BOOL )
			{
				retValue.MakeUpper();
			}
		}
	}

	return retValue;
}

HRESULT LinkedValue::SetInputValue(const SVString& valueString)
{
	HRESULT retVal = S_OK;
	int iBucket = 1;

	SVObjectClass* pOldObject = m_inputConnectionInfo.GetInputObjectInfo().PObject;
	SVObjectClass* pObject = nullptr;
	SVObjectManagerClass::Instance().GetObjectByDottedName( valueString, pObject );

	if ( nullptr != pOldObject )
	{	// disconnect existing connection
		DWORD_PTR messageReturn = DisconnectInput();
		ASSERT( SVMR_SUCCESS == messageReturn );
		//return value will not changed if disconnect failed, because if old objected is not connected anymore, the new value will tried to be set however.
	}

	if ( nullptr != pObject )
	{
		retVal = m_variantObject.SetValue(iBucket, valueString.c_str());
		if (S_OK == retVal)
		{
			DWORD_PTR messageReturn = ConnectInput(pObject);
			ASSERT( SVMR_SUCCESS == messageReturn );
		}
	}
	else
	{
		m_variantObject.SetType( m_variantObject.GetDefaultType() );
		retVal = m_variantObject.SetValueKeepType(iBucket, valueString.c_str());
	}
	return retVal;
}

HRESULT LinkedValue::convertStringToValue(const SVString& valueString, _variant_t &vtNew) const
{
	_variant_t  vtItem( valueString.c_str() );

	return ::VariantChangeType( &vtNew, &vtItem, VARIANT_ALPHABOOL, m_variantObject.GetDefaultType() );
}

void LinkedValue::RenameToolSetSymbol(const SVObjectClass* pObject, const SVString& originalName)
{
	if ( nullptr != pObject && nullptr != m_inputConnectionInfo.GetInputObjectInfo().PObject )
	{
		SVString sNewPrefix = pObject->GetCompleteObjectNameToObjectType( NULL, SVToolSetObjectType ) + _T( "." );
		SVString sOldPrefix = sNewPrefix;
		sOldPrefix.replace( pObject->GetName(), originalName.c_str() );

		// input objects
		if ( pObject == m_inputConnectionInfo.GetInputObjectInfo().PObject->GetAncestor(pObject->GetObjectInfo().ObjectTypeInfo.ObjectType) )
		{
			CString sValue;
			m_variantObject.GetValue(sValue);
			sValue.Replace( sOldPrefix.c_str(), sNewPrefix.c_str() );
			m_variantObject.SetValue(1, sValue);
		}
	}
}

void LinkedValue::DisconnectObject(SVObjectClass* pObject)
{
	if ( m_inputConnectionInfo.GetInputObjectInfo().PObject == pObject )
	{
		DisconnectInput();
	}
}

void LinkedValue::UpdateTaskInfo(const SVObjectInfoStruct& rTaskInfoStruct)
{
	m_inputConnectionInfo.SetObject(rTaskInfoStruct);
}

HRESULT LinkedValue::ResetObject()
{
	HRESULT retVal = UpdateConnection();
	m_isValid = (retVal == S_OK);
	return retVal;
}

bool LinkedValue::IsValid() const
{
	return m_isValid;
}

void LinkedValue::SetAttributes(unsigned int bits, bool isOn)
{
	SetBits( m_variantObject.ObjectAttributesAllowedRef(),  bits, isOn );
}
#pragma endregion Public Methods

#pragma region Protected Methods

#pragma endregion Protected Methods

#pragma region Private Methods
HRESULT LinkedValue::UpdateConnection()
{
	if (nullptr == m_pInspection)
	{
		return SvOi::Err_10014_LinkedValueConnectInput_NoInspection;
	}

	HRESULT hr = S_OK;
	SVObjectClass* pObject = nullptr;
	CString strObjectName; 
	CString strCompleteObjectName = m_pInspection->GetCompleteObjectName();
	m_variantObject.GetValue(strObjectName);

	CString toolSetText;
	toolSetText.LoadString(IDS_CLASSNAME_SVTOOLSET);

	// if object name starts with tool set, inspection name must be added
	// else it must not be added, because it can be another object (e.g. "PPQ_1.Length" or "Environment.Mode.IsRun")
	if( strObjectName.Find(toolSetText) == 0)
	{	
		// Inspection name plus object name.
		strCompleteObjectName += "." + strObjectName;
	}
	else
	{
		// Object name is already complete.
		strCompleteObjectName = strObjectName;
	}

	SVObjectManagerClass::Instance().GetObjectByDottedName( static_cast< LPCTSTR >( strCompleteObjectName ), pObject );
	//
	// Inputs that are Variables must be found in the Inspection or the PPQ
	// otherwise the inputs are just variant inputs.
	//
	if ( nullptr != pObject )	// input is another VO
	{
		//disconnect old object and connect new object if old and new different.
		if ( m_inputConnectionInfo.GetInputObjectInfo().PObject != pObject )
		{
			DisconnectInput();
			DWORD_PTR messageReturn = ConnectInput(pObject);
			ASSERT( SVMR_SUCCESS == messageReturn );
			if( SVMR_SUCCESS != messageReturn )
			{
				hr = SvOi::Err_10015_LinkedValueConnectInput_ConnectFailed;
			}
		}
	}
	else	// plain data
	{
		DisconnectInput();

		_variant_t value;
		m_variantObject.GetValue( value );
		if ( m_variantObject.GetDefaultType() != value.vt)
		{
			if (m_isInvalidStringSetToDefault)
			{
				m_variantObject.GetDefaultValue( value );
				//set value back to default
				m_variantObject.SetDefaultValue( value, TRUE);
			}
			else
			{
				hr = SvOi::Err_10016_LinkedValueConnectInput_InvalidValue;
			}
		}

		if( SVMR_SUCCESS != ::SVSendMessage( &m_variantObject, SVM_RESET_ALL_OBJECTS, NULL, NULL ) )
		{
			hr = SvOi::Err_10017_LinkedValueConnectInput_ResetFailed;
		}
	}

	return hr;
}

DWORD_PTR LinkedValue::DisconnectInput()
{
	DWORD_PTR retVal = ::SVSendMessage(	m_inputConnectionInfo.GetInputObjectInfo().UniqueObjectID, 
		SVM_DISCONNECT_OBJECT_INPUT, 
		reinterpret_cast<DWORD_PTR>(&m_inputConnectionInfo), 
		NULL );
	m_inputConnectionInfo.SetInputObject( nullptr );
	return retVal;
}

DWORD_PTR LinkedValue::ConnectInput(SVObjectClass* pObject)
{
	m_inputConnectionInfo.SetInputObject( pObject );
	return ::SVSendMessage( pObject, SVM_CONNECT_OBJECT_INPUT, reinterpret_cast<DWORD_PTR>(&m_inputConnectionInfo), NULL );
}
#pragma endregion Private Methods
