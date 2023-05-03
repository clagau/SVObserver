//******************************************************************************
//* COPYRIGHT (c) 2011 by Körber Pharma Inspection GmbH. All Rights Reserved Vission, Inc.
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
#include "Definitions/GlobalConst.h"
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
};

struct IOLinkedValueObj
{
	uint32_t m_inspectionId {0UL};
	SVObjectClass* m_pObject {nullptr};
	SvOi::IValueObject* m_pValueObject {nullptr};
};

struct SVIOEntryHostStruct 
{
	SVIOEntryHostStruct() = default;
	~SVIOEntryHostStruct() = default;

	void clear();

	std::string m_name {};
	bool m_Enabled{false};
	long m_PPQIndex{-1L};

	SVIOObjectType m_ObjectType{IO_INVALID_OBJECT};

	uint32_t m_IOId{0UL};

	void setValueObject(SVObjectClass* pObject, DWORD objectIDIndex = 0);
	SVObjectClass* getObject(DWORD objectIDIndex = 0) const;
	SvOi::IValueObject* getValueObject(DWORD objectIDIndex = 0) const;
	uint32_t getInspectionID(DWORD objectIDIndex = 0) const;

	static bool PtrGreater( std::shared_ptr< SVIOEntryHostStruct > elem1, std::shared_ptr< SVIOEntryHostStruct > elem2 );

private:
	std::array<IOLinkedValueObj, SvDef::cObjectIndexMaxNr> m_ioLinkedValues {};
};

typedef std::shared_ptr< SVIOEntryHostStruct > SVIOEntryHostStructPtr;
typedef std::vector< SVIOEntryHostStructPtr > SVIOEntryHostStructPtrVector;

