//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVStringValueObjectClass
//* .File Name       : $Workfile:   SVStringValueObjectClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   02 Oct 2013 08:17:48  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <assert.h>
#include "SVStringValueObjectClass.h"
#include "SVObjectLibrary\SVClsids.h"
#include "SVObjectLibrary\SVToolsetScriptTags.h"
#include "SVObjectLibrary/SVObjectAttributeClass.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "Definitions/TextDefineSVDef.h"
#pragma endregion Includes

namespace SvVol
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS(SVStringValueObjectClass, SVStringValueObjectClassGuid);

SVStringValueObjectClass::SVStringValueObjectClass( LPCTSTR ObjectName )
: SVValueObjectClass<std::string>( ObjectName )
{
	LocalInitialize();
}

SVStringValueObjectClass::SVStringValueObjectClass(	SVObjectClass* pOwner, 	int StringResourceID )
: SVValueObjectClass<std::string>( pOwner, StringResourceID )
{
	LocalInitialize();
}

SVStringValueObjectClass::SVStringValueObjectClass( const SVStringValueObjectClass& rhs)
: SVValueObjectClass<std::string>()
{
	LocalInitialize();
	*this = rhs;
}

const SVStringValueObjectClass& SVStringValueObjectClass::operator = (const SVStringValueObjectClass& rhs)
{
	__super::operator = (rhs);
	return *this;
}

HRESULT  SVStringValueObjectClass::SetObjectValue(SVObjectAttributeClass* pDataObject)
{
	HRESULT Result( E_FAIL );
	bool	bOk( false );

	std::vector<ValueVector> BucketArray;		//This is for backward compatibility
	ValueVector ValueArray;

	if (bOk = pDataObject->GetAttributeData(scDefaultTag, ValueArray))
	{
		if (0 < ValueArray.size())
		{
			SvUl::RemoveEscapedSpecialCharacters( ValueArray[ValueArray.size() - 1], true);
			DefaultValue() = ValueArray[ValueArray.size() - 1];
		}

		SetDefaultValue(GetDefaultValue().c_str(), false);
	}
	else if(bOk = pDataObject->GetAttributeData(SvDef::cBucketTag, BucketArray, DefaultValue()))
	{
		for (int i = 0; i < static_cast<int> (BucketArray.size()); i++)
		{
			SvUl::RemoveEscapedSpecialCharacters(BucketArray[i][0], true);
			SetValue(BucketArray[i][0], i);
		}
	}
	// new-style: store all array elements:
	else if ( bOk = pDataObject->GetArrayData( SvDef::cArrayTag, ValueArray, DefaultValue() ) )
	{
		int32_t arraySize = static_cast<int32_t> (ValueArray.size());
		SetArraySize(arraySize);
		for (int i = 0; i < arraySize; i++)
		{
			// Remove any escapes
			SvUl::RemoveEscapedSpecialCharacters(ValueArray[i], true);
			SetValue(ValueArray[i], i);
		}
	}
	else if ( bOk = pDataObject->GetAttributeData(_T("StrDefault"), ValueArray) )
	{
		if ( 0 < ValueArray.size() )
		{
			SvUl::RemoveEscapedSpecialCharacters(ValueArray[ValueArray.size() - 1], true);
			DefaultValue() = ValueArray[ValueArray.size() - 1];
		}

		SetDefaultValue( GetDefaultValue().c_str(), false );
	}
	else if ( bOk = pDataObject->GetAttributeData( _T("StrArray"), BucketArray, DefaultValue() ) )
	{
		for (int i = 0; i < static_cast<int > (BucketArray.size()); i++)
		{
			SvUl::RemoveEscapedSpecialCharacters(BucketArray[i][0], true);
			SetValue(BucketArray[i][0], i);
		}
	}
	else
	{
		Result = __super::SetObjectValue(pDataObject);
		return Result;
	}

	Result = bOk ? S_OK : E_FAIL;
	return Result;
}

HRESULT SVStringValueObjectClass::SetArrayValues(const ValueVector& rValues)
{
	HRESULT Result(E_FAIL);

	int32_t Size = static_cast<int32_t> (rValues.size());
	assert(Size <= getArraySize());
	if (Size <= m_stringData.size())
	{
		SetResultSize(Size);
		if (0 < Size)
		{
			std::copy(rValues.begin(), rValues.end(), m_stringData.begin());
		}
		Result = S_OK;
	}
	else
	{
		Result = SVMSG_SVO_33_OBJECT_INDEX_INVALID;
	}
	return Result;
}

