
#include "StdAfx.h"
#include "UILabel.h"


/////////////////////////////////////////////////////////////////////////////////////
//
//

CUILabel::CUILabel() : m_nLinks(0), 
								 m_cxWidth(0), 
								 m_uTextStyle(DT_SINGLELINE | DT_VCENTER),
								 m_TextColor(RGB(0, 0, 0)),
								 m_TextColor_Disable(RGB(0xA7, 0xA6, 0xAA)),
								 m_BackColor(INVALID__UICOLOR),
								 m_BorderColorInside(INVALID__UICOLOR),
								 m_BorderColorOutside(INVALID__UICOLOR),
								 m_nImageWidth(0),
								 m_nIndex(0),
								 m_nImageCount(1),
								 m_nInterval(100),
								 m_bImageAsLinker(false),
								 m_fAlpha(1.0),
								 m_nDelayTime(0),
								 m_nCurLink(-1)
{
	m_pImageOwner = NULL;

	m_sizeAnimate = CUISize(0, 0);

	::ZeroMemory(m_rcLinks, sizeof(m_rcLinks));
}

CUILabel::~CUILabel()
{
	if (m_pImageOwner)
	{
		delete m_pImageOwner;
		m_pImageOwner = NULL;
	}
}

LPCTSTR CUILabel::GetClass() const
{
   return _T("LabelPanelUI");
}

UINT CUILabel::GetControlFlags() const
{
	return m_nLinks > 0 ? UIFLAG_SETCURSOR : 0;
}

void CUILabel::SetText(LPCTSTR pstrText)
{
   // Automatic assignment of keyboard shortcut
   if( _tcschr(pstrText, '&') != NULL ) m_chShortcut = *(_tcschr(pstrText, '&') + 1);
   CUIControl::SetText(pstrText);
}

void CUILabel::SetWidth(int cx)
{
   m_cxWidth = cx;
   UpdateLayout();
}

void CUILabel::SetTextStyle(UINT uStyle)
{
	if ((uStyle & DT_CENTER) != 0 || (uStyle & DT_RIGHT) != 0)
	{
		uStyle |= DT_SINGLELINE | DT_VCENTER;
	}

	m_uTextStyle = uStyle;
	Invalidate();
}

void CUILabel::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
   if( _tcscmp(pstrName, _T("align")) == 0 ) {
      if( _tcsstr(pstrValue, _T("center")) != NULL ) m_uTextStyle |= DT_CENTER;
      if( _tcsstr(pstrValue, _T("right")) != NULL ) m_uTextStyle |= DT_RIGHT;
   }
   else if( _tcscmp(pstrName, _T("width")) == 0 ) SetWidth(_ttoi(pstrValue));
   else CUIControl::SetAttribute(pstrName, pstrValue);
}

SIZE CUILabel::EstimateSize(SIZE /*szAvailable*/)
{
   return CUISize(m_cxWidth, m_pManager->GetThemeFontInfo(UIFONT_NORMAL).tmHeight + 4);
}

