// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVProcessFunctor
// * .File Name       : $Workfile:   SVProcessFunctor.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   25 Apr 2013 12:22:22  $
// ******************************************************************************

#pragma once

template< typename SVT_CLASS >
class SVProcessFunctor
{
public:
	SVProcessFunctor();
	explicit SVProcessFunctor(SVT_CLASS* p_pObject);
	SVProcessFunctor( const SVProcessFunctor& p_rObject );

	const SVProcessFunctor& operator=( const SVProcessFunctor& p_rObject );

	void operator()( bool& p_WaitForEvents );

protected:
	SVT_CLASS* m_pObject;

};

#include "SVProcessFunctor.inl"

