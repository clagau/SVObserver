//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "SVIODigitalLoadLibraryClass.h"
#include "Definitions/StringTypeDef.h"
#pragma endregion Includes

namespace SvTrig
{
struct ResultData;
}

typedef std::vector<std::pair<uint32_t, _variant_t>> ObjectIdVariantPairVector;

class SVOutputObjectList : public SVObjectClass
{
public:
	explicit SVOutputObjectList( LPCSTR ObjectName );
	SVOutputObjectList( SVObjectClass *pOwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVOUTPUTOBJECTLIST );

	virtual ~SVOutputObjectList() = default;

	SVOutputObjectPtr GetOutput(uint32_t outputID) const;

	SVOutputObjectPtr GetOutput(const std::string& rName) const;

	SVOutputObjectPtr GetOutputFlyweight(const std::string& rName, SvPb::SVObjectSubTypeEnum ObjectSubType, int GuidIndex = -1);

	HRESULT AttachOutput(SVOutputObjectPtr pOutput);
	HRESULT DetachOutput(uint32_t outputID );

	ObjectIdVariantPairVector getOutputValues(const SVIOEntryHostStructPtrVector& rIOEntries, const std::vector<bool>& rOutputResult, bool useDefaults, bool p_ACK, bool p_NAK);
	bool ResetOutputs(const SVIOEntryHostStructPtrVector& rIOEntries);
	bool WriteOutputs(const ObjectIdVariantPairVector& rOutputValues);
	bool WriteOutput( SVIOEntryHostStructPtr pIOEntry, bool p_ACK, bool p_NAK );
	bool WriteOutputValue( SVIOEntryHostStructPtr pIOEntry, const _variant_t& rValue );
	bool ResetOutput( SVIOEntryHostStructPtr pIOEntry );
	bool RenameInspection( LPCTSTR OldInspection, LPCTSTR NewInspectionName);
	HRESULT RemoveUnusedOutputs( const SvDef::StringVector& rInspectionNames, const SvDef::StringVector& rPPQNames );

	SVIOEntryHostStructPtrVector getOutputList() const;

	bool OutputIsNotValid(const std::string& rName);

	typedef  std::map<int, _variant_t> IntVariantMap;
	///This function returns no error because the IO PLC dll is the only type which supports this functionality
	void WriteOutputData(const SvTrig::ResultData& rResultData);
	void setModuleReady(bool value);

#pragma region Methods to replace processMessage
	virtual void OnObjectRenamed(const SVObjectClass& rRenamedObject, const std::string& rOldName) override;
#pragma endregion Methods to replace processMessage

private:
	std::pair<uint32_t, _variant_t> getDigitalOutputValue(const SVIOEntryHostStructPtr& pIOEntry, const std::vector<bool>& rOutputResult, bool useDefault, bool p_ACK, bool p_NAK );
	SVOutputObjectPtr findOutputName(const std::string& rOutputName) const;

	mutable std::mutex m_protectOutputList;

	ObjectIdSVOutputObjectPtrMap m_outputObjectMap;
};


