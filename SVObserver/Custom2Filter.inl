//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : Custom2Filter
//* .File Name       : $Workfile:   Custom2Filter.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Oct 2014 11:15:00  $
//******************************************************************************

#pragma region Public Methods
inline SVLongValueObjectClass& Custom2Filter::getKernelArray()
{
	return m_KernelArray;
}

inline SVLongValueObjectClass& Custom2Filter::getKernelWidth()
{
	return m_KernelWidth;
}

inline SVLongValueObjectClass& Custom2Filter::getKernelHeight()
{
	return m_KernelHeight;
}

inline SVLongValueObjectClass& Custom2Filter::getNormalizationFactor()
{
	return m_NormalizationFactor;
}

inline SVBoolValueObjectClass& Custom2Filter::getAbsoluteValue()
{
	return m_AbsoluteValue;
}

inline SVBoolValueObjectClass& Custom2Filter::getClippingEnabled()
{
	return m_ClippingEnabled;
}
#pragma endregion Public Methods

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\Custom2Filter.inl_v  $
 * 
 *    Rev 1.0   24 Oct 2014 11:15:00   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  942
 * SCR Title:  Create new Custom2 Filter SVO-324 SVO-67 SVO-74
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Initial check in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 */