void CUILabel::DoPaint(HDC hDC, const RECT& rcPaint)
{
	if (m_BackColor != INVALID__UICOLOR)
	{
		CUIBlueRenderEngine::DoFillRect(hDC, m_pManager, m_rcItem, m_BackColor);
	}

	if (m_BorderColorOutside != INVALID__UICOLOR)
	{
		RECT rect = {m_rcItem.left + 1, m_rcItem.top + 1, m_rcItem.right - 1, m_rcItem.bottom - 1};
		CUIBlueRenderEngine::DoPaintBorder(hDC, m_pManager, m_rcItem, m_BorderColorOutside);
	}

	if (m_BorderColorInside != INVALID__UICOLOR)
	{
		RECT rect = {m_rcItem.left + 1, m_rcItem.top + 1, m_rcItem.right - 1, m_rcItem.bottom - 1};
		CUIBlueRenderEngine::DoPaintBorder(hDC, m_pManager, rect, m_BorderColorInside);
	}

	if (m_pImageOwner != NULL && m_nIndex >= 0)
	{
		RECT rc = m_rcItem;

		Graphics graph(hDC);

		int nStartX = rc.left;
		int nStartY = rc.top;

		int nImageWidth = m_pImageOwner->GetWidth() / m_nImageCount;
		int nImageHeight = m_pImageOwner->GetHeight();

		double dblScal = (double)nImageWidth / nImageHeight;

		int nRectWidth = nImageWidth;
		int nRectHeight = nImageHeight;

		if (m_sizeAnimate.cx != 0 && m_sizeAnimate.cy != 0)
		{
			nRectWidth = m_sizeAnimate.cx;
			nRectHeight = m_sizeAnimate.cy;
		}


		if ((m_rcItem.bottom - m_rcItem.top) * dblScal < (m_rcItem.right - m_rcItem.left))
		{
			nRectWidth = (int)(nRectHeight * dblScal);
		}
		else
		{
			nRectHeight = (int)(nRectWidth / dblScal);
		}


		nStartX = rc.left + (rc.right - rc.left - nRectWidth) / 2;
		if (nStartX < rc.left)
		{
			nStartX = rc.left;

			nRectWidth = rc.right - rc.left;
		}

		nStartY = rc.top + (rc.bottom - rc.top - nRectHeight) / 2;
		if (nStartY < rc.top)
		{
			nStartY = rc.top;

			nRectHeight = rc.bottom - rc.top;
		}


		RectF rfDest(PointF((REAL)nStartX, (REAL)nStartY), SizeF((REAL)nRectWidth, (REAL)nRectHeight));

		if (m_fAlpha <= 1.0)
		{
			ColorMatrix colorMatrix = {	1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 
				0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 
				0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 
				0.0f, 0.0f, 0.0f, m_fAlpha, 0.0f, 
				0.0f, 0.0f, 0.0f, 0.0f, 1.0f
			}; 

			ImageAttributes imageAtt; 
			imageAtt.SetColorMatrix(&colorMatrix, ColorMatrixFlagsDefault, ColorAdjustTypeBitmap); 
			graph.DrawImage(m_pImageOwner, rfDest, (REAL)nImageWidth * m_nIndex, (REAL)0, (REAL)nImageWidth, (REAL)nImageHeight, UnitPixel, &imageAtt);
		}
		else
		{
			graph.DrawImage(m_pImageOwner, rfDest, (REAL)nImageWidth * m_nIndex, (REAL)0, (REAL)nImageWidth, (REAL)nImageHeight, UnitPixel, NULL);
		}


		if (m_bImageAsLinker && m_sText.GetLength() == 0)
		{
			m_nLinks = 1;
			m_rcLinks[0] = CUIRect(nStartX, nStartY, nStartX + nRectWidth, nStartY + nRectHeight);
		}
	}
	
	if (m_sText.GetLength() > 0)
	{
		RECT rcText = m_rcItem;
		m_nLinks = lengthof(m_rcLinks);
		COLORREF color = m_TextColor;
		if (!IsEnabled())
		{
			color = m_TextColor_Disable;
		}

		CUIBlueRenderEngine::DoPaintPrettyText(hDC, m_pManager, rcText, m_sText, color, INVALID__UICOLOR, m_rcLinks, m_nLinks, m_uTextStyle);

		if (m_nLinks == 1 && m_sText.GetLength() > 4)
		{
			if ((m_sText.GetAt(0) == _T('<') && m_sText.GetAt(1) == _T('a')) && 
				(m_sText.GetAt(m_sText.GetLength() - 1) == _T('>') && m_sText.GetAt(m_sText.GetLength() - 2) == _T('a'))
				)
			{
				if (m_rcLinks[0].right - m_rcLinks[0].left < 20)
				{
					m_rcLinks[0].left = m_rcItem.left;
					m_rcLinks[0].right = m_rcItem.right;
				}
			}	
		}
	}
}

void CUILabel::Event(TEventUI& event)
{
	if( !CUIControl::Activate() ) 
		return;

	if( event.Type == UIEVENT_TIMER ) {
		if (m_nIndex == 0)
		{
			KillTimer(2006);
			SetTimer(2006, m_nInterval);
		}

		m_nIndex++;
		if (m_nIndex >= m_nImageCount)
		{
			m_nIndex = 0;
		}

		m_fAlpha = m_fAlpha + 0.2;
		if (m_fAlpha  > 1.0)
		{
			m_fAlpha = 1.0;
		}
		Invalidate();
	}

	if( event.Type == UIEVENT_SETCURSOR ) {
		::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));

		for( int i = 0; i < m_nLinks; i++ ) {
			if( ::PtInRect(&m_rcLinks[i], event.ptMouse) ) {
				::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND)));
				return;
			}
		}      
	}
	if( event.Type == UIEVENT_BUTTONDOWN && IsEnabled() ) {
		for( int i = 0; i < m_nLinks; i++ ) {
			if( ::PtInRect(&m_rcLinks[i], event.ptMouse) ) {
				m_uButtonState |= UISTATE_PUSHED;
				Invalidate();
				return;
			}
		}      
	}
	if( event.Type == UIEVENT_BUTTONUP ) {
		m_nCurLink = -1;
		for( int i = 0; i < m_nLinks; i++ ) {
			if( ::PtInRect(&m_rcLinks[i], event.ptMouse) )
			{
				m_nCurLink = i;
				Activate();
				break;
			}
		}      
		m_uButtonState &= ~UISTATE_PUSHED;
		Invalidate();
		return;
	}
	// When you move over a link
	if( m_nLinks > 0 && event.Type == UIEVENT_MOUSEMOVE ) Invalidate();
	if( m_nLinks > 0 && event.Type == UIEVENT_MOUSELEAVE ) Invalidate();

	if( event.Type == UIEVENT_CONTEXTMENU ) 
	{
		if( m_pManager != NULL )
		{
			m_pManager->SendNotify(this, UI_NOTIFY_MENU_CLICK);
		}
	}
	
}

RECT CUILabel::GetButtonRect(RECT rc) const
{
	int cy = m_pManager->GetThemeFontInfo(UIFONT_NORMAL).tmHeight + 8;
	return CUIRect(rc.left + 10, rc.top, rc.right, rc.top + cy);
}

