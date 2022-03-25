//******************************************************************************
//* COPYRIGHT (c) 2009 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#pragma endregion Includes

class SVMatroxDigitizerInterface;

/**
@SVObjectName Matrox Digitizer

@SVObjectOverview This class is a data holder for a matrox digitizer id.

@SVObjectOperations The empty function is used to detect if the handle is empty.  When this class is destroyed the the LocalClear will free the matrox handle.

*/

class SVMatroxDigitizer
{
	friend SVMatroxDigitizerInterface;

public:
	SVMatroxDigitizer();
	SVMatroxDigitizer(const SVMatroxDigitizer& rRhs) = delete;
	virtual ~SVMatroxDigitizer();

	SVMatroxDigitizer& operator=(const SVMatroxDigitizer& rRhs) = delete;
	bool operator<( const SVMatroxDigitizer& rDigitizer ) const;

	bool empty() const;

protected:
	__int64 m_DigitizerIdentifier;

private:
	void LocalClear();
};

typedef std::shared_ptr<SVMatroxDigitizer> SVMatroxDigitizerPtr;
