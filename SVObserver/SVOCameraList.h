//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOCameraList
//* .File Name       : $Workfile:   SVOCameraList.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:48:40  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVContainerLibrary/SVList.h"
#include "SVOMFCLibrary/SVDeviceParamCollection.h"
#include "SVOCameraObj.h"
#pragma endregion Includes

class SVOCameraList  
{
public:
	typedef SVList< SVOCameraObjPtr > SVCameraList;
	typedef SVCameraList::iterator iterator;
	typedef SVCameraList::const_iterator const_iterator;

	SVOCameraList();
	virtual ~SVOCameraList();

    SVOCameraList& operator=(const SVOCameraList &source);
    SVOCameraList* operator=(const SVOCameraList *source);

    BOOL AddCameraToList(CString sCameraName);
    BOOL AddCameraToList(CString sCameraName, int iDig);
    BOOL AddCameraToList(CString sCameraName, int iDig, int iBands);
    BOOL SetCameraFile(CString sCameraName, CString sFileName);
	BOOL SetCameraDeviceParams( CString sCameraName, const SVDeviceParamCollection& rCameraDeviceParams, const SVDeviceParamCollection& rCameraFileParams );
    BOOL RemoveCameraFromList(CString sCameraName);

    SVOCameraObjPtr GetCameraObjectByName(CString sCameraName);
    SVOCameraObjPtr GetCameraObjectByPosition(int iPos);
	const SVOCameraObjPtr GetCameraObjectByPosition(int iPos) const;
    BOOL IsCameraInList(CString sCameraName) const;

    void ResetContent();

    int GetCameraListCount() const;

private:
	SVCameraList m_CameraList;
    
	iterator FindCameraPosition( CString sCameraName );
};

