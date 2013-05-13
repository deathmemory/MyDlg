
#include "StdAfx.h"
#include "UITab.h"


/////////////////////////////////////////////////////////////////////////////////////
//
//

CUIPageSheet::CUIPageSheet() : m_iCurSel(-1), m_pCurPage(NULL), m_aTabAreas(sizeof(RECT))
{
   m_chShortcut = VK_NEXT;
   m_pTabImageOwner=NULL;
   m_pBoxImageOwner=NULL;
   m_colorBackGround = RGB(255, 255, 255);
   m_colorOutside = RGB(148,166,181);
   m_colorInside = INVALID__UICOLOR;
   m_uTabLeftOffset=0;
   m_crTabTextNormal=RGB(0,0,0);
   m_crTabTextFocused=RGB(0,0,20);
   m_crTabTextPushed=RGB(0,0,20);
   m_crTabTextDisabled=RGB(128,128,128);
   m_uTabOverBoxPixels=2;
}

CUIPageSheet::~CUIPageSheet()
{
	if (m_pTabImageOwner)
	{
		delete m_pTabImageOwner;
		m_pTabImageOwner = NULL;
	}

	if (m_pBoxImageOwner)
	{
		delete m_pBoxImageOwner;
		m_pBoxImageOwner = NULL;
	}
}

LPCTSTR CUIPageSheet::GetClass() const
{
   return _T("TabFolderUI");
}

void CUIPageSheet::Init()
{
   if( m_iCurSel == -1 ) SetCurSel(0);
}

bool CUIPageSheet::Add(CUIControl* pControl)
{
   pControl->SetVisible(false);
   return CUIContainer::Add(pControl);
}

int CUIPageSheet::GetCurSel() const
{
   return m_iCurSel;
}

bool CUIPageSheet::SetCurSel(int iIndex)
{
   int iPrevSel = m_iCurSel;
   if( iIndex < 0 || iIndex >= m_items.GetSize() ) return false;
   if( m_pManager != NULL ) m_pManager->SendNotify(this, UI_NOTIFY_CLICK);
   if( iIndex == m_iCurSel ) return true;
   // Assign page to internal pointers
   if( m_pCurPage != NULL ) m_pCurPage->SetVisible(false);
   m_iCurSel = iIndex;
   m_pCurPage = static_cast<CUIControl*>(m_items[iIndex]);
   if( m_pManager != NULL ) m_pManager->SendNotify(this, UI_NOTIFY_ITEM_SELECT_CHANGE);
   m_pCurPage->SetVisible(true);
   // Need to re-think the layout
   if( m_pManager != NULL ) m_pManager->UpdateLayout();
   // Set focus on page
   m_pCurPage->SetFocus();
   if( m_pManager != NULL ) m_pManager->SetNextTabControl();
   return true;
}

void CUIPageSheet::Event(TEventUI& event)
{
   if( event.Type == UIEVENT_BUTTONDOWN && IsEnabled() )
   {
      for( int i = 0; i < m_items.GetSize() && i < m_aTabAreas.GetSize(); i++ ) {
         if( ::PtInRect( static_cast<LPRECT>(m_aTabAreas[i]), event.ptMouse) ) {
            SetCurSel(i);
            return;
         }
      }
   }
   if( event.Type == UIEVENT_SYSKEY && IsEnabled() )
   {
      if( event.chKey == VK_PRIOR && (event.wKeyState & MK_ALT) != 0 ) {
         SetCurSel(m_iCurSel - 1);
         return;
      }
      if( event.chKey == VK_NEXT && (event.wKeyState & MK_ALT) != 0 ) {
         SetCurSel(m_iCurSel + 1);
         return;
      }
   }
}

void CUIPageSheet::SetPos(RECT rc)
{
   CUIControl::SetPos(rc);
   //文字的高度
   int cyFont = m_pManager->GetThemeFontInfo(UIFONT_BOLD).tmHeight;
   int liTabHeight=cyFont + 20;
   //Box区域去掉缩进，去掉Tab高度
   ::SetRect(&m_rcBox, rc.left + m_rcInset.left, rc.top + m_rcInset.top + liTabHeight, rc.right - m_rcInset.right, rc.bottom - m_rcInset.bottom);
   //Page区域
   m_rcPage = m_rcBox;
   ::InflateRect(&m_rcPage, -8, -8);
   if( m_pCurPage != NULL ) m_pCurPage->SetPos(m_rcPage);

   if(m_pTabImageOwner != NULL)
   {
	   ResetSubRects();
   }
}

