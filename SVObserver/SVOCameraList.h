//******************************************************************************
//* COPYRIGHT (c) 2003 by K�rber Pharma Inspection GmbH. All Rights Reserved
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
//Moved to precompiled header: #include <list>
#include "SVOCameraObj.h"
#pragma endregion Includes

class SVDeviceParamCollection;

class SVOCameraList  
{
public:
	typedef std::list< SVOCameraObjPtr > SVCameraList;
	typedef SVCameraList::iterator iterator;
	typedef SVCameraList::const_iterator const_iterator;

	SVOCameraList();
	virtual ~SVOCameraList();

    SVOCameraList& operator=(const SVOCameraList &source);
    SVOCameraList* operator=(const SVOCameraList *source);

    bool AddCameraToList( LPCTSTR CameraName, int Dig, int CameraID );
    bool AddCameraToList( LPCTSTR CameraName, int Dig, int iBands, int CameraID );
    bool SetCameraFile( LPCTSTR CameraName, LPCTSTR FileName );
	bool SetCameraDeviceParams( LPCTSTR CameraName, const SVDeviceParamCollection& rCameraDeviceParams, const SVDeviceParamCollection& rCameraFileParams );
    bool RemoveCameraFromList( LPCTSTR CameraName );

    SVOCameraObjPtr GetCameraObjectByName( LPCTSTR CameraName );
    SVOCameraObjPtr GetCameraObjectByPosition(int iPos);
	const SVOCameraObjPtr GetCameraObjectByPosition(int iPos) const;
    bool IsCameraInList( LPCTSTR CameraName ) const;

    void ResetContent();

    int GetCameraListCount() const;

private:
	SVCameraList m_CameraList;
    
	iterator FindCameraPosition( LPCTSTR CameraName );
};

