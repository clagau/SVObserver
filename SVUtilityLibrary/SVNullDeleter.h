//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVNullDeleter
//* .File Name       : $Workfile:   SVNullDeleter.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 19:22:58  $
//******************************************************************************

#pragma once

/**
@SVObjectName Null Deleter

@SVObjectOverview A NULL deleter to use a Shared Pointer with a statically allocated object

@SVObjectOperations

*/

struct SVNullDeleter
{
    void operator()(void const *) const
    {
    }
};

