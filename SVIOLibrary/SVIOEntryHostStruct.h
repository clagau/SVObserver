//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vission, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVIOEntryHostStruct
//* .File Name       : $Workfile:   SVIOEntryHostStruct.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   27 Aug 2014 01:24:38  $
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <memory>
//Moved to precompiled header: #include <vector>
#pragma endregion Includes

namespace SvOi
{
class IValueObject;
}

class SVObjectClass;


enum SVIOObjectType
{
	IO_INVALID_OBJECT	= 0x0000,	// 0000 0000 0000 0000
	IO_DIGITAL_INPUT	= 0x0001,	// 0000 0000 0000 0001
	IO_REMOTE_INPUT		= 0x0004,	// 0000 0000 0000 0100
	IO_DIGITAL_OUTPUT	= 0x0008,	// 0000 0000 0000 1000
	IO_PLC_OUTPUT		= 0x0010,	// 0000 0000 0001 0000
	IO_REMOTE_OUTPUT	= 0x0020,	// 0000 0000 0010 0000
	IO_CAMERA_DATA_INPUT = 0x8000,	// 1000 0000 0000 0000
};

struct SVIOEntryHostStruct 
{
	SVIOEntryHostStruct() = default;
	~SVIOEntryHostStruct() = default;

	void clear();

	bool m_Enabled{false};
	long m_PPQIndex{-1L};

	SVIOObjectType m_ObjectType{IO_INVALID_OBJECT};

	uint32_t m_IOId{0UL};

	void setValueObject(std::shared_ptr<SvOi::IValueObject> pValueObject);
	void setLinkedObject(SVObjectClass* pObject);
	SVObjectClass* getObject() { return m_pObject; };
	SvOi::IValueObject* getValueObject() { return (nullptr != m_pValueObject) ? m_pValueObject.get() : m_pLinkedValueObject; }

	static bool PtrGreater( std::shared_ptr< SVIOEntryHostStruct > elem1, std::shared_ptr< SVIOEntryHostStruct > elem2 );

private:
	SVIOEntryHostStruct( const SVIOEntryHostStruct& p_rsvObject ) = default;

	SVIOEntryHostStruct& operator=( const SVIOEntryHostStruct& p_rsvObject ) = default;

	SVObjectClass* m_pObject{nullptr};
	///m_pLinkedValueObject is not owned while m_pValueObject is owned by IO entry
	SvOi::IValueObject* m_pLinkedValueObject {nullptr};
	std::shared_ptr<SvOi::IValueObject> m_pValueObject;
};

typedef std::shared_ptr< SVIOEntryHostStruct > SVIOEntryHostStructPtr;
typedef std::vector< SVIOEntryHostStructPtr > SVIOEntryHostStructPtrVector;

