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
#include "SVIOEntryHostStruct.h"
#include "Definitions/StringTypeDef.h"
#pragma endregion Includes

typedef std::vector<std::pair<uint32_t, _variant_t>> ObjectIdVariantPairVector;

class SVOutputObjectList : public SVObjectClass
{
public:
	explicit SVOutputObjectList( LPCSTR ObjectName );
	SVOutputObjectList( SVObjectClass *pOwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVOUTPUTOBJECTLIST );

	virtual ~SVOutputObjectList();

	bool Create();
	void Destroy();

	SVOutputObject* GetOutput(uint32_t outputID) const;

	SVOutputObject* GetOutput(const std::string& rName) const;

	SVOutputObject* GetOutputFlyweight(const std::string& rName, SvPb::SVObjectSubTypeEnum ObjectSubType, int idIndex = -1);

	HRESULT AttachOutput( SVOutputObject *pOutput );
	HRESULT DetachOutput(uint32_t outputID );

	ObjectIdVariantPairVector getOutputValues(const SVIOEntryHostStructPtrVector& rIOEntries, bool useDefaults, bool p_ACK, bool p_NAK);
	bool ResetOutputs(SVIOEntryHostStructPtrVector& rIOEntries);
	bool WriteOutputs(const ObjectIdVariantPairVector& rOutputValues);
	bool WriteOutput( SVIOEntryHostStructPtr pIOEntry, bool p_ACK, bool p_NAK );
	bool WriteOutputValue( SVIOEntryHostStructPtr pIOEntry, const _variant_t& rValue );
	bool ResetOutput( SVIOEntryHostStructPtr pIOEntry );
	bool RenameInspection( LPCTSTR OldInspection, LPCTSTR NewInspectionName);
	HRESULT RemoveUnusedOutputs( const SvDef::StringVector& rInspectionNames, const SvDef::StringVector& rPPQNames );

	bool FillOutputs( SVIOEntryHostStructPtrVector& rIOEntries );

	bool OutputIsNotValid( std::string p_strName );

	typedef  std::map<int, _variant_t> IntVariantMap;
	///This function returns no error because the IO PLC dll is the only type which supports this functionality
	void WriteOutputData(unsigned long triggerChannel, const IntVariantMap& rData);

#pragma region Methods to replace processMessage
	virtual void OnObjectRenamed(const SVObjectClass& rRenamedObject, const std::string& rOldName) override;
#pragma endregion Methods to replace processMessage

private:
	std::pair<uint32_t, _variant_t> getDigitalOutputValue(const SVIOEntryHostStructPtr& pIOEntry, bool useDefault, bool p_ACK, bool p_NAK );

	void ClearOutputList();

	mutable std::mutex m_protectOutputList;

	ObjectIdSVOutputObjectPtrMap m_OutputObjects;
};


