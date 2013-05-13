
#include "StdAfx.h"
#include "UIBlue.h"


/////////////////////////////////////////////////////////////////////////////////////
//
//

#ifndef BlendRGB
   #define BlendRGB(c1, c2, factor) \
      RGB( GetRValue(c1) + ((GetRValue(c2) - GetRValue(c1)) * factor / 100L), \
           GetGValue(c1) + ((GetGValue(c2) - GetGValue(c1)) * factor / 100L), \
           GetBValue(c1) + ((GetBValue(c2) - GetBValue(c1)) * factor / 100L) )
#endif


/////////////////////////////////////////////////////////////////////////////////////
//
//

CRenderClip::~CRenderClip()
{
   ASSERT(::GetObjectType(hDC)==OBJ_DC || ::GetObjectType(hDC)==OBJ_MEMDC);
   ASSERT(::GetObjectType(hRgn)==OBJ_REGION);
   ASSERT(::GetObjectType(hOldRgn)==OBJ_REGION);
   ::SelectClipRgn(hDC, hOldRgn);
   ::DeleteObject(hOldRgn);
   ::DeleteObject(hRgn);
}


/////////////////////////////////////////////////////////////////////////////////////
//
//

void CUIBlueRenderEngine::GenerateClip(HDC hDC, RECT rcItem, CRenderClip& clip)
{
   RECT rcClip = { 0 };
   ::GetClipBox(hDC, &rcClip);
   clip.hOldRgn = ::CreateRectRgnIndirect(&rcClip);
   clip.hRgn = ::CreateRectRgnIndirect(&rcItem);
   ::ExtSelectClipRgn(hDC, clip.hRgn, RGN_AND);
   clip.hDC = hDC;
   clip.rcItem = rcItem;
}

void CUIBlueRenderEngine::DoFillRect(HDC hDC, CUIPaintManager* pManager, RECT rcItem, UITYPE_COLOR Color)
{
   DoFillRect(hDC, pManager, rcItem, pManager->GetThemeColor(Color));
}

void CUIBlueRenderEngine::DoFillRect(HDC hDC, CUIPaintManager* pManager, RECT rcItem, COLORREF clrFill)
{
   ASSERT(::GetObjectType(hDC)==OBJ_DC || ::GetObjectType(hDC)==OBJ_MEMDC);
   ::SetBkColor(hDC, clrFill);
   ::ExtTextOut(hDC, 0, 0, ETO_OPAQUE, &rcItem, NULL, 0, NULL);
}

void CUIBlueRenderEngine::DoFillRect(HDC hDC, CUIPaintManager* pManager, RECT rc, RECT rcImage, Image* pImage, bool bBrush)
{
	Graphics graph(hDC);

	if (pImage == NULL)
	{
		return;
	}

	int nWidth = (rcImage.right - rcImage.left) / 3;
	int nHeight = (rcImage.bottom - rcImage.top) / 3;


	for (int y = 0; y < 3; y++)
	{
		for (int x = 0; x < 3; x++)
		{
			RectF rfDest;
			RectF rfScr;

			if (x == 2)
			{
				rfDest.X = (REAL)(rc.right - nWidth);
				rfScr.X =  (REAL)(rcImage.right - nWidth);
			}
			else
			{
				rfDest.X = (REAL)(rc.left + nWidth * x);
				rfScr.X = (REAL)(rcImage.left + nWidth * x);
			}

			if (y == 2)
			{
				rfDest.Y = (REAL)(rc.bottom - nHeight);

				rfScr.Y = (REAL)(rcImage.bottom - nHeight);
			}
			else
			{
				rfDest.Y = (REAL)(rc.top + nHeight * y);

				rfScr.Y = (REAL)(rcImage.top + nHeight * y);
			}

			if (x == 1)
			{
				rfDest.Width = (REAL)(rc.right - rc.left - nWidth * 2);

				rfScr.Width = (REAL)(rcImage.right - rcImage.left - nWidth * 2);
			}
			else
			{
				rfDest.Width = (REAL)(nWidth);

				rfScr.Width = (REAL)(nWidth);
			}

			if (y == 1)
			{
				rfDest.Height = (REAL)(rc.bottom - rc.top - nHeight * 2);

				rfScr.Height = (REAL)(rcImage.bottom - rcImage.top - nHeight * 2);
			}
			else
			{
				rfDest.Height = (REAL)(nHeight);

				rfScr.Height = (REAL)(nHeight);
			}

			if (bBrush)
			{
				TextureBrush brush(pImage, WrapModeTile, rfScr);
				brush.TranslateTransform( rfDest.X , rfDest.Y ); 
				graph.FillRectangle(&brush, rfDest);
			}
			else
			{
					
				// 不用刷子，直接绘图
				ImageAttributes ImgAtt;
				ImgAtt.SetWrapMode(WrapModeTileFlipXY);

				graph.DrawImage(pImage, rfDest, rfScr.X , rfScr.Y, rfScr.Width, rfScr.Height, UnitPixel, &ImgAtt);
			}
			


		}
	}
}

void CUIBlueRenderEngine::DoPaintLine(HDC hDC, CUIPaintManager* pManager, RECT rcItem, UITYPE_COLOR Color)
{
   ASSERT(::GetObjectType(hDC)==OBJ_DC || ::GetObjectType(hDC)==OBJ_MEMDC);
   POINT ptTemp = { 0 };
   ::SelectObject(hDC, pManager->GetThemePen(Color));
   ::MoveToEx(hDC, rcItem.left, rcItem.top, &ptTemp);
   ::LineTo(hDC, rcItem.right, rcItem.bottom);
}

void CUIBlueRenderEngine::DoPaintLine(HDC hDC, CUIPaintManager* pManager, RECT rcItem, COLORREF Color)
{
	ASSERT(::GetObjectType(hDC)==OBJ_DC || ::GetObjectType(hDC)==OBJ_MEMDC);
	POINT ptTemp = { 0 };

	HPEN hPen = ::CreatePen(PS_SOLID, 1, Color);

	HPEN hOldPen = (HPEN)::SelectObject(hDC, hPen);
	::MoveToEx(hDC, rcItem.left, rcItem.top, &ptTemp);
	::LineTo(hDC, rcItem.right, rcItem.bottom);

	if (hOldPen)
	{
		::SelectObject(hDC, hOldPen);
	}
	::DeleteObject(hPen);
}

