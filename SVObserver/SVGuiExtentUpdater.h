//******************************************************************************
//* COPYRIGHT (c) 2012 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGuiExtentUpdater
//* .File Name       : $Workfile:   SVGuiExtentUpdater.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   12 Feb 2015 03:00:46  $
//******************************************************************************
#pragma once
#pragma region Includes
#pragma endregion Includes

class SVImageExtentClass;

namespace SvIe
{
class SVTaskObjectClass;
}

class SVGuiExtentUpdater
{
public:
	static HRESULT UpdateImageExtent(SvIe::SVTaskObjectClass* pTaskObject, const SVImageExtentClass& rExtents);
	static HRESULT SetImageExtentToFit(SvIe::SVTaskObjectClass* pTaskObject, const SVImageExtentClass& rExtents);

private:
	static const int TIMEOUT_FOR_SYNCHRONOUS_EXECUTE_IN_MS = 120000;
};
