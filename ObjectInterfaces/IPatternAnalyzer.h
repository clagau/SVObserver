//******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// Interface class for pattern analyzer class
//******************************************************************************

#pragma once
#pragma region Includes
#pragma endregion Includes

namespace Seidenader
{
	namespace ObjectInterfaces
	{	
		class IPatternAnalyzer
		{
		public:
			virtual ~IPatternAnalyzer() {}

			/// Create a model from input image and the it as current model.
			/// \param posX [in] X-position in the input image.
			/// \param posY [in] Y-position in the input image.
			/// \param modelWidth [in] Requested width of the new model.
			/// \param modelHeight [in] Requested height of the new model.
			/// \returns bool True if update was successfully.
			virtual bool UpdateModelFromInputImage(long posX, long posY, long modelWidth, long modelHeight) = 0;
		};
	}
}

namespace SvOi = Seidenader::ObjectInterfaces;