void SVStringValueObjectClass::setMemBlockPointer(uint8_t* pMemBlockBase)
{
	int32_t memOffset = getMemOffset();
	if (nullptr != pMemBlockBase && -1 != memOffset)
	{
		if (isArray())
		{
			setResultSizePointer(reinterpret_cast<int32_t*> (pMemBlockBase + memOffset));
			m_pMemBlockData = (pMemBlockBase + memOffset + sizeof(int32_t));
		}
		else
		{
			setResultSizePointer(nullptr);
			m_pMemBlockData = pMemBlockBase + memOffset;
		}
		updateMemBlockData();
	}
}

void SVStringValueObjectClass::updateMemBlockData() const
{
	///Here we only want the data byte size
	int32_t dataByteSize = getByteSize(true, false);
	///This is to make sure that enough space has been reserved for memory block data
	if (0 < dataByteSize &&  dataByteSize <= getMemSizeReserved() && nullptr != m_pMemBlockData)
	{
		if(hasChanged())
		{
			uint8_t* pMemoryLocation = m_pMemBlockData;
			int resultSize = isArray() ? getResultSize() : 1;
			for (int i = 0; i < resultSize; ++i)
			{
				std::string valueString;
				GetValue(valueString, i);

				//Copy also the ending \0 of the string
				memcpy(pMemoryLocation, valueString.c_str(), valueString.size() + 1);
				pMemoryLocation += valueString.size() + 1;
			}
			setHasChanged(false);
		}
	}
	else
	{
		if(0 < getMemSizeReserved() && nullptr != m_pMemBlockData)
		{
			///Clear the memory block data
			memset(m_pMemBlockData, 0, getMemSizeReserved());
		}
	}
}

std::string* SVStringValueObjectClass::reserveLocalMemory()
{
	std::string* pResult{nullptr};
	if(m_stringData.size() != static_cast<size_t> (getArraySize()))
	{
		m_stringData.resize(getArraySize(), GetDefaultValue());
	}
	if(0 < m_stringData.size())
	{
		pResult = &m_stringData.at(0);
	}
	return pResult;
}

int32_t SVStringValueObjectClass::getByteSize(bool useResultSize, bool memBlockData) const
{
	int32_t result(0L);

	///When for memory block and has no attributes then no memory requirements and return 0
	if (memBlockData && 0 == ObjectAttributesAllowed())
	{
		return result;
	}

	int32_t numberOfElements = useResultSize ? getResultSize() : getArraySize();
	if(0 == m_maxByteSize)
	{
		for (int i = 0; i < numberOfElements; ++i)
		{
			std::string valueString;
			SVStringValueObjectClass::GetValue(valueString, i);
			//Add place for the ending \0 of the string
			result += static_cast<long> (valueString.size() + 1);
		}
	}
	else
	{
		result = m_maxByteSize;
		result *= numberOfElements;
	}
	//For memory block data that is an array the first value shall contain the result size which can be variable
	if (memBlockData && isArray())
	{
		result += sizeof(int32_t);
	}

	return result;
}

void SVStringValueObjectClass::WriteValues(SvOi::IObjectWriter& rWriter)
{
	// Where does Object Depth Get put into the Script ??? (maybe at the SVObjectClass)
	// Object Depth is implicit (it's the count of the values)
	std::vector<_variant_t> list;

	// Get the Data Values (Member Info, Values)
	// Check for DoubleQuotes in variable
	std::string TempValue(_T(""));
	SvUl::AddEscapeSpecialCharacters(TempValue, true);

	_variant_t Value;
	Value.Clear();

	// for all elements in the array
	for (int32_t i = 0; i < getArraySize(); i++)
	{
		//Make sure this is not a derived virtual method which is called
		SVStringValueObjectClass::GetValue(TempValue, i);
		SvUl::AddEscapeSpecialCharacters(TempValue, true);
		Value.SetString(TempValue.c_str());
		list.push_back(Value);
		Value.Clear();
	}
	rWriter.WriteAttribute(scElementTag, list);
}

void SVStringValueObjectClass::WriteDefaultValues(SvOi::IObjectWriter& rWriter)
{
	std::string TempValue(GetDefaultValue());
	SvUl::AddEscapeSpecialCharacters(TempValue, true);
	_variant_t Value;
	Value.SetString(TempValue.c_str());
	rWriter.WriteAttribute(scDefaultTag, Value);
}

void SVStringValueObjectClass::LocalInitialize()
{
	m_outObjectInfo.m_ObjectTypeInfo.SubType = SvPb::SVStringValueObjectType;
	
	SetObjectAttributesAllowed( SvPb::viewable | SvPb::publishable | SvPb::archivable | SvPb::embedable | SvPb::printable | SvPb::dataDefinitionValue, SvOi::SetAttributeType::OverwriteAttribute );
	SetTypeName( _T("Text") );

	init();
}

} //namespace SvVol
