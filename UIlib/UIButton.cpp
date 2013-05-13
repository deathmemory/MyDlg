
#include "StdAfx.h"
#include "UIButton.h"


/////////////////////////////////////////////////////////////////////////////////////
//
//

CUIButton::CUIButton() : m_cxWidth(0), m_uButtonState(0), m_uDropDownState(0), m_uTextStyle(DT_SINGLELINE | DT_CENTER | DT_VCENTER)
{
   m_szPadding.cx = 4;
   m_szPadding.cy = 0;

   m_nAreaPointCount = 0;

   ZeroMemory(&m_AreaPoint, sizeof(POINT) * 20);

   ZeroMemory(&m_rectDropDown, sizeof(RECT));

   m_pImageOwner = NULL;
   m_pImageDropDown = NULL;

   m_bSelected = false;
   m_bDropDown = false;
   m_bShowDropDownAlway = false;

   m_nButtonType = BUTTON_TYPE_NORMAL;

   m_nMinGroupId = -1;
   m_nMaxGroupId = -1;

   m_bEstimateWidth = false;

   m_crTextNormal = RGB(0, 0, 0);
   m_crTextFocused = RGB(0, 0,20);
   m_crTextPushed = RGB(0, 0, 20);
   m_crTextDisable = RGB(128,128,128);

   m_nTextPosX = 0;
   m_nTextPosY = 0;
}

CUIButton::~CUIButton()
{
	if (m_pImageOwner)
	{
		delete m_pImageOwner;
		m_pImageOwner = NULL;
	}

	if (m_pImageDropDown)
	{
		delete m_pImageDropDown;
		m_pImageDropDown = NULL;
	}
}

LPCTSTR CUIButton::GetClass() const
{
   return _T("ButtonUI");
}

UINT CUIButton::GetControlFlags() const
{
   return UIFLAG_TABSTOP | UIFLAG_SETCURSOR;;
}

LPVOID CUIButton::GetInterface(LPCTSTR pstrName)
{
	if( _tcscmp(pstrName, _T("UIChecked")) == 0 ) return static_cast<IUIChecked*>(this);
	return CUIControl::GetInterface(pstrName);
}

void CUIButton::SetTextStyle(UINT uStyle)
{
	if ((uStyle & DT_CENTER) != 0 || (uStyle & DT_RIGHT) != 0)
	{
		uStyle |= DT_SINGLELINE | DT_VCENTER;
	}

	m_uTextStyle = uStyle;
	Invalidate();
}

