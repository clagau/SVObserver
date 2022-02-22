//******************************************************************************
//* COPYRIGHT (c) 2010 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObserverTemplate
//* .File Name       : $Workfile:   SVObserverTemplate.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 17:20:30  $
//******************************************************************************

#pragma once

template< typename SVDataType >
class SVObserverTemplate
{
public:
	virtual ~SVObserverTemplate() {}
	virtual HRESULT ObserverUpdate( const SVDataType& p_rData ) = 0;
};