void CUIBlueRenderEngine::DoPaintLine(HDC hDC, CUIPaintManager* pManager, POINT pt1, POINT pt2, COLORREF Color)
{
	ASSERT(::GetObjectType(hDC)==OBJ_DC || ::GetObjectType(hDC)==OBJ_MEMDC);
	POINT ptTemp = { 0 };

	HPEN hPen = ::CreatePen(PS_SOLID, 1, Color);

	HPEN hOldPen = (HPEN)::SelectObject(hDC, hPen);
	::MoveToEx(hDC, pt1.x, pt1.y, &ptTemp);
	::LineTo(hDC, pt2.x, pt2.y);

	if (hOldPen)
	{
		::SelectObject(hDC, hOldPen);
	}
	::DeleteObject(hPen);

}

void CUIBlueRenderEngine::DoPaintRectangle(HDC hDC, CUIPaintManager* pManager, RECT rcItem, UITYPE_COLOR Border, UITYPE_COLOR Fill)
{
   ASSERT(::GetObjectType(hDC)==OBJ_DC || ::GetObjectType(hDC)==OBJ_MEMDC);
   ::SelectObject(hDC, pManager->GetThemePen(Border));
   ::SelectObject(hDC, Fill == UICOLOR__INVALID ? ::GetStockObject(HOLLOW_BRUSH) : pManager->GetThemeBrush(Fill));
   ::Rectangle(hDC, rcItem.left, rcItem.top, rcItem.right, rcItem.bottom);
}

void CUIBlueRenderEngine::DoPaintFrame(HDC hDC, CUIPaintManager* pManager, RECT rcItem, UITYPE_COLOR Light, UITYPE_COLOR Dark, UITYPE_COLOR Background, UINT uStyle)
{
   if( Background != UICOLOR__INVALID ) {
      DoFillRect(hDC, pManager, rcItem, Background);
   }
   if( (uStyle & UIFRAME_ROUND) != 0 )
   {
      POINT ptTemp;
      ::SelectObject(hDC, pManager->GetThemePen(Light));
      ::MoveToEx(hDC, rcItem.left, rcItem.bottom - 2, &ptTemp);
      ::LineTo(hDC, rcItem.left, rcItem.top + 1);
      ::LineTo(hDC, rcItem.left + 1, rcItem.top);
      ::LineTo(hDC, rcItem.right - 2, rcItem.top);
      ::SelectObject(hDC, pManager->GetThemePen(Dark));
      ::LineTo(hDC, rcItem.right - 1, rcItem.top + 1);
      ::LineTo(hDC, rcItem.right - 1, rcItem.bottom - 2);
      ::LineTo(hDC, rcItem.right - 2, rcItem.bottom - 1);
      ::LineTo(hDC, rcItem.left, rcItem.bottom - 1);
   }
   if( (uStyle & UIFRAME_FOCUS) != 0 )
   {
      HPEN hPen = ::CreatePen(PS_DOT, 1, pManager->GetThemeColor(UICOLOR_BUTTON_BORDER_FOCUS));
      HPEN hOldPen = (HPEN) ::SelectObject(hDC, hPen);
      POINT ptTemp;
      ::MoveToEx(hDC, rcItem.left, rcItem.bottom - 1, &ptTemp);
      ::LineTo(hDC, rcItem.left, rcItem.top);
      ::LineTo(hDC, rcItem.right - 1, rcItem.top);
      ::LineTo(hDC, rcItem.right - 1, rcItem.bottom - 1);
      ::LineTo(hDC, rcItem.left, rcItem.bottom - 1);
      ::SelectObject(hDC, hOldPen);
      ::DeleteObject(hPen);
   }
   else
   {
      POINT ptTemp;
      ::SelectObject(hDC, pManager->GetThemePen(Light));
      ::MoveToEx(hDC, rcItem.left, rcItem.top, &ptTemp);
      ::LineTo(hDC, rcItem.right - 1, rcItem.top);
      ::SelectObject(hDC, pManager->GetThemePen(Dark));
      ::LineTo(hDC, rcItem.right - 1, rcItem.bottom - 1);
      ::LineTo(hDC, rcItem.left, rcItem.bottom - 1);
	  ::LineTo(hDC, rcItem.left, rcItem.top);
   }
}

void CUIBlueRenderEngine::DoPaintBorder(HDC hDC, CUIPaintManager* pManager, RECT rcItem, UITYPE_COLOR Color)
{
	
	POINT ptTemp;
	::SelectObject(hDC, pManager->GetThemePen(Color));
	::MoveToEx(hDC, rcItem.left, rcItem.bottom - 1, &ptTemp);
	::LineTo(hDC, rcItem.left, rcItem.top);
	::LineTo(hDC, rcItem.right - 1, rcItem.top);
	::LineTo(hDC, rcItem.right - 1, rcItem.bottom - 1);
	::LineTo(hDC, rcItem.left, rcItem.bottom - 1);
	
}

void CUIBlueRenderEngine::DoPaintBorder(HDC hDC, CUIPaintManager* pManager, RECT rcItem, COLORREF Color)
{

	POINT ptTemp;
	HPEN pen = ::CreatePen(PS_SOLID, 1, Color);
	::SelectObject(hDC, pen);
	::MoveToEx(hDC, rcItem.left, rcItem.bottom - 1, &ptTemp);
	::LineTo(hDC, rcItem.left, rcItem.top);
	::LineTo(hDC, rcItem.right - 1, rcItem.top);
	::LineTo(hDC, rcItem.right - 1, rcItem.bottom - 1);
	::LineTo(hDC, rcItem.left, rcItem.bottom - 1);
	
}


