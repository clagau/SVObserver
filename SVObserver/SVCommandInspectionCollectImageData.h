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
#include <set>
#include "SVImageLibrary/SVExtentMultiLineStruct.h"
#include "SVRunControlLibrary/SVImageIndexStruct.h"
#include "SVUtilityLibrary/SVSharedPtr.h"
#include "SVUtilityLibrary/SVUtilityGlobals.h"
#include "SVIPProductStruct.h"
#include "SVInfoStructs.h"
#pragma endregion Includes

struct SVCommandInspectionCollectImageData
{
	typedef std::set< SVGUID > SVImageIdSet;

#pragma region Constructor
	SVCommandInspectionCollectImageData(const SVCommandInspectionCollectImageData& p_rObject);
	SVCommandInspectionCollectImageData(const SVGUID& p_rInspectionId, const SVImageIdSet& p_rImageIds);

	virtual ~SVCommandInspectionCollectImageData();
#pragma endregion Constructor

	HRESULT Execute();

	bool empty() const;

	const SVGUID& GetInspectionId() const;
	const SVImageIdSet& GetImageIds() const;

	const SVIPProductStruct& GetProduct() const;

protected:
	HRESULT UpdateResults( SVInspectionProcess* p_pInspection, SVIPResultData& p_rResultData );

	HRESULT UpdateBuffer( const SVGUID& p_rImageId, SVStdMapSVVirtualCameraPtrSVCameraInfoStruct& p_rsvCameraInfos,
		SVImageIndexStruct p_svResultImageIndex, long p_ResultDataIndex, 
		SVByteVector& p_rImageDIB, SVExtentMultiLineStructCArray& p_rMultiLineArray );

private:
	SVGUID m_InspectionId;
	SVImageIdSet m_ImageIds;
	SVIPProductStruct m_Product;
};

typedef SVSharedPtr< SVCommandInspectionCollectImageData > SVCommandInspectionCollectImageDataPtr;