void CUIButton::Event(TEventUI& event)
{
	if( event.Type == UIEVENT_SETCURSOR )
	{
		if( ::PtInRect(&m_rcItem, event.ptMouse) && IsEnabled() ) {
			::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND)));
			return;
		}
	}

   if( event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_DBLCLICK )
   {
      if( ::PtInRect(&m_rcItem, event.ptMouse) && IsEnabled() ) {

		 if (::PtInRect(&m_rectDropDown, event.ptMouse) && IsEnabled())
		 {
			 m_uDropDownState |= UISTATE_PUSHED | UISTATE_CAPTURED;
		 }

         m_uButtonState |= UISTATE_PUSHED | UISTATE_CAPTURED;
         Invalidate();

		 if (event.Type == UIEVENT_DBLCLICK)
		 {
			 DoubleClick();
		 }

		 if (m_nButtonType != BUTTON_TYPE_NORMAL)
		 {
			 if( ::PtInRect(&m_rectDropDown, event.ptMouse)) 
			 {
				 DropDown(); 
				 return;
			 }

			 if (m_nMinGroupId != - 1  && m_nMaxGroupId != - 1)
			 {
				 if (!m_bSelected)
				 {
					 SetCheck(true);
				 }

				 if (m_nButtonType == BUTTON_TYPE_TAB)
				 {
					 Activate();
				 }
			 }
			 else
			 {
				 if (m_bSelected)
				 {
					 SetCheck(false);
				 }
				 else
				 {
					 SetCheck(true);
					 if (m_nButtonType == BUTTON_TYPE_TAB)
					 {
						 Activate();
					 }
				 }
			 }
		 }
      }
   }
   if( event.Type == UIEVENT_MOUSEMOVE )
   {
      if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) {
         if( ::PtInRect(&m_rcItem, event.ptMouse) ) 
		 {
			 m_uButtonState |= UISTATE_PUSHED;
		 }
         else 
		 { 
			 m_uButtonState &= ~UISTATE_PUSHED;
		 }
         Invalidate();
      }

	  if (m_bSelected)
	  {
		  if (::PtInRect(&m_rcItem, event.ptMouse))
		  {
			  if ((m_uButtonState & UISTATE_CAPTURED) != 0)
			  {
					m_uDropDownState |= UISTATE_PUSHED;
			  }
			  else
			  {
					m_uDropDownState |= UISTATE_HOT;
			  }
			  
		  }
		  else
		  {
			  m_uDropDownState &= ~UISTATE_PUSHED;
			  m_uDropDownState &= ~UISTATE_HOT;
		  }

		  Invalidate();
	  }

   }
   if( event.Type == UIEVENT_BUTTONUP )
   {
      if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) {
         if( ::PtInRect(&m_rcItem, event.ptMouse) ) 
		 {
			 if (m_nButtonType != BUTTON_TYPE_TAB)
			 {
				Activate(); 
			 }
		 }

         m_uButtonState &= ~(UISTATE_PUSHED | UISTATE_CAPTURED);
		 m_uDropDownState &= ~(UISTATE_PUSHED | UISTATE_CAPTURED);
         Invalidate();
      }
   }
   if( event.Type == UIEVENT_MOUSEENTER)
   {
      m_uButtonState |= UISTATE_HOT;
	  if (::PtInRect(&m_rectDropDown, event.ptMouse))
	  {
		  m_uDropDownState |= UISTATE_HOT;
	  }
      Invalidate();
   }
   if( event.Type == UIEVENT_MOUSELEAVE)
   {
      m_uButtonState &= ~UISTATE_HOT;
	  m_uDropDownState &= ~UISTATE_HOT;
      Invalidate();
   }
   CUIControl::Event(event);
}

void CUIButton::SetText(LPCTSTR pstrText)
{
   CUIControl::SetText(pstrText);
   // Automatic assignment of keyboard shortcut
   if( _tcschr(pstrText, '&') != NULL ) m_chShortcut = *(_tcschr(pstrText, '&') + 1);

   if (m_bEstimateWidth)
   {
	   m_cxWidth = 0;

	   SIZE szAvailable = {0};
	   SIZE btnSize = EstimateSize(szAvailable);
	   int nWidth = m_nMinWidth + btnSize.cx;

	   RECT rcTemp = m_rcItem;
	   if (m_pImageOwner)
	   {
			rcTemp.right = rcTemp.left + (m_pImageOwner->GetWidth() / 12.0) * 2;
	   }
	   else
	   {
			rcTemp.right = rcTemp.left + m_nMinWidth;
	   }
	   
	   SetPos(rcTemp);
	   Invalidate();
   }

}

bool CUIButton::Activate()
{
   if( !CUIControl::Activate() ) return false;
   if( m_pManager != NULL ) m_pManager->SendNotify(this, UI_NOTIFY_CLICK);
   return true;
}

bool CUIButton::DoubleClick()
{
	if( !CUIControl::Activate() ) return false;
	if( m_pManager != NULL ) m_pManager->SendNotify(this, UI_NOTIFY_DBCLICK);
	return true;
}

bool CUIButton::DropDown()
{
	if( !CUIControl::Activate() ) return false;
	if (m_bDropDown == false)
	{
		if( m_pManager != NULL ) m_pManager->SendNotify(this, UI_NOTIFY_DROPDOWN);
	}
	return true;
}

void CUIButton::SetWidth(int cxWidth)
{
   m_cxWidth = cxWidth;
   UpdateLayout();
}

