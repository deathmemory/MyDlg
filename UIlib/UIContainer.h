#if !defined(AFX_UICONTAINER_H__20060218_C077_501B_DC6B_0080AD509054__INCLUDED_)
#define AFX_UICONTAINER_H__20060218_C077_501B_DC6B_0080AD509054__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


/////////////////////////////////////////////////////////////////////////////////////
//
class CUIVScrollBar;

class IContainerUI
{
public:
   virtual CUIControl* GetItem(int iIndex) const = 0;
   virtual int GetCount() const = 0;
   virtual bool Add(CUIControl* pControl) = 0;
   virtual bool Remove(CUIControl* pControl) = 0;
   virtual void RemoveAll() = 0;
};


/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API CUIContainer : public CUIControl, public IContainerUI
{
public:
   CUIContainer();
   virtual ~CUIContainer();

public:
   LPCTSTR GetClass() const;
   LPVOID GetInterface(LPCTSTR pstrName);

   CUIControl* GetItem(int iIndex) const;
   int GetCount() const;
   bool Add(CUIControl* pControl);
   bool Remove(CUIControl* pControl);
   void RemoveAll();

   void Event(TEventUI& event);
   void SetVisible(bool bVisible);

   virtual void SetInset(SIZE szInset);
   virtual void SetInset(RECT rcInset);
   virtual void SetPadding(int iPadding);
   virtual void SetWidth(int cx);
   virtual void SetHeight(int cy);
   virtual void SetAutoDestroy(bool bAuto);

   virtual int FindSelectable(int iIndex, bool bForward = true) const;

   void SetPos(RECT rc);
   SIZE EstimateSize(SIZE szAvailable);
   void DoPaint(HDC hDC, const RECT& rcPaint);

   void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

   void SetManager(CUIPaintManager* pManager, CUIControl* pParent);
   CUIControl* FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags);

   virtual int GetScrollPos() const;
   virtual int GetScrollPage() const;
   virtual int GetScrollLine() const;
   virtual SIZE GetScrollRange() const;
   virtual void SetScrollPos(int iPos);
   virtual void EnableScrollBar(bool bEnable = true);


   void SetVScrollImage(Image* pImage);
   void SetVScrollImage(UINT nResourceID);
   void SetVScrollImage(LPCTSTR pstrName);

   void SetVScrollBtnUpPos(const RECT& rc);
   void SetVScrollBtnDownPos(const RECT& rc);
   void SetVScrollPageUpPos(const RECT& rc);
   void SetVScrollPageDownPos(const RECT& rc);

   void SetVScrollSliderPos(const RECT& rc);
   void SetVScrollSlider_MPos(const RECT& rc);
   void SetVScrollSlider_MTPos(const RECT& rc);
   void SetVScrollBtnSize(int nWidth, int nHeight);

protected:
   virtual void ProcessScrollbar(RECT rc, int cyRequired, int cyLine);

protected:
   CStdPtrArray m_items;
   RECT m_rcInset;
   int m_iPadding;
   SIZE m_cxyFixed;
   bool m_bAutoDestroy;
   bool m_bAllowScrollbars;
   CUIVScrollBar* m_pVScroll;
   int m_iScrollPos;

   Image *m_pImageVScroll;
   RECT m_rcImageBtnUp;
   RECT m_rcImageBtnDown;
   RECT m_rcImagePageUp;
   RECT m_rcImagePageDown;

   RECT m_rcImageSlider;
   RECT m_rcImageSlider_M;
   RECT m_rcImageSlider_MT;

   int	m_iScrollBtnWidth;
   int	m_iScrollBtnHeight;
};


/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API CCanvasUI : public CUIContainer
{
public:
   CCanvasUI();
   virtual ~CCanvasUI();

   LPCTSTR GetClass() const;

   bool SetWatermark(UINT iBitmapRes, int iOrientation = HTBOTTOMRIGHT);
   bool SetWatermark(LPCTSTR pstrBitmap, int iOrientation = HTBOTTOMRIGHT);

   void DoPaint(HDC hDC, const RECT& rcPaint);

   void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

protected:
   COLORREF m_clrBack;
   HBITMAP m_hBitmap;
   BITMAP m_BitmapInfo;
   int m_iOrientation;
};


/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API CWindowCanvasUI : public CCanvasUI
{
public:
   CWindowCanvasUI();
   LPCTSTR GetClass() const;
};


/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API CControlCanvasUI : public CCanvasUI
{
public:
   CControlCanvasUI();
   LPCTSTR GetClass() const;
};


/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API CWhiteCanvasUI : public CCanvasUI
{
public:
   CWhiteCanvasUI();
   LPCTSTR GetClass() const;
};


/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API CDialogCanvasUI : public CCanvasUI
{
public:
   CDialogCanvasUI();
   LPCTSTR GetClass() const;
};


/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API CVerticalLayoutUI : public CUIContainer
{
public:
   CVerticalLayoutUI();

   LPCTSTR GetClass() const;
   void SetPos(RECT rc);

protected:
   int m_cyNeeded;
   int m_nPrevItems;
};


/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API CHorizontalLayoutUI : public CUIContainer
{
public:
   CHorizontalLayoutUI();

   LPCTSTR GetClass() const;
   void SetPos(RECT rc);
};


/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API CTileLayoutUI : public CUIContainer
{
public:
   CTileLayoutUI();

   LPCTSTR GetClass() const;

   void SetPos(RECT rc);
   void SetColumns(int nCols);

protected:
   int m_nColumns;
   int m_cyNeeded;
};


/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API CDialogLayoutUI : public CUIContainer
{
public:
   CDialogLayoutUI();

   LPCTSTR GetClass() const;
   LPVOID GetInterface(LPCTSTR pstrName);

   void SetStretchMode(CUIControl* pControl, UINT uMode);
   void DoPaint(HDC hDC, const RECT& rcPaint);

   void SetPos(RECT rc);
   SIZE EstimateSize(SIZE szAvailable);

protected:
   void RecalcArea();

protected:  
   typedef struct 
   {
      CUIControl* pControl;
      UINT uMode;
      RECT rcItem;
   } STRETCHMODE;

   RECT m_rcDialog;
   RECT m_rcOriginal;
   bool m_bFirstResize;
   CStdValArray m_aModes;

protected:
	COLORREF m_clrBack;
	HBITMAP m_hBitmap;
	BITMAP m_BitmapInfo;
	int m_iOrientation;
};



class UILIB_API CMenuCanvasUI : public CCanvasUI
{
public:
	CMenuCanvasUI();
	LPCTSTR GetClass() const;
	void DoPaint(HDC hDC, const RECT& rcPaint);
};


/////////////////////////////////////////////////////////////////////////////////
class UILIB_API CComboCanvasUI : public CCanvasUI
{
public:
	CComboCanvasUI();
	LPCTSTR GetClass() const;
	void DoPaint(HDC hDC, const RECT& rcPaint);
};

class UILIB_API CUIGroup : public CCanvasUI
{
public:
	CUIGroup();
	virtual ~CUIGroup();

	LPCTSTR GetClass() const;
	void DoPaint(HDC hDC, const RECT& rcPaint);

	void SetTextStyle(UINT uStyle);

	void SetImage(Image* pImage);
	void SetImage(UINT nResourceID);
	void SetImage(LPCTSTR pstrName);

	void SetTextBKColor(COLORREF color);
private:
	UINT m_uTextStyle;
	Image* m_pImageOwner;

	COLORREF m_colorTextBk;;
};


#endif // !defined(AFX_UICONTAINER_H__20060218_C077_501B_DC6B_0080AD509054__INCLUDED_)