void CUIPageSheet::DoPaint(HDC hDC, const RECT& rcPaint)
{
	{
		CRenderClip clip;
		CUIBlueRenderEngine::GenerateClip(hDC, rcPaint, clip);

		// Fill client area background
		RECT rcFill = { 0 };
		if(m_pBoxImageOwner != NULL)
		{
			RECT rcImage = {0};
			rcImage.right=m_pBoxImageOwner->GetWidth();
			rcImage.bottom=m_pBoxImageOwner->GetHeight();
			CUIBlueRenderEngine::DoFillRect(hDC, m_pManager, m_rcBox, rcImage, m_pBoxImageOwner, false);
		}
		else
		{
			CUIBlueRenderEngine::DoFillRect(hDC, m_pManager, m_rcBox, m_colorBackGround);

			// Frame around client area
			CUIBlueRenderEngine::DoPaintBorder(hDC, m_pManager, m_rcBox, m_colorOutside);

			if (m_colorInside != INVALID__UICOLOR)
			{
				RECT rectTemp = m_rcBox;
				rectTemp.left += 1;
				rectTemp.top += 1;
				rectTemp.right -= 1;
				rectTemp.bottom -= 1;

				CUIBlueRenderEngine::DoPaintBorder(hDC, m_pManager, rectTemp, m_colorInside);
			}
		}

		// Paint tab strip
		RECT rcTabs = m_rcItem;
		rcTabs.left += m_rcInset.left;
		rcTabs.top += m_rcInset.top;
		rcTabs.right -= m_rcInset.right;
		rcTabs.bottom = m_rcBox.top;
		RECT rcTemp = { 0 };
		if( ::IntersectRect(&rcTemp, &rcPaint, &rcTabs) ) 
		{
			int iPosX = m_uTabLeftOffset;
			m_aTabAreas.Empty();
			for( int i = 0; i < GetCount(); i++ ) 
			{
				const CUIControl* pPage = GetItem(i);
				const CStdString& strText = pPage->GetText();
				RECT rcTab = { rcTabs.left + iPosX, rcTabs.top, rcTabs.right, m_rcBox.top };
				UINT uState = 0;
				if( IsFocused() ) uState |= UISTATE_FOCUSED;
				if( !IsEnabled() ) uState |= UISTATE_DISABLED;
				if( m_iCurSel == i ) uState = UISTATE_PUSHED;
				CUIBlueRenderEngine::DoPaintTabBackground(hDC, m_pManager, rcTab, strText, uState, m_uTabOverBoxPixels, m_pTabImageOwner);
				//------------------------------------------------------------------------------------------------------
				COLORREF lcrTabText;
				if( (uState & UISTATE_DISABLED) != 0 )
				{
					lcrTabText=m_crTabTextDisabled;
				}
				else if( (uState & UISTATE_PUSHED) != 0 )
				{
					lcrTabText=m_crTabTextPushed;
				}
				else if( (uState & UISTATE_HOT) != 0 )
				{
					lcrTabText=m_crTabTextFocused;
				}
				else
				{
					lcrTabText=m_crTabTextNormal;
				}
				RECT rcText = { rcTab.left, rcTab.top + 1, rcTab.right, rcTab.bottom };
				int nLinks = 0;
				CUIBlueRenderEngine::DoPaintPrettyText(hDC, m_pManager, rcText, strText, lcrTabText, INVALID__UICOLOR, NULL, nLinks, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
				//------------------------------------------------------------------------------------------------------
				iPosX += (rcTab.right - rcTab.left) + 2;
				m_aTabAreas.Add(&rcTab);
			}
		}
	}

   if( m_pCurPage != NULL ) m_pCurPage->DoPaint(hDC, rcPaint);
}

void CUIPageSheet::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
   if( _tcscmp(pstrName, _T("select")) == 0 ) SetCurSel(_ttoi(pstrValue));
   else CUIContainer::SetAttribute(pstrName, pstrValue);
}

bool CUIPageSheet::SetHotItem(int iIndex)
{
	return false;
}


CUIPageOption* CUIPageSheet::AddPageOption(UINT uPageIdx, PCTSTR pstrText)
{
	CUIPageOption *pPage = new CUIPageOption;
	if (pPage == NULL)
	{
		return NULL;
	}

	TCHAR buffer[10];

	UINT uId = GetId() * 10 + uPageIdx;

	_itot(uId, buffer, 10);

	pPage->SetName(buffer);

	pPage->SetText(pstrText);
	pPage->SetId(uId);

	pPage->SetPos(m_rcPage);

	bool bRet =  Add(pPage);

	if (bRet)
	{
		return pPage;
	}
	else
	{
		return NULL;
	}
}

