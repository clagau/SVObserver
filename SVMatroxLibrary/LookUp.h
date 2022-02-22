//*****************************************************************************
// \copyright COPYRIGHT (c) 2021/08/02,2021/08/02 by Körber Pharma Inspection GmbH. All Rights Reserved 
/// \file LookUp.h
/// Class for managing a 2dim array using as LUT table for arithmetic image operation
//******************************************************************************

#pragma once
#include <winsmcrd.h>
#include <limits.h>

class LookUp
{
public:
	explicit LookUp(int depth);
	~LookUp();
	LookUp(const LookUp& rhs )=delete;
	LookUp& operator=(const LookUp& rhs) = delete;
	
	void Init(double gain, double offset, int op);
	BYTE get(int x, int y);
	int getDepth() { return m_depth; }
	const BYTE*& getDataRef() { return const_cast<const BYTE*&>(  m_Data); }
private:
	int m_depth {0};
	double m_gain = INT_MAX;
	double m_offset = INT_MAX;
	int m_operator = INT_MAX;
	BYTE* m_Data;
};



