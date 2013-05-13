#if !defined(AFX_UISLIDERCTRL_H__20060218_72F5_1B46_6300_0080AD509054__INCLUDED_)
#define AFX_UISLIDERCTRL_H__20060218_72F5_1B46_6300_0080AD509054__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class UILIB_API CUISliderctrl : public CUIControl
{
public:
   CUISliderctrl();
   ~CUISliderctrl();

   LPCTSTR GetClass() const;
   UINT GetControlFlags() const;

   bool Activate();

   void SetWidth(int cxWidth);
   void SetPadding(int cx, int cy);

   void Event(TEventUI& event);

   SIZE EstimateSize(SIZE szAvailable);
   void DoPaint(HDC hDC, const RECT& rcPaint);
   void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

   void SetSliderImage(Image* pImage);
   void SetSliderImage(UINT nResourceID);
   void SetSliderImage(LPCTSTR pstrName);

   void SetShaftImage(Image* pImage);
   void SetShaftImage(UINT nResourceID);
   void SetShaftImage(LPCTSTR pstrName);

   int GetPageSize();
   void SetPageSize(int nPageSize);

   void GetRange(int& nMin, int& nMax ) const;
   void SetRange(int nMin, int nMax, bool bRedraw = true);

   int GetCurrentPos();
   void SetCurrentPos(int nPos, bool bRedraw = true);

   void SetBkColor(COLORREF color);
   void SetScaleColor(COLORREF color);

private:
	void CheckPosValue();

protected:
   int m_cxWidth;
   SIZE m_szPadding;
   UINT m_uTextStyle;
   UINT m_uButtonState;

private:
   Image *m_pImageSlider;
   Image *m_pImageShaft;

   RECT m_rcButton;

   int m_nPosMax;
   int m_nPosMin;

   int m_nPos;

   POINT m_ptLast;

   int m_nButtonOffset;

   bool m_bLButtonDown;

   int m_nPageSize;

   int m_nScaleHeight;

   COLORREF m_colorBk;
   COLORREF m_colorScale;

};

#endif // !defined(AFX_UISLIDERCTRL_H__20060218_72F5_1B46_6300_0080AD509054__INCLUDED_)

