
#include "StdAfx.h"
#include "UIProgressBar.h"


/////////////////////////////////////////////////////////////////////////////////////
//
//

CUIProgressBar::CUIProgressBar() : m_pImageOwner(NULL),m_uPos(0), m_cxWidth(0), m_uTextStyle(DT_VCENTER | DT_CENTER)
{
	m_nPosMax = 100;
	m_nPosMin = 0;

	m_bDrawBackGround = false;
}

CUIProgressBar:: ~CUIProgressBar()
{
	if (m_pImageOwner)
	{
		delete m_pImageOwner;
		m_pImageOwner = NULL;
	}
}

LPCTSTR CUIProgressBar::GetClass() const
{
   return _T("ProgressBarUI");
}

SIZE CUIProgressBar::EstimateSize(SIZE /*szAvailable*/)
{
	return CUISize(m_cxWidth, m_pManager->GetThemeFontInfo(UIFONT_NORMAL).tmHeight + 4);
}

void CUIProgressBar::SetText(LPCTSTR pstrText)
{
	// Automatic assignment of keyboard shortcut
	if( _tcschr(pstrText, '&') != NULL ) m_chShortcut = *(_tcschr(pstrText, '&') + 1);
	CUIControl::SetText(pstrText);
}

void CUIProgressBar::SetProgressPos(UINT uPos)
{
   // Automatic assignment of keyboard shortcut
	if (uPos< m_nPosMin)
	{
		uPos = m_nPosMin;
	}
	if (uPos > m_nPosMax)
	{
		uPos = m_nPosMax;
	}
	m_uPos = uPos;
	Invalidate();
}

void CUIProgressBar::SetProgressImage(Image*	pImage)
{
	if (pImage)
	{
		m_pImageOwner = pImage->Clone();
	}
	Invalidate();
}

void CUIProgressBar::SetProgressImage(UINT nResourceID)
{
	CUIPaintManager::SetImage(&m_pImageOwner, nResourceID);
	Invalidate();
}
void CUIProgressBar::SetProgressImage(LPCTSTR pstrName)
{
	m_pImageOwner = Image::FromFile(pstrName);
	Invalidate();
}

void CUIProgressBar::SetWidth(int cx)
{
	m_cxWidth = cx;
	UpdateLayout();
}


void CUIProgressBar::DoPaint(HDC hDC, const RECT& rcPaint)
{
	Graphics graph(hDC);
	if (m_pImageOwner != NULL)
	{
		if (m_pManager != NULL && (m_pManager->m_pImageSkinNormal == NULL || m_bDrawBackGround == true))
		{
			int nWidth = m_pImageOwner->GetWidth() / 2;
			int nHeight = m_pImageOwner->GetHeight();

			{
				RECT rcImage ={nWidth *0, 0, nWidth, nHeight};
				CUIBlueRenderEngine::DoFillRect(hDC, m_pManager, m_rcItem, rcImage, m_pImageOwner);
			}


			if (m_nPosMax > m_nPosMin)
			{
				int nPosWidth = (int)((m_rcItem.right - m_rcItem.left) * ((double)m_uPos / (double)(m_nPosMax - m_nPosMin)) + 0.5);
				if (nPosWidth >= (m_rcItem.right - m_rcItem.left))
				{
					nPosWidth = m_rcItem.right - m_rcItem.left;
				}

				RECT rectTemp = m_rcItem;
				rectTemp.right = rectTemp.left + nPosWidth;

				if (nPosWidth > nWidth)
				{
					nPosWidth = nWidth;
				}

				RECT rcImage ={nWidth *1, 0, nWidth + nPosWidth, nHeight};

				if (m_uPos == m_nPosMin)
				{
					rcImage.right = nWidth;
				}
				if (m_uPos == m_nPosMax)
				{
					rcImage.right = nWidth * 2;
				}

				CUIBlueRenderEngine::DoFillRect(hDC, m_pManager, rectTemp, rcImage, m_pImageOwner);
			}

		}
		else
		{
			int nWidth = m_pImageOwner->GetWidth();
			int nHeight = m_pImageOwner->GetHeight();

			if (m_nPosMax > m_nPosMin)
			{
				nWidth = (int)(nWidth * ((double)m_uPos / (double)(m_nPosMax - m_nPosMin)));
				RectF rfDest(PointF((REAL)m_rcItem.left, (REAL)m_rcItem.top), SizeF((REAL)nWidth, (REAL)nHeight));
				graph.DrawImage(m_pImageOwner, rfDest, (REAL)0, (REAL)0, (REAL)nWidth, (REAL)nHeight, UnitPixel, NULL);
			}
		}

	}

   if (m_sText.GetLength() > 0)
   {
	   RECT rcText = m_rcItem;
	   int nLinks = 0;
	   CUIBlueRenderEngine::DoPaintPrettyText(hDC, m_pManager, rcText, m_sText, UICOLOR_CONTROL_TEXT_NORMAL, UICOLOR__INVALID, NULL, nLinks, DT_SINGLELINE | m_uTextStyle);
   }
}

void CUIProgressBar::GetRange(int& nMin, int& nMax ) const
{
	nMin = m_nPosMin;
	nMax = m_nPosMax;
}
void CUIProgressBar::SetRange(int nMin, int nMax, bool bRedraw)
{
	m_nPosMin = nMin;
	m_nPosMax = nMax;

	if (bRedraw)
	{
		Invalidate();
	}
}

void CUIProgressBar::SetDrawBackGround(bool bDraw)
{
	m_bDrawBackGround = bDraw;
}


