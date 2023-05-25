//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVInputObject
//* .File Name       : $Workfile:   SVInputObject.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:59:36  $
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <map>
#include "SVObjectLibrary/SVObjectClass.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#pragma endregion Includes

class SVInputObject : public SVObjectClass
{
public:
	explicit SVInputObject( LPCSTR  ) { m_isCreated = true; }
	explicit SVInputObject(SVObjectClass*, int)
	{
		m_isCreated = true;
	}

	virtual ~SVInputObject() = default;

	virtual HRESULT Read(_variant_t& rValue) const = 0;
	virtual long GetChannel() const = 0;
	virtual SVObjectClass* GetValueObject() { return nullptr; }

	void SetPpqIndex(long ppqIndex) { m_ppqIndex = ppqIndex; }
	long GetPpqIndex() const { return m_ppqIndex; }

	/// Update the objectId to a fix ID depend of a position (must between 0 and 0x100).
	/// \param position [in]
	void updateObjectId(int position)
	{
		if (0 <= position && 0x100 > position && SvDef::InvalidObjectId != m_startID)
		{
			SVObjectManagerClass::Instance().ChangeUniqueObjectID(this, m_startID + position);
			SVObjectClass* pValueObj = GetValueObject();
			if (nullptr != pValueObj)
			{
				//this is to set the value object to a fixed objectID and splits the available range (0x100 = 256) between input and value object
				constexpr int cValueObjectOffset {128};
				SVObjectManagerClass::Instance().ChangeUniqueObjectID(pValueObj, m_startID + cValueObjectOffset + position);
			}
		}
	}

protected:
	uint32_t m_startID{SvDef::InvalidObjectId};
	long m_ppqIndex {-1L};
};

typedef std::shared_ptr<SVInputObject> SVInputObjectPtr;
typedef std::map<uint32_t, SVInputObjectPtr> ObjectIdSVInputObjectPtrMap;

