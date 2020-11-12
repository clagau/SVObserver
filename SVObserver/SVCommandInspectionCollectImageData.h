//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCommandInspectionCollectImageData
//* .File Name       : $Workfile:   SVCommandInspectionCollectImageData.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   07 Jan 2015 17:39:56  $
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <memory>
//Moved to precompiled header: #include <set>
#include "SVImageLibrary/SVExtentMultiLineStruct.h"
#include "SVIPProductStruct.h"
#include "ObjectInterfaces/ITriggerRecordR.h"
#pragma endregion Includes

class SVInspectionProcess;

class  CommandInspectionCollectImageData
{
public:
#pragma region Constructor
	CommandInspectionCollectImageData(uint32_t inspectionId, const std::set<uint32_t>& p_rImageIds);
	~CommandInspectionCollectImageData();
#pragma endregion Constructor

	std::shared_ptr<SVIPProductStruct> operator() ();
private:

	HRESULT UpdateResults(SVInspectionProcess* pInspection, SvIe::SVIPResultData& rResultData);

	HRESULT UpdateBuffer(uint32_t imageId, const SvOi::ITriggerRecordRPtr& pTriggerRecord, std::string& rImageDIB, SVExtentMultiLineStructVector& rMultiLineArray);


	uint32_t m_InspectionId;
	std::set<uint32_t> m_ImageIds;

};


using CollectImageDataTask = std::packaged_task<std::shared_ptr<SVIPProductStruct>()>;
