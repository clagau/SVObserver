// ******************************************************************************
// * COPYRIGHT (c) 2007 by K�rber Pharma Inspection GmbH. All Rights Reserved
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMatroxOcr
// * .File Name       : $Workfile:   SVMatroxOcr.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 15:21:12  $
// ******************************************************************************

#pragma once

#pragma region Includes
#pragma endregion Includes

class SVMatroxOcrInterface;

/**
@SVObjectName Matrox Ocr

@SVObjectOverview This is a data class that holds a font object identifier.

@SVObjectOperations This class has a function to detect if it is empty.  When this class is destroyed the matrox handle will be freed.

*/
class SVMatroxOcr  
{
	friend SVMatroxOcrInterface;
public:
	SVMatroxOcr();
	virtual ~SVMatroxOcr();
	bool empty() const;
	bool operator<(const SVMatroxOcr& p_rBuf) const {return m_OcrFontID < p_rBuf.m_OcrFontID;};
	bool operator==( const SVMatroxOcr& p_rhs ) const ;

	bool				m_bVerify;
	std::string		m_VerifyString;

protected:
	__int64	m_OcrFontID;

private:
	void LocalClear();
};

