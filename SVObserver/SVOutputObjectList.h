//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOutputObjectList
//* .File Name       : $Workfile:   SVOutputObjectList.h  $
//* ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.6  $
// * .Check In Date   : $Date:   15 May 2014 11:21:44  $
//******************************************************************************

#pragma once

//Moved to precompiled header: #include <comdef.h>

#include "SVOutputObject.h"
#include "SVInfoStructs.h"

class SVOutputObjectList : public SVObjectClass
{
public:
	typedef std::vector<CString> StringVect;
	SVOutputObjectList( LPCSTR ObjectName );
	SVOutputObjectList( SVObjectClass *pOwner = NULL, int StringResourceID = IDS_CLASSNAME_SVOUTPUTOBJECTLIST );

	virtual ~SVOutputObjectList();

	BOOL Create();
	BOOL Destroy();

	template< typename SVOutputType >
	HRESULT GetOutput( const SVGUID& p_rOutputID, SVOutputType*& p_rpOutput ) const;

	template< typename SVOutputType >
	HRESULT GetOutput( LPCTSTR p_szName, SVOutputType*& p_rpOutput ) const;

	template< typename SVOutputType >
	HRESULT GetOutputFlyweight( LPCTSTR p_szName, SVOutputType*& p_rpOutput );

	HRESULT AttachOutput( SVOutputObject *pOutput );
	HRESULT DetachOutput( const SVGUID& p_rOutputID );

	BOOL WriteOutputs( SVIOEntryStructVector& p_IOEntries, long lDataIndex, bool p_ACK, bool p_NAK );
	BOOL WriteOutputs( SVIOEntryHostStructPtrList& p_IOEntries, long lDataIndex, bool p_ACK, bool p_NAK );
	BOOL ResetOutputs( SVIOEntryStructVector& p_IOEntries );
	BOOL ResetOutputs( SVIOEntryHostStructPtrList& p_IOEntries );
	BOOL WriteOutput( SVIOEntryStruct pIOEntry, long lDataIndex, bool p_ACK, bool p_NAK );
	BOOL WriteOutput( SVIOEntryHostStructPtr pIOEntry, long lDataIndex, bool p_ACK, bool p_NAK );
	BOOL WriteOutputValue( SVIOEntryHostStructPtr pIOEntry, const _variant_t& p_rValue );
	BOOL ResetOutput( SVIOEntryHostStructPtr pIOEntry );
	bool RenameInspection( CString& OldInspection, CString& NewInspectionName);
	HRESULT RemoveUnusedOutputs( const StringVect& p_aStrInspNames, const StringVect& p_astrPPQNames );

	BOOL FillOutputs( SVIOEntryHostStructPtrList& p_IOEntries );

	virtual DWORD_PTR processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext );

	bool OutputIsNotValid( SVString p_strName );

private:
	HRESULT WriteDigitalOutput( SVIOEntryStruct& pIOEntry, long lDataIndex, bool p_ACK, bool p_NAK );

	void ClearOutputList();

	BOOL Lock() const;
	BOOL Unlock() const;

	BOOL m_bCreated;
	CRITICAL_SECTION m_hCriticalSection;

	SVGuidSVOutputObjectPtrMap m_OutputObjects;

};

#include "SVOutputObjectList.inl"

