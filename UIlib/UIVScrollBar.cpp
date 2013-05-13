
#include "StdAfx.h"
#include "UIVScrollBar.h"


/////////////////////////////////////////////////////////////////////////////////////
//
//

CUIVScrollBar::CUIVScrollBar() : m_cxWidth(0), m_uTextStyle(DT_SINGLELINE | DT_CENTER | DT_VCENTER)
{
   m_szPadding.cx = 4;
   m_szPadding.cy = 0;

   m_pImageOwner = NULL;

   m_bLButtonDown = false;

   m_nPosMax = 100;
   m_nPosMin = 0;
   m_nPos = 0;
   
   m_nPageSize = 20;
   m_nLineSize = 1;

   m_nScrollBtnWidth = 0;
   m_nScrollBtnHeight = 0;

   m_uSliderState = 0;
   m_uBtnUpState = 0;
   m_uBtnDownState = 0;
   m_uPageUpState = 0;
   m_uPageDownState = 0;

   m_nLastTop = 0;

   m_nSpan = 3;

   m_dblSliderHeight = 0.0;

}

CUIVScrollBar::~CUIVScrollBar()
{
	if (m_pImageOwner)
	{
		delete m_pImageOwner;
		m_pImageOwner = NULL;
	}
}

LPCTSTR CUIVScrollBar::GetClass() const
{
   return _T("SliderCtrlUI");
}

UINT CUIVScrollBar::GetControlFlags() const
{
   return UIFLAG_TABSTOP | UIFLAG_SETCURSOR;;
}

