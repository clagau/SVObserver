//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVIPResultItemData
//* .File Name       : $Workfile:   SVIPResultItemData.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 11:18:04  $
//******************************************************************************

#include "stdafx.h"
#include "SVIPResultItemData.h"

SVIPResultItemData::SVIPResultItemData()
: m_Value(), m_Color( 0 ), m_IOTypePresent( false ), m_IOType( 0 )
{
}

SVIPResultItemData::SVIPResultItemData( const SVString& p_rValue, unsigned long p_Color )
: m_Value( p_rValue ), m_Color( p_Color ), m_IOTypePresent( false ), m_IOType( 0 )
{
}

SVIPResultItemData::SVIPResultItemData( const SVString& p_rValue, unsigned long p_Color, unsigned long p_IOType )
: m_Value( p_rValue ), m_Color( p_Color ), m_IOTypePresent( true ), m_IOType( p_IOType )
{
}

SVIPResultItemData::SVIPResultItemData( const SVIPResultItemData& p_rObject )
: m_Value( p_rObject.m_Value ), m_Color( p_rObject.m_Color ), m_IOTypePresent( p_rObject.m_IOTypePresent ), m_IOType( p_rObject.m_IOType )
{
}

const SVString& SVIPResultItemData::GetValue() const
{
	return m_Value;
}

unsigned long SVIPResultItemData::GetColor() const
{
	return m_Color;
}

bool SVIPResultItemData::IsIOTypePresent() const
{
	return m_IOTypePresent;
}

unsigned long SVIPResultItemData::GetIOType() const
{
	return m_IOType;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVIPResultItemData.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 11:18:04   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   02 Jul 2012 16:58:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/