//******************************************************************************
//* COPYRIGHT (c) 2010 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObserverTemplate
//* .File Name       : $Workfile:   SVObserverTemplate.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 17:20:30  $
//******************************************************************************

#ifndef SVOBSERVERTEMPLATE_H
#define SVOBSERVERTEMPLATE_H

template< typename SVDataType >
class SVObserverTemplate
{
public:
	virtual HRESULT ObserverUpdate( const SVDataType& p_rData ) = 0;

};

#endif