void CUIBlueRenderEngine::DoPaintButton(HDC hDC, CUIPaintManager* pManager, RECT rc, UINT uState, POINT* pt, int nCount)
{
	ASSERT(::GetObjectType(hDC)==OBJ_DC || ::GetObjectType(hDC)==OBJ_MEMDC);
	// Draw focus rectangle
#if 0
	if( ((uState & UISTATE_FOCUSED) != 0) && pManager->GetSystemSettings().bShowKeyboardCues ) {
	   CUIBlueRenderEngine::DoPaintFrame(hDC, pManager, rc, UICOLOR_BUTTON_BORDER_FOCUS, UICOLOR_BUTTON_BORDER_FOCUS, UICOLOR__INVALID, UIFRAME_ROUND);
	   ::InflateRect(&rc, -1, -1);
	}
#endif

	Graphics graph(hDC);
	TextureBrush* pBrush = NULL;
	Bitmap* pImage = NULL;
	Bitmap* pImageBtn = NULL;

	// Draw frame and body
	if( (uState & UISTATE_DISABLED) != 0 ) {
		pImage = dynamic_cast<Bitmap*>(pManager->m_pImageSkinDisabled);
	}
	else if( (uState & UISTATE_PUSHED) != 0 ) {
		pImage = dynamic_cast<Bitmap*>(pManager->m_pImageSkinPushed);
	}
	else if( (uState & UISTATE_HOT) != 0 ) {
	   pImage = dynamic_cast<Bitmap*>(pManager->m_pImageSkinFocused);
	}
	else {
		pImage = dynamic_cast<Bitmap*>(pManager->m_pImageSkinNormal);
	}

	if (pImage)
	{
		pImageBtn = pImage->Clone((REAL)rc.left, (REAL)rc.top, (REAL)(rc.right - rc.left), (REAL)(rc.bottom - rc.top), pImage->GetPixelFormat());

		if (pImageBtn)
		{
			RectF rcTemp((REAL)0, (REAL)0, (REAL)pImageBtn->GetWidth(), (REAL)pImageBtn->GetHeight());

			pBrush = new TextureBrush(pImageBtn, WrapModeTile, rcTemp);

			delete pImageBtn;
		}

	}

	if (pBrush)
	{
		Point destPoints[20];
		int nPointCount = 0;

		if (pt == NULL)
		{
			destPoints[0] = Point(rc.left, rc.top);
			destPoints[1] = Point(rc.right, rc.top);
			destPoints[2] = Point(rc.right, rc.bottom);
			destPoints[3] = Point(rc.left, rc.bottom);

			nPointCount = 4;
		}
		else
		{
			for (int i = 0; i < nCount; i++)
			{
				destPoints[i] = Point(pt[i].x, pt[i].y);
			}

			nPointCount = nCount;
		}


		RectF rfScr;
		rfScr.X = (REAL)rc.left;
		rfScr.Y = (REAL)rc.top;

		rfScr.Width = (REAL)(rc.right - rc.left) ;
		rfScr.Height = (REAL)(rc.bottom - rc.top);

		GraphicsPath myGraphicsPath;
		myGraphicsPath.AddPolygon(destPoints, nPointCount); 

		pBrush->TranslateTransform(rc.left, rc.top); 

		graph.FillPath(pBrush, &myGraphicsPath);


		delete pBrush;
	}
	else
	{
		// Draw frame and body
		COLORREF clrColor1, clrColor2;
		UITYPE_COLOR clrBorder1, clrBorder2, clrBack;
		if( (uState & UISTATE_DISABLED) != 0 ) {
		   clrBorder1 = UICOLOR_BUTTON_BORDER_DISABLED;
		   clrBorder2 = UICOLOR_BUTTON_BORDER_DISABLED;
		   clrBack = UICOLOR_BUTTON_BACKGROUND_DISABLED;
		}
		else if( (uState & UISTATE_PUSHED) != 0 ) {
		   clrBorder1 = UICOLOR_BUTTON_BORDER_DARK;
		   clrBorder2 = UICOLOR_BUTTON_BORDER_LIGHT;
		   clrBack = UICOLOR_BUTTON_BACKGROUND_PUSHED;
		}
		else {
		   clrBorder1 = UICOLOR_BUTTON_BORDER_LIGHT;
		   clrBorder2 = UICOLOR_BUTTON_BORDER_DARK;
		   clrBack = UICOLOR_BUTTON_BACKGROUND_NORMAL;
		}
		// Draw button
		DoPaintFrame(hDC, pManager, rc, clrBorder1, clrBorder2, UICOLOR__INVALID, UIFRAME_ROUND);
		::InflateRect(&rc, -1, -1);
		// The pushed button has an inner light shade
		if( (uState & UISTATE_PUSHED) != 0 ) {
		   DoPaintFrame(hDC, pManager, rc, UICOLOR_STANDARD_LIGHTGREY, UICOLOR_STANDARD_LIGHTGREY, UICOLOR__INVALID);
		   rc.top += 1;
		   rc.left += 1;
		}
		// Gradient background
		pManager->GetThemeColorPair(clrBack, clrColor1, clrColor2);
		DoPaintGradient(hDC, pManager, rc, clrColor1, clrColor2, true, 32);
		// Draw hightlight inside button
		::SelectObject(hDC, pManager->GetThemePen(UICOLOR_DIALOG_BACKGROUND));
		POINT ptTemp;
		::MoveToEx(hDC, rc.left, rc.top, &ptTemp);
		::LineTo(hDC, rc.left, rc.bottom - 1);
		::LineTo(hDC, rc.right - 2, rc.bottom - 1);
		::LineTo(hDC, rc.right - 2, rc.top);
	}
}
void CUIBlueRenderEngine::DoPaintEditBox( HDC hDC, CUIPaintManager* pManager, RECT rcItem, LPCTSTR pstrText, UINT uState, UINT uDrawStyle,UINT uBorderWidth, Image* pImage /*= NULL*/ )
{
	ASSERT(::GetObjectType(hDC)==OBJ_DC || ::GetObjectType(hDC)==OBJ_MEMDC);
	if(pImage != NULL)
	{
		RECT rcImage = {0};
		rcImage.right=pImage->GetWidth();
		rcImage.bottom=pImage->GetHeight();
		CUIBlueRenderEngine::DoFillRect(hDC, pManager, rcItem, rcImage, pImage, false);
	}
	else
	{
		if(uBorderWidth > 0) 
		{
			if( (uState & UISTATE_DISABLED) != 0 ) {
				DoPaintFrame(hDC, pManager, rcItem, UICOLOR_EDIT_BORDER_DISABLED, UICOLOR_EDIT_BORDER_DISABLED_DARK, UICOLOR_EDIT_BACKGROUND_DISABLED);
			}
			else if( (uState & UISTATE_READONLY) != 0 ) {
				DoPaintFrame(hDC, pManager, rcItem, UICOLOR_EDIT_BORDER_DISABLED, UICOLOR_EDIT_BORDER_DISABLED_DARK, UICOLOR_EDIT_BACKGROUND_NORMAL);
			}
			else if( (uState & UISTATE_FOCUSED) != 0 ) {
				DoPaintFrame(hDC, pManager, rcItem, UICOLOR_EDIT_BORDER_SELECTED, UICOLOR_EDIT_BORDER_SELECTED_DARK, UICOLOR_EDIT_BACKGROUND_NORMAL);
			}
			else {
				DoPaintFrame(hDC, pManager, rcItem, UICOLOR_EDIT_BORDER_NORMAL, UICOLOR_EDIT_BORDER_NORMAL_DARK, UICOLOR_EDIT_BACKGROUND_NORMAL);
			}
		}
	}
	// We should also draw the actual text
	COLORREF clrText = pManager->GetThemeColor(UICOLOR_EDIT_TEXT_NORMAL);
	if( (uState & UISTATE_READONLY) != 0 ) clrText = pManager->GetThemeColor(UICOLOR_EDIT_TEXT_READONLY);
	if( (uState & UISTATE_DISABLED) != 0 ) clrText = pManager->GetThemeColor(UICOLOR_EDIT_TEXT_DISABLED);
	::SetBkMode(hDC, TRANSPARENT);
	::SetTextColor(hDC, clrText);
	::SelectObject(hDC, pManager->GetThemeFont(UIFONT_NORMAL));
	RECT rcEdit = rcItem;
	if(pImage != NULL)
	{
		::InflateRect(&rcEdit, 0-uBorderWidth, 0-uBorderWidth);
	}
	else
	{
		::InflateRect(&rcEdit, -3, -2);
	}
	if((uDrawStyle & ES_MULTILINE) == 0)
	{
		::DrawText(hDC, pstrText, -1, &rcEdit, DT_SINGLELINE|DT_VCENTER|DT_NOPREFIX|DT_EDITCONTROL|uDrawStyle);
	}
	else
	{
		::DrawText(hDC, pstrText, -1, &rcEdit,DT_NOPREFIX|DT_TOP|DT_EDITCONTROL|uDrawStyle);
	}
}

