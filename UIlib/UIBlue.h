#if !defined(AFX_BLUEUI_H__20050424_6E62_6B42_BC3F_0080AD509054__INCLUDED_)
#define AFX_BLUEUI_H__20050424_6E62_6B42_BC3F_0080AD509054__INCLUDED_

#pragma once


/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API CRenderClip
{
public:
   ~CRenderClip();
   RECT rcItem;
   HDC hDC;
   HRGN hRgn;
   HRGN hOldRgn;
};

class UILIB_API CUIBlueRenderEngine
{
public:
   static void DoFillRect(HDC hDC, CUIPaintManager* pManager, RECT rc, UITYPE_COLOR Color);
   static void DoFillRect(HDC hDC, CUIPaintManager* pManager, RECT rc, COLORREF clrFill);
   static void DoFillRect(HDC hDC, CUIPaintManager* pManager, RECT rc, RECT rcImage, Image* pImage, bool bBrush = true);


   static void DoPaintLine(HDC hDC, CUIPaintManager* pManager, RECT rc, UITYPE_COLOR Color);
   static void DoPaintLine(HDC hDC, CUIPaintManager* pManager, RECT rc, COLORREF Color);

   static void DoPaintLine(HDC hDC, CUIPaintManager* pManager, POINT pt1, POINT pt2, COLORREF Color);


   static void DoPaintRectangle(HDC hDC, CUIPaintManager* pManager, RECT rcItem, UITYPE_COLOR Border, UITYPE_COLOR Fill);
   static void DoPaintFrame(HDC hDC, CUIPaintManager* pManager, RECT rc, UITYPE_COLOR Light, UITYPE_COLOR Dark, UITYPE_COLOR Background = UICOLOR__INVALID, UINT uStyle = 0);
   
   static void DoPaintBorder(HDC hDC, CUIPaintManager* pManager, RECT rcItem, UITYPE_COLOR Color);
   static void DoPaintBorder(HDC hDC, CUIPaintManager* pManager, RECT rcItem, COLORREF Color);
  
   static void DoPaintButton(HDC hDC, CUIPaintManager* pManager, RECT rc, UINT uState, POINT* pt = NULL, int nCount = 0);
   static void DoPaintEditBox(HDC hDC, CUIPaintManager* pManager, RECT rcItem, LPCTSTR pstrText, UINT uState, UINT uDrawStyle,UINT uBorderWidth, Image* pImage = NULL);

   static void DoPaintOptionBox(HDC hDC, CUIPaintManager* pManager, RECT rcItem, LPCTSTR pstrText, UINT uState, UINT uStyle, BUTTON_TYPE uButtonType = BUTTON_TYPE_RADIOBOX);
 
   static void DoPaintPrettyText(HDC hDC, CUIPaintManager* pManager, RECT& rc, LPCTSTR pstrText, UITYPE_COLOR iTextColor, UITYPE_COLOR iBackColor, RECT* prcLinks, int& nLinkRects, UINT uStyle);
   static void DoPaintPrettyText(HDC hDC, CUIPaintManager* pManager, RECT& rc, LPCTSTR pstrText, COLORREF iTextColor, COLORREF iBackColor, RECT* prcLinks, int& nLinkRects, UINT uStyle);
  
   static void DoPaintGradient(HDC hDC, CUIPaintManager* pManager, RECT rc, COLORREF clrFirst, COLORREF clrSecond, bool bVertical, int nSteps);

   static void DoPaintAlphaBitmap(HDC hDC, CUIPaintManager* pManager, HBITMAP hBitmap, RECT rc, BYTE iAlpha);
 
   static void GenerateClip(HDC hDC, RECT rcItem, CRenderClip& clip);

   static void DoPaintTabBackground(HDC hDC, CUIPaintManager* pManager, RECT& rcItem, LPCTSTR pstrText, UINT uState, UINT uOverBoxPixels, Image* pImage = NULL);

  
};


#endif // !defined(AFX_BLUEUI_H__20050424_6E62_6B42_BC3F_0080AD509054__INCLUDED_)
