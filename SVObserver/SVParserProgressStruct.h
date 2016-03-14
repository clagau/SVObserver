//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVParserProgressStruct
//* .File Name       : $Workfile:   SVParserProgressStruct.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:18:16  $
//******************************************************************************
#ifndef INCL_SVPARSERPROGRESSSTRUCT_H
#define INCL_SVPARSERPROGRESSSTRUCT_H

struct SVParserProgressStruct
{
	size_t amtProcessed;
	size_t amtTotal;
	unsigned long parserHandle;

	SVParserProgressStruct(unsigned long p_parserHandle)
	: parserHandle(p_parserHandle), amtProcessed(0), amtTotal(0) 
	{
	}
};

#endif

