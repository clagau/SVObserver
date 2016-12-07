//*****************************************************************************
/// \copyright (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
/// \Author	Marc Ziegler
//*****************************************************************************
/// Property page for the tool adjustment sheet for parameter in the RingBuffer-tool.
//*****************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <boost\function.hpp>
#include "SVOResource/resource.h"
#include "ObjectInterfaces/GlobalConst.h"
#include "ISVPropertyPageDialog.h"
#include "PictureDisplay.h"
#include "ValuesAccessor.h"
#include "GuiController.h"
#include "ObjectSelectorController.h"
#pragma endregion Includes

namespace Seidenader { namespace SVOGui
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
		TADialogRingBufferParameterPage( const GUID& rInspectionID, const GUID& rTaskObjectID );

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
		HRESULT SetImageIndex(int indexNumber);
#pragma endregion Private Methods

#pragma region Member Variables
	private:
		GUID m_InspectionID;
		GUID m_TaskObjectID;

		CEdit m_EditRingDepth;
		CEdit m_EditImageIndex[SvOi::cRingBufferNumberOutputImages];
		CButton m_ButtonImageIndex1;
		CButton m_ButtonImageIndex2;
		CBitmap m_downArrowBitmap;
		typedef SvOg::ValuesAccessor<SvOg::BoundValues> ValueCommand;
		typedef SvOg::GuiController<ValueCommand, ValueCommand::value_type> Controller;
		Controller m_Values;
		ObjectSelectorController m_objectSelector;
#pragma endregion Member Variables
	};
} /* namespace SVOGui */ } /* namespace Seidenader */

namespace SvOg = Seidenader::SVOGui;
