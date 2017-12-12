//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxDigitizer
//* .File Name       : $Workfile:   SVMatroxDigitizer.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 14:27:10  $
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <memory>
//Moved to precompiled header: #include <boost/utility.hpp>
#include "SVMatroxLibrary/SVMatroxTypedefs.h"
#pragma endregion Includes

class SVMatroxDigitizerInterface;

/**
@SVObjectName Matrox Digitizer

@SVObjectOverview This class is a data holder for a matrox digitizer id.

@SVObjectOperations The empty function is used to detect if the handle is empty.  When this class is destroyed the the LocalClear will free the matrox handle.

*/

class SVMatroxDigitizer : public boost::noncopyable
{
	friend SVMatroxDigitizerInterface;

public:
	SVMatroxDigitizer();
	virtual ~SVMatroxDigitizer();

	bool operator<( const SVMatroxDigitizer& p_rDigitizer ) const;

	bool empty() const;

protected:
	SVMatroxIdentifier m_DigitizerIdentifier;

private:
	void LocalClear();
};

typedef std::shared_ptr<SVMatroxDigitizer> SVMatroxDigitizerPtr;
