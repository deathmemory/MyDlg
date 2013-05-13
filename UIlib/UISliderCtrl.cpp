
#include "StdAfx.h"
#include "UISliderCtrl.h"


/////////////////////////////////////////////////////////////////////////////////////
//
//

CUISliderctrl::CUISliderctrl() : m_cxWidth(0), m_uButtonState(0), m_uTextStyle(DT_SINGLELINE | DT_CENTER | DT_VCENTER)
{
   m_szPadding.cx = 4;
   m_szPadding.cy = 0;

   m_pImageSlider = NULL;
   m_pImageShaft = NULL;

   m_bLButtonDown = false;

   m_nPosMax = 100;
   m_nPosMin = 0;
   m_nPos = 0;

   m_nButtonOffset = 0;
   
   m_nPageSize = 20;

   m_nScaleHeight = 3;

   m_colorBk = INVALID__UICOLOR;
   m_colorScale = INVALID__UICOLOR;

}

CUISliderctrl::~CUISliderctrl()
{
	if (m_pImageSlider)
	{
		delete m_pImageSlider;
		m_pImageSlider = NULL;
	}
}

LPCTSTR CUISliderctrl::GetClass() const
{
   return _T("SliderCtrlUI");
}

UINT CUISliderctrl::GetControlFlags() const
{
   return UIFLAG_TABSTOP | UIFLAG_SETCURSOR;;
}

void CUISliderctrl::Event(TEventUI& event)
{
	if( event.Type == UIEVENT_SETCURSOR )
	{
		if( ::PtInRect(&m_rcButton, event.ptMouse) && IsEnabled() ) {
			::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND)));
			return;
		}
	}

	int nPosLast = m_nPos;

   if( event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_DBLCLICK )
   {
      if( ::PtInRect(&m_rcButton, event.ptMouse) && IsEnabled() ) {
         m_uButtonState |= UISTATE_PUSHED | UISTATE_CAPTURED;
         Invalidate();

		 m_ptLast = event.ptMouse;
		 m_nButtonOffset = m_ptLast.x - m_rcButton.left;
      }

	  if (::PtInRect(&m_rcItem, event.ptMouse) && IsEnabled())
	  {
		  if (event.ptMouse.x < m_rcButton.left)
		  {
			  m_nPos -= m_nPageSize;
			  CheckPosValue();
			  Invalidate();
		  }

		  if (event.ptMouse.x > m_rcButton.right)
		  {
			  m_nPos += m_nPageSize;
			  CheckPosValue();
			  Invalidate();
		  }
	  }
   }
   if( event.Type == UIEVENT_MOUSEMOVE )
   {
      if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) {

		  int nButtonWidth = (m_rcButton.right - m_rcButton.left);

		  int nOffsetX =  event.ptMouse.x - m_rcItem.left - m_nButtonOffset;

		  TRACE(_T("the pos x is %d\n"), nOffsetX);


		  if (nOffsetX < 0)
		  {
			  nOffsetX = 0;
		  }

		  if (nOffsetX > m_rcItem.right - m_rcItem.left - nButtonWidth)
		  {
			  nOffsetX = m_rcItem.right - m_rcItem.left - nButtonWidth;
		  }

		  double dblOffset = (double)nOffsetX / (double)(m_rcItem.right - m_rcItem.left - nButtonWidth);

		  TRACE(_T("the pos dblOffset is %.2f\n"), dblOffset);

		  m_nPos = m_nPosMin + (m_nPosMax - m_nPosMin) * dblOffset;
		  CheckPosValue();

		  TRACE(_T("the m_nPos  is %d\n"), m_nPos);

		 int nValue1 = m_nPos / m_nPageSize;
		 int nValue2 = m_nPos % m_nPageSize;

		 if (event.ptMouse.x - m_ptLast.x > 0 )
		 {
			 if (nValue2 != 0 && nValue2 > m_nPageSize / 2)
			 {
				 m_nPos = (nValue1 + 1) * m_nPageSize;
				 Invalidate();
			 }
			 else
			 {
				 if (nValue2 != 0)
				 {
					m_nPos = nValue1 * m_nPageSize;
				 }
				 else
				 {
					Invalidate();
				 }
				 
			 }
		 }
		 else
		 {
			 if (nValue2 != 0 && nValue2 < m_nPageSize / 2)
			 {
				 m_nPos = (nValue1 + 0) * m_nPageSize;
				 Invalidate();
			 }
			 else
			 {
				 if (nValue2 != 0)
				 {
					m_nPos = (nValue1 + 1) * m_nPageSize;
				 }
				 else
				 {
					 Invalidate();
				 }
				 
			 }
		 }
      }
	  else
	  {
		  if( ::PtInRect(&m_rcButton, event.ptMouse) )
		  {
		   m_uButtonState |= UISTATE_HOT;
		  }
		  else 
		  {
		   m_uButtonState &= ~UISTATE_HOT;
		  }

		  Invalidate();
	  }

	  m_ptLast = event.ptMouse;
   }

   if( event.Type == UIEVENT_BUTTONUP )
   {
      if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) {
         if( ::PtInRect(&m_rcButton, event.ptMouse) ) Activate();
         m_uButtonState &= ~(UISTATE_PUSHED | UISTATE_CAPTURED);
		 CheckPosValue();
         Invalidate();
      }
   }

   if (event.Type == UIEVENT_SCROLLWHEEL)
   {

	   if (::PtInRect(&m_rcItem, event.ptMouse) && IsEnabled())
	   {
		   switch( LOWORD(event.wParam) ) {
		  case SB_LINEUP:
			  m_nPos += m_nPageSize;
			  break;
		  case SB_LINEDOWN:
			  m_nPos -= m_nPageSize;
			  break; 
		   }
		   CheckPosValue();
		   Invalidate();
	   }

   }

   if (nPosLast != m_nPos)
   {
	   if (m_pManager)
	   {
		   m_pManager->SendNotify(this, UI_NOTIFY_SLIDER_POS_CHANGED);

		   TRACE(_T("SendNotify\n"));
	   }
   }

   if( event.Type == UIEVENT_MOUSELEAVE)
   {
	   m_uButtonState &= ~UISTATE_HOT;
	   Invalidate();
   }

   CUIControl::Event(event);
}


