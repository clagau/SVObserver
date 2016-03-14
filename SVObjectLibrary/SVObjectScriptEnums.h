//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObjectScriptEnums
//* .File Name       : $Workfile:   SVObjectScriptEnums.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 17:00:34  $
//******************************************************************************

#ifndef _SVOBJECTSCRIPTENUMS_H
#define _SVOBJECTSCRIPTENUMS_H

#ifdef SV_SHOW_INCLUDES
	#pragma message( "Including " __FILE__ )
#endif


////////////////////////////////////////////////////////////////////////////////
// .Title       : SVObjectScriptDataObjectTypeEnum
// -----------------------------------------------------------------------------
// .Description : Enumerated Types for Object Script Data Objects
//              : 
//              :
// -----------------------------------------------------------------------------
// .Export
//	 Public Method				Description
//  :
//  :
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :08.07.1999 SEJ			First Implementation
////////////////////////////////////////////////////////////////////////////////
enum SVObjectScriptDataObjectTypeEnum
{
	SV_UNKNOWN_Type,
	SV_BOOL_Type,
	SV_BYTE_Type,
	SV_DWORD_Type,
	SV_LONG_Type,
	SV_DOUBLE_Type,
	SV_POINT_Type,
	SV_STRING_Type,
	SV_DPOINT_Type,
	SV_VARIANT_Type,
	SV_INT64_Type,
};

#endif   /* _SVOBJECTSCRIPTENUMS_H */