void CUIBlueRenderEngine::DoPaintOptionBox(HDC hDC, CUIPaintManager* pManager, RECT rcItem, LPCTSTR pstrText, UINT uState, UINT uStyle, BUTTON_TYPE uType)
{
   ASSERT(::GetObjectType(hDC)==OBJ_DC || ::GetObjectType(hDC)==OBJ_MEMDC);
   // Determine placement of elements
   RECT rcText = rcItem;
   RECT rcButton = rcItem;
   if( (uStyle & DT_RIGHT) != 0 )
   {
      rcText.right -= 18;
      rcButton.left = rcButton.right - 18;
   }
   else
   {
      rcText.left += 18;
      rcButton.right = rcButton.left + 18;
   }
   bool bSelected = (uState & UISTATE_CHECKED) != 0;

   Graphics graph(hDC);
   Image *pImage = NULL;
   if (uType == BUTTON_TYPE_RADIOBOX)
   {
	   if (pManager != NULL) pImage = pManager->m_pImageSkinRadiobox;
   }
   else
   {
	   if (pManager != NULL) pImage = pManager->m_pImageSkinCheckbox;
   }

   if (pImage)
   {
	   RECT rcImage = {0};
	   // Draw frame and body
	   int nWidth = pImage->GetWidth() / 4;
	   int nHeight = pImage->GetHeight() / 2;

	   if( (uState & UISTATE_DISABLED) != 0 )
	   {
		   if (!bSelected)
		   {
			   RECT rcTemp = {nWidth *3, 0, nWidth * 4, nHeight};
			   rcImage = rcTemp;
		   }
		   else
		   {
			   RECT rcTemp = {nWidth *3, nHeight, nWidth * 4, nHeight * 2};
			   rcImage = rcTemp;
		   }

	   }
	   else if( (uState & UISTATE_PUSHED) != 0 )
	   {
		   if (!bSelected)
		   {
			   RECT rcTemp ={nWidth *2, 0, nWidth * 3, nHeight};
			   rcImage = rcTemp;
		   }
		   else
		   {
			   RECT rcTemp ={nWidth *2, nHeight, nWidth * 3, nHeight * 2};
			   rcImage = rcTemp;
		   }
	   }
	   else if( (uState & UISTATE_HOT) != 0 )
	   {
		   if (!bSelected)
		   {
			   RECT rcTemp ={nWidth *1, 0, nWidth * 2, nHeight};
			   rcImage = rcTemp;
		   }
		   else
		   {
			   RECT rcTemp ={nWidth *1, nHeight, nWidth * 2, nHeight * 2};
			   rcImage = rcTemp;
		   }

	   }
	   else 
	   {
		   if (!bSelected)
		   {
			   RECT rcTemp ={nWidth *0, 0, nWidth * 1, nHeight};
			   rcImage = rcTemp;
		   }
		   else
		   {
			   RECT rcTemp ={nWidth *0, nHeight, nWidth * 1, nHeight * 2};
			   rcImage = rcTemp;
		   }

	   }

	   int nStartY = rcButton.top + ((rcButton.bottom - rcButton.top) - (rcImage.bottom - rcImage.top)) / 2;
	   if (uType != BUTTON_TYPE_RADIOBOX)
	   {
		   nStartY+=1;
	   }

	   RectF rfDest(PointF((REAL)rcButton.left, (REAL)nStartY), SizeF((REAL)(rcImage.right - rcImage.left), (REAL)(rcImage.bottom - rcImage.top)));

	   if (pImage != NULL)
	   {
		   graph.DrawImage(pImage, rfDest, (REAL)rcImage.left, (REAL)rcImage.top, (REAL)(rcImage.right - rcImage.left), (REAL)(rcImage.bottom - rcImage.top), UnitPixel, NULL);
	   }
   }

   // Paint text
   UITYPE_COLOR iTextColor = ((uState & UISTATE_DISABLED) != 0) ? UICOLOR_EDIT_TEXT_DISABLED : UICOLOR_EDIT_TEXT_NORMAL;
   int nLinks = 0;
   CUIBlueRenderEngine::DoPaintPrettyText(hDC, pManager, rcText, pstrText, iTextColor, UICOLOR__INVALID, NULL, nLinks, DT_SINGLELINE | DT_VCENTER);
   // Paint focus rectangle
   if( ((uState & UISTATE_FOCUSED) != 0) && pManager->GetSystemSettings().bShowKeyboardCues )
   {
      RECT rcFocus = { 0, 0, 9999, 9999 };;
      int nLinks = 0;
      CUIBlueRenderEngine::DoPaintPrettyText(hDC, pManager, rcFocus, pstrText, iTextColor, UICOLOR__INVALID, NULL, nLinks, DT_SINGLELINE | DT_CALCRECT);
      rcText.right = rcText.left + (rcFocus.right - rcFocus.left);
      rcText.bottom = rcText.top + (rcFocus.bottom - rcFocus.top);
      ::InflateRect(&rcText, 2, 0);
      //CUIBlueRenderEngine::DoPaintFrame(hDC, pManager, rcText, UICOLOR_STANDARD_BLACK, UICOLOR_STANDARD_BLACK, UICOLOR__INVALID, UIFRAME_FOCUS);
   }
}