bool CUISliderctrl::Activate()
{
   if( !CUIControl::Activate() ) return false;
   if( m_pManager != NULL ) m_pManager->SendNotify(this, UI_NOTIFY_CLICK);
   return true;
}

void CUISliderctrl::SetWidth(int cxWidth)
{
   m_cxWidth = cxWidth;
   UpdateLayout();
}

void CUISliderctrl::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
   if( _tcscmp(pstrName, _T("width")) == 0 ) SetWidth(_ttoi(pstrValue));
   else if( _tcscmp(pstrName, _T("align")) == 0 ) {
      if( _tcsstr(pstrValue, _T("center")) != NULL ) m_uTextStyle |= DT_CENTER;
      if( _tcsstr(pstrValue, _T("right")) != NULL ) m_uTextStyle |= DT_RIGHT;
   }
   else CUIControl::SetAttribute(pstrName, pstrValue);
}

void CUISliderctrl::SetPadding(int cx, int cy)
{
   m_szPadding.cx = cx;
   m_szPadding.cy = cy;
   UpdateLayout();
}

SIZE CUISliderctrl::EstimateSize(SIZE /*szAvailable*/)
{
   SIZE sz = { m_cxWidth, 12 + m_pManager->GetThemeFontInfo(UIFONT_NORMAL).tmHeight };
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

void CUISliderctrl::DoPaint(HDC hDC, const RECT& rcPaint)
{
   // Draw button
   UINT uState = m_uButtonState;
   if( IsFocused() ) uState |= UISTATE_FOCUSED;
   if( !IsEnabled() ) uState |= UISTATE_DISABLED;

	// Ìî³ä±³¾°ÑÕÉ«
   if (m_colorBk != INVALID__UICOLOR)
   {
		CUIBlueRenderEngine::DoFillRect(hDC, m_pManager, m_rcItem, m_colorBk);
   }
   

   // »­¿Ì¶È
   int nPageCount = (m_nPosMax - m_nPosMin) / m_nPageSize;
   int nButtonWidth = 0;
   if (m_pImageSlider != NULL)
   {
	   nButtonWidth = m_pImageSlider->GetWidth() / 4;
   }
   int nItemWidth = m_rcItem.right - m_rcItem.left - nButtonWidth;

	// »­¿Ì¶È
   if (m_colorScale != INVALID__UICOLOR)
   {
	   for (int i = 0; i <= nPageCount; i++)
	   {	  
		   POINT pt1 = {m_rcItem.left + i * ((double)nItemWidth / nPageCount) + nButtonWidth / 2, m_rcItem.top};
		   POINT pt2 = {m_rcItem.left + i * ((double)nItemWidth / nPageCount) + nButtonWidth / 2, m_rcItem.top + m_nScaleHeight};

		   CUIBlueRenderEngine::DoPaintLine(hDC, m_pManager, pt1, pt2, m_colorScale);
	   }
   }

   // »­Öá
   if (m_pImageShaft != NULL)
   {
	 int nShaftHeight = m_pImageShaft->GetHeight();

	 int nTop = m_rcItem.top + (m_rcItem.bottom - m_rcItem.top - nShaftHeight - m_nScaleHeight) / 2 + m_nScaleHeight;

	 RECT rcPadding = {m_rcItem.left + nButtonWidth / 2, nTop, m_rcItem.right - nButtonWidth / 2, nTop + nShaftHeight};

	 RECT rcImage= {0, 0, m_pImageShaft->GetWidth(), m_pImageShaft->GetHeight()};

	 CUIBlueRenderEngine::DoFillRect(hDC, m_pManager, rcPadding, rcImage, m_pImageShaft);
   }
  
   
   if (m_pImageSlider != NULL)
   {
	   int nWidth = m_pImageSlider->GetWidth() / 4;
	   int nHeight = m_pImageSlider->GetHeight();

	   int nIndex = 0;

	   // Draw frame and body
	   if( (uState & UISTATE_DISABLED) != 0 ) {
		   nIndex = 3;;
	   }
	   else if( (uState & UISTATE_PUSHED) != 0 ) {
		   nIndex = 2;
	   }
	   else if( (uState & UISTATE_HOT) != 0 ) {
		   nIndex = 1;
	   }
	   else {
		   nIndex = 0;
	   }

	   Graphics graph(hDC);
	   RECT rcImage = {nWidth *nIndex, 0, nWidth * (nIndex + 1), nHeight};

		double dblPos = (double)(m_nPos - m_nPosMin) / (double)(m_nPosMax - m_nPosMin);

		int nButtonPosX = m_rcItem.left  + (m_rcItem.right - m_rcItem.left - nWidth) * dblPos;
		if (nButtonPosX < m_rcItem.left)
		{
			nButtonPosX = m_rcItem.left;
		}

		if (nButtonPosX > m_rcItem.right - nWidth)
		{
			nButtonPosX = m_rcItem.right - nWidth;
		}

		int nButtonPosY = m_rcItem.top + ((m_rcItem.bottom - m_rcItem.top) - nHeight - m_nScaleHeight)/ 2 + m_nScaleHeight;

		m_rcButton.left = nButtonPosX;
		m_rcButton.right = m_rcButton.left + nWidth;

		m_rcButton.top = nButtonPosY;
		m_rcButton.bottom = m_rcButton.top + nHeight;

		RectF rfDest(PointF((REAL)nButtonPosX, (REAL)nButtonPosY), SizeF((REAL)nWidth, (REAL)nHeight));

		graph.DrawImage(m_pImageSlider, rfDest, (REAL)rcImage.left, (REAL)rcImage.top, (REAL)nWidth, (REAL)nHeight, UnitPixel, NULL);

   }
}


void CUISliderctrl::SetSliderImage(Image*	pImage)
{
	if (pImage)
	{
		m_pImageSlider = pImage->Clone();
	}
	Invalidate();
}

void CUISliderctrl::SetSliderImage(UINT nResourceID)
{
	CUIPaintManager::SetImage(&m_pImageSlider, nResourceID);
	Invalidate();
}

void CUISliderctrl::SetSliderImage(LPCTSTR pstrName)
{
	m_pImageSlider = Image::FromFile(pstrName);
	Invalidate();
}

void CUISliderctrl::SetShaftImage(Image*	pImage)
{
	if (pImage)
	{
		m_pImageShaft = pImage->Clone();
	}
	Invalidate();
}

void CUISliderctrl::SetShaftImage(UINT nResourceID)
{
	CUIPaintManager::SetImage(&m_pImageShaft, nResourceID);
	Invalidate();
}

void CUISliderctrl::SetShaftImage(LPCTSTR pstrName)
{
	m_pImageShaft = Image::FromFile(pstrName);
	Invalidate();
}

int CUISliderctrl::GetPageSize()
{
	return m_nPageSize / 10;
}
void CUISliderctrl::SetPageSize(int nPageSize)
{
	m_nPageSize = nPageSize * 10;
}

void CUISliderctrl::GetRange(int& nMin, int& nMax ) const
{
	nMin = m_nPosMin / 10;
	nMax = m_nPosMax / 10;
}
void CUISliderctrl::SetRange(int nMin, int nMax, bool bRedraw)
{
	m_nPosMin = nMin * 10;
	m_nPosMax = nMax * 10;

	if (bRedraw)
	{
		Invalidate();
	}
	
}

int CUISliderctrl::GetCurrentPos()
{
	return m_nPos / 10;
}
void CUISliderctrl::SetCurrentPos(int nPos, bool bRedraw)
{
	m_nPos = nPos * 10;
	if (bRedraw)
	{
		Invalidate();
	}
}

void CUISliderctrl::SetBkColor(COLORREF color)
{
	m_colorBk = color;
	Invalidate();
}
void CUISliderctrl::SetScaleColor(COLORREF color)
{
	m_colorScale = color;
	Invalidate();
}

void CUISliderctrl::CheckPosValue()
{
	if (m_nPos > m_nPosMax)
	{
		m_nPos = m_nPosMax;
	}
	if (m_nPos < m_nPosMin)
	{
		m_nPos = m_nPosMin;
	}
}