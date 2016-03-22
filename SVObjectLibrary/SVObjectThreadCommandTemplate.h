//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObjectThreadCommandTemplate
//* .File Name       : $Workfile:   SVObjectThreadCommandTemplate.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 17:01:08  $
//******************************************************************************

#pragma once

#include "SVCommandLibrary/SVCommandTemplate.h"

template< typename SVCommandPtr >
class SVObjectThreadCommandTemplate : public SVCommandTemplate
{
public:
	SVObjectThreadCommandTemplate( const SVCommandPtr& p_rCommandPtr );

	virtual ~SVObjectThreadCommandTemplate();

	virtual HRESULT Execute();

private:
	SVCommandPtr m_CommandPtr;

};

#include "SVObjectThreadCommandTemplate.inl"

