#if !defined(AFX_UIBUTTON_H__20060218_72F5_1B46_6300_0080AD509054__INCLUDED_)
#define AFX_UIBUTTON_H__20060218_72F5_1B46_6300_0080AD509054__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class IUIChecked
{
public:
	virtual bool IsChecked() const = 0;
	virtual void SetCheck(bool bSelected) = 0;
	virtual void SetGroupMinMaxID(UINT nMinId, UINT nMaxId) = 0;
	virtual void SetButtonType(BUTTON_TYPE nType) = 0;
	virtual BUTTON_TYPE GeyButtonType() = 0;
};

/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API CUIButton : public CUIControl, public IUIChecked
{
public:
   CUIButton();
   ~CUIButton();

   LPCTSTR GetClass() const;
   UINT GetControlFlags() const;
   LPVOID GetInterface(LPCTSTR pstrName);

   void SetTextStyle(UINT uStyle);

   bool Activate();
   bool DoubleClick();
   bool	DropDown();

   void SetText(LPCTSTR pstrText);

   void SetWidth(int cxWidth);
   void SetPadding(int cx, int cy);

   void Event(TEventUI& event);

   SIZE EstimateSize(SIZE szAvailable);
   void DoPaint(HDC hDC, const RECT& rcPaint);
   void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

   void SetImage(Image* pImage);
   void SetImage(UINT nResourceID, HMODULE hResHandle=NULL);
   void SetImage(LPCTSTR pstrName);

   void SetDropDownImage(Image* pImage);
   void SetDropDownImage(UINT nResourceID, HMODULE hResHandle=NULL);
   void SetDropDownImage(LPCTSTR pstrName);


   void SetAreaPoint(POINT* pt, int nCount);

   bool IsChecked() const;
   void SetCheck(bool bSelected);

   bool IsDropDown() const;
   void SetDropDown(bool bDropDown);
   void SetShowDropDownAlway(bool bShow);

   void SetGroupMinMaxID(UINT nMinId, UINT nMaxId);
   void SetButtonType(BUTTON_TYPE nType);
   BUTTON_TYPE GeyButtonType();

   void SetTextColor(COLORREF crNormal=RGB(0,0,0), COLORREF crFocused=RGB(0,0,20), COLORREF crPushed=RGB(0,0,20), COLORREF crDisabled=RGB(128,128,128));

   void SetAutoEstimateWidth(bool bEstimate);
   void SetMinWidth(int nWidth);

   void SetTextPos(int nX = 0, int nY = 0);

protected:
   int m_cxWidth;
   SIZE m_szPadding;
   UINT m_uTextStyle;
   UINT m_uButtonState;

   UINT m_uDropDownState;

   Image *m_pImageOwner;
   Image *m_pImageDropDown;

   bool m_bSelected;

   bool m_bEstimateWidth;
   int  m_nMinWidth;

   int  m_nTextPosX;
   int  m_nTextPosY;

   bool m_bDropDown;
   bool m_bShowDropDownAlway;


   POINT m_AreaPoint[20];
   UINT  m_nAreaPointCount;

   UINT m_nMinGroupId;
   UINT m_nMaxGroupId;

   BUTTON_TYPE m_nButtonType;

   RECT m_rectDropDown;


   COLORREF m_crTextNormal;
   COLORREF m_crTextFocused;
   COLORREF m_crTextPushed;
   COLORREF m_crTextDisable;

};

#endif // !defined(AFX_UIBUTTON_H__20060218_72F5_1B46_6300_0080AD509054__INCLUDED_)

