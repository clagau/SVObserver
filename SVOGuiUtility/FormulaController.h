//******************************************************************************
//* COPYRIGHT (c) 2013 by K�rber Pharma Inspection GmbH. All Rights Reserved
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
#include "Definitions/SVObjectTypeInfoStruct.h"
#include "DataController.h"
#include "BoundValue.h"
#pragma endregion

namespace SvOgu
{
class FormulaController : public SvOi::IFormulaController
{
#pragma region Constructor
public:
	FormulaController(uint32_t inspectionID, uint32_t taskObjectID, uint32_t equationID);
	FormulaController(uint32_t inspectionID, uint32_t taskObjectID, const SvDef::SVObjectTypeInfoStruct& rInfo);
#pragma endregion Constructor

#pragma region Destructor
	virtual ~FormulaController();
#pragma endregion Destructor

#pragma region Public Methods
public:
#pragma region Virtual Methods (IFormulaController)
	virtual std::string GetInspectionName() const override;
	virtual std::string GetEquationText() const override;
	virtual std::string GetEquationName() const override;
	virtual HRESULT SetEquationName(const std::string& rNewName) override;
	virtual uint32_t GetTaskId() const override { return m_TaskObjectID; };

	virtual void BuildSelectableItems() override;

	virtual HRESULT IsOwnerAndEquationEnabled(bool& ownerEnabled, bool& equationEnabled) const override;
	virtual void SetOwnerAndEquationEnabled(bool ownerEnabled, bool equationEnabled) override;
	virtual bool getEditModeFreezeFlag() const override;
	virtual void setEditModeFreezeFlag(bool flag) override;

	//**********
	/// Validate an equationstring
	/// \param equationString [in] the equation string.
	/// \param result [out] return the result of the equation, if the validation is successfully.
	/// \param bSetValue[in] boolean for determining whether to set (true) the new value or restore (false) to the previous equation string. if the string is invalid the previous equation string is restored in every case 
	/// \return return the position of the failure. If the validation is successful, the value will be "validateSuccessful". If the reset of the object failed the value will be "resetFailed", but the string will be set (if bSetValue == true).
	//**********
	virtual int ValidateEquation(const std::string &equationString, double& result, bool bSetValue, SvStl::MessageContainerVector& rErrorMessages) const override;
#pragma endregion Virtual Methods (IFormulaController)	
	int  ValidateEquation_NoReset(const std::string& equationString, double& result, bool bSetValue, SvStl::MessageContainerVector& rErrorMessages) const;
	bool  ResetOwner()const; 
#pragma endregion Public Methods

#pragma region Private Methods
private:
	void Init();
#pragma endregion Private Methods

#pragma region Member Variables
private:
	const uint32_t m_InspectionID; // Instance ID of the Inspection
	const uint32_t m_TaskObjectID; // Instance ID of the Owner (Toolset or Tool or other TaskObject)
	const SvDef::SVObjectTypeInfoStruct m_Info;

	bool m_isConditional;		//Note this variable must be above the variables m_values and m_EquationValues due to initialization
	uint32_t m_EquationID;		// Instance ID of the Equation
	SvPb::EmbeddedIdEnum m_EnableID;

	ValueController m_values;
	ValueController m_EquationValues;
#pragma endregion Member Variables

};
} //namespace SvOgu

