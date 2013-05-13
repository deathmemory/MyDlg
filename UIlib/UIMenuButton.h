#if !defined(AFX_UIMENUBUTTON_H__20060218_C01D_1618_FBA5_0080AD509054__INCLUDED_)
#define AFX_UIMENUBUTTON_H__20060218_C01D_1618_FBA5_0080AD509054__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////////////
//
class CMenuButtonWnd;

class UILIB_API CUIMenuButton : public CUIContainer, public IUIListOwner
{
public:
   CUIMenuButton();

   LPCTSTR GetClass() const;
   LPVOID GetInterface(LPCTSTR pstrName);
   
   void Init();
   UINT GetControlFlags() const;

   CStdString GetText() const;

   void SetText(LPCTSTR pstrText);

   SIZE GetDropDownSize() const;
   void SetDropDownSize(SIZE szDropBox);

   int GetCurSel() const;  
   bool SetCurSel(int iIndex);
   void SendSelectNotify();

   bool Add(CUIControl* pControl);
   bool Remove(CUIControl* pControl);
   void RemoveAll();

   bool Activate();
   bool NotActivate();

   bool AddSubMenu(UINT uMenuId, LPCTSTR pstrText);

   void SetPos(RECT rc);
   void Event(TEventUI& event);
   SIZE EstimateSize(SIZE szAvailable);
   void DoPaint(HDC hDC, const RECT& rcPaint);

    bool SetHotItem(int iIndex);
	int FindSelectable(int iIndex, bool bForward = true) const;

   static CMenuButtonWnd* m_pMenuWnd;
   static UINT m_uSelectId;

protected:
   int m_iCurSel;
   int m_iCurHot;
   int m_cxWidth;
   SIZE m_szDropBox;
   RECT m_rcButton;

   UINT m_uButtonState;
};


#endif // !defined(AFX_UICOMBO_H__20060218_C01D_1618_FBA5_0080AD509054__INCLUDED_)

