//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxBufferArray
//* .File Name       : $Workfile:   SVMatroxBufferArray.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 14:59:58  $
//******************************************************************************
#include "stdafx.h"
#include "SVMatroxBufferArray.h"
#include "SVMatroxBufferInterface.h"

SVMatroxBufferArray::SVMatroxBufferArray()
{
}

SVMatroxBufferArray::SVMatroxBufferArray(const SVMatroxBufferArray& p_rBufArray)
{
	m_BufferIdentifierList = p_rBufArray.m_BufferIdentifierList;
}

SVMatroxBufferArray::~SVMatroxBufferArray()
{
	// Destroy(); // can this be called here or is the Framework too far gone at this point ?
}

SVMatroxBufferArray::SVStatusCode SVMatroxBufferArray::Create(const SVMatroxSystem& rSystem, const SVMatroxBufferCreateStruct& p_rCreateStruct, long count)
{
	LocalClear();
	
	SVStatusCode statusCode( SVMEE_STATUS_OK );
	m_BufferIdentifierList.resize(count);	// reserve storage

	SVMatroxBufferInterface l_Interface;
	SVMatroxBuffer l_Buffer;
	for (int i = 0;i < count;i++)
	{
		SVMatroxBufferInterface::SVStatusCode l_Code = l_Interface.Create(rSystem, l_Buffer, p_rCreateStruct);
		if (l_Code == SVMEE_STATUS_OK)
		{
			m_BufferIdentifierList[i] = l_Buffer;
		}
		else
		{
			statusCode = l_Code;
			break;
		}
	}
	
	return statusCode;
}

void SVMatroxBufferArray::Destroy()
{
	LocalClear();
}

bool SVMatroxBufferArray::empty() const
{
	return (m_BufferIdentifierList.size()) ? false : true;
}

SVMatroxBufferArray::BufferIDList SVMatroxBufferArray::GetList() const
{
	BufferIDList l_List( m_BufferIdentifierList.size() );

	for( size_t i = 0; i < m_BufferIdentifierList.size(); ++i )
	{
		l_List[ i ] = m_BufferIdentifierList[ i ].GetIdentifier();
	}

	return l_List;
}

void SVMatroxBufferArray::LocalClear()
{
	m_BufferIdentifierList.clear();		// release elements
	m_BufferIdentifierList.resize(0);	// release storage
}

void SVMatroxBufferArray::swap( SVMatroxBufferArray& p_rBufArray )
{
	BufferList l_IDList = m_BufferIdentifierList;
	m_BufferIdentifierList = p_rBufArray.m_BufferIdentifierList;
	p_rBufArray.m_BufferIdentifierList = l_IDList;
}

SVMatroxBufferArray& SVMatroxBufferArray::operator=( const SVMatroxBufferArray& p_rBufArray )
{
	m_BufferIdentifierList = p_rBufArray.m_BufferIdentifierList;

	return *this;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMatroxLibrary\SVMatroxBufferArray.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 14:59:58   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   27 Jan 2011 10:31:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   02 Jun 2009 15:27:06   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