void CUIPageSheet::ResetSubRects()
{
	if(m_pTabImageOwner != NULL)
	{
		int liTabHeight=m_pTabImageOwner->GetHeight()-m_uTabOverBoxPixels;
		//Box区域去掉缩进，去掉Tab高度
		::SetRect(&m_rcBox, m_rcItem.left + m_rcInset.left, m_rcItem.top + m_rcInset.top + liTabHeight, m_rcItem.right - m_rcInset.right, m_rcItem.bottom - m_rcInset.bottom);
		//Page区域
		m_rcPage = m_rcBox;
		::InflateRect(&m_rcPage, -8, -8);
		if( m_pCurPage != NULL ) m_pCurPage->SetPos(m_rcPage);
	}
}
void CUIPageSheet::SetTabImage(Image*	pImage)
{
	if (m_pTabImageOwner)
	{
		delete m_pTabImageOwner;
		m_pTabImageOwner = NULL;
	}

	if (pImage)
	{
		m_pTabImageOwner = pImage->Clone();
	}
	ResetSubRects();
	Invalidate();
}

void CUIPageSheet::SetTabImage( UINT nResourceID, HMODULE hResHandle/*=NULL*/ )
{
	if (m_pTabImageOwner)
	{
		delete m_pTabImageOwner;
		m_pTabImageOwner = NULL;
	}

	CUIPaintManager::SetImage(&m_pTabImageOwner, nResourceID, hResHandle);
	ResetSubRects();
	Invalidate();
}
void CUIPageSheet::SetTabImage(LPCTSTR pstrName)
{
	if (m_pTabImageOwner)
	{
		delete m_pTabImageOwner;
		m_pTabImageOwner = NULL;
	}

	m_pTabImageOwner = Image::FromFile(pstrName);
	ResetSubRects();
	Invalidate();
}

void CUIPageSheet::SetBorderColor(COLORREF colorOutside, COLORREF colorInside)
{
	m_colorOutside = colorOutside;
	m_colorInside = colorInside;

	Invalidate();
}

void CUIPageSheet::SetBackGroundColor(COLORREF color)
{
	m_colorBackGround = color;
}

void CUIPageSheet::SetBoxImage( Image* pImage )
{
	if (m_pBoxImageOwner)
	{
		delete m_pBoxImageOwner;
		m_pBoxImageOwner = NULL;
	}

	if (pImage)
	{
		m_pBoxImageOwner = pImage->Clone();
	}
	Invalidate();
}
void CUIPageSheet::SetBoxImage( UINT nResourceID, HMODULE hResHandle/*=NULL*/ )
{
	if (m_pBoxImageOwner)
	{
		delete m_pBoxImageOwner;
		m_pBoxImageOwner = NULL;
	}

	CUIPaintManager::SetImage(&m_pBoxImageOwner, nResourceID, hResHandle);
	Invalidate();
}

void CUIPageSheet::SetBoxImage( LPCTSTR pstrName )
{
	if (m_pBoxImageOwner)
	{
		delete m_pBoxImageOwner;
		m_pBoxImageOwner = NULL;
	}

	m_pBoxImageOwner = Image::FromFile(pstrName);
	Invalidate();
}
void CUIPageSheet::SetTabLeftOffset( UINT auOffset )
{
	m_uTabLeftOffset=auOffset;
}

void CUIPageSheet::SetTabTextColor( COLORREF crNormal/*=RGB(0,0,0)*/, COLORREF crFocused/*=RGB(0,0,20)*/, COLORREF crPushed/*=RGB(0,0,20)*/, COLORREF crDisabled/*=RGB(128,128,128)*/ )
{
	m_crTabTextNormal=crNormal;
	m_crTabTextFocused=crFocused;
	m_crTabTextPushed=crPushed;
	m_crTabTextDisabled=crDisabled;

	Invalidate();
}

void CUIPageSheet::SetTabOverBoxPixels( UINT auHPixels )
{
	m_uTabOverBoxPixels=auHPixels;
	ResetSubRects();
}
////////////////////////////////////////////////////////////////////////////////
CUIPageOption::CUIPageOption()
{

}

LPCTSTR CUIPageOption::GetClass() const
{
	return _T("CUIPageOption");
}


CUIButton* CUIPageOption::RegiserButton(UINT nId, CRect rect, LPCTSTR lpszText)
{
	CUIButton* pBtn = new CUIButton;
	if (pBtn == NULL)
	{
		return NULL;
	}

	pBtn->SetId(nId);
	pBtn->SetPos(rect);

	CString strTemp;
	strTemp.Format(_T("%d"), nId);

	pBtn->SetName(strTemp);

	if (lpszText)
	{
		pBtn->SetText(lpszText);
	}

	CUIGroup* pGroup = FindGroupByRect(rect);
	if (pGroup)
	{
		pGroup->Add(pBtn);
	}
	else
	{
		Add(pBtn);
	}

	return pBtn;
}

