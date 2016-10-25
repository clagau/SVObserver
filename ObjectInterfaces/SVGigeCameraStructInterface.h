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

#pragma region Includes
#include "SVUtilityLibrary\SVString.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace ObjectInterfaces
	{	
		struct SVGigeCameraStructInterface
		{
			SVGigeCameraStructInterface(){};
			virtual ~SVGigeCameraStructInterface(){};

			virtual bool HasSerialNumber() const=0;
			virtual SVString GetSerialNumber() const=0;
			virtual bool HasModelName() const=0;
			virtual SVString GetModelName() const=0;

			virtual bool HasIPAddress() const=0;
			virtual SVString GetIPAddress() const=0;
		};
	}
}

namespace SvOi = Seidenader::ObjectInterfaces;

