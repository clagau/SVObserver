//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCommandInspectionCollectImageData
//* .File Name       : $Workfile:   SVCommandInspectionCollectImageData.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:56:12  $
//******************************************************************************

#ifndef SVCOMMANDINSPECTIONCOLLECTIMAGEDATA_H
#define SVCOMMANDINSPECTIONCOLLECTIMAGEDATA_H

#include <set>
#include "SVImageLibrary/SVExtentMultiLineStruct.h"
#include "SVRunControlLibrary/SVImageIndexStruct.h"
#include "SVUtilityLibrary/SVSharedPtr.h"
#include "SVUtilityLibrary/SVUtilityGlobals.h"
#include "SVIPProductStruct.h"
#include "SVInfoStructs.h"

struct SVCommandInspectionCollectImageData
{
	typedef std::set< SVGUID > SVImageIdSet;

	SVCommandInspectionCollectImageData();
	SVCommandInspectionCollectImageData(const SVCommandInspectionCollectImageData& p_rObject);
	SVCommandInspectionCollectImageData(const SVGUID& p_rInspectionId, const SVImageIdSet& p_rImageIds);

	virtual ~SVCommandInspectionCollectImageData();

	HRESULT Execute();

	bool empty() const;
	void clear();

	const SVGUID& GetInspectionId() const;
	const SVImageIdSet& GetImageIds() const;

	HRESULT SetCommandData(const SVGUID& p_rInspectionId, const SVImageIdSet& p_rImageIds);

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

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVCommandInspectionCollectImageData.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 09:56:12   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   05 Apr 2013 12:26:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  812
 * SCR Title:  Add New Remote Command Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include guard.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   01 Aug 2012 11:44:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new command functionality to allow for synchronous command execution through the inspection thread.  This will fix problems with race conditions with the inspection process.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   02 Jul 2012 16:39:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/