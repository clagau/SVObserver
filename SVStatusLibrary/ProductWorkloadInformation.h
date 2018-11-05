#pragma once
//*****************************************************************************
/// \copyright COPYRIGHT (c) 2016, 2016 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// Contains the struct ProductWorkloadInformation for information on the workload 
/// associated with a particular product
//*****************************************************************************


/// collects information on the workload associated with a particular product
struct ProductWorkloadInformation
{
public:
	double m_TriggerTime {0.0};			///< time stamp at which the camera was triggered
	double m_ProcessingStartTime {0.0};	///< time stamp at which the actual processing started
	double m_CompletionTime {0.0};		///< time stamp at which the inspection of the most recently completed product was finished
	double m_AcquisitionStartTime {0.0};	///< time stamp at which the image acquisition is started (first packet received) 
	double m_AcquisitionTime {0.0};		///< time required for the complete image acquisition
	long   m_PPQIndexAtCompletion {0};	///< Position in the PPQ at which the product was when it was completed

	void Reset() ///< resets all variables. also used by the constructor
	{
		m_TriggerTime = 0.0;
		m_ProcessingStartTime = 0.0;
		m_CompletionTime = 0.0;
		m_AcquisitionStartTime = 0.0;
		m_AcquisitionTime = 0.0;
		m_PPQIndexAtCompletion = 0;
	}

	/// returns the elapsed time (in microseconds) between trigger and completion of the product
	double TriggerToCompletionInMilliseconds() const { return m_CompletionTime - m_TriggerTime; }
	/// returns the elapsed time (in microseconds) between trigger and the start of processing
	double TriggerToStartInMilliseconds() const { return m_ProcessingStartTime - m_TriggerTime; }
	/// returns the elapsed time (in microseconds) between trigger and the acquisition start
	double TriggerToAcquisitionStartInMilliseconds() const { return m_AcquisitionStartTime - m_TriggerTime; }

	ProductWorkloadInformation() { Reset(); }

#ifdef _DEBUG_PERFORMANCE_INFO //Arvid 160212 this is helpful for debugging the creation of Performance Information

	void logWorkloadInformation(const ProductWorkloadInformation &pwi, LPCSTR heading);

#endif
};

