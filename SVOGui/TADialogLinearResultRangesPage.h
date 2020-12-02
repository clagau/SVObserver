//*****************************************************************************
/// \copyright (c) 2015 by Seidenader Maschinenbau GmbH
/// \file TADialogLinearResultRangesPage.h
/// All Rights Reserved 
//*****************************************************************************
/// Property page for the tool adjustment sheet for setting Result Ranges for Linear Tool.
//*****************************************************************************


#pragma once

#pragma region Includes
#include "SVOResource/resource.h"
#include "SVProtoBuf/SVO-Enum.h"
#include "DataController.h"
#pragma endregion Includes


namespace SvOg
{
	
	//**********
	// TADialogLinearResultRangesPage
	/// The dialog provides an interface for seeting ranges for Min/Max/Delta Profile Values in the Linear Tool.
	/// Since these values are at "tool" level, this is integrated as a new page in the Adjustment Dialog for this tool.
	//**********
	class TADialogLinearResultRangesPage : public CPropertyPage 
	{
	public:
		TADialogLinearResultRangesPage(uint32_t inspectionId, uint32_t taskObjectId);
		virtual ~TADialogLinearResultRangesPage();

	protected:
		//{{AFX_DATA(TADialogLinearResultRangesPage)
		enum { IDD = IDD_TA_LINEAR_TOOL_RESULT_RANGES };
		//}}AFX_DATA

		DECLARE_MESSAGE_MAP()

		//{{AFX_MSG(TADialogLinearResultRangesPage)
		virtual BOOL OnInitDialog();
		afx_msg void OnBnClickedBtnRangeMin();
		afx_msg void OnBnClickedBtnRangeMax();
		afx_msg void OnBnClickedBtnRangeDelta();
		//}}AFX_MSG

	private:
		/// Calls the Set Ranges dialog for the specified value. 
		/// The function searches the Result object corresponding to the value object identified by its EmbeddedId provided as argument and pass it to the SetupDialog().
		/// 
		/// \param embeddedId [in] the EmbeddedIdEnum value identifying the value object.
		/// \return returns true if the ranges dialog was successfully open and no other error occured, false otherwise. 
		/// Note: If the user press Cancel in the dialog, this function returns also true!
		bool setRanges(SvPb::EmbeddedIdEnum embeddedId);
		uint32_t getOperatorListObjectId();
		std::vector<SvPb::TaskObjectInfo> getResults(uint32_t operatorListObjectId);
		std::vector<SvPb::InputNameObjectNameIdPair> getMatchedInputsObjectsForResult(uint32_t resultId, const std::map<uint32_t, SvPb::EmbeddedIdEnum>& valueObjects);
	private:
		const uint32_t m_InspectionID;
		const uint32_t m_TaskObjectID;
		std::unique_ptr<SvOg::ValueController> m_pValueController;
		std::unordered_map< SvPb::EmbeddedIdEnum, uint32_t> m_ValueEmbeddedIdResultMap;
	};
}
