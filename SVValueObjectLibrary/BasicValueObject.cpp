//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : BasicValueObject
//* .File Name       : $Workfile:   BasicValueObject.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.6  $
//* .Check In Date   : $Date:   16 Dec 2014 17:51:34  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "BasicValueObject.h"

#include "SVObjectLibrary\SVObjectLevelCreateStruct.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "SVUtilityLibrary\SVSafeArray.h"
#include "SVStatusLibrary/MessageManager.h"
#include "CameraLibrary\SVBoolValueDeviceParam.h"
#include "CameraLibrary\SVLongValueDeviceParam.h"
#include "CameraLibrary\SVStringValueDeviceParam.h"
#include "Definitions/Color.h"
#include "Definitions/ObjectDefines.h"
#include "Definitions\GlobalConst.h"
#include "SVMessage\SVMessage.h"
#pragma endregion Includes


namespace SvVol
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

#pragma region Constructor
	BasicValueObject::BasicValueObject(LPCTSTR ObjectName, SVObjectClass* pOwner, bool Node, SvPb::SVObjectSubTypeEnum ObjectSubType)
		: SVObjectClass(ObjectName)
		, m_Node(Node)

	{
		m_ObjectTypeInfo.m_ObjectType = SvPb::SVBasicValueObjectType;
		m_ObjectTypeInfo.m_SubType = ObjectSubType;
		Create(pOwner);
	}

	BasicValueObject::~BasicValueObject()
	{
		Destroy();
	}
#pragma endregion Constructor

