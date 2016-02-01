//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : ResultViewItemDef
//* .File Name       : $Workfile:   ResultViewItemDef.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   07 Jan 2015 17:35:06  $
//******************************************************************************

#pragma region Includes
#include "StdAfx.h"
#include "ResultViewItemDef.h"
#pragma endregion Includes

#pragma region Constructor
ResultViewItemDef::ResultViewItemDef()
: m_ObjectID( SV_GUID_NULL )
, m_hasIndex(false)
,m_Index(0)
{
}

ResultViewItemDef::ResultViewItemDef(const SVGUID& guid)
: m_ObjectID(guid)
, m_hasIndex(false)
,m_Index(0)
{
}

ResultViewItemDef::ResultViewItemDef(const SVGUID& guid, int index)
: m_ObjectID(guid)
, m_hasIndex(true)
, m_Index(index)
{
}

ResultViewItemDef::ResultViewItemDef(const SVObjectReference& objRef)
{
	m_ObjectID = objRef.Guid();
	m_hasIndex = objRef.IsIndexPresent();
	m_Index = objRef.ArrayIndex();
}

ResultViewItemDef::~ResultViewItemDef()
{
}
#pragma endregion Constructor

#pragma region Public Methods
void ResultViewItemDef::setIndex(int  index)
{
	m_hasIndex = true;
	m_Index = index;
}

const SVGUID& ResultViewItemDef::getGuid() const
{
	return m_ObjectID;
}

long ResultViewItemDef::getIndexValue() const
{
	long ret = 0;
	if(m_hasIndex)
	{
		ret = m_Index;
	}

	return ret;
}

bool ResultViewItemDef::hasIndex() const
{
	return m_hasIndex;
}
#pragma endregion Public Methods

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\ResultViewItemDef.cpp_v  $
 * 
 *    Rev 1.0   07 Jan 2015 17:35:06   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  980
 * SCR Title:  Add Non-Inspection Objects to the Result View
 * Checked in by:  mEichengruen;  Marcus Eichengruen
 * Change Description:  
 *   Initial check in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/