void CUIButton::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
   if( _tcscmp(pstrName, _T("width")) == 0 ) SetWidth(_ttoi(pstrValue));
   else if( _tcscmp(pstrName, _T("align")) == 0 ) {
      if( _tcsstr(pstrValue, _T("center")) != NULL ) m_uTextStyle |= DT_CENTER;
      if( _tcsstr(pstrValue, _T("right")) != NULL ) m_uTextStyle |= DT_RIGHT;
   }
   else CUIControl::SetAttribute(pstrName, pstrValue);
}

void CUIButton::SetPadding(int cx, int cy)
{
   m_szPadding.cx = cx;
   m_szPadding.cy = cy;
   UpdateLayout();
}

SIZE CUIButton::EstimateSize(SIZE /*szAvailable*/)
{
   SIZE sz = { m_cxWidth, 4 + m_pManager->GetThemeFontInfo(UIFONT_NORMAL).tmHeight };
   if( m_cxWidth == 0 && m_pManager != NULL ) {
      RECT rcText = { 0, 0, 9999, 20 };
      int nLinks = 0;
      CUIBlueRenderEngine::DoPaintPrettyText(m_pManager->GetPaintDC(), m_pManager, rcText, m_sText, UICOLOR_STANDARD_BLACK, UICOLOR__INVALID, NULL, nLinks, DT_SINGLELINE | DT_CALCRECT);
      sz.cx = rcText.right - rcText.left;
   }
   sz.cx += m_szPadding.cx * 2;
   sz.cy += m_szPadding.cy * 2;
   return sz;
}

