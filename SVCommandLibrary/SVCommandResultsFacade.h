//******************************************************************************
//* COPYRIGHT (c) 2010 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCommandResultsFacade
//* .File Name       : $Workfile:   SVCommandResultsFacade.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   18 Apr 2013 18:31:40  $
//******************************************************************************

#ifndef SVCOMMANDRESULTSFACADE_H
#define SVCOMMANDRESULTSFACADE_H

/*
*/
template< typename SVResultCollection >
class SVCommandResultsFacade
{
public:
	virtual ~SVCommandResultsFacade() {}

	virtual HRESULT GetResults( SVResultCollection& p_rResults ) const = 0;

};

#endif