#pragma region Public Methods
	bool BasicValueObject::operator==(const BasicValueObject& rRhs) const
	{
		bool Result = false;

		if (m_Value == rRhs.m_Value)
		{
			Result = true;
		}
		return Result;
	}

	HRESULT BasicValueObject::getValue(double& rValue, int  /*= -1*/) const
	{
		return getValue<double>(rValue);
	}

	HRESULT BasicValueObject::setValue(const _variant_t& rValue, int  /*= -1*/, bool)
	{
		HRESULT	Result = S_OK;
		// cppcheck-suppress variableScope symbolName=bisArray
		bool bisArray{ false };
		_variant_t	TempValue(rValue);
		
		//SVRC sends data as array
		if (VT_ARRAY == (TempValue.vt & VT_ARRAY))
		{
			
			Result = ConvertArray(TempValue);
			// cppcheck-suppress unreadVariable symbolName=bisArray
			bisArray = true;
		}
		//If variant is of type VT_EMPTY then this is only a node
		if (VT_EMPTY == rValue.vt)	
		{
			m_Node = true;
		}
		else
		{
			if (S_OK == Result )
			{
#ifdef  SAFEARRAY_FORMAT_IN_SETITEM				
				if (VT_EMPTY != m_Value.vt && VT_EMPTY != rValue.vt && !bisArray && m_Value.vt != rValue.vt)
				{
					SvStl::MessageManager e(SvStl::MsgType::Log);
					e.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_UnexpectedConversion, SvStl::SourceFileParams(StdMessageParams));
				}
#endif 
				
				Result = ConvertVariantToType(TempValue);
			}
			
			
			
			if (S_OK == Result)
			{
				//If it is VT_BSTR then we need to copy the text otherwise we get just a pointer to the string
				if (VT_BSTR == TempValue.vt)
				{
					boost::unique_lock<boost::shared_mutex> guard(m_valueMutex);
					m_Value = _bstr_t(TempValue);

				
				}
				else
				{
					boost::unique_lock<boost::shared_mutex> guard(m_valueMutex);
					m_Value = TempValue;
				}
				m_hasChanged = true;
			}
		}

		RefreshOwner(SVObjectClass::PostRefresh);
		return Result;
	}

	HRESULT BasicValueObject::setValue(const std::string& rValue, int  /*= -1*/)
	{
		HRESULT Result(S_FALSE);

		{
			boost::unique_lock<boost::shared_mutex> guard(m_valueMutex);
		
			m_Value = _bstr_t(rValue.c_str());
			m_hasChanged = true;
		}

		RefreshOwner(SVObjectClass::PostRefresh);
		return Result;
	}

	HRESULT BasicValueObject::getValue(_variant_t& rValue, int  /*= -1*/) const
	{
		RefreshOwner(SVObjectClass::PreRefresh);

		rValue.Clear();
		{
			boost::shared_lock<boost::shared_mutex> guard(m_valueMutex);
			rValue = m_Value;
		}
		return S_OK;
	}

	HRESULT BasicValueObject::getValue(std::string& rValueString, int, const std::string&) const
	{
		//BasicValueObject currently always ignores rFormatString!
		HRESULT	Result = S_OK;

		RefreshOwner(SVObjectClass::PreRefresh);

		rValueString.clear();
		{
			boost::shared_lock<boost::shared_mutex> guard(m_valueMutex);
			switch (m_Value.vt)
			{
			case VT_BSTR:
			{
				rValueString = SvUl::createStdString(m_Value.bstrVal);
			}
			break;
			case VT_BOOL:
			{
				rValueString = m_Value.boolVal ? _T("True") : _T("False");
			}
			break;
			//The old BOOL converts to VT_IINT so this is also of type boolean
			case VT_INT:
			{
				rValueString = m_Value.intVal == 1 ? _T("True") : _T("False");
			}
			break;
			case VT_I4:
			{
				rValueString = SvUl::Format(_T("%d"), m_Value.lVal);
			}
			break;
			case VT_I8:
			{
				rValueString = SvUl::Format(_T("%d"), m_Value.llVal);
			}
			break;
			case VT_R4:
			{
				rValueString = SvUl::Format(_T("%f"), m_Value.fltVal);
			}
			break;
			case VT_R8:
			{
				rValueString = SvUl::Format(_T("%f"), m_Value.dblVal);
			}
			break;
			default:
			{
				Result = S_FALSE;
			}
			break;
			}
		}
		return Result;
	}


	int32_t BasicValueObject::getByteSize(bool, bool memBlockData) const
	{
		int32_t result(0L);

		///When for memory block and has no attributes then no memory requirements and return 0
		if (memBlockData && 0 == ObjectAttributesAllowed())
		{
			return result;
		}

		//Note: BasicValueObject cannot be an array so memory size only for one element
		switch (GetType())
		{
		case VT_BOOL:
			result = sizeof(VARIANT::boolVal);
			break;
		case VT_I4:
			result = sizeof(VARIANT::lVal);
			break;
		case VT_I8:
			result = sizeof(VARIANT::llVal);
			break;
		case VT_INT:
			result = sizeof(VARIANT::intVal);
			break;
		case VT_R4:
			result = sizeof(VARIANT::fltVal);
			break;
		case VT_R8:
			result = sizeof(VARIANT::dblVal);
			break;
		case VT_BSTR:
		{

			boost::shared_lock<boost::shared_mutex> guard(m_valueMutex);
			std::string tempString = SvUl::createStdString(m_Value.bstrVal);

			//Add place for the ending \0 of the string
			result = static_cast<long> (tempString.size() + 1);
			break;
		}
		default:
			break;
		}

		return result;
	}

	void BasicValueObject::setTrData(int32_t memOffset, int32_t memSize, int32_t pos)
	{
		m_trPos = pos;
		///When memOffset and memSize are -1 then only the trigger record position is changed
		if (-1 == memOffset && -1 == memSize)
		{
			return;
		}
		m_memOffset = memOffset;
		m_memSizeReserved = memSize;
	}

	void BasicValueObject::setMemBlockPointer(uint8_t* pMemBlockBase)
	{
		if (nullptr != pMemBlockBase && -1 != m_memOffset)
		{
			m_pMemBlock = pMemBlockBase + m_memOffset;
			///Initialize the memory block value
			updateMemBlockData();
		}
	}

	void BasicValueObject::updateMemBlockData()
	{
		///Copy only if it has changed since last update
		if (m_hasChanged && nullptr != m_pMemBlock)
		{
			int32_t dataByteSize = getByteSize(true, false);
			///This is to make sure that enough space has been reserved for memory block data
			if (dataByteSize <= m_memSizeReserved)
			{
				boost::shared_lock<boost::shared_mutex> guard(m_valueMutex);
				//Note: BasicValueObject cannot be an array so copy only one element
				const void* pValue{ nullptr };

				switch (m_Value.vt)
				{
				case VT_BOOL:
				case VT_I4:
				case VT_I8:
				case VT_INT:
				case VT_R4:
				case VT_R8:
					pValue = &m_Value;
					break;
				case VT_BSTR:
				{
					std::string tempString = SvUl::createStdString(m_Value.bstrVal);
					//Copy also the ending \0 of the string
					memcpy(m_pMemBlock, tempString.c_str(), tempString.size() + 1);
					break;
				}
				default:
					break;
				}
				//This is for all types except VT_BSTR
				if (nullptr != pValue)
				{
					memcpy(m_pMemBlock, pValue, dataByteSize);
				}
			}
		}
	}

	HRESULT BasicValueObject::getValue(BOOL& rValue) const
	{
		HRESULT Result = S_OK;

		RefreshOwner(SVObjectClass::PreRefresh);

		{
			boost::shared_lock<boost::shared_mutex> guard(m_valueMutex);

			switch (m_Value.vt)
			{
			case VT_BOOL:
			case VT_INT:
			case VT_I4:
			case VT_I8:
				rValue = static_cast<BOOL> (m_Value);
				break;
			case VT_R4:
			case VT_R8:
			case VT_BSTR:
			default:
				Result = S_FALSE;
				break;
			}
		}
		return Result;


	}

	HRESULT BasicValueObject::getValue(bool& rValue) const
	{
		HRESULT Result = S_OK;

		BOOL Value(false);
		//Use the BOOL version and then convert to type bool
		Result = getValue(Value);
		rValue = Value ? true : false;

		return Result;
	}

	std::string BasicValueObject::getTypeName() const
	{
		std::string Result = _T("Invalid");
		boost::shared_lock<boost::shared_mutex> guard(m_valueMutex);
		switch (m_Value.vt)
		{
		case VT_BSTR:
		{
			Result = _T("Text");
		}
		break;
		case VT_BOOL:
		case VT_INT:		//BOOL implicit conversion to VT_INT
		{
			Result = _T("Bool");
		}
		break;
		case VT_I4:
		{
			Result = _T("Integer32");
		}
		break;
		case VT_I8:
		{
			Result = _T("Integer64");
		}
		break;
		case VT_R4:
		case VT_R8:
		{
			Result = _T("Decimal");
		}
		break;
		default:
			break;
		}
		return Result;
	}

	HRESULT BasicValueObject::updateDeviceParameter(SVDeviceParam* pDeviceParam)
	{
		HRESULT Status = S_OK;
		boost::shared_lock<boost::shared_mutex> guard(m_valueMutex);

		if (nullptr == pDeviceParam)
		{
			Status = S_FALSE;
			return Status;
		}
		switch (pDeviceParam->DataType())
		{
		case DeviceDataTypeBool:
		{
			SVBoolValueDeviceParam* pBoolDeviceParam = nullptr;
			pBoolDeviceParam = dynamic_cast<SVBoolValueDeviceParam*> (pDeviceParam);
			if (nullptr != pBoolDeviceParam)
			{
				if (VT_BOOL == m_Value.vt)
				{
					Status = pBoolDeviceParam->SetValue(m_Value.GetVARIANT());
				}
				else
				{
					Status = S_FALSE;
				}
			}
			else
			{
				Status = S_FALSE;
			}
		}
		break;

		case DeviceDataTypeLong:
		{
			SVLongValueDeviceParam* pLongDeviceParam = nullptr;
			pLongDeviceParam = dynamic_cast<SVLongValueDeviceParam*> (pDeviceParam);
			if (nullptr != pLongDeviceParam)
			{
				if (VT_I4 == m_Value.vt)
				{
					if ((m_Value.lVal >= pLongDeviceParam->GetScaledMin()) && (m_Value.lVal <= pLongDeviceParam->GetScaledMax()))
					{
						pLongDeviceParam->SetScaledValue(m_Value.lVal);
					}
					else
					{
						//When failed then need to set the value to the same DeviceParam value
						m_Value.lVal = pLongDeviceParam->GetScaledValue();
						Status = S_FALSE;
					}
				}
				else
				{
					Status = S_FALSE;
				}
			}
			else
			{
				Status = S_FALSE;
			}
		}
		break;

		case DeviceDataTypeString:
		{
			SVStringValueDeviceParam* pStringDeviceParam = nullptr;
			pStringDeviceParam = dynamic_cast<SVStringValueDeviceParam*> (pDeviceParam);
			if (nullptr != pStringDeviceParam)
			{
				if (VT_BSTR == m_Value.vt)
				{
					Status = pStringDeviceParam->SetValue(m_Value.GetVARIANT());
				}
				else
				{
					Status = S_FALSE;
				}
			}
			else
			{
				Status = S_FALSE;
			}
		}
		break;

		default:
			break;
		}
		return Status;
	}

	DWORD BasicValueObject::GetObjectColor() const
	{
		return SvDef::DefaultWhiteColor;
	}


	std::string BasicValueObject::getFixedWidthFormatString(uint32_t, uint32_t)
	{
		return _T("<invalid>"); //BasicValueObject::getValue(std::string& rValueString, int Index, const std::string& rFormatString) currently ignores rFormatString anyway!
	}