void CUIButton::DoPaint(HDC hDC, const RECT& rcPaint)
{
   // Draw button
   UINT uState = m_uButtonState;
   if( IsFocused() ) uState |= UISTATE_FOCUSED;
   if( !IsEnabled() ) uState |= UISTATE_DISABLED;
  

   RECT rcPadding = { m_szPadding.cx, m_szPadding.cy, m_szPadding.cx, m_szPadding.cy };

   if (m_pImageDropDown)
   {
	   m_rectDropDown = m_rcItem;
	   m_rectDropDown.left = m_rectDropDown.right - m_pImageDropDown->GetWidth() / 4;
   }

   if (m_nButtonType == BUTTON_TYPE_CHECKBOX)
   {
	   if( m_bSelected ) uState |= UISTATE_CHECKED;
	   CUIBlueRenderEngine::DoPaintOptionBox(hDC, m_pManager, m_rcItem, m_sText, uState, m_uTextStyle, BUTTON_TYPE_CHECKBOX);
	   return;
   }

   if (m_nButtonType == BUTTON_TYPE_RADIOBOX)
   {
	   if( m_bSelected ) uState |= UISTATE_CHECKED;
	   CUIBlueRenderEngine::DoPaintOptionBox(hDC, m_pManager, m_rcItem, m_sText, uState, m_uTextStyle);
	   return;
   }

   if( m_bSelected )
   {
	   uState |= UISTATE_PUSHED;
   }
   else
   {
	   if (m_nMaxGroupId != -1 && m_nMinGroupId != -1)
	   {
		   uState &= ~UISTATE_PUSHED;
	   }
   }
   
   if (m_pImageOwner != NULL)
   {
	   RECT rcImage = {0};
	   int nWidth = m_pImageOwner->GetWidth() / 4;
	   int nHeight = m_pImageOwner->GetHeight();

	   // Draw frame and body
	   if( (uState & UISTATE_DISABLED) != 0 ) {
		   RECT rcTemp = {nWidth *3, 0, nWidth * 4, nHeight};
		   rcImage = rcTemp;
	   }
	   else if( (uState & UISTATE_PUSHED) != 0 ) {
		  RECT rcTemp ={nWidth *2, 0, nWidth * 3, nHeight};
		  rcImage = rcTemp;
	   }
	   else if( (uState & UISTATE_HOT) != 0 ) {
		   RECT rcTemp ={nWidth *1, 0, nWidth * 2, nHeight};
		   rcImage = rcTemp;
	   }
	   else {
		   RECT rcTemp ={nWidth *0, 0, nWidth * 1, nHeight};
		   rcImage = rcTemp;
	   }

	   CUIBlueRenderEngine::DoFillRect(hDC, m_pManager, m_rcItem, rcImage, m_pImageOwner, false);

   }
   else
   {
	   POINT* pt = NULL;
	   if (m_nAreaPointCount > 0)
	   {
		   pt = m_AreaPoint;
	   }
	   CUIBlueRenderEngine::DoPaintButton(hDC, m_pManager, m_rcItem, uState, pt, m_nAreaPointCount);
   }

   if (m_pImageDropDown != NULL && (m_bSelected == true ||m_bDropDown == true || m_bShowDropDownAlway == true || (uState & UISTATE_HOT) != 0 ))
   {
	   if( m_bDropDown )
	   {
		   m_uDropDownState |= UISTATE_PUSHED;
	   }
	   else
	   {
		   m_uDropDownState &= ~UISTATE_PUSHED;
	   }

	   RECT rcImage = {0};
	   int nWidth = m_pImageDropDown->GetWidth() / 4;
	   int nHeight = m_pImageDropDown->GetHeight();

	   // Draw frame and body
	   if( (m_uDropDownState & UISTATE_DISABLED) != 0 ) {
		   RECT rcTemp = {nWidth *3, 0, nWidth * 4, nHeight};
		   rcImage = rcTemp;
	   }
	   else if( (m_uDropDownState & UISTATE_PUSHED) != 0 ) {
		   RECT rcTemp ={nWidth *2, 0, nWidth * 3, nHeight};
		   rcImage = rcTemp;
	   }
	   else if( (m_uDropDownState & UISTATE_HOT) != 0 ) {
		   RECT rcTemp ={nWidth *1, 0, nWidth * 2, nHeight};
		   rcImage = rcTemp;
	   }
	   else {
		   RECT rcTemp ={nWidth *0, 0, nWidth * 1, nHeight};
		   rcImage = rcTemp;
	   }

	   RECT rectDraw = m_rcItem;

	   rectDraw.left = rectDraw.right - nWidth;

	   CUIBlueRenderEngine::DoFillRect(hDC, m_pManager, rectDraw, rcImage, m_pImageDropDown, false);
   }

   if (m_sText.GetLength() > 0)
   {
	   COLORREF clrText;
	   if( (uState & UISTATE_DISABLED) != 0 ) {
		   clrText = m_crTextDisable;
	   }
	   else if( (uState & UISTATE_PUSHED) != 0 ) {
		   clrText = m_crTextPushed;
	   }
	   else if( (uState & UISTATE_HOT) != 0 ) {
		   clrText = m_crTextFocused;
	   }
	   else {
		   clrText = m_crTextNormal;
	   }

	   RECT rcText = m_rcItem;
	   ::InflateRect(&rcText, -1, -1);

	   if (m_bEstimateWidth)
	   {
			rcText.left += m_nMinWidth;

			m_uTextStyle |= DT_LEFT;
	   }
	   else
	   {
			rcText.left += rcPadding.left;
			rcText.right -= rcPadding.right;
	   }
	   
	   rcText.top += rcPadding.top;
	   rcText.bottom -= rcPadding.bottom;


	   SIZE textSize;
	   textSize.cx = m_rcItem.right - m_rcItem.left;
	   textSize.cy = m_rcItem.bottom - m_rcItem.top;
	   textSize = EstimateSize(textSize);

	   if (m_nTextPosX != 0)
	   { 
		   rcText.left = m_rcItem.left + m_nTextPosX;
		   rcText.right = rcText.left + textSize.cx;
	   } 

	   if ( m_nTextPosY != 0)
	   {
			 rcText.top = m_rcItem.top + m_nTextPosY;
			 rcText.bottom = rcText.top + textSize.cy;
	   }

	   int nLinks = 0;

	   CUIBlueRenderEngine::DoPaintPrettyText(hDC, m_pManager, rcText, m_sText, clrText, INVALID__UICOLOR, NULL, nLinks, DT_SINGLELINE | m_uTextStyle);
   }
}


