//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMainImageClass
//* .File Name       : $Workfile:   SVMainImageClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   08 May 2013 16:16:48  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVCameraImageTemplate.h"
#pragma endregion Includes

namespace SvIe
{

class SVMainImageClass : public SVCameraImageTemplate 
{
// Wasn't in old implementation, take back out for now. may be needed in future.
// 	SV_DECLARE_CLASS( SVMainImageClass );

public:
	explicit SVMainImageClass( LPCSTR ObjectName );
	SVMainImageClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVMAINIMAGE );

	virtual ~SVMainImageClass();
	
protected:
	virtual bool CreateBuffers( const SVImageInfoClass& p_rImageInfo) override;

private:
	void init();
};

} //namespace SvIe
