//******************************************************************************
/// \copyright COPYRIGHT (c) 2015 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved
/// \Author: Steve Jones
//******************************************************************************

#pragma once

namespace SvMc
{
	class SVSliderCtrl : public CSliderCtrl
	{
#pragma region Public
	public:
		SVSliderCtrl();
		virtual ~SVSliderCtrl();
		void SetInvertVerticalArrowKeys(bool bInvert);

		bool m_isClicked;
#pragma endregion Public
#pragma region Protected
	protected:
		//{{AFX_MSG(SVSliderCtrl)
		//}}AFX_MSG

		DECLARE_MESSAGE_MAP()

		//{{AFX_VIRTUAL(SVSliderCtrl)
		virtual BOOL PreTranslateMessage(MSG* pMsg) override;
		//}}AFX_VIRTUAL
#pragma endregion Protected
#pragma region Private
	private:
		bool m_bInvertVerticalArrowKeys;
#pragma endregion Private
	};
} //namespace SvMc
