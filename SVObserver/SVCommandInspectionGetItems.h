//******************************************************************************
//* COPYRIGHT (c) 2013 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCommandInspectionGetItems
//* .File Name       : $Workfile:   SVCommandInspectionGetItems.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   13 Nov 2014 10:09:08  $
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <memory>
//Moved to precompiled header: #include <set>
#include "SVStorageResult.h"
#include "ObjectInterfaces/ITriggerRecordR.h"
#pragma endregion Includes

class SVInspectionProcess;
class SVObjectReference;
namespace SvIe
{
class SVImageClass;
}
 
class  CommandInspectionGetItems
{
	
public:	

	typedef std::pair<std::string, SVObjectReference> SVFullNameObjectPair;
	typedef std::set<SVFullNameObjectPair> SVNameObjectSet;

	CommandInspectionGetItems(const SVInspectionProcess& p_rInspection, const SVNameObjectSet& p_rItemNames);

	~CommandInspectionGetItems();

	std::pair<HRESULT, std::shared_ptr<SVNameStorageResultMap>>  operator()();
	

private:
	HRESULT UpdateResultsWithImageData(const std::string& rItemName, const SvIe::SVImageClass& rImage, unsigned long TriggerCount, const SvOi::ITriggerRecordRPtr pTriggerRecord);
	HRESULT UpdateResultsWithValueData(const std::string& rItemName, const SVObjectReference& rValueRef, unsigned long TriggerCount);
	HRESULT UpdateResultsWithErrorData(const std::string& rItemName, HRESULT errorStatus, unsigned long TriggerCount);


	const SVInspectionProcess* m_Inspection;
	SVNameObjectSet m_ItemNames;
	std::shared_ptr<SVNameStorageResultMap> m_pResultItems;
	
};

