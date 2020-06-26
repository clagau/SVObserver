//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOutputObject
//* .File Name       : $Workfile:   SVOutputObject.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:15:50  $
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <map>
//Moved to precompiled header: #include <vector>
#include "SVOResource/resource.h"
#include "SVObjectLibrary/SVObjectClass.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#pragma endregion Includes

class SVOutputObject : public SVObjectClass
{
public:
	explicit SVOutputObject( LPCSTR  ) { m_isCreated = true; }
	explicit SVOutputObject( SVObjectClass*  = nullptr, int  = IDS_CLASSNAME_SVOUTPUTOBJECT )
	{
		m_isCreated = true;
	}

	virtual ~SVOutputObject() = default;

	virtual HRESULT Write( const _variant_t& rValue ) = 0;
	virtual HRESULT Reset() = 0;
	virtual bool isCombined() const = 0;
	virtual bool isAndACK() const = 0;		///The signal is combined is set either to And ACK or OR NAK
	virtual long GetChannel() const = 0;

	/// Update the objectId to a fixed ID depended on an index (must between 0 and 0x100).
	/// \param position [in]
	void updateObjectId(int index)
	{
		if (0 <= index && 0x100 > index && SvDef::InvalidObjectId != m_startID)
		{
			SVObjectManagerClass::Instance().ChangeUniqueObjectID(this, m_startID + index);
		}
	}

protected:
	uint32_t m_startID{SvDef::InvalidObjectId};
};

typedef std::shared_ptr<SVOutputObject> SVOutputObjectPtr;
typedef std::map<uint32_t, SVOutputObjectPtr> ObjectIdSVOutputObjectPtrMap;

