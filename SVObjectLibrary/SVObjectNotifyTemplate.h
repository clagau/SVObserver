//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObjectNotifyTemplate
//* .File Name       : $Workfile:   SVObjectNotifyTemplate.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 16:58:12  $
//******************************************************************************

#pragma once

template< typename SVNotifyData >
class SVObjectNotifyTemplate
{
public:
	virtual ~SVObjectNotifyTemplate() {}
	virtual HRESULT ProcessNotifyData( SVNotifyData& p_rData ) = 0;
};