void CUIBlueRenderEngine::DoPaintPrettyText(HDC hDC, CUIPaintManager* pManager, RECT& rc, LPCTSTR pstrText, COLORREF iTextColor, COLORREF iBackColor, RECT* prcLinks, int& nLinkRects, UINT uStyle)
{
	ASSERT(::GetObjectType(hDC)==OBJ_DC || ::GetObjectType(hDC)==OBJ_MEMDC);

	// The string formatter supports a kind of "mini-html" that consists of various short tags:
	//
	//   Link:             <a>text</a>
	//   Bold:             <b>text</b>
	//   Icon:             <i x y>      where x = icon type and  y = icon id
	//   Color:            <c #xxxxxx>  where x = RGB in hex


	if( ::IsRectEmpty(&rc) ) return;

	bool bDraw = (uStyle & DT_CALCRECT) == 0;

	RECT rcClip = { 0 };
	::GetClipBox(hDC, &rcClip);
	HRGN hOldRgn = ::CreateRectRgnIndirect(&rcClip);
	HRGN hRgn = ::CreateRectRgnIndirect(&rc);
	if( bDraw ) ::ExtSelectClipRgn(hDC, hRgn, RGN_AND);

	CStdString sText = pstrText;
	sText.ProcessResourceTokens();
	pstrText = sText;

	HFONT hOldFont = (HFONT) ::SelectObject(hDC, pManager->GetThemeFont(UIFONT_NORMAL));
	::SetBkMode(hDC, TRANSPARENT);
	::SetTextColor(hDC, iTextColor);

	// If the drawstyle includes an alignment, we'll need to first determine the text-size so
	// we can draw it at the correct position...
	if( (uStyle & DT_SINGLELINE) != 0 && (uStyle & DT_VCENTER) != 0 && (uStyle & DT_CALCRECT) == 0 ) {
		RECT rcText = { 0, 0, 9999, 100 };
		int nLinks = 0;
		DoPaintPrettyText(hDC, pManager, rcText, pstrText, iTextColor, iBackColor, NULL, nLinks, uStyle | DT_CALCRECT);
		rc.top = rc.top + ((rc.bottom - rc.top) / 2) - ((rcText.bottom - rcText.top) / 2);
		rc.bottom = rc.top + (rcText.bottom - rcText.top);
	}
	if( (uStyle & DT_SINGLELINE) != 0 && (uStyle & DT_CENTER) != 0 && (uStyle & DT_CALCRECT) == 0 ) {
		RECT rcText = { 0, 0, 9999, 100 };
		int nLinks = 0;
		DoPaintPrettyText(hDC, pManager, rcText, pstrText, iTextColor, iBackColor, NULL, nLinks, uStyle | DT_CALCRECT);
		::OffsetRect(&rc, (rc.right - rc.left) / 2 - (rcText.right - rcText.left) / 2, 0);
	}
	if( (uStyle & DT_SINGLELINE) != 0 && (uStyle & DT_RIGHT) != 0 && (uStyle & DT_CALCRECT) == 0 ) {
		RECT rcText = { 0, 0, 9999, 100 };
		int nLinks = 0;
		DoPaintPrettyText(hDC, pManager, rcText, pstrText, iTextColor, iBackColor, NULL, nLinks, uStyle | DT_CALCRECT);
		rc.left = rc.right - (rcText.right - rcText.left);
	}

	// Paint backhground
	if( iBackColor != INVALID__UICOLOR ) DoFillRect(hDC, pManager, rc, iBackColor);

	// Determine if we're hovering over a link, because we would like to
	// indicate it by coloring the link text.
	// BUG: This assumes that the prcLink has already been filled once with
	//      link coordinates! That is usually not the case at first repaint. We'll clear
	//      the remanining entries at exit.
	int i;
	int nHoverLink = -1;
	POINT ptMouse = pManager->GetMousePos();
	for( i = 0; (nHoverLink < 0) && i < nLinkRects; i++ ) {
		if( ::PtInRect(prcLinks + i, ptMouse) ) nHoverLink = i;
	}


	HPEN hUnderLinePen = ::CreatePen(PS_SOLID, 1, iTextColor);

	HPEN hOldLinePen = NULL;

	TEXTMETRIC tm = pManager->GetThemeFontInfo(UIFONT_NORMAL);
	POINT pt = { rc.left, rc.top };
	int iLineIndent = 0;
	int iLinkIndex = 0;
	int cyLine = tm.tmHeight + tm.tmExternalLeading;
	int cyMinHeight = 0;
	POINT ptLinkStart = { 0 };
	bool bInLink = false;

	while( *pstrText != '\0' ) 
	{
		if( pt.x  >= rc.right || *pstrText == '\n' ) 
		{
			// A new link was detected/requested. We'll adjust the line height
			// for the next line and expand the link hitbox (if any)
			if( bInLink && iLinkIndex < nLinkRects) ::SetRect(&prcLinks[iLinkIndex++], ptLinkStart.x, ptLinkStart.y, pt.x, pt.y + tm.tmHeight);
			if( (uStyle & DT_SINGLELINE) != 0 ) break;
			if( *pstrText == '\n' ) pstrText++;
			pt.x = rc.left + iLineIndent;
			pt.y += cyLine/* - tm.tmDescent*/;
			ptLinkStart = pt;
			cyLine = tm.tmHeight + tm.tmExternalLeading ;
			if( pt.x >= rc.right ) break;
			while( *pstrText == ' ' ) pstrText++;
		}
		else if( *pstrText == '<' 
			&& (pstrText[1] >= 'a' && pstrText[1] <= 'z')
			&& (pstrText[2] == ' ' || pstrText[2] == '>') )
		{
			pstrText++;
			switch( *pstrText++ )
			{
			case 'a':  // Link
				{
					// <<<modified by wangpf in 2010-0813
					//::SetTextColor(hDC, pManager->GetThemeColor(bHoverLink ? UICOLOR_LINK_TEXT_HOVER : UICOLOR_LINK_TEXT_NORMAL));
					::SetTextColor(hDC, iTextColor);
					tm = pManager->GetThemeFontInfo(UIFONT_LINK);
					cyLine = MAX(cyLine, tm.tmHeight + tm.tmExternalLeading);
					ptLinkStart = pt;
					bInLink = true;
					// --->>>

			
				}
				break;
			case 'f':  // Font
				{
					UITYPE_FONT iFont = (UITYPE_FONT) _tcstol(pstrText, const_cast<LPTSTR*>(&pstrText), 10);
					::SelectObject(hDC, pManager->GetThemeFont(iFont));
					tm = pManager->GetThemeFontInfo(iFont);
					cyLine = MAX(cyLine, tm.tmHeight + tm.tmExternalLeading);
				}
				break;
			case 'b':  // Bold text
				{
					::SelectObject(hDC, pManager->GetThemeFont(UIFONT_BOLD));
					tm = pManager->GetThemeFontInfo(UIFONT_BOLD);
					cyLine = MAX(cyLine, tm.tmHeight + tm.tmExternalLeading);
				}
				break;
			case 'x':  // Indent
				{
					iLineIndent = (int) _tcstol(pstrText, const_cast<LPTSTR*>(&pstrText), 10);
					if( pt.x < rc.left + iLineIndent ) pt.x = rc.left + iLineIndent;
				}
				break;
			case 'p':  // Paragraph
				{
					pt.x = rc.right;
					cyLine = MAX(cyLine, tm.tmHeight + tm.tmExternalLeading) + 5;
					iLineIndent = 0;
					::SelectObject(hDC, pManager->GetThemeFont(UIFONT_NORMAL));
					::SetTextColor(hDC, iTextColor);
					tm = pManager->GetThemeFontInfo(UIFONT_NORMAL);
				}
				break;
			case 'h':  // Horizontal line
				{
					::SelectObject(hDC, pManager->GetThemePen(UICOLOR_STANDARD_GREY));
					if( bDraw ) {
						POINT ptTemp = { 0 };
						::MoveToEx(hDC, pt.x, pt.y + 5, &ptTemp);
						::LineTo(hDC, rc.right - iLineIndent, pt.y + 5);
					}
					cyLine = 12;
				}
				break;
			case 'i':  // Icon
				{
					int iSize = 16;
								
					if( *pstrText == ' ' ) pstrText++;
					if( isdigit(*pstrText) ) {
						UINT iIndex = (int) _tcstol(pstrText, const_cast<LPTSTR*>(&pstrText), 10);
						UINT iIconIndex = _ttoi(pstrText);

						map<UINT, IconInfo>::iterator iter = pManager->m_mapSkinCommonIcon.find(iIndex);
						if( bDraw && iter != pManager->m_mapSkinCommonIcon.end()) {
							Graphics graph(hDC);
							int nIconWidth = iter->second.nWindth;
							Image* pIconImage = iter->second.pImage;

							iSize = MAX(16, iter->second.nWindth);

							if (pIconImage)
							{
								int nIconHeight = pIconImage->GetHeight();

								int nStartY = pt.y + (cyLine - nIconHeight) / 2;
	
								RectF rfDest(PointF((REAL)pt.x, (REAL)nStartY), SizeF((REAL)nIconWidth, (REAL)nIconHeight));

								graph.DrawImage(pIconImage, rfDest, (REAL)iIconIndex * nIconWidth, (REAL)0, (REAL)nIconWidth, (REAL)nIconHeight, UnitPixel, NULL);
							}


							//HICON hIcon = pManager->GetThemeIcon(iIndex, iSize);
							//ASSERT(hIcon!=NULL);
							//::DrawIconEx(hDC, pt.x, pt.y, hIcon, iSize, iSize, 0, NULL, DI_NORMAL);
							//::DestroyIcon(hIcon);
						}
					}
					// A special feature with an icon at the left edge is that it also sets
					// the paragraph indent.
					if( pt.x == rc.left ) iLineIndent = iSize + (iSize / 8); else cyLine = MAX(iSize, cyLine);
					pt.x += iSize + (iSize / 8);
					cyMinHeight = pt.y + iSize;
				}
				break;
			case 'c':  // Color
				{
					if( *pstrText == ' ' ) pstrText++;
					if( *pstrText == '#') {
						pstrText++;
						COLORREF clrColor = _tcstol(pstrText, const_cast<LPTSTR*>(&pstrText), 16);
						clrColor = RGB(GetBValue(clrColor), GetGValue(clrColor), GetRValue(clrColor));
						::SetTextColor(hDC, clrColor);
					}
					else {
						UITYPE_COLOR Color = (UITYPE_COLOR) _tcstol(pstrText, const_cast<LPTSTR*>(&pstrText), 10);
						::SetTextColor(hDC, pManager->GetThemeColor(Color));
					}
				}
				break;
			}
			while( *pstrText != '\0' && *pstrText != '>' ) pstrText++;
			pstrText++;
		}
		else if( *pstrText == '<' && pstrText[1] == '/' )
		{
			pstrText += 2;
			switch( *pstrText++ )
			{
			case 'a':
				if( iLinkIndex < nLinkRects ) ::SetRect(&prcLinks[iLinkIndex++], ptLinkStart.x, ptLinkStart.y, pt.x, pt.y + tm.tmHeight + tm.tmExternalLeading);
				::SetTextColor(hDC, iTextColor);
				::SelectObject(hDC, pManager->GetThemeFont(UIFONT_NORMAL));
				tm = pManager->GetThemeFontInfo(UIFONT_NORMAL);
				bInLink = false;
				break;
			case 'f':
			case 'b':
				// TODO: Use a context stack instead
				::SelectObject(hDC, pManager->GetThemeFont(UIFONT_NORMAL));
				tm = pManager->GetThemeFontInfo(UIFONT_NORMAL);
				break;
			case 'c':
				::SetTextColor(hDC, iTextColor);
				break;
			}
			while( *pstrText != '\0' && *pstrText != '>' ) pstrText++;
			pstrText++;
		}
		else if( *pstrText == '&' )
		{
			if( (uStyle & DT_NOPREFIX) == 0 ) {
				if( bDraw  && pManager->GetSystemSettings().bShowKeyboardCues ) ::TextOut(hDC, pt.x, pt.y, _T("_"), 1);
			}
			else {
				SIZE szChar = { 0 };
				::GetTextExtentPoint32(hDC, _T("&"), 1, &szChar);
				if( bDraw ) ::TextOut(hDC, pt.x, pt.y, _T("&"), 1);
				pt.x += szChar.cx;
			}
			pstrText++;
		}
		else if( *pstrText == ' ' )
		{
			SIZE szSpace = { 0 };
			::GetTextExtentPoint32(hDC, _T(" "), 1, &szSpace);
			// Still need to paint the space because the font might have
			// underline formatting.
			if( bDraw ) ::TextOut(hDC, pt.x, pt.y, _T(" "), 1);
			pt.x += szSpace.cx;
			pstrText++;
		}
		else
		{
			POINT ptPos = pt;
			int cchChars = 0;
			int cchLastGoodWord = 0;
			LPCTSTR p = pstrText;
			SIZE szText = { 0 };
			if( *p == '<' ) p++, cchChars++;
			while( *p != '\0' && *p != '<' && *p != '\n' && *p != '&' ) {
				// This part makes sure that we're word-wrapping if needed or providing support
				// for DT_END_ELLIPSIS. Unfortunately the GetTextExtentPoint32() call is pretty
				// slow when repeated so often.
				// TODO: Rewrite and use GetTextExtentExPoint() instead!
				cchChars++;
				szText.cx = cchChars * tm.tmMaxCharWidth;
				if( pt.x + szText.cx >= rc.right ) {
					::GetTextExtentPoint32(hDC, pstrText, cchChars, &szText);
				}
				if( pt.x + szText.cx >= rc.right) {
					if( (uStyle & DT_WORDBREAK) != 0 && cchLastGoodWord > 0 ) {
						cchChars = cchLastGoodWord;
						pt.x = rc.right;
					}
					if( (uStyle & DT_END_ELLIPSIS) != 0 && cchChars > 2 ) {
						cchChars -= 2;
						pt.x = rc.right;
					}

					if (pt.x + szText.cx > rc.right)
					{
						cchChars--;
						pt.x = rc.right;
					}
					
					break;
				}
				if( *p == ' ' ) cchLastGoodWord = cchChars;
				p = ::CharNext(p);
			}
			if( cchChars > 0 ) {
				::GetTextExtentPoint32(hDC, pstrText, cchChars, &szText);
				if( bDraw ) {
					
					int nTempY = ptPos.y;
					if ((uStyle & DT_VCENTER) != 0)
					{
						nTempY= ptPos.y + (cyLine - szText.cy) / 2;
					}
					
					::TextOut(hDC, ptPos.x, nTempY, pstrText, cchChars);
					if( pt.x == rc.right && (uStyle & DT_END_ELLIPSIS) != 0 ) ::TextOut(hDC, rc.right - 10, ptPos.y, _T("..."), 3);

					if (nHoverLink == iLinkIndex && bInLink)
					{
						hOldLinePen = (HPEN)::SelectObject(hDC, hUnderLinePen);
						POINT ptTemp;

						::MoveToEx(hDC, ptPos.x, nTempY + szText.cy - 1, &ptTemp);
						::LineTo(hDC, ptPos.x + szText.cx, nTempY + szText.cy - 1);

					}
				}
				pt.x += szText.cx;
				pstrText += cchChars;
			}
		}
		ASSERT(iLinkIndex<=nLinkRects);
	}

	// Clear remaining link rects and return number of used rects
	for( i = iLinkIndex; i < nLinkRects; i++ ) ::ZeroMemory(prcLinks + i, sizeof(RECT));
	nLinkRects = iLinkIndex;

	// Return size of text when requested
	if( (uStyle & DT_CALCRECT) != 0 ) {
		rc.bottom = MAX(cyMinHeight, pt.y + cyLine);
		if( rc.right >= 9999 ) {
			if( _tcslen(pstrText) > 0 ) pt.x += 3;
			rc.right = pt.x;
		}
	}

	if( bDraw ) ::SelectClipRgn(hDC, hOldRgn);
	::DeleteObject(hOldRgn);
	::DeleteObject(hRgn);

	::SelectObject(hDC, hOldFont);

	if (hOldLinePen)
	{
		::SelectObject(hDC, hOldLinePen);
	}
	::DeleteObject(hUnderLinePen);

}



