#if !defined(AFX_UIVSCROLLBARL_H__20060218_72F5_1B46_6300_0080AD509054__INCLUDED_)
#define AFX_UISLIDERCTRL_H__20060218_72F5_1B46_6300_0080AD509054__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class UILIB_API CUIVScrollBar : public CUIControl
{
public:
   CUIVScrollBar();
   ~CUIVScrollBar();

   LPCTSTR GetClass() const;
   UINT GetControlFlags() const;

   bool Activate();

   void SetWidth(int cxWidth);
   void SetPadding(int cx, int cy);

   void Event(TEventUI& event);

   SIZE EstimateSize(SIZE szAvailable);
   void DoPaint(HDC hDC, const RECT& rcPaint);
   void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

   void SetImage(Image* pImage);
   void SetImage(UINT nResourceID);
   void SetImage(LPCTSTR pstrName);


   int GetPageSize();
   void SetPageSize(int nPageSize);

   int GetLineSize();
   void SetLineSize(int nLineSize);

   void GetRange(int& nMin, int& nMax ) const;
   void SetRange(int nMin, int nMax, bool bRedraw = true);

   int GetCurrentPos();
   void SetCurrentPos(int nPos, bool bRedraw = true);


   void SetScrollBtnSize(int nWidth, int nHeight);
   SIZE GetScrollBtnSize();
 
   void SetBtnUpPos(const RECT& rc);
   void SetBtnDownPos(const RECT& rc);
   void SetPageUpPos(const RECT& rc);
   void SetPageDownPos(const RECT& rc);

   void SetSliderPos(const RECT& rc);
   void SetSlider_MPos(const RECT& rc);
   void SetSlider_MTPos(const RECT& rc);
private:
	void CheckPosValue();

protected:
   int m_cxWidth;
   SIZE m_szPadding;
   UINT m_uTextStyle;

   

private:
   Image *m_pImageOwner;

   RECT m_rcBtnSlider;
   RECT m_rcBtnUp;
   RECT m_rcBtnDown;
   RECT m_rcPageUp;
   RECT m_rcPageDown;

   RECT m_rcImageBtnUp;
   RECT m_rcImageBtnDown;
   RECT m_rcImagePageUp;
   RECT m_rcImagePageDown;

   RECT m_rcImageSlider;
   RECT m_rcImageSlider_M;
   RECT m_rcImageSlider_MT;

   UINT m_uSliderState;
   UINT m_uBtnUpState;
   UINT m_uBtnDownState;
   UINT m_uPageUpState;
   UINT m_uPageDownState;


   int m_nPosMax;
   int m_nPosMin;

   int m_nPageSize;
   int m_nLineSize;
   int m_nPos;

   int	m_nScrollBtnWidth;
   int	m_nScrollBtnHeight;

   int m_nSpan;

   POINT m_ptLast;
   int m_nLastTop;

   int m_nButtonOffset;
   bool m_bLButtonDown;

   double m_dblSliderHeight;
};

#endif // !defined(AFX_UIVSCROLLBARL_H__20060218_72F5_1B46_6300_0080AD509054__INCLUDED_)

