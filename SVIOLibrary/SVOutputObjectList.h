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
#include "SVIOEntryStruct.h"
#include "Definitions/StringTypeDef.h"
#pragma endregion Includes

typedef std::vector<std::pair<GUID, _variant_t>> GuidVariantPairVector;

class SVOutputObjectList : public SVObjectClass
{
public:
	SVOutputObjectList( LPCSTR ObjectName );
	SVOutputObjectList( SVObjectClass *pOwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVOUTPUTOBJECTLIST );

	virtual ~SVOutputObjectList();

	bool Create();
	void Destroy();

	SVOutputObject* GetOutput(const SVGUID& rOutputID) const;

	SVOutputObject* GetOutput(const std::string& rName) const;

	SVOutputObject* GetOutputFlyweight(const std::string& rName, SvPb::SVObjectSubTypeEnum ObjectSubType, int GuidIndex = -1);

	HRESULT AttachOutput( SVOutputObject *pOutput );
	HRESULT DetachOutput( const SVGUID& rOutputID );

	GuidVariantPairVector getOutputValues(const SVIOEntryHostStructPtrVector& rIOEntries, long lDataIndex, bool p_ACK, bool p_NAK);
	bool ResetOutputs(SVIOEntryHostStructPtrVector& rIOEntries);
	bool WriteOutputs(const GuidVariantPairVector& rOutputValues);
	bool WriteOutput( SVIOEntryHostStructPtr pIOEntry, long lDataIndex, bool p_ACK, bool p_NAK );
	bool WriteOutputValue( SVIOEntryHostStructPtr pIOEntry, const _variant_t& rValue );
	bool ResetOutput( SVIOEntryHostStructPtr pIOEntry );
	bool RenameInspection( LPCTSTR OldInspection, LPCTSTR NewInspectionName);
	HRESULT RemoveUnusedOutputs( const SvDef::StringVector& rInspectionNames, const SvDef::StringVector& rPPQNames );

	bool FillOutputs( SVIOEntryHostStructPtrVector& rIOEntries );

	bool OutputIsNotValid( std::string p_strName );

#pragma region Methods to replace processMessage
	virtual void OnObjectRenamed(const SVObjectClass& rRenamedObject, const std::string& rOldName) override;
#pragma endregion Methods to replace processMessage

private:
	std::pair<GUID, _variant_t> getDigitalOutputValue(const SVIOEntryHostStructPtr& pIOEntry, long lDataIndex, bool p_ACK, bool p_NAK );

	void ClearOutputList();

	bool Lock() const;
	bool Unlock() const;

	bool m_bCreated;
	CRITICAL_SECTION m_hCriticalSection;

	SVGuidSVOutputObjectPtrMap m_OutputObjects;
};

