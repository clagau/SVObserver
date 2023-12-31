//******************************************************************************
//* COPYRIGHT (c) 2012 by K�rber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVParserProgressStruct
//* .File Name       : $Workfile:   SVParserProgressStruct.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:18:16  $
//******************************************************************************

#pragma once

struct SVParserProgressStruct
{
	size_t amtProcessed;
	size_t amtTotal;
	unsigned long parserHandle;

	explicit SVParserProgressStruct(unsigned long p_parserHandle)
	: parserHandle(p_parserHandle), amtProcessed(0), amtTotal(0) 
	{
	}
};


