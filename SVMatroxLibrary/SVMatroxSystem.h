//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxSystem
//* .File Name       : $Workfile:   SVMatroxSystem.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 15:27:34  $
//******************************************************************************

#pragma once

//Moved to precompiled header: #include <boost/utility.hpp>

class SVMatroxSystemInterface;
class SVMatroxBufferInterface;
class SVMatroxDigitizerInterface;

/**
@SVObjectName Matrox System

@SVObjectOverview This class is a data holder for a matrox system id.

@SVObjectOperations The empty function is used to detect if the handle is empty.  When this class is destroyed the the LocalClear will free the matrox handle.

*/
class SVMatroxSystem
{
	friend SVMatroxSystemInterface;
	friend SVMatroxBufferInterface;
	friend SVMatroxDigitizerInterface;

public:
	SVMatroxSystem();
	SVMatroxSystem(const SVMatroxSystem&) = delete;
	SVMatroxSystem(SVMatroxSystem&&) = delete;

	virtual ~SVMatroxSystem();

	SVMatroxSystem& operator=(const SVMatroxSystem&) = delete;

	bool operator<( const SVMatroxSystem& p_rSystem ) const;

	bool empty() const;

protected:
	__int64 m_SystemIdentifier;

private:
	void LocalClear();
};

