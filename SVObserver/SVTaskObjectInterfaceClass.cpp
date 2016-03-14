//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTaskObjectInterfaceClass
//* .File Name       : $Workfile:   SVTaskObjectInterfaceClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2013 11:25:32  $
//******************************************************************************

#include "StdAfx.h"
#include "SVTaskObjectInterfaceClass.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SVTaskObjectInterfaceClass::SVTaskObjectInterfaceClass()
: SVTaskObjectValueInterface()
{
}

SVTaskObjectInterfaceClass::SVTaskObjectInterfaceClass( SVTaskObjectClass *p_psvTaskObject )
: SVTaskObjectValueInterface( p_psvTaskObject )
{
}

SVTaskObjectInterfaceClass::~SVTaskObjectInterfaceClass()
{
}

