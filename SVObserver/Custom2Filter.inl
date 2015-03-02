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
#pragma region virtual method (ICustom2Filter)
inline HRESULT Custom2Filter::addKernelWidthRequest(long value)
{
	return m_taskObjectValueInterface.AddInputRequest( &m_KernelWidth, value );
}

inline long Custom2Filter::getKernelWidth() const
{ 
	long value = 0;
	m_KernelWidth.GetValue(value);
	return value;
}

inline HRESULT Custom2Filter::addKernelHeightRequest(long value)
{
	return m_taskObjectValueInterface.AddInputRequest( &m_KernelHeight, value );
}

inline long Custom2Filter::getKernelHeight() const
{ 
	long value = 0;
	m_KernelHeight.GetValue(value);
	return value;
}

inline HRESULT Custom2Filter::addClippingEnabledRequest(bool value)
{
	return m_taskObjectValueInterface.AddInputRequest( &m_ClippingEnabled, value );
}

inline bool Custom2Filter::isClippingEnabled() const
{ 
	bool value = 0;
	m_ClippingEnabled.GetValue(value);
	return value;
}

inline HRESULT Custom2Filter::addAbsoluteValueRequest(bool value)
{
	return m_taskObjectValueInterface.AddInputRequest( &m_AbsoluteValue, value );
}

inline bool Custom2Filter::isAbsoluteValue() const
{ 
	bool value = 0;
	m_AbsoluteValue.GetValue(value);
	return value;
}

inline HRESULT Custom2Filter::addNormalizationFactorRequest(long value)
{
	return m_taskObjectValueInterface.AddInputRequest( &m_NormalizationFactor, value );
}

inline long Custom2Filter::getNormalizationFactor() const
{ 
	long value = 0;
	m_NormalizationFactor.GetValue(value);
	return value;
}

inline HRESULT Custom2Filter::addKernelValueRequest(LongArray value)
{
	return m_taskObjectValueInterface.AddInputRequest( &m_KernelArray, value.begin(), value.end() );
}

inline Custom2Filter::LongArray Custom2Filter::getKernelValues() const
{ 
	LongArray value;
	m_KernelArray.GetValues(value);
	return value;
}

#pragma endregion virtual method (ICustom2Filter)
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