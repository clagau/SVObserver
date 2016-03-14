//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGigeCameraManager
//* .File Name       : $Workfile:   SVGigeCameraManager.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   05 Jul 2013 09:11:02  $
//******************************************************************************

#ifndef SVGIGECAMERAMANAGER_H
#define SVGIGECAMERAMANAGER_H

#include <map>
#include "SVGigeCameraStruct.h"

class SVGigeCameraManager  
{
public:
	static SVGigeCameraManager& Instance();

	HRESULT Refresh();

	const SVGigeCameraStructSet& GetCameraOrder() const;

	const SVGigeCameraStructSet& GetOriginalCameraIniOrder() const;

	HRESULT UpdateCameraOrder( const SVGigeCameraStructSet& rSVCameraList, bool ignoreHandles=false );
	HRESULT UpdateConnectedCameras( const SVGigeCameraStructSet& rSVCameraList );

	void UpdateCameraIniList();

protected:
	typedef std::map< CString, SVGigeCameraStruct > SVConnectedCameraMap;

	SVGigeCameraManager();

	// For a singleton object, this constructor is here to warn of a invalid copy construction.  Do not implement this constructor.
	SVGigeCameraManager( const SVGigeCameraManager& p_rObject );

	// For a singleton object, this operator is here to warn of a invalid assignement operator.  Do not implement this operator.
	const SVGigeCameraManager& operator=( const SVGigeCameraManager& p_rObject );

	void GetCameraOrder( SVGigeCameraStructSet& rSVCameraList ) const;
	void GetConnectedCameras( SVConnectedCameraMap& rSVCameraMap ) const;

	HRESULT OrderCameras( SVConnectedCameraMap& rSVCameraMap );

	SVGigeCameraStructSet m_OrderedCamaras;

	//Hold the orginal camera order from INI
	SVGigeCameraStructSet m_iniCameras;

};

#define TheSVGigeCameraManager SVGigeCameraManager::Instance()

#endif

