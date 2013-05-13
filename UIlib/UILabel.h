#if !defined(AFX_UILABEL_H__20060218_34CC_2871_036E_0080AD509054__INCLUDED_)
#define AFX_UILABEL_H__20060218_34CC_2871_036E_0080AD509054__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API CUILabel : public CUIControl
{
public:
   CUILabel();
   ~CUILabel();

   LPCTSTR GetClass() const;
   UINT GetControlFlags() const;

   void SetText(LPCTSTR pstrText);

   void SetWidth(int cxWidth);
   void SetTextStyle(UINT uStyle);

   SIZE EstimateSize(SIZE szAvailable);
   void DoPaint(HDC hDC, const RECT& rcPaint);
   void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

   void Event(TEventUI& event);
   RECT GetButtonRect(RECT rc) const;

   void SetImage(Image* pImage);
   void SetImage(UINT nResourceID, HMODULE hResHandle=NULL);
   void SetImage(LPCTSTR pstrName);


   bool Activate();

   void SetTextColor(COLORREF TextColor, COLORREF TextColor_Disable = INVALID__UICOLOR);
   void SetBkColor(COLORREF BackColor);

   void SetBorderOutsideColor(COLORREF Color);
   void SetBorderInsideColor(COLORREF Color);


   void SetImageCount(int nCount);
   void Play(int nDalayTime = 0);
   void Pause(void);
   void Reset(void);
   void SetInterval(int interval);
   void SetImageSize(SIZE size);

   void SetImageAsLinker(bool bLink);

   CStdString GetLinkText();


protected:
   int m_cxWidth;
   UINT m_uTextStyle;

   int m_nLinks;
   int m_nCurLink;
   RECT m_rcLinks[10];

protected:
	UINT m_uButtonState;
	Image*	m_pImageOwner;

	COLORREF m_TextColor;
	COLORREF m_TextColor_Disable;
	COLORREF m_BackColor;

	COLORREF m_BorderColorInside;
	COLORREF m_BorderColorOutside;

	UINT	m_nImageWidth;
	UINT	m_nIndex;
	UINT	m_nImageCount;
	UINT	m_nInterval;
	SIZE	m_sizeAnimate;

	bool	m_bImageAsLinker;

	float	m_fAlpha;

	int		m_nDelayTime;
};

class UILIB_API CUILabelLine : public CUIControl
{
public:
	CUILabelLine();

	LPCTSTR GetClass() const;
	void DoPaint(HDC hDC, const RECT& rcPaint);
	SIZE EstimateSize(SIZE szAvailable);

	void SetLineTopColor(COLORREF Color);
	
	void SetLineBottomColor(COLORREF Color);
protected:
	COLORREF m_LineColorTop;
	COLORREF m_LineColorBottom;
};

#endif // !defined(AFX_UILABEL_H__20060218_34CC_2871_036E_0080AD509054__INCLUDED_)

