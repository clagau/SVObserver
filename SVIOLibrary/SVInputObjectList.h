//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
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
	virtual ~SVInputObjectList();

	bool Create();
	bool Destroy();

	SVInputObject* GetInput( const SVGUID& rInputID ) const;

	SVInputObject* GetInput( const std::string& rInputName ) const;

	SVInputObject* GetInputFlyweight( const std::string& rInputName, SvPb::SVObjectSubTypeEnum ObjectSubType, int GuidIndex = -1);

	HRESULT AttachInput( SVInputObject *pInput );
	HRESULT DetachInput( const SVGUID& p_rOutputID );

	bool ReadInputs(const SVIOEntryHostStructPtrVector& rInputs, std::vector<_variant_t>& rInputValues);

	bool FillInputs( SVIOEntryHostStructPtrVector& p_IOEntries );

	bool GetRemoteInputCount( long &lSize );
	bool SetRemoteInput( long lIndex, const _variant_t& rValue );

private:
	bool Lock() const;
	bool Unlock() const;

	CRITICAL_SECTION m_hCriticalSection;
	bool m_bCreated;

	SVGuidSVInputObjectPtrMap m_InputObjects;
};