#pragma endregion Public Methods


#pragma region Private Methods
	void BasicValueObject::Create(SVObjectClass* pOwner)
	{
		boost::unique_lock<boost::shared_mutex> guard(m_valueMutex);
		m_Value.Clear();
		if (!m_Node)
		{
			SetObjectAttributesAllowed(SvDef::defaultValueObjectAttributes, SvOi::SetAttributeType::OverwriteAttribute);
		}
		if (nullptr != pOwner)
		{
			SVObjectLevelCreateStruct CreateStruct;

			CreateStruct.OwnerObjectInfo.SetObject(pOwner);
			CreateObject(CreateStruct);

		}
		m_Created = true;
	}

	void BasicValueObject::Destroy()
	{
		if (m_Created)
		{
			boost::unique_lock<boost::shared_mutex> guard(m_valueMutex);
			m_Value.Clear();
			m_Created = false;
		}
	}

	HRESULT BasicValueObject::RefreshObject(const SVObjectClass* const pSender, RefreshObjectType Type)
	{
		HRESULT Result(S_FALSE);

		SVObjectClass* pOwner = GetParent();

		if (nullptr != pOwner)
		{
			Result = pOwner->RefreshObject(pSender, Type);
		}

		return Result;
	}

	bool BasicValueObject::RefreshOwner(RefreshObjectType Type) const
	{
		bool Result(false);

		SVObjectClass* pOwner = GetParent();

		if (nullptr != pOwner)
		{
			Result = S_OK == pOwner->RefreshObject(this, Type) ? true : false;
		}

		return Result;
	}

	HRESULT BasicValueObject::ConvertArray(_variant_t& rValue) const
	{
		HRESULT Status = S_OK;
		if (VT_ARRAY == (rValue.vt & VT_ARRAY) && rValue.parray)
		{
			SvUl::SVSAFEARRAY SafeArray(rValue);

			if (0 <  SafeArray.size())
			{
				Status = SafeArray.GetElement(0, rValue);
			}
			else
			{
				Status = E_FAIL;
			}


		}
		return Status;
	}


	HRESULT BasicValueObject::ConvertVariantToType(_variant_t& rValue) const
	{
		HRESULT Status = S_OK;
		boost::shared_lock<boost::shared_mutex> guard(m_valueMutex);
		if (VT_EMPTY != m_Value.vt && VT_EMPTY != rValue.vt && m_Value.vt != rValue.vt)
		{
			Status = ::VariantChangeTypeEx(&rValue, &rValue, SvDef::LCID_USA, 0, m_Value.vt);
		}
		return Status;
	}
#pragma endregion Private Methods

} //namespace SvVol
