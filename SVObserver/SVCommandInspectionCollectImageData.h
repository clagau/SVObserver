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
#include "TriggerRecordController/ITriggerRecordR.h"
#pragma endregion Includes

class SVInspectionProcess;

struct SVCommandInspectionCollectImageData
{
#pragma region Constructor
	SVCommandInspectionCollectImageData(const SVCommandInspectionCollectImageData& p_rObject) = delete;
	SVCommandInspectionCollectImageData(uint32_t inspectionId, const std::set<uint32_t>& p_rImageIds);

	virtual ~SVCommandInspectionCollectImageData();
#pragma endregion Constructor

	HRESULT Execute();

	bool empty() const;

	uint32_t GetInspectionId() const;
	const std::set<uint32_t>& GetImageIds() const;

	const SVIPProductStruct& GetProduct() const;

protected:
	HRESULT UpdateResults( SVInspectionProcess* pInspection, SvIe::SVIPResultData& rResultData );

	HRESULT UpdateBuffer(uint32_t imageId, const SvTrc::ITriggerRecordRPtr& pTriggerRecord, std::string& rImageDIB, SVExtentMultiLineStructVector& rMultiLineArray);

private:
	uint32_t m_InspectionId;
	std::set<uint32_t> m_ImageIds;
	SVIPProductStruct m_Product;
};

typedef std::shared_ptr< SVCommandInspectionCollectImageData > SVCommandInspectionCollectImageDataPtr;