void CUIBlueRenderEngine::DoPaintPrettyText(HDC hDC, CUIPaintManager* pManager, RECT& rc, LPCTSTR pstrText, UITYPE_COLOR iTextColor, UITYPE_COLOR iBackColor, RECT* prcLinks, int& nLinkRects, UINT uStyle)
{
 
	COLORREF colorText = pManager->GetThemeColor(iTextColor);

	COLORREF colorBack;
	if (iBackColor != UICOLOR__INVALID)
	{
		colorBack = pManager->GetThemeColor(iBackColor);
	}
	else
	{
		colorBack = INVALID__UICOLOR;
	}

	DoPaintPrettyText(hDC, pManager, rc, pstrText, colorText, colorBack, prcLinks, nLinkRects, uStyle);
}

void CUIBlueRenderEngine::DoPaintGradient(HDC hDC, CUIPaintManager* pManager, RECT rc, COLORREF clrFirst, COLORREF clrSecond, bool bVertical, int nSteps)
{
   typedef BOOL (WINAPI *PGradientFill)(HDC, PTRIVERTEX, ULONG, PVOID, ULONG, ULONG);
   static PGradientFill lpGradientFill = (PGradientFill) ::GetProcAddress(::GetModuleHandle(_T("msimg32.dll")), "GradientFill");
   if( lpGradientFill != NULL ) 
   {
      // Use Windows gradient function from msimg32.dll
      // It may be slower than the code below but makes really pretty gradients on 16bit colors.
      TRIVERTEX triv[2] = 
      {
         { rc.left, rc.top, GetRValue(clrFirst) << 8, GetGValue(clrFirst) << 8, GetBValue(clrFirst) << 8, 0xFF00 },
         { rc.right, rc.bottom, GetRValue(clrSecond) << 8, GetGValue(clrSecond) << 8, GetBValue(clrSecond) << 8, 0xFF00 }
      };
      GRADIENT_RECT grc = { 0, 1 };
      lpGradientFill(hDC, triv, 2, &grc, 1, bVertical ? GRADIENT_FILL_RECT_V : GRADIENT_FILL_RECT_H);
   }
   else 
   {
      // Determine how many shades
      int nShift = 1;
      if( nSteps >= 64 ) nShift = 6;
      else if( nSteps >= 32 ) nShift = 5;
      else if( nSteps >= 16 ) nShift = 4;
      else if( nSteps >= 8 ) nShift = 3;
      else if( nSteps >= 4 ) nShift = 2;
      int nLines = 1 << nShift;
      for( int i = 0; i < nLines; i++ ) {
         // Do a little alpha blending
         BYTE bR = (BYTE) ((GetRValue(clrSecond) * (nLines - i) + GetRValue(clrFirst) * i) >> nShift);
         BYTE bG = (BYTE) ((GetGValue(clrSecond) * (nLines - i) + GetGValue(clrFirst) * i) >> nShift);
         BYTE bB = (BYTE) ((GetBValue(clrSecond) * (nLines - i) + GetBValue(clrFirst) * i) >> nShift);
         // ... then paint with the resulting color
         HBRUSH hBrush = ::CreateSolidBrush(RGB(bR,bG,bB));
         RECT r2 = rc;
         if( bVertical ) {
            r2.bottom = rc.bottom - ((i * (rc.bottom - rc.top)) >> nShift);
            r2.top = rc.bottom - (((i + 1) * (rc.bottom - rc.top)) >> nShift);
            if( (r2.bottom - r2.top) > 0 ) ::FillRect(hDC, &r2, hBrush);
         }
         else {
            r2.left = rc.right - (((i + 1) * (rc.right - rc.left)) >> nShift);
            r2.right = rc.right - ((i * (rc.right - rc.left)) >> nShift);
            if( (r2.right - r2.left) > 0 ) ::FillRect(hDC, &r2, hBrush);
         }
         ::DeleteObject(hBrush);
      }
   }
}

