//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVInputObjectList
//* .File Name       : $Workfile:   SVInputObjectList.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   07 Aug 2013 13:27:30  $
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <comdef.h>
#include "SVOResource/resource.h"
#include "SVInputObject.h"
#include "SVIOEntryHostStruct.h"
#pragma endregion Includes

class SVInputObjectList : public SVObjectClass
{
public:
	explicit SVInputObjectList( LPCSTR ObjectName );
	SVInputObjectList( SVObjectClass *pOwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVINPUTOBJECTLIST );
	virtual ~SVInputObjectList() = default;

	SVInputObjectPtr GetInput(uint32_t inputID) const;

	SVInputObjectPtr GetInput(const std::string& rInputName) const;

	SVInputObjectPtr GetInputFlyweight( const std::string& rInputName, SvPb::SVObjectSubTypeEnum ObjectSubType, int GuidIndex = -1);

	HRESULT AttachInput(SVInputObjectPtr pInput);
	HRESULT DetachInput(uint32_t inputID);

	bool ReadInputs(const SVIOEntryHostStructPtrVector& rInputs, std::vector<_variant_t>& rInputValues);

	SVIOEntryHostStructPtrVector getInputList() const;

	long getRemoteInputCount() const;
	bool SetRemoteInput( long lIndex, const _variant_t& rValue );

	std::vector<SVInputObjectPtr> GetInputs(SvPb::SVObjectSubTypeEnum ObjectSubType = SvPb::SVNotSetSubObjectType) const;

private:
	SVInputObjectPtr findInputName(const std::string& rInputName) const;

	mutable std::mutex m_protectInputObjectList;

	ObjectIdSVInputObjectPtrMap m_inputObjectMap;
};

