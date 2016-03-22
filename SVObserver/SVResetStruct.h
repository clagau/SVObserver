//******************************************************************************
//* COPYRIGHT (c) 2010 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVResetStruct
//* .File Name       : $Workfile:   SVResetStruct.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 14:55:12  $
//******************************************************************************

#pragma once

enum SVResetStateEnum : unsigned long
{
	SVResetStateEmpty                  = 0x00000000,
	SVResetStateAll                    = 0xFFFFFFFF,
	SVResetStateInitializeOnReset      = 0x00000001,
	SVResetStateArchiveToolCreateFiles = 0x00000002,
	SVResetStateLoadFiles              = 0x00000004,
	SVResetAutoMoveAndResize           = 0x00000008,
};

struct SVResetStruct
{
	unsigned long m_State;

	SVResetStruct() : m_State( SVResetStateEmpty ) {}

	bool IsStateSet( unsigned long p_State ) const { return ( ( m_State & p_State ) == p_State ); }

	void AddState( unsigned long p_State ) { m_State = ( m_State | p_State ); }
	void RemoveState( unsigned long p_State ) { m_State = ( m_State & ~( p_State ) ); }

};


