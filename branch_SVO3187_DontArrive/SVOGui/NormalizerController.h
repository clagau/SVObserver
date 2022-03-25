//*****************************************************************************
// \copyright COPYRIGHT (c) 2015,2015 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file NormalizerController.h
/// Defines the NormalizerController class.
/// This class exchanges information between SVObserver and GUI, via Protobuf messages, fulfilling the following tasks:
/// - fetch values of a Normalizer object (SVValueBaseNormalizer) owned by a SVLinearAnalyzer.
/// - set the value ranges for the Normalizer object, needed to perform some computation
//******************************************************************************

#pragma once

namespace SvOg
{
//*****************************************************************************
/// \class NormalizerController
/// This class exchanges information between SVObserver and GUI, via Protobuf messages, fulfilling the following tasks:
/// - fetch values of a Normalizer object (SVValueBaseNormalizer) owned by a SVLinearAnalyzer.
/// - set the value ranges for the Normalizer object, needed to perform some computation
/// The values are stored locally and computations can be done based on these values, as long as the ranges of the Normalizer object 
/// do not change. In this way, message round-trip is avoided.
/// Note: Currently, it is not possible to call the Normalizer for performing the computation! 
/// If the logic for calcRealValue and calcNormalizedValue will be changed, this class have to be updated as well: 
/// - either duplicate the new logic, or implenent functions to call the corresponding functions.
//******************************************************************************
	class NormalizerController
	{
	public:
		struct NormalizerValues
		{
			double normalMax = 0.0;
			double normalMin = 0.0;
			double normalStep = 0.0;
			double normalBaseSize = 0.0;
			double rangeMax = 0.0;
			double rangeMin = 0.0;
			double rangeStep = 0.0;
			double rangeBaseSize = 0.0;

			NormalizerValues() = default;

			NormalizerValues(const NormalizerValues& rhs)
				: normalMax(rhs.normalMax), normalMin(rhs.normalMin), normalStep(rhs.normalStep), normalBaseSize(rhs.normalBaseSize),
				rangeMax(rhs.rangeMax), rangeMin(rhs.rangeMin), rangeStep(rhs.rangeStep), rangeBaseSize(rhs.rangeBaseSize)
			{}

			NormalizerValues& operator=(const NormalizerValues& rhs)
			{
				normalMax = rhs.normalMax;
				normalMin = rhs.normalMin; 
				normalStep = rhs.normalStep; 
				normalBaseSize = rhs.normalBaseSize;
				rangeMax = rhs.rangeMax; 
				rangeMin = rhs.rangeMin; 
				rangeStep = rhs.rangeStep; 
				rangeBaseSize = rhs.rangeBaseSize;
				return *this;
			}
		};
		
	public:
		//************************************
		// Method:    Default constructor, deleted
		// Description: Default constructor not needed, thus deleted.
		//************************************
		NormalizerController() = delete;
		NormalizerController(const NormalizerController&) = delete;
		NormalizerController& operator=(const NormalizerController&) = delete;
		//************************************
		// Method:    Constructor, overload with 2 params
		// Description: construct a NormalizerController based on an inspection and an analyzer. 
		// These parameters are used identify and query the object Analyzer. 
		// Parameter: p_InspectionId - Id of the inspection
		// Parameter: p_AnalyzerId - Id of the Analyzer tool holding the Normalizer to be used here.
		// Returns:   
		//************************************
		explicit NormalizerController(uint32_t p_InspectionId, uint32_t p_AnalyzerId);
		~NormalizerController() = default;

		//************************************
		// Method: getValues()
		// Description: fetches the values of the Normalizer using a protobuf message. 
		// Parameter: p_NormalizerValues [in/out] - contains the values of the Normalizer
		// Returns:  bool true if the call succeeded, false otherwise (e.g. InspectionCommand call fails).
		//************************************
		SvOg::NormalizerController::NormalizerValues getValues();
		//************************************
		// Method: setRanges()
		// Description: sends the range values to the Normalizer using a protobuf message and returns the new values on success.
		// Parameter: p_NormalizerValues [in/out] - contains the values of the Normalizer
		// Returns:  bool true if the call succeeded, false otherwise (e.g. InspectionCommand call fails).
		//************************************
		SvOg::NormalizerController::NormalizerValues setRanges(double realMin, double realMax, double realStep, double normalMin, double normalMax, double normalStep);
		//************************************
		// Method: calcRealValueFromLocalValues
		// Description: computes the RealValue based on local values and the XN parameter. 
		// Implementation is duplicate of SVValueBaseNormalizerClass::calcRealValue()
		// Parameter: XN [in] -  
		// Returns:  double - computed value
		//************************************
		double calcRealValueFromLocalValues(double XN);
		//************************************
		// Method: calcNormalizedValueFromLocalValues
		// Description: computes the Normalized value based on local values and the X parameter. 
		// Implementation is duplicate of SVValueBaseNormalizerClass::calcNormalizedValue()
		// Parameter: X [in] -  
		// Returns:  double - computed value
		//************************************
		double calcNormalizedValueFromLocalValues(double X);

	private:
		uint32_t m_inspectionId;
		uint32_t m_analyzerId;
		NormalizerValues m_normalizerValues;
	};
}

