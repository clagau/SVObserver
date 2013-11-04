//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxBufferArray
//* .File Name       : $Workfile:   SVMatroxBufferArray.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 15:00:04  $
//******************************************************************************
#ifndef SVMATROXBUFFERARRAY_H
#define SVMATROXBUFFERARRAY_H

#include "SVMatroxStatusInformation.h"
#include "SVMatroxSystem.h"
#include "SVMatroxBufferCreateStruct.h"
#include "SVMatroxBuffer.h"

class SVMatroxBufferArray
{
public:
	friend class SVMatroxBufferInterface;
	friend class SVMatroxDigitizerInterface;

	typedef SVMatroxStatusInformation::SVStatusCode SVStatusCode;

	SVMatroxBufferArray();
	SVMatroxBufferArray(const SVMatroxBufferArray& p_rBufArray);

	virtual ~SVMatroxBufferArray();

	void swap( SVMatroxBufferArray& p_rBufArray );

	SVMatroxBufferArray& operator=( const SVMatroxBufferArray& p_rBufArray );

	SVStatusCode Create(const SVMatroxSystem& rSystem, const SVMatroxBufferCreateStruct& rCreateStruct, long count);
	void Destroy();

	bool empty() const;

protected:
	typedef std::vector< SVMatroxIdentifier > BufferIDList;
	typedef std::vector< SVMatroxBuffer > BufferList;

	BufferIDList GetList() const;

	BufferList m_BufferIdentifierList;

private:
	void LocalClear();
};

#endif
//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMatroxLibrary\SVMatroxBufferArray.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 15:00:04   bWalter
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
 *    Rev 1.0   02 Jun 2009 15:27:04   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/

