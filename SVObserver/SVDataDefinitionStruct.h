// ******************************************************************************
// * COPYRIGHT (c) 2013 by Seidenader
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVDataDefinitionStruct
// * .File Name       : $Workfile:   SVDataDefinitionStruct.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   04 Nov 2013 14:35:26  $
// ******************************************************************************

#ifndef SVDATADEFINITIONSTRUCT_H
#define SVDATADEFINITIONSTRUCT_H

#include "SVUtilityLibrary/SVString.h"
#include <comdef.h>
#include <vector>
#include <string>

#pragma pack (push, 1)
struct SVDataDefinitionStruct
{
	SVString m_Name;
	bool m_Writable;
	bool m_Published;
	SVString m_Type;
	SVStringArray m_AdditionalInfo;
public:
	SVDataDefinitionStruct( );
	~SVDataDefinitionStruct( );
	SVDataDefinitionStruct(const SVDataDefinitionStruct& rhs );
	const SVDataDefinitionStruct& operator = (const SVDataDefinitionStruct& rhs );
	bool operator == (const SVDataDefinitionStruct& rhs);
private:
	void Clear();
	void Init();
};
#pragma pack (pop)

typedef std::vector<SVDataDefinitionStruct> SVDataDefinitionStructArray;

#endif

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVDataDefinitionStruct.h_v  $
 * 
 *    Rev 1.0   04 Nov 2013 14:35:26   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  866
 * SCR Title:  Add GetDataDefinitionList Command to SVObserver's Remote Command Socket
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 */