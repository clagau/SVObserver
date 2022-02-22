//******************************************************************************
//* COPYRIGHT (c) 2010 by K�rber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCommandResultsFacade
//* .File Name       : $Workfile:   SVCommandResultsFacade.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   18 Apr 2013 18:31:40  $
//******************************************************************************

#pragma once

template< typename SVResultCollection >
class SVCommandResultsFacade
{
public:
	virtual ~SVCommandResultsFacade() {}

	virtual HRESULT GetResults( SVResultCollection& p_rResults ) const = 0;

};