void CUIVScrollBar::Event(TEventUI& event)
{
	int nPosLast = m_nPos;

	CUIContainer* pParent = dynamic_cast<CUIContainer*>(GetParent());
	if (pParent == NULL)
	{
		return;
	}
	
	TEventUI eventSend = { 0 };
	eventSend.pSender = this;
	eventSend.dwTimestamp = ::GetTickCount();
	eventSend.Type = UIEVENT_VSCROLL;


   if( event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_DBLCLICK )
   {
      if( ::PtInRect(&m_rcBtnSlider, event.ptMouse)) {
         m_uSliderState |= UISTATE_PUSHED | UISTATE_CAPTURED;
         Invalidate();

		 m_ptLast = event.ptMouse;

		 m_nLastTop = m_rcBtnSlider.top;
      }
	  else if (::PtInRect(&m_rcBtnUp, event.ptMouse))
	  {
		  m_uBtnUpState |= UISTATE_PUSHED | UISTATE_CAPTURED;
		  m_nPos -= m_nLineSize;
		  CheckPosValue();
		  Invalidate();

		  eventSend.ptMouse = m_ptLast;
		  eventSend.wParam = SB_LINEUP;
		  pParent->Event(eventSend);

	  }
	  else if (::PtInRect(&m_rcBtnDown, event.ptMouse))
	  {
		  m_uBtnDownState |= UISTATE_PUSHED | UISTATE_CAPTURED;
		  m_nPos += m_nLineSize;
		  CheckPosValue();
		  Invalidate();

		  eventSend.ptMouse = m_ptLast;
		  eventSend.wParam = SB_LINEDOWN;
		  pParent->Event(eventSend);

		
	  }
	  else if (::PtInRect(&m_rcPageUp, event.ptMouse))
	  {
		  m_uPageUpState |= UISTATE_PUSHED | UISTATE_CAPTURED;
		  m_nPos -= m_nPageSize;
		  CheckPosValue();
		  Invalidate();

		  eventSend.ptMouse = m_ptLast;
		  eventSend.wParam = SB_PAGEUP;
		  pParent->Event(eventSend);
	  }
	  else if (::PtInRect(&m_rcPageDown, event.ptMouse))
	  {
		  m_uPageDownState |= UISTATE_PUSHED | UISTATE_CAPTURED;
		  m_nPos += m_nPageSize;
		  CheckPosValue();
		  Invalidate();

		  eventSend.ptMouse = m_ptLast;
		  eventSend.wParam = SB_PAGEDOWN;
		  pParent->Event(eventSend);
	  }

	  SetTimer(8888, 500);
		
   }
   if( event.Type == UIEVENT_MOUSEMOVE )
   {
	  
	   if( (m_uSliderState & UISTATE_CAPTURED) != 0 ) {

		   int nOffsetY =  event.ptMouse.y - m_ptLast.y;

		   int nTop = m_nLastTop + nOffsetY;

		   m_nPos = m_nPosMin + (double)(nTop -  m_rcItem.top - m_nScrollBtnHeight) / (double)(m_rcItem.bottom - m_rcItem.top - m_nScrollBtnHeight * 2 - m_dblSliderHeight) * (m_nPosMax - m_nPosMin - m_nPageSize);

		   CheckPosValue();

		   int nValue1 = m_nPos / m_nLineSize;
		   int nValue2 = m_nPos % m_nLineSize;

		   if (event.ptMouse.x - m_ptLast.x > 0 )
		   {
			   if (nValue2 != 0 && nValue2 > m_nLineSize / 2)
			   {
				   m_nPos = (nValue1 + 1) * m_nLineSize;
		
			   }
			   else
			   {
				   if (nValue2 != 0)
				   {
					   m_nPos = nValue1 * m_nLineSize;
				   }
			   }
		   }
		   else
		   {
			   if (nValue2 != 0 && nValue2 < m_nLineSize / 2)
			   {
				   m_nPos = (nValue1 + 0) * m_nLineSize;
			   }
			   else
			   {
				   if (nValue2 != 0)
				   {
					   m_nPos = (nValue1 + 1) * m_nLineSize;
				   }
			   }
		   }

		   eventSend.ptMouse = m_ptLast;
		   eventSend.wParam = SB_THUMBPOSITION;
		   pParent->Event(eventSend);

		   TRACE(_T("the pos is %d\n"), m_nPos);

		   Invalidate();
	   }
	   else if( (m_uBtnUpState & UISTATE_CAPTURED) != 0 )
	   {

	   }
	   else if( (m_uBtnDownState & UISTATE_CAPTURED) != 0 )
	   {

	   }
	   else if( (m_uPageUpState & UISTATE_CAPTURED) != 0 )
	   {

	   }
	   else if( (m_uPageDownState & UISTATE_CAPTURED) != 0 )
	   {

	   }
	   else
	   {
		   m_uSliderState &= ~UISTATE_HOT;
		   m_uBtnUpState &= ~UISTATE_HOT;
		   m_uBtnDownState &= ~UISTATE_HOT;
		   m_uPageUpState &= ~UISTATE_HOT;
		   m_uPageDownState &= ~UISTATE_HOT;

		   if (::PtInRect(&m_rcBtnSlider, event.ptMouse))
		   {
				m_uSliderState |= UISTATE_HOT;
				m_uPageUpState |= UISTATE_HOT;
				m_uPageDownState |= UISTATE_HOT;
		   }
		   else if( ::PtInRect(&m_rcBtnUp, event.ptMouse) )
		   {
			   m_uBtnUpState |= UISTATE_HOT;
		   }
		   else if( ::PtInRect(&m_rcBtnDown, event.ptMouse) )
		   {

			   m_uBtnDownState |= UISTATE_HOT;
		   }
		   else if( ::PtInRect(&m_rcPageUp, event.ptMouse) )
		   {
			   m_uPageUpState |= UISTATE_HOT;
			   m_uPageDownState |= UISTATE_HOT;
		   }
		   else if( ::PtInRect(&m_rcPageDown, event.ptMouse) )
		   {
			   m_uPageUpState |= UISTATE_HOT;
			   m_uPageDownState |= UISTATE_HOT;
		   }

			Invalidate();

	   }
   }

   if( event.Type == UIEVENT_BUTTONUP )
   {
	  m_uSliderState &= ~(UISTATE_PUSHED | UISTATE_CAPTURED);
	  m_uBtnUpState &= ~(UISTATE_PUSHED | UISTATE_CAPTURED);
	  m_uBtnDownState &= ~(UISTATE_PUSHED | UISTATE_CAPTURED);
	  m_uPageUpState &= ~(UISTATE_PUSHED | UISTATE_CAPTURED);
	  m_uPageDownState &= ~(UISTATE_PUSHED | UISTATE_CAPTURED);

	  CheckPosValue();
	  KillTimer(8888);

	  Invalidate();
   }

   if( event.Type == UIEVENT_TIMER )
   {
	   if( (m_uBtnUpState & UISTATE_CAPTURED) != 0 )
	   {
		   m_nPos -= m_nLineSize;
		   CheckPosValue();
		   Invalidate();

		   eventSend.ptMouse = m_ptLast;
		   eventSend.wParam = SB_LINEUP;
		   pParent->Event(eventSend);
	   }
	   else  if( (m_uBtnDownState & UISTATE_CAPTURED) != 0 )
	   {
		   m_nPos += m_nLineSize;
		   CheckPosValue();
		   Invalidate();

		   eventSend.ptMouse = m_ptLast;
		   eventSend.wParam = SB_LINEDOWN;
		   pParent->Event(eventSend);
	   }

	   else  if( (m_uPageUpState & UISTATE_CAPTURED) != 0 )
	   {
		   m_nPos -= m_nPageSize;
		   CheckPosValue();
		   Invalidate();

		   eventSend.ptMouse = m_ptLast;
		   eventSend.wParam = SB_PAGEUP;
		   pParent->Event(eventSend);
	   }
	   else  if( (m_uPageDownState & UISTATE_CAPTURED) != 0 )
	   {
		   m_nPos += m_nPageSize;
		   CheckPosValue();
		   Invalidate();

		   eventSend.ptMouse = m_ptLast;
		   eventSend.wParam = SB_PAGEDOWN;
		   pParent->Event(eventSend);
	   }
   }

   if (event.Type == UIEVENT_SCROLLWHEEL)
   {
	  switch( LOWORD(event.wParam) ) {
	  case SB_LINEUP:
		  m_nPos -= m_nLineSize;;
		   eventSend.wParam = SB_LINEUP;
		  break;
	  case SB_LINEDOWN:
		  m_nPos += m_nLineSize;;
		   eventSend.wParam = SB_LINEDOWN;
		  break; 
	   }
	   CheckPosValue();

	   eventSend.ptMouse = m_ptLast;
	  
	   pParent->Event(eventSend);

	   TRACE(_T("the pos x is %d\n"), m_nPos);
	   Invalidate();
   }

   if (nPosLast != m_nPos)
   {
	   if (m_pManager)
	   {
		   m_pManager->SendNotify(this, UI_NOTIFY_VSCROLL_POS_CHANGED);
	   }
   }

   if( event.Type == UIEVENT_MOUSELEAVE)
   {
	   m_uSliderState &= ~UISTATE_HOT;
	   m_uBtnUpState &= ~UISTATE_HOT;
	   m_uBtnDownState &= ~UISTATE_HOT;
	   m_uPageUpState &= ~UISTATE_HOT;
	   m_uPageDownState &= ~UISTATE_HOT;
	   Invalidate();
   }

   CUIControl::Event(event);
}


