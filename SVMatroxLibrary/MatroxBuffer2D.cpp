//*****************************************************************************
/// \copyright (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file MatroxBuffer2D.cpp
/// All Rights Reserved 
//*****************************************************************************
/// 2dim array for MatroxBuffer 
//******************************************************************************
#include "StdAfx.h"
#include "MatroxBuffer2D.h"


MatroxBuffer2D::MatroxBuffer2D(void): m_SlotCount(0), m_ImageCount(0)
{
}


MatroxBuffer2D::~MatroxBuffer2D(void)
{
	Clear();
}


SVMatroxBuffer&  MatroxBuffer2D::GetAt(DWORD SlotIndex,DWORD ImageIndex)
{
	DWORD index = SlotIndex   + ImageIndex* m_SlotCount ;
	return m_data[index];

}
void MatroxBuffer2D::Clear()
{
	m_data.clear();
	m_SlotCount = 0;
	m_ImageCount = 0;
}

void MatroxBuffer2D::SetDim(DWORD SlotCount,DWORD ImageCount )
{
	m_data.clear();
	m_SlotCount = SlotCount;
	m_ImageCount = ImageCount;
	m_data.resize(SlotCount*ImageCount);
}