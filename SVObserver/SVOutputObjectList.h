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

#pragma region Includes
//Moved to precompiled header: #include <comdef.h>

#include "SVOutputObject.h"
#include "SVInfoStructs.h"
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

class SVOutputObjectList : public SVObjectClass
{
public:
	SVOutputObjectList( LPCSTR ObjectName );
	SVOutputObjectList( SVObjectClass *pOwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVOUTPUTOBJECTLIST );

	virtual ~SVOutputObjectList();

	BOOL Create();
	BOOL Destroy();

	SVOutputObject* GetOutput(const SVGUID& rOutputID) const;

	SVOutputObject* GetOutput(const SVString& rName) const;

	SVOutputObject* GetOutputFlyweight(const SVString& rName, SVObjectSubTypeEnum ObjectSubType, int GuidIndex = -1);

	HRESULT AttachOutput( SVOutputObject *pOutput );
	HRESULT DetachOutput( const SVGUID& rOutputID );

	BOOL WriteOutputs( SVIOEntryStructVector& rIOEntries, long lDataIndex, bool p_ACK, bool p_NAK );
	BOOL WriteOutputs( SVIOEntryHostStructPtrVector& rIOEntries, long lDataIndex, bool p_ACK, bool p_NAK );
	BOOL ResetOutputs( SVIOEntryStructVector& rIOEntries );
	BOOL ResetOutputs( SVIOEntryHostStructPtrVector& rIOEntries );
	BOOL WriteOutput( SVIOEntryStruct pIOEntry, long lDataIndex, bool p_ACK, bool p_NAK );
	BOOL WriteOutput( SVIOEntryHostStructPtr pIOEntry, long lDataIndex, bool p_ACK, bool p_NAK );
	BOOL WriteOutputValue( SVIOEntryHostStructPtr pIOEntry, const _variant_t& rValue );
	BOOL ResetOutput( SVIOEntryHostStructPtr pIOEntry );
	bool RenameInspection( LPCTSTR OldInspection, LPCTSTR NewInspectionName);
	HRESULT RemoveUnusedOutputs( const SVStringVector& rInspectionNames, const SVStringVector& rPPQNames );

	BOOL FillOutputs( SVIOEntryHostStructPtrVector& rIOEntries );

	bool OutputIsNotValid( SVString p_strName );

#pragma region Methods to replace processMessage
	virtual void OnObjectRenamed(const SVObjectClass& rRenamedObject, const SVString& rOldName) override;
#pragma endregion Methods to replace processMessage

private:
	HRESULT WriteDigitalOutput( SVIOEntryStruct& pIOEntry, long lDataIndex, bool p_ACK, bool p_NAK );

	void ClearOutputList();

	bool Lock() const;
	bool Unlock() const;

	BOOL m_bCreated;
	CRITICAL_SECTION m_hCriticalSection;

	SVGuidSVOutputObjectPtrMap m_OutputObjects;
};


