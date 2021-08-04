//*****************************************************************************
/// \copyright (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
/// \Author	Marc Ziegler
//*****************************************************************************
/// Property page for the tool adjustment sheet for parameter in the RingBuffer-tool.
//*****************************************************************************

#pragma once

#pragma region Includes
#include "SVOResource/resource.h"
#include "Definitions/GlobalConst.h"
#include "ISVPropertyPageDialog.h"
#include "DataController.h"
#include "LinkedValueWidgetHelper.h"
#pragma endregion Includes

namespace SvOg
{
	class TADialogRingBufferParameterPage : public CPropertyPage, public ISVPropertyPageDialog
	{
#pragma region Declarations
	protected:
		//{{AFX_DATA(SVToolAdjustmentDialogFilterPageClass)
		enum { IDD = IDD_TA_RING_PARAMETER_DIALOG };
		//}}AFX_DATA
#pragma endregion Declarations

#pragma region Constructor
	public:
		// Standard constructor
		TADialogRingBufferParameterPage( uint32_t inspectionID, uint32_t taskID );

		// Standard destructor
		virtual ~TADialogRingBufferParameterPage();
#pragma endregion Constructor

#pragma region Public Methods
	public:
		virtual bool QueryAllowExit() override; //from ISVPropertyPageDialog
#pragma endregion Public Methods

#pragma region Protected Methods
		DECLARE_MESSAGE_MAP()

		//{{AFX_VIRTUAL(SVToolAdjustmentDialogFilterPageClass)
	protected:
		virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV-Unterstützung
		//}}AFX_VIRTUAL
	protected:
		//{{AFX_MSG(SVToolAdjustmentDialogFilterPageClass)
		virtual BOOL OnInitDialog() override;
		virtual BOOL OnKillActive( ) override;
		//afx_msg void OnButtonBufferDepth();
		afx_msg void OnButtonImageIndex( UINT nID );
		afx_msg void OnKillFocusImageIndex(UINT nID);
		afx_msg void OnButtonDepth();
		afx_msg void OnKillFocusDepth();
		//}}AFX_MSG

		//************************************
		/// Create a object selector for a image index value and save the result to this field.
		/// \param index [in] The number of the image index.
		/// \param titleStringID [in] string ID for the object selector title.
		//************************************
		void ObjectSelectorForIndex(int index, int titleStringID);
#pragma endregion Protected Methods

#pragma region Private Methods
	private:
		HRESULT SetPageData();
		HRESULT SetRingDepth();
#pragma endregion Private Methods

#pragma region Member Variables
	private:
		uint32_t m_InspectionID;
		uint32_t m_TaskObjectID;

		CEdit m_EditRingDepth;
		CButton m_ButtonRingDepth;
		std::unique_ptr<LinkedValueWidgetHelper> m_DepthWidget;
		CEdit m_EditImageIndex[SvDef::cRingBufferNumberOutputImages];
		CButton m_ButtonImageIndex[SvDef::cRingBufferNumberOutputImages];
		std::array<std::unique_ptr<LinkedValueWidgetHelper>, SvDef::cRingBufferNumberOutputImages> m_ImageIndexWidget;
		ValueController m_values;
#pragma endregion Member Variables
	};
} //namespace SvOg
