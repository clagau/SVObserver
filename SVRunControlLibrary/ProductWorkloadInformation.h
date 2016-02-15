#pragma once
//*****************************************************************************
/// \copyright COPYRIGHT (c) 2016, 2016 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
/// \Author	Arvid: Breitenbach
//*****************************************************************************
/// Contains the struct ProductWorkloadInformation for information on the workload 
/// associated with a particular product
//*****************************************************************************


const double c_MicrosecondsPerMilisecond=1000.0; ///< the number of seconds per minute //@TODO ideally, this should not be member of this class (the number of seconds in a minute has nothing to do with the autosave functionality) but of some module with globals constants

/// collects information on the workload associated with a particular product
struct ProductWorkloadInformation
{
public: 
	double m_TriggerTime;			///< time stamp at which the camera was triggered
	double m_ProcessingStartTime;	///< time stamp at which the actual processing started
	double m_CompletionTime;		///< time stamp at which the inspection of the most recently completed product was finished
	long   m_PPQIndexAtCompletion;	///< Position in the PPQ at which the product was when it was completed

	void Reset() ///< resets all variables. also used by the constructor
	{
		m_PPQIndexAtCompletion = 0;
		m_CompletionTime = 0.0;
		m_TriggerTime = 0.0;
		m_ProcessingStartTime = 0.0;
	}
	
/// returns the elapsed time (in microseconds) between trigger and completion of the product
	double TriggerToCompletionInMicroseconds() const {return 1000.0 * (m_CompletionTime - m_TriggerTime);}
/// returns the elapsed time (in microseconds) between trigger and the start of processing
	double TriggerToStartInMicroseconds() const {return 1000.0 * (m_ProcessingStartTime - m_TriggerTime);}

	ProductWorkloadInformation(){Reset();}

#ifdef _DEBUG_PERFORMANCE_INFO //Arvid 161212 this is helpful for debugging the creation of Performance Information

	void logWorkloadInformation(const ProductWorkloadInformation &pwi, LPCSTR heading);

#endif
};