void CUIBlueRenderEngine::DoPaintAlphaBitmap(HDC hDC, CUIPaintManager* pManager, HBITMAP hBitmap, RECT rc, BYTE iAlpha)
{
   // Alpha blitting is only supported of the msimg32.dll library is located on the machine.
   typedef BOOL (WINAPI *LPALPHABLEND)(HDC, int, int, int, int,HDC, int, int, int, int, BLENDFUNCTION);
   static LPALPHABLEND lpAlphaBlend = (LPALPHABLEND) ::GetProcAddress(::GetModuleHandle(_T("msimg32.dll")), "AlphaBlend");
   if( lpAlphaBlend == NULL ) return;
   if( hBitmap == NULL ) return;
   HDC hCloneDC = ::CreateCompatibleDC(pManager->GetPaintDC());
   HBITMAP hOldBitmap = (HBITMAP) ::SelectObject(hCloneDC, hBitmap);
   int cx = rc.right - rc.left;
   int cy = rc.bottom - rc.top;
   ::SetStretchBltMode(hDC, COLORONCOLOR);
   BLENDFUNCTION bf = { 0 };
   bf.BlendOp = AC_SRC_OVER; 
   bf.BlendFlags = 0; 
   bf.AlphaFormat = AC_SRC_ALPHA;
   bf.SourceConstantAlpha = iAlpha;
   lpAlphaBlend(hDC, rc.left, rc.top, cx, cy, hCloneDC, 0, 0, cx, cy, bf);
   ::SelectObject(hCloneDC, hOldBitmap);
   ::DeleteDC(hCloneDC);
}