void CUILabel::SetImage(Image*	pImage)
{
	if (m_pImageOwner)
	{
		delete m_pImageOwner;
		m_pImageOwner = NULL;
	}

	if (pImage)
	{
		m_pImageOwner = pImage->Clone();
		Invalidate();
	}

}

void CUILabel::SetImage( UINT nResourceID, HMODULE hResHandle/*=NULL*/ )
{
	if (m_pImageOwner)
	{
		delete m_pImageOwner;
		m_pImageOwner = NULL;
	}

	CUIPaintManager::SetImage(&m_pImageOwner, nResourceID, hResHandle);
	Invalidate();
}

void CUILabel::SetImage(LPCTSTR pstrName)
{
	if (m_pImageOwner)
	{
		delete m_pImageOwner;
		m_pImageOwner = NULL;
	}

	m_pImageOwner = Image::FromFile(pstrName);
	Invalidate();
}

bool CUILabel::Activate()
{
	if( !CUIControl::Activate() ) return false;
	if( m_nLinks > 0 ) m_pManager->SendNotify(this, UI_NOTIFY_LINK);
	return true;
}

void CUILabel::SetTextColor(COLORREF TextColor, COLORREF TextColor_Disable)
{
	m_TextColor = TextColor;
	if (TextColor_Disable != INVALID__UICOLOR)
	{
		m_TextColor_Disable = TextColor_Disable;
	}
	Invalidate();
}

void CUILabel::SetBkColor(COLORREF BackColor)
{
	m_BackColor = BackColor;
	Invalidate();
}

void CUILabel::SetBorderOutsideColor(COLORREF Color)
{
	m_BorderColorOutside = Color;
	Invalidate();
}

void CUILabel::SetBorderInsideColor(COLORREF Color)
{
	m_BorderColorInside = Color;
	Invalidate();
}

void CUILabel::Play(int nDalayTime)
{
	m_nDelayTime = nDalayTime;
	m_fAlpha = 1.0;
	m_nIndex = -1;

	KillTimer(2006);
	if (m_nDelayTime > 0)
	{
		SetTimer(2006, m_nDelayTime);
	}
	else
	{
		SetTimer(2006, m_nInterval);
	}
	
}

void CUILabel::Pause(void)
{
	KillTimer(2006);
}

void CUILabel::Reset(void)
{
	m_nIndex = 0;
}

void CUILabel::SetInterval(int interval)
{
	m_nInterval = interval;
}

void CUILabel::SetImageCount(int nCount)
{
	m_nImageCount = nCount;
}

void CUILabel::SetImageSize(SIZE size)
{
	m_sizeAnimate = size;
}

void CUILabel::SetImageAsLinker(bool bLink)
{
	m_bImageAsLinker = bLink;
}

CStdString CUILabel::GetLinkText()
{
	if (m_nCurLink == -1)
	{
		return _T("");
	}

	CStdString strText = GetText();

	int nStartPos = 0;
	int nFindIdx = -1;
	while (nFindIdx < m_nCurLink)
	{
		nStartPos = strText.Find(_T("<a>"), nStartPos);

		if (nStartPos >= 0)
		{
			nFindIdx++;
		}
		else
		{
			break;
		}

		nStartPos += 3;
	}

	if (nFindIdx == m_nCurLink)
	{
		int nEndPos = strText.Find(_T("</a>"), nStartPos);
		if (nEndPos == -1)
		{
			nEndPos = strText.GetLength();
		}

		CStdString strLinker = strText.Mid(nStartPos, nEndPos - nStartPos);

		return strLinker;
	}

	return _T("");
	
}


///////////////////////////////////////////////////////////////////////
CUILabelLine::CUILabelLine() : m_LineColorTop(INVALID__UICOLOR),m_LineColorBottom(INVALID__UICOLOR)
{
}

LPCTSTR CUILabelLine::GetClass() const
{
	return _T("LabelLinelUI");
}

void CUILabelLine::DoPaint(HDC hDC, const RECT& rcPaint)
{
	if (m_LineColorTop != INVALID__UICOLOR)
	{
		RECT rect = {m_rcItem.left, m_rcItem.top, m_rcItem.right , m_rcItem.top + 1};
		CUIBlueRenderEngine::DoFillRect(hDC, m_pManager, rect, m_LineColorTop);
	}

	if (m_LineColorBottom != INVALID__UICOLOR)
	{
		RECT rect = {m_rcItem.left, m_rcItem.top + 1, m_rcItem.right , m_rcItem.top + 2};
		CUIBlueRenderEngine::DoFillRect(hDC, m_pManager, rect, m_LineColorBottom);
	}
}

SIZE CUILabelLine::EstimateSize(SIZE /*szAvailable*/)
{
	return CUISize(m_rcItem.right - m_rcItem.left, 2);
}

void CUILabelLine::SetLineTopColor(COLORREF Color)
{
	m_LineColorTop = Color;
	Invalidate();
}

void CUILabelLine::SetLineBottomColor(COLORREF Color)
{
	m_LineColorBottom = Color;
	Invalidate();
}