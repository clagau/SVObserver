//*****************************************************************************
/// \copyright (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file MatroxBuffer2D.h
/// All Rights Reserved 
//*****************************************************************************
/// 2dim array for MatroxBuffer 
//******************************************************************************
#pragma once
#include "SVMatroxBuffer.h"
//Moved to precompiled header: #include <vector>
/// 2dim array for MatroxBuffer 
class MatroxBuffer2D
{
public:
	MatroxBuffer2D(void);
	~MatroxBuffer2D(void);
	
	void SetDim(DWORD SlotCount,DWORD ImageCount );
	SVMatroxBuffer&  GetAt(DWORD SlotIndex,DWORD ImageIndex);
	void Clear();
	
private:
	//Do not implement
	MatroxBuffer2D(const MatroxBuffer2D& mb);

	DWORD  m_SlotCount;
	DWORD  m_ImageCount;
	std::vector<SVMatroxBuffer> m_data; 
};

typedef std::unique_ptr<MatroxBuffer2D>  UP_MatroxBuffer2D;