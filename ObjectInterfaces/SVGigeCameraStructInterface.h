//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGigeCameraStructInterface
//* .File Name       : $Workfile:   SVGigeCameraStructInterface.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   12 Jun 2013 15:48:52  $
//******************************************************************************
#pragma once

namespace Seidenader
{
	namespace ObjectInterfaces
	{	
		struct SVGigeCameraStructInterface
		{
			enum CameraChangeTypeEnum
			{
				SVUndefined,
				SVNoChange,
				SVNewCamera,
				SVReplacedCamera,
				SVCameraRemoved
			};

			SVGigeCameraStructInterface(){};
			virtual ~SVGigeCameraStructInterface(){};

			virtual bool HasSerialNumber() const=0;
			virtual CString GetSerialNumber() const=0;
			virtual bool HasModelName() const=0;
			virtual CString GetModelName() const=0;

			virtual bool HasIPAddress() const=0;
			virtual CString GetIPAddress() const=0;

			CameraChangeTypeEnum eChangeType;
		};
	}
}

namespace SvOi = Seidenader::ObjectInterfaces;

