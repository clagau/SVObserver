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

//Moved to precompiled header: #include <comdef.h>

#include "SVUtilityLibrary/SVString.h"
#include "SVInputObject.h"
#include "SVInfoStructs.h"

class SVInputObjectList : public SVObjectClass
{
public:
	typedef std::vector< std::pair< _variant_t, bool > > SVVariantBoolVector;

	SVInputObjectList( LPCSTR ObjectName );
	SVInputObjectList( SVObjectClass *pOwner = NULL, int StringResourceID = IDS_CLASSNAME_SVINPUTOBJECTLIST );
	virtual ~SVInputObjectList();

	BOOL Create();
	BOOL Destroy();

	template< typename SVInputType >
	HRESULT GetInput( const SVGUID& p_rInputID, SVInputType*& p_rpInput ) const;

	template< typename SVInputType >
	HRESULT GetInput( const SVString& p_rInputName, SVInputType*& p_rpInput ) const;

	template< typename SVInputType >
	HRESULT GetInputFlyweight( const SVString& p_rInputName, SVInputType*& p_rpInput );

	HRESULT AttachInput( SVInputObject *pInput );
	HRESULT DetachInput( const SVGUID& p_rOutputID );

	BOOL ReadInputs( const SVIOEntryHostStructPtrList& p_rInputs, SVVariantBoolVector& p_rInputValues );
	BOOL ReadInput( SVIOEntryStruct pIOEntry, _variant_t& p_rVariant );

	BOOL FillInputs( SVIOEntryHostStructPtrList& p_IOEntries );

	BOOL GetRemoteInputCount( long &lSize );
	BOOL SetRemoteInput( long lIndex, VARIANT vtValue );

private:
	BOOL Lock() const;
	BOOL Unlock() const;

	CRITICAL_SECTION m_hCriticalSection;
	BOOL m_bCreated;

	SVGuidSVInputObjectPtrMap m_InputObjects;

};

#include "SVInputObjectList.inl"

