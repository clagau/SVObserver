//*****************************************************************************
/// \copyright COPYRIGHT (c) 2023,2023 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved
/// \file RElement.h
/// \brief include file defines struct RElement used in SVEquation.y
//******************************************************************************
#pragma once

struct RElement
{
	RElement()
	{
		val = -1.0;
		next = NULL;
	};
	
	double val;
	struct RElement* next;
};