bool CUIVScrollBar::Activate()
{
   if( !CUIControl::Activate() ) return false;
   if( m_pManager != NULL ) m_pManager->SendNotify(this, UI_NOTIFY_CLICK);
   return true;
}

void CUIVScrollBar::SetWidth(int cxWidth)
{
   m_cxWidth = cxWidth;
   UpdateLayout();
}

void CUIVScrollBar::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
   if( _tcscmp(pstrName, _T("width")) == 0 ) SetWidth(_ttoi(pstrValue));
   else if( _tcscmp(pstrName, _T("align")) == 0 ) {
      if( _tcsstr(pstrValue, _T("center")) != NULL ) m_uTextStyle |= DT_CENTER;
      if( _tcsstr(pstrValue, _T("right")) != NULL ) m_uTextStyle |= DT_RIGHT;
   }
   else CUIControl::SetAttribute(pstrName, pstrValue);
}

void CUIVScrollBar::SetPadding(int cx, int cy)
{
   m_szPadding.cx = cx;
   m_szPadding.cy = cy;
   UpdateLayout();
}

SIZE CUIVScrollBar::EstimateSize(SIZE /*szAvailable*/)
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

void CUIVScrollBar::DoPaint(HDC hDC, const RECT& rcPaint)
{
   // Draw button
   UINT uState = m_uSliderState;
   if( IsFocused() ) uState |= UISTATE_FOCUSED;
   if( !IsEnabled() ) uState |= UISTATE_DISABLED;

   if (m_pImageOwner == NULL)
   {
	   return;
   }

   typedef struct 
   {
	   int x, y;
	   int width, height;
   } CustomDrawTable;

   // »­±³¾°
   Graphics graph(hDC);

   CUIBlueRenderEngine::DoFillRect(hDC, m_pManager, m_rcItem, RGB(255,255,255));

   CustomDrawTable *pTable;
   CustomDrawTable drawTable;
   pTable = &drawTable;

   int nWidth = m_rcItem.right - m_rcItem.left;
   int nHeight = m_rcItem.bottom - m_rcItem.top;
   
   m_dblSliderHeight = (double)m_nPageSize / (double)(m_nPosMax - m_nPosMin) * (nHeight - m_nScrollBtnHeight * 2);

   if (m_dblSliderHeight < m_nScrollBtnHeight * 2)
   {
		m_dblSliderHeight = m_nScrollBtnHeight * 2;
   }

   double dblBtnSliderTop = m_rcItem.top + m_nScrollBtnHeight + (double)(m_nPos - m_nPosMin) / (double)(m_nPosMax - m_nPosMin - m_nPageSize) * (nHeight - m_nScrollBtnHeight * 2 - m_dblSliderHeight);
   if (dblBtnSliderTop < m_rcItem.top + m_nScrollBtnHeight + m_nSpan)
   {
	   dblBtnSliderTop = m_rcItem.top + m_nScrollBtnHeight + m_nSpan;
   }
   if (dblBtnSliderTop > m_rcItem.bottom - m_nScrollBtnHeight - m_dblSliderHeight - m_nSpan)
   {
	   dblBtnSliderTop = m_rcItem.bottom - m_nScrollBtnHeight - m_dblSliderHeight  - m_nSpan;
   }

   m_rcBtnSlider.top = dblBtnSliderTop;
   m_rcBtnSlider.bottom = dblBtnSliderTop + m_dblSliderHeight;

   m_rcBtnSlider.left = m_rcItem.left;
   m_rcBtnSlider.right = m_rcItem.right;

   m_rcBtnUp.top = m_rcItem.top;
   m_rcBtnUp.bottom = m_rcBtnUp.top + m_nScrollBtnHeight;
   m_rcBtnUp.left = m_rcItem.left;
   m_rcBtnUp.right = m_rcItem.right;

   m_rcBtnDown.top = m_rcItem.bottom - m_nScrollBtnHeight;
   m_rcBtnDown.bottom = m_rcItem.bottom;
   m_rcBtnDown.left = m_rcItem.left;
   m_rcBtnDown.right = m_rcItem.right;

   m_rcPageUp.top = m_rcItem.top + m_nScrollBtnHeight;
   m_rcPageUp.bottom = m_rcBtnSlider.top;
   m_rcPageUp.left = m_rcItem.left;
   m_rcPageUp.right = m_rcItem.right;

   m_rcPageDown.top = m_rcBtnSlider.bottom;
   m_rcPageDown.bottom = m_rcItem.bottom - m_nScrollBtnHeight;
   m_rcPageDown.left = m_rcItem.left;
   m_rcPageDown.right = m_rcItem.right;


   {
	   pTable->width = (m_rcImageBtnUp.right - m_rcImageBtnUp.left) / 4;
	   pTable->height = m_rcImageBtnUp.bottom - m_rcImageBtnUp.top;
	   pTable->y = m_rcImageBtnUp.top;

	   if( (m_uBtnUpState & UISTATE_PUSHED) != 0 )
	   {
		   pTable->x = pTable->width * 2;
	   }
	   else if ( (m_uBtnUpState & UISTATE_HOT) != 0 )
	   {
		  pTable->x = pTable->width * 1;
	   }
	   else
	   {
		  pTable->x = pTable->width * 0;
	   }

	   RectF rfDest(PointF((REAL)m_rcBtnUp.left, (REAL)m_rcBtnUp.top), SizeF((REAL)m_rcBtnUp.right - m_rcBtnUp.left, (REAL)(m_rcBtnUp.bottom - m_rcBtnUp.top)));
	   graph.DrawImage(m_pImageOwner, rfDest, (REAL)pTable->x, (REAL)pTable->y, (REAL)pTable->width, (REAL)pTable->height, UnitPixel, NULL);
   }


   {
	   pTable->width = (m_rcImageBtnDown.right - m_rcImageBtnDown.left) / 4;
	   pTable->height = m_rcImageBtnDown.bottom - m_rcImageBtnDown.top;
	   pTable->y = m_rcImageBtnDown.top;

	   if( (m_uBtnDownState & UISTATE_PUSHED) != 0 )
	   {
		  pTable->x = pTable->width * 2;
	   }
	   else if ( (m_uBtnDownState & UISTATE_HOT) != 0 )
	   {
		   pTable->x = pTable->width * 1;
	   }
	   else
	   {
		   pTable->x = pTable->width * 0;
	   }

	   RectF rfDest(PointF((REAL)m_rcBtnDown.left, (REAL)m_rcBtnDown.top), SizeF((REAL)m_rcBtnDown.right - m_rcBtnDown.left, (REAL)(m_rcBtnDown.bottom - m_rcBtnDown.top)));
	   graph.DrawImage(m_pImageOwner, rfDest, (REAL)pTable->x, (REAL)pTable->y, (REAL)pTable->width, (REAL)pTable->height, UnitPixel, NULL);
   }


   {
	   pTable->width = (m_rcImagePageUp.right - m_rcImagePageUp.left) / 4;
	   pTable->height = m_rcImagePageUp.bottom - m_rcImagePageUp.top;
	   pTable->y = m_rcImagePageUp.top;

	   if( (m_uPageUpState & UISTATE_PUSHED) != 0 )
	   {
			pTable->x = pTable->width * 2;
	   }
	   else if ( (m_uPageUpState & UISTATE_HOT) != 0 )
	   {
		   pTable->x = pTable->width * 1;
	   }
	   else
	   {
			pTable->x = pTable->width * 0;
	   }
	   
	   RectF rfDest(PointF((REAL)m_rcPageUp.left, (REAL)m_rcPageUp.top), SizeF((REAL)m_rcPageUp.right - m_rcPageUp.left, (REAL)(m_rcPageUp.bottom - m_rcPageUp.top)));

	   int nDrawHeight = MIN(pTable->height, m_rcPageUp.bottom - m_rcPageUp.top);

	   graph.DrawImage(m_pImageOwner, rfDest, (REAL)pTable->x, (REAL)pTable->y, (REAL)pTable->width, (REAL)nDrawHeight, UnitPixel, NULL);
   }

   {
	   pTable->width = (m_rcImagePageDown.right - m_rcImagePageDown.left) / 4;
	   pTable->height = m_rcImagePageDown.bottom - m_rcImagePageDown.top;
	   pTable->y = m_rcImagePageDown.top;

	   if( (m_uPageDownState & UISTATE_PUSHED) != 0 )
	   {
		   pTable->x = pTable->width * 2;
	   }
	   else if ( (m_uPageDownState & UISTATE_HOT) != 0 )
	   {
		   pTable->x = pTable->width * 1;
	   }
	   else
	   {
		   pTable->x = pTable->width * 0;
	   }
	   RectF rfDest(PointF((REAL)m_rcPageDown.left, (REAL)m_rcPageUp.bottom), SizeF((REAL)(m_rcPageDown.right - m_rcPageDown.left), (REAL)(m_rcPageDown.bottom- m_rcPageUp.bottom)));

	   int nDrawHeight = MIN(pTable->height, m_rcPageDown.bottom - m_rcPageUp.bottom);

	   graph.DrawImage(m_pImageOwner, rfDest, (REAL)pTable->x, (REAL)(pTable->y + pTable->height - nDrawHeight), (REAL)pTable->width, (REAL)nDrawHeight, UnitPixel, NULL);
   }


   {
	   pTable->width = (m_rcImageSlider_M.right - m_rcImageSlider_M.left) / 4;
	   pTable->height = m_rcImageSlider_M.bottom - m_rcImageSlider_M.top;
	   pTable->y = m_rcImageSlider_M.top;

	   if( (m_uSliderState & UISTATE_PUSHED) != 0 )
	   {
		   pTable->x = pTable->width * 2;
	   }
	   else if ( (m_uSliderState & UISTATE_HOT) != 0 )
	   {
		   pTable->x = pTable->width * 1;
	   }
	   else
	   {
		   pTable->x = pTable->width * 0;
	   }
	   RectF rfDest(PointF((REAL)m_rcBtnSlider.left, (REAL)m_rcBtnSlider.top), SizeF((REAL)pTable->width, m_dblSliderHeight));
	   graph.DrawImage(m_pImageOwner, rfDest, (REAL)pTable->x, (REAL)pTable->y, (REAL)pTable->width, (REAL)pTable->height, UnitPixel, NULL);
   }

   {
	   pTable->width = (m_rcImageSlider.right - m_rcImageSlider.left) / 4;
	   pTable->height = (m_rcImageSlider.bottom - m_rcImageSlider.top) / 2;
	   pTable->y = m_rcImageSlider.top;

	   if( (m_uSliderState & UISTATE_PUSHED) != 0 )
	   {
		   pTable->x = pTable->width * 2;
	   }
	   else if ( (m_uSliderState & UISTATE_HOT) != 0 )
	   {
		   pTable->x = pTable->width * 1;
	   }
	   else
	   {
		   pTable->x = pTable->width * 0;
	   }
	   RectF rfDest(PointF((REAL)m_rcBtnSlider.left, (REAL)m_rcBtnSlider.top), SizeF((REAL)pTable->width, (REAL)pTable->height));
	   graph.DrawImage(m_pImageOwner, rfDest, (REAL)pTable->x, (REAL)pTable->y, (REAL)pTable->width, (REAL)pTable->height, UnitPixel, NULL);
   }

   {
	   pTable->width = (m_rcImageSlider.right - m_rcImageSlider.left) / 4;
	   pTable->height = (m_rcImageSlider.bottom - m_rcImageSlider.top) / 2;
	   pTable->y = m_rcImageSlider.bottom - pTable->height;

	   if( (m_uSliderState & UISTATE_PUSHED) != 0 )
	   {
		   pTable->x = pTable->width * 2;
	   }
	   else if ( (m_uSliderState & UISTATE_HOT) != 0 )
	   {
		   pTable->x = pTable->width * 1;
	   }
	   else
	   {
		   pTable->x = pTable->width * 0;
	   }

	   RectF rfDest(PointF((REAL)m_rcBtnSlider.left, (REAL)m_rcBtnSlider.bottom - pTable->height), SizeF((REAL)pTable->width, pTable->height));
	   graph.DrawImage(m_pImageOwner, rfDest, (REAL)pTable->x, (REAL)pTable->y, (REAL)pTable->width, (REAL)pTable->height, UnitPixel, NULL);
   }

   {
	   pTable->width = (m_rcImageSlider_MT.right - m_rcImageSlider_MT.left) / 4;
	   pTable->height = (m_rcImageSlider_MT.bottom - m_rcImageSlider_MT.top);
	   pTable->y = m_rcImageSlider_MT.top;

	   if( (m_uSliderState & UISTATE_PUSHED) != 0 )
	   {
		   pTable->x = pTable->width * 2;
	   }
	   else if ( (m_uSliderState & UISTATE_HOT) != 0 )
	   {
		   pTable->x = pTable->width * 1;
	   }
	   else
	   {
		   pTable->x = pTable->width * 0;
	   }

	   RectF rfDest(PointF((REAL)m_rcBtnSlider.left, (REAL)m_rcBtnSlider.top + (m_dblSliderHeight - pTable->height) / 2), SizeF((REAL)pTable->width, pTable->height));
	   graph.DrawImage(m_pImageOwner, rfDest, (REAL)pTable->x, (REAL)pTable->y, (REAL)pTable->width, (REAL)pTable->height, UnitPixel, NULL);
   }


}


