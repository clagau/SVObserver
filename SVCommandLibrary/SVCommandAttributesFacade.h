//******************************************************************************
//* COPYRIGHT (c) 2010 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCommandAttributesFacade
//* .File Name       : $Workfile:   SVCommandAttributesFacade.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   18 Apr 2013 18:24:12  $
//******************************************************************************

#ifndef SVCOMMANDATTRIBUTESFACADE_H
#define SVCOMMANDATTRIBUTESFACADE_H

/*
*/
template< typename SVAttributeCollection >
class SVCommandAttributesFacade
{
public:
	virtual ~SVCommandAttributesFacade() {}

	virtual HRESULT GetAttributes( SVAttributeCollection& p_rAttributes ) const = 0;
	virtual HRESULT SetAttributes( const SVAttributeCollection& p_rAttributes ) = 0;

};

#endif

