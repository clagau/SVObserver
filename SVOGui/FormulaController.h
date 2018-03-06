//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader, Markt Schwaben
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : FormulaController
//* .File Name       : $Workfile:   FormulaController.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   17 Jul 2014 17:39:42  $
//******************************************************************************
//Description:  FormulaController is the class to get/set and validate equation objects from the GUI,
//               IFormulaController is the interface.

#pragma once

#pragma region Includes
#include "ObjectInterfaces/IFormulaController.h"
#include "DataController.h"
#include "ValuesAccessor.h"
#include "BoundValue.h"
#pragma endregion

namespace SvOg
{
	class FormulaController : public SvOi::IFormulaController
	{
	#pragma region Constructor
	public:
		FormulaController(const SVGUID& rInspectionID, const SVGUID& rTaskObjectId, const SVGUID& rEquationObjectId, bool isConditional=false);
	#pragma endregion Constructor

	#pragma region Destructor
		virtual ~FormulaController();
	#pragma endregion Destructor

	#pragma region Public Methods
	public:
	#pragma region Virtual Methods (IFormulaController)
		virtual std::string GetInspectionName() const override;
		virtual std::string GetPPQName() const override;
		virtual std::string GetOwnerName() const override;
		virtual std::string GetEquationText() const override;
		virtual std::string GetEquationName() const override;
		virtual HRESULT SetEquationName(const std::string& rNewName) override;
		virtual GUID GetTaskId() const override { return m_TaskObjectID; };

		virtual void BuildSelectableItems() override;

		virtual HRESULT IsOwnerAndEquationEnabled(bool& ownerEnabled, bool& equationEnabled) const override;
		virtual HRESULT SetOwnerAndEquationEnabled(bool ownerEnabled, bool equationEnabled) override;

		//**********
		/// Validate an equationstring
		/// \param equationString [in] the equation string.
		/// \param result [out] return the result of the equation, if the validation is successfully.
		/// \param bSetValue[in] boolean for determining whether to set (true) the new value or restore (false) to the previous equation string. if the string is invalid the previous equation string is restored in every case 
		/// \return return the position of the failure. If the validation is successful, the value will be "validateSuccessful". If the reset of the object failed the value will be "resetFailed", but the string will be set (if bSetValue == true).
		//**********
		virtual int ValidateEquation(const std::string &equationString, double& result, bool bSetValue, SvStl::MessageContainerVector& rErrorMessages) const override;
		virtual HRESULT SetDefaultInputs() override;
	#pragma endregion Virtual Methods (IFormulaController)
	#pragma endregion Public Methods

	#pragma region Member Variables
	private:
		const SVGUID m_InspectionID; // Instance ID of the Inspection
		const SVGUID m_TaskObjectID; // Instance ID of the Owner (Toolset or Tool or other TaskObject)
		const SVGUID m_EquationID; // Instance ID of the Equation
		const bool m_isConditional;
		SVGUID m_EnableID;

		typedef SvOg::ValuesAccessor<SvOg::BoundValues> FormulaCommand;
		typedef SvOg::DataController<FormulaCommand, FormulaCommand::value_type> Controller;
		Controller m_Values;
		Controller m_EquationValues;
#pragma endregion Member Variables

	};
} //namespace SvOg