void CUIButton::SetImage(Image*	pImage)
{
	if (pImage)
	{
		m_pImageOwner = pImage->Clone();
	}
	Invalidate();
}

void CUIButton::SetImage( UINT nResourceID, HMODULE hResHandle/*=NULL*/ )
{
	CUIPaintManager::SetImage(&m_pImageOwner, nResourceID, hResHandle);
	Invalidate();
}

void CUIButton::SetImage(LPCTSTR pstrName)
{
	m_pImageOwner = Image::FromFile(pstrName);
	Invalidate();
}

void CUIButton::SetDropDownImage(Image*	pImage)
{
	if (pImage)
	{
		m_pImageDropDown = pImage->Clone();
	}
	Invalidate();
}

void CUIButton::SetDropDownImage( UINT nResourceID, HMODULE hResHandle/*=NULL*/ )
{
	CUIPaintManager::SetImage(&m_pImageDropDown, nResourceID, hResHandle);
	Invalidate();
}

void CUIButton::SetDropDownImage(LPCTSTR pstrName)
{
	m_pImageDropDown = Image::FromFile(pstrName);
	Invalidate();
}

bool CUIButton::IsChecked() const
{
	return m_bSelected;
}

void CUIButton::SetCheck(bool bSelected)
{
	if( m_bSelected == bSelected ) return;
	m_bSelected = bSelected;

	Invalidate();

	if (m_bSelected == true)
	{
		for (int i = m_nMinGroupId; i <= m_nMaxGroupId; i++)
		{
			if (m_nMinGroupId == -1 || m_nMaxGroupId == -1)
			{
				break;
			}

			if (i == GetId())
			{
				continue;
			}

			CUIControl* pControl = m_pManager ? m_pManager->FindControl(i) : NULL;
			if (pControl)
			{
				IUIChecked* pCheck = static_cast<IUIChecked*>(pControl->GetInterface(_T("UIChecked")));
				if (pCheck)
				{
					pCheck->SetCheck(false);
				}
			}
		}
	}
}

bool CUIButton::IsDropDown() const
{
	return m_bDropDown;
}

void CUIButton::SetDropDown(bool bDropDown)
{
	m_bDropDown = bDropDown;
	Invalidate();
}

void CUIButton::SetShowDropDownAlway(bool bShow)
{
	m_bShowDropDownAlway = bShow;
}


void CUIButton::SetGroupMinMaxID(UINT nMinId, UINT nMaxId)
{
	m_nMinGroupId = nMinId;
	m_nMaxGroupId = nMaxId;
}

void CUIButton::SetButtonType(BUTTON_TYPE nType)
{
	m_nButtonType = nType;
}

BUTTON_TYPE CUIButton::GeyButtonType()
{
	return m_nButtonType;
}

void CUIButton::SetAreaPoint(POINT* pt, int nCount)
{
	if (pt == NULL)
	{
		return;
	}

	m_nAreaPointCount = nCount;

	for (int i = 0; i < nCount; i++)
	{
		m_AreaPoint[i] = pt[i];
	}
}

void CUIButton::SetAutoEstimateWidth(bool bEstimate)
{
	m_bEstimateWidth = bEstimate;
	Invalidate();
}

void CUIButton::SetMinWidth(int nWidth)
{
	m_nMinWidth = nWidth;
	Invalidate();
	
}

void CUIButton::SetTextPos(int nX , int nY)
{
	m_nTextPosX = nX;
	m_nTextPosY = nY;

	Invalidate();
}

void CUIButton::SetTextColor( COLORREF crNormal/*=RGB(0,0,0)*/, COLORREF crFocused/*=RGB(0,0,20)*/, COLORREF crPushed/*=RGB(0,0,20)*/, COLORREF crDisabled/*=RGB(128,128,128)*/ )
{
	m_crTextNormal=crNormal;
	m_crTextFocused=crFocused;
	m_crTextPushed=crPushed;
	m_crTextDisable=crDisabled;

	Invalidate();
}