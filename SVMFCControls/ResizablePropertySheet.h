//*****************************************************************************
/// \copyright COPYRIGHT (c) 2023, 2023 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved
/// \file ResizablePropertySheet.h
/// \brief Make CPropertySheet resizable using dynamic layout
//******************************************************************************
#pragma once

namespace SvMc
{
class ResizablePropertySheet : public CPropertySheet
{
public:
#pragma region Constructor
	ResizablePropertySheet();
	explicit ResizablePropertySheet(UINT nIDCaption, CWnd *pParentWnd = nullptr, UINT iSelectPage = 0);
	explicit ResizablePropertySheet(LPCTSTR pszCaption, CWnd *pParentWnd = nullptr, UINT iSelectPage = 0);
	virtual ~ResizablePropertySheet();
#pragma endregion Constructor

public:
#pragma region Public Methods
	virtual BOOL OnInitDialog() override;
#pragma endregion Public Methods

protected:
#pragma region Protected Methods
	void SetupDynamicLayout();
	DECLARE_MESSAGE_MAP()
#pragma endregion Protected Methods

private:
#pragma region Member Variables
	CScrollBar m_resizeGripper;
#pragma endregion Member Variables
};
} //namespace SvMc
