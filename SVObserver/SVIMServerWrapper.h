//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVIMServerWrapper
//* .File Name       : $Workfile:   SVIMServerWrapper.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:58:50  $
//******************************************************************************

#ifndef SVIMSERVERWRAPPER_H
#define SVIMSERVERWRAPPER_H

#include "SVIMServer/SVIMServer.h"

class SVIMServerWrapper  
{
public:
	SVIMServer mSVIMServer;
	SVIMServerWrapper();
	virtual ~SVIMServerWrapper();

};

#endif

