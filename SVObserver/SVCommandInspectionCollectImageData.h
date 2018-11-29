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
#include "InspectionEngine/SVCameraInfoStruct.h"
#include "TriggerRecordController/ITriggerRecordR.h"
#pragma endregion Includes

class SVInspectionProcess;

struct SVCommandInspectionCollectImageData
{
#pragma region Constructor
	SVCommandInspectionCollectImageData(const SVCommandInspectionCollectImageData& p_rObject);
	SVCommandInspectionCollectImageData(const SVGUID& p_rInspectionId, const SVGuidSet& p_rImageIds);

	virtual ~SVCommandInspectionCollectImageData();
#pragma endregion Constructor

	HRESULT Execute();

	bool empty() const;

	const SVGUID& GetInspectionId() const;
	const SVGuidSet& GetImageIds() const;

	const SVIPProductStruct& GetProduct() const;

protected:
	HRESULT UpdateResults( SVInspectionProcess* p_pInspection, SVIPResultData& p_rResultData );

	HRESULT UpdateBuffer(const SVGUID& rImageId, const SvTrc::ITriggerRecordRPtr& pTriggerRecord, std::string& rImageDIB, SVExtentMultiLineStructVector& rMultiLineArray);

private:
	SVGUID m_InspectionId;
	SVGuidSet m_ImageIds;
	SVIPProductStruct m_Product;
};

typedef std::shared_ptr< SVCommandInspectionCollectImageData > SVCommandInspectionCollectImageDataPtr;