void CUIBlueRenderEngine::DoPaintTabBackground(HDC hDC, CUIPaintManager* pManager, RECT& rcItem, LPCTSTR pstrText, UINT uState, UINT uOverBoxPixels, Image* pImage)
{
	ASSERT(::GetObjectType(hDC)==OBJ_DC || ::GetObjectType(hDC)==OBJ_MEMDC);


	RECT rcImage = {0};
	if (pImage != NULL)
	{
		int nWidth = pImage->GetWidth() / 4;
		int nHeight = pImage->GetHeight();

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

	}

	::SetBkMode(hDC, TRANSPARENT);
	::SelectObject(hDC, pManager->GetThemeFont(UIFONT_NORMAL));
	CStdString sText = pstrText;
	sText.ProcessResourceTokens();
	int cchText = sText.GetLength();
	SIZE szText = { 0 };
	::GetTextExtentPoint32(hDC, sText, cchText, &szText);
	RECT rcTab = { 0 };
	if( (uState & UISTATE_PUSHED) != 0 ) 
	{
		if (pImage)
		{
			rcTab=rcItem;
			rcTab.right=rcTab.left+szText.cx+14;
			rcTab.bottom+=uOverBoxPixels;

			CUIBlueRenderEngine::DoFillRect(hDC, pManager, rcTab, rcImage, pImage);
		}
		else
		{
			::SetRect(&rcTab, rcItem.left, rcItem.top + 1, rcItem.left + szText.cx + 14, rcItem.bottom+2);

			DoFillRect(hDC, pManager, rcTab, UICOLOR_TAB_BACKGROUND_NORMAL);

			::SelectObject(hDC, pManager->GetThemePen(UICOLOR_TAB_BORDER));
			POINT ptTemp;
			::MoveToEx(hDC, rcTab.left, rcTab.bottom, &ptTemp);
			::LineTo(hDC, rcTab.left, rcTab.top + 2);
			::LineTo(hDC, rcTab.left + 1, rcTab.top + 1);
			::LineTo(hDC, rcTab.right - 1, rcTab.top + 1);
			::LineTo(hDC, rcTab.right, rcTab.top + 2);
			::LineTo(hDC, rcTab.right, rcTab.bottom);
			::SelectObject(hDC, pManager->GetThemePen(UICOLOR_TAB_BACKGROUND_NORMAL));
			::LineTo(hDC, rcTab.left, rcTab.bottom);		
			
			RECT rcTop = { rcTab.left + 0, rcTab.top, rcTab.right - 0, rcTab.top + 3 };
			DoPaintGradient(hDC, pManager, rcTop, RGB(222,142,41), RGB(255,199,25), true, 4);
		}
	}
	else if( (uState & UISTATE_DISABLED) != 0 )
	{

	}
	else
	{
		if (pImage)
		{
			rcTab=rcItem;
			rcTab.right=rcTab.left+szText.cx+14;

			CUIBlueRenderEngine::DoFillRect(hDC, pManager, rcTab, rcImage, pImage);
		}
		else
		{
			::SetRect(&rcTab, rcItem.left, rcItem.top + 3, rcItem.left + szText.cx + 12, rcItem.bottom);

			COLORREF clrFolder1, clrFolder2;
			pManager->GetThemeColorPair(UICOLOR_TAB_FOLDER_NORMAL, clrFolder1, clrFolder2);
			DoPaintGradient(hDC, pManager, rcTab, clrFolder1, clrFolder2, true, 32);

			::SelectObject(hDC, pManager->GetThemePen(UICOLOR_TAB_BORDER));
			POINT ptTemp;
			::MoveToEx(hDC, rcTab.left, rcTab.bottom, &ptTemp);
			::LineTo(hDC, rcTab.left, rcTab.top);
			::LineTo(hDC, rcTab.right, rcTab.top);
			::LineTo(hDC, rcTab.right, rcTab.bottom);
		}
	}
	rcItem = rcTab;
}


