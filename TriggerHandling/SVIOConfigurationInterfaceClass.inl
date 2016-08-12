// ******************************************************************************
// * COPYRIGHT (c) 2003 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVIOConfigurationInterfaceClass
// * .File Name       : $Workfile:   SVIOConfigurationInterfaceClass.inl  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 12:44:34  $
// ******************************************************************************

#ifndef SVIOCONFIGURATIONINTERFACECLASS_INL
#define SVIOCONFIGURATIONINTERFACECLASS_INL

#include "SVIOConfigurationInterfaceClass.h"

SVIOConfigurationInterfaceClass& SVIOConfigurationInterfaceClass::Instance()
{
	static SVIOConfigurationInterfaceClass l_Object;

	return l_Object;
}

#endif

