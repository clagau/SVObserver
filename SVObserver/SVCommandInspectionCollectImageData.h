//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCommandInspectionCollectImageData
//* .File Name       : $Workfile:   SVCommandInspectionCollectImageData.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   30 Sep 2014 16:07:52  $
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

	//************************************
	// Method:    setToolSetResultData
	// Description:  Set the viewable toolset objects to the resultData-list
	// Parameter: SVInspectionProcess* p_pInspection Pointer to the inspection
	// Parameter: SVIPResultData& p_rResultData Reference to the resultData-list
	// Returns:   HRESULT Error return value S_OK if no error, else E_FAIL.
	//************************************
	HRESULT setToolSetResultData( SVInspectionProcess* p_pInspection, SVIPResultData& p_rResultData );

	//************************************
	// Method:    setPPQInputResultData
	// Description:  Set the viewable PPQ input objects to the resultData-list
	// Parameter: SVInspectionProcess* p_pInspection Pointer to the inspection
	// Parameter: SVIPResultData& p_rResultData Reference to the resultData-list
	// Returns:   HRESULT Error return value S_OK if no error, else E_FAIL.
	//************************************
	HRESULT setPPQInputResultData( SVInspectionProcess* p_pInspection, SVIPResultData& p_rResultData );

	//************************************
	// Method:    setPPQ_XParameterResultData
	// Description:  Set the viewable PPQ parameter objects to the resultData-list
	// Parameter: SVInspectionProcess* p_pInspection Pointer to the inspection
	// Parameter: SVIPResultData& p_rResultData Reference to the resultData-list
	// Returns:   HRESULT Error return value S_OK if no error, else E_FAIL.
	//************************************
	HRESULT setPPQ_XParameterResultData( SVInspectionProcess* p_pInspection, SVIPResultData& p_rResultData );

	//************************************
	// Method:    setEnvironmentResultData
	// Description:  Set the viewable environment parameter objects to the resultData-list
	// Parameter: SVIPResultData& p_rResultData Reference to the resultData-list
	// Returns:   HRESULT Error return value S_OK if no error, else E_FAIL.
	//************************************
	HRESULT setEnvironmentResultData( SVIPResultData& p_rResultData );

	//************************************
	// Method:    addViewableObjfct2ResultData
	// Description:  Add one object to the resultData-list if it viewable
	// Parameter: SVObjectClass* object 
	// Parameter: SVIPResultData& p_rResultData Reference to the resultData-list
	// Parameter: unsigned long colorValue Displayed color, Default value is 0x00ffffff.
	// Returns:   HRESULT Error return value S_OK if no error or object is not viewable, else E_FAIL.
	//************************************
	HRESULT addViewableObject2ResultData( SVObjectClass* object, SVIPResultData& p_rResultData, unsigned long colorValue = 0x00ffffff );

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
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVCommandInspectionCollectImageData.h_v  $
 * 
 *    Rev 1.1   30 Sep 2014 16:07:52   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  925
 * SCR Title:  Add PPQ Items and SVObserver Modes to Equation Editor Object Selector
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Added new methods:
 * setToolSetResultData,
 * setPPQInputResultData,
 * setPPQ_XParameterResultData,
 * setEnvironmentResultData,
 * and addViewableObject2ResultData.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
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