void CUIVScrollBar::SetImage(Image*	pImage)
{
	if (pImage)
	{
		m_pImageOwner = pImage->Clone();
	}
	
	if (m_pImageOwner)
	{
		int nWidth = m_pImageOwner->GetWidth();
		int nHeight = m_pImageOwner->GetHeight();

		SetScrollBtnSize(nWidth / 4, nHeight / 6);
	}

	Invalidate();
}

void CUIVScrollBar::SetImage(UINT nResourceID)
{
	CUIPaintManager::SetImage(&m_pImageOwner, nResourceID);
	if (m_pImageOwner)
	{
		int nWidth = m_pImageOwner->GetWidth();
		int nHeight = m_pImageOwner->GetHeight();

		SetScrollBtnSize(nWidth / 4, nHeight / 6);
	}

	Invalidate();
}

void CUIVScrollBar::SetImage(LPCTSTR pstrName)
{
	m_pImageOwner = Image::FromFile(pstrName);
	if (m_pImageOwner)
	{
		int nWidth = m_pImageOwner->GetWidth();
		int nHeight = m_pImageOwner->GetHeight();

		SetScrollBtnSize(nWidth / 4, nHeight / 6);
	}

	Invalidate();
}

int CUIVScrollBar::GetPageSize()
{
	return m_nPageSize / 10;
}
void CUIVScrollBar::SetPageSize(int nPageSize)
{
	m_nPageSize = MIN(m_nPosMax, nPageSize * 10);
}

void CUIVScrollBar::GetRange(int& nMin, int& nMax ) const
{
	nMin = m_nPosMin / 10;
	nMax = m_nPosMax / 10;
}
void CUIVScrollBar::SetRange(int nMin, int nMax, bool bRedraw)
{
	m_nPosMin = nMin * 10;
	m_nPosMax = nMax * 10;

	if (bRedraw)
	{
		Invalidate();
	}
	
}

int CUIVScrollBar::GetCurrentPos()
{
	return m_nPos / 10;
}
void CUIVScrollBar::SetCurrentPos(int nPos, bool bRedraw)
{
	m_nPos = nPos * 10;
	CheckPosValue();
	if (bRedraw)
	{
		Invalidate();
	}
}

void CUIVScrollBar::CheckPosValue()
{
	if (m_nPos > m_nPosMax - m_nPageSize)
	{
		m_nPos = m_nPosMax - m_nPageSize;
	}
	if (m_nPos < m_nPosMin)
	{
		m_nPos = m_nPosMin;
	}
}

int CUIVScrollBar::GetLineSize()
{
	return m_nLineSize / 10;
}
void CUIVScrollBar::SetLineSize(int nLineSize)
{
	m_nLineSize = nLineSize * 10;
}

void CUIVScrollBar::SetScrollBtnSize(int nWidth, int nHeight)
{
	m_nScrollBtnWidth = nWidth;
	m_nScrollBtnHeight = nHeight;

	{
		RECT rcTemp = {0, 0, nWidth * 4, nHeight};
		m_rcImageBtnUp = rcTemp;
	}

	{
		RECT rcTemp = {0, nHeight * 1, nWidth * 4, nHeight * 2};
		m_rcImageBtnDown = rcTemp;
	}

	{
		RECT rcTemp = {0, nHeight * 2, nWidth * 4, nHeight * 3};
		m_rcImagePageUp = rcTemp;
	}

	{
		RECT rcTemp = {0, nHeight * 2, nWidth * 4, nHeight * 3};
		m_rcImagePageDown = rcTemp;
	}

	{
		RECT rcTemp = {0, nHeight * 3, nWidth * 4, nHeight * 4};
		m_rcImageSlider = rcTemp;
	}

	{
		RECT rcTemp = {0, nHeight * 4, nWidth * 4, nHeight * 5};
		m_rcImageSlider_MT = rcTemp;
	}

	{
		RECT rcTemp = {0, nHeight * 5, nWidth * 4, nHeight * 6};
		m_rcImageSlider_M = rcTemp;
	}
}

SIZE CUIVScrollBar::GetScrollBtnSize()
{
	SIZE size = {m_nScrollBtnWidth, m_nScrollBtnHeight};

	return size;
}


void CUIVScrollBar::SetBtnUpPos(const RECT& rc)
{
	m_rcImageBtnUp = rc;
}
void CUIVScrollBar::SetBtnDownPos(const RECT& rc)
{
	m_rcImageBtnDown = rc;
}
void CUIVScrollBar::SetPageUpPos(const RECT& rc)
{
	m_rcImagePageUp = rc;
}
void CUIVScrollBar::SetPageDownPos(const RECT& rc)
{
	m_rcImagePageDown = rc;
}

void CUIVScrollBar::SetSliderPos(const RECT& rc)
{
	m_rcImageSlider = rc;
}
void CUIVScrollBar::SetSlider_MPos(const RECT& rc)
{
	m_rcImageSlider_M = rc;
}
void CUIVScrollBar::SetSlider_MTPos(const RECT& rc)
{
	m_rcImageSlider_MT = rc;
}