CUILabel* CUIPageOption::RegiserLabel(UINT nId, CRect rect, LPCTSTR lpszText)
{
	CUILabel* pLabel = new CUILabel;
	if (pLabel == NULL)
	{
		return NULL;
	}

	pLabel->SetId(nId);
	pLabel->SetPos(rect);

	CString strTemp;
	strTemp.Format(_T("%d"), nId);

	pLabel->SetName(strTemp);

	if (lpszText)
	{
		pLabel->SetText(lpszText);
	}

	CUIGroup* pGroup = FindGroupByRect(rect);
	if (pGroup)
	{
		pGroup->Add(pLabel);
	}
	else
	{
		Add(pLabel);
	}

	return pLabel;
}

CUIEdit* CUIPageOption::RegiserEdit(UINT nId, CRect rect, LPCTSTR lpszText)
{
	CUIEdit* pEdit = new CUIEdit;
	if (pEdit == NULL)
	{
		return NULL;
	}

	pEdit->SetId(nId);
	pEdit->SetPos(rect);

	CString strTemp;
	strTemp.Format(_T("%d"), nId);

	pEdit->SetName(strTemp);

	if (lpszText)
	{
		pEdit->SetText(lpszText);
	}

	CUIGroup* pGroup = FindGroupByRect(rect);
	if (pGroup)
	{
		pGroup->Add(pEdit);
	}
	else
	{
		Add(pEdit);
	}

	return pEdit;
}

CUIComboBox* CUIPageOption::RegiserComboBox(UINT nId, CRect rect, LPCTSTR lpszText)
{
	CUIComboBox* pCombo = new CUIComboBox;
	if (pCombo == NULL)
	{
		return NULL;
	}

	pCombo->SetId(nId);
	pCombo->SetPos(rect);

	CString strTemp;
	strTemp.Format(_T("%d"), nId);

	pCombo->SetName(strTemp);

	if (lpszText)
	{
		pCombo->SetText(lpszText);
	}

	CUIGroup* pGroup = FindGroupByRect(rect);
	if (pGroup)
	{
		pGroup->Add(pCombo);
	}
	else
	{
		Add(pCombo);
	}

	return pCombo;
}

CUIProgressBar* CUIPageOption::RegiserProgressBar(UINT nId, CRect rect, LPCTSTR lpszText)
{
	CUIProgressBar* pBar = new CUIProgressBar;
	if (pBar == NULL)
	{
		return NULL;
	}

	pBar->SetId(nId);

	pBar->SetPos(rect);

	CString strTemp;
	strTemp.Format(_T("%d"), nId);

	pBar->SetName(strTemp);

	if (lpszText)
	{
		pBar->SetText(lpszText);
	}

	CUIGroup* pGroup = FindGroupByRect(rect);
	if (pGroup)
	{
		pGroup->Add(pBar);
	}
	else
	{
		Add(pBar);
	}

	return pBar;
}

CUISliderctrl* CUIPageOption::RegiserSliderCtrl(UINT nId, CRect rect, LPCTSTR lpszText)
{
	CUISliderctrl* pSlider = new CUISliderctrl;
	if (pSlider == NULL)
	{
		return NULL;
	}

	pSlider->SetId(nId);
	pSlider->SetPos(rect);

	CString strTemp;
	strTemp.Format(_T("%d"), nId);

	pSlider->SetName(strTemp);

	if (lpszText)
	{
		pSlider->SetText(lpszText);
	}

	CUIGroup* pGroup = FindGroupByRect(rect);
	if (pGroup)
	{
		pGroup->Add(pSlider);
	}
	else
	{
		Add(pSlider);
	}

	return pSlider;
}

CUIGroup* CUIPageOption::RegiserGroup(UINT nId, CRect rect, LPCTSTR lpszText)
{
	CUIGroup* pGroup = new CUIGroup;
	if (pGroup == NULL)
	{
		return NULL;
	}

	pGroup->SetId(nId);
	pGroup->SetPos(rect);

	CString strTemp;
	strTemp.Format(_T("%d"), nId);

	pGroup->SetName(strTemp);

	if (lpszText)
	{
		pGroup->SetText(lpszText);
	}

	Add(pGroup);

	m_pGroupList.push_back(pGroup);

	return pGroup;
}

CUIGroup* CUIPageOption::FindGroupByRect(RECT rect)
{
	CUIGroup* pGroup = NULL;

	for (int i = 0; i < (int)m_pGroupList.size(); i++)
	{
		pGroup = m_pGroupList[i];
		if (pGroup)
		{
			RECT rcTemp = { 0 };
			RECT rectGroup = pGroup->GetPos();

			if(::IntersectRect(&rcTemp, &rectGroup, &rect))
			{
				if (rect.left == rcTemp.left && rect.top == rcTemp.top && rect.right == rcTemp.right && rect.bottom == rcTemp.bottom)
				{
					break;
				}
				else
				{
					pGroup = NULL;
				}
			}
			else
			{
				pGroup = NULL;
			}
		}
	}

	return pGroup;
}

void CUIPageOption::Event(TEventUI& event)
{

}


