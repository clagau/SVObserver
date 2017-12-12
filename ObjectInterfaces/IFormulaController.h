//******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// IFormulaController is the interface to get/set and validate equations 
/// from the GUI.
//******************************************************************************
#pragma once

#pragma region Includes
#pragma endregion Includes

#pragma region Declarations
//! Declaration is in SVStatusLibrary\MessageContainer.h
namespace SvStl
{
	class MessageContainer;
	typedef std::vector<MessageContainer> MessageContainerVector;
}
#pragma endregion Declarations

namespace SvOi
{
	class IFormulaController
	{
#pragma region Public Methods
	public:
		virtual ~IFormulaController() {}

		/**********
		Get the current used equation text.
		\return std::string equation text.
		**********/
		virtual std::string GetEquationText() const = 0;

		virtual std::string GetInspectionName() const = 0;
		virtual std::string GetPPQName() const = 0;
		virtual std::string GetOwnerName() const = 0;

		//************************************
		//! Sets the Selectable items for the ObjectSelector.
		//************************************
		virtual void BuildSelectableItems() = 0;

		/**********
		Return the state of the enable flags for owner and equation.
		\param ownerEnabled [out] flag if owner(toolset/tool) is enabled.
		\param equationEnabled [out] flag if equation is enabled.
		\return HRESULT return S_OK if values a set.
		**********/
		virtual HRESULT IsOwnerAndEquationEnabled(bool& ownerEnabled, bool& equationEnabled) const = 0;

		/**********
		Set the state of the enable flags for owner and equation.
		\param ownerEnabled [in] flag if owner (toolset/tool) is enabled.
		\param equationEnabled [in] flag if equation is enabled.
		\return HRESULT return S_OK if values a set.
		**********/
		virtual HRESULT SetOwnerAndEquationEnabled(bool ownerEnabled, bool equationEnabled) = 0;

		/// Validate a string and set the value if bSetValue is true.
		/// \param equationString [in] the equation string.
		/// \param result [out] return the result of the equation, if the validation is successfully.
		/// \param bSetValue [in] boolean if true, set value and reset object, else the old value will restored.
		/// \param rErrorMessages [out] Return a list of ErrorMessages.
		/// \returns int return the position of the failure. If the validation is successful, the value will be "validateSuccessful".
		virtual int ValidateEquation(const std::string &equationString, double& result, bool bSetValue, SvStl::MessageContainerVector& rErrorMessages) const = 0;

		/**********
		/// Set the Default Inputs
		**********/
		virtual HRESULT SetDefaultInputs() = 0;
#pragma endregion Public Methods

#pragma region Constants
	public:
		// Constant for the validation is successful
		static const int validateSuccessful = -1;
		// Constant for the validation is successful, but the reset of object is not
		static const int resetFailed = -2;
#pragma endregion Constants
	};
	typedef std::shared_ptr<IFormulaController> IFormulaControllerPtr;

} //namespace SvOi
