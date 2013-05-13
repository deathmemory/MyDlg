#if !defined(AFX_UICOMBO_H__20060218_C01D_1618_FBA5_0080AD509054__INCLUDED_)
#define AFX_UICOMBO_H__20060218_C01D_1618_FBA5_0080AD509054__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////////////
//
class CUIComboBoxWnd;
class UILIB_API CUIComboBox : public CUIContainer, public IUIListOwner
{
public:
   CUIComboBox();
   virtual ~CUIComboBox();

   LPCTSTR GetClass() const;
   LPVOID GetInterface(LPCTSTR pstrName);
   
   void Init();
   UINT GetControlFlags() const;

   CStdString GetText() const;

   SIZE GetDropDownSize() const;
   void SetDropDownSize(SIZE szDropBox);

   int GetCurSel() const;  
   bool SetCurSel(int iIndex);

   void SendSelectNotify();
   bool AddSubItem(UINT uIndex, LPCTSTR pstrText);
   CStdString GetSubItemText(UINT auIndex);

   virtual DWORD_PTR GetItemData(int nItem);
   virtual bool	  SetItemData(int nItem, DWORD_PTR dwData);

   bool Remove(CUIControl* pControl);
   void RemoveAll();

   bool Activate();

   void SetPos(RECT rc);
   void Event(TEventUI& event);
   SIZE EstimateSize(SIZE szAvailable);
   void DoPaint(HDC hDC, const RECT& rcPaint);

   bool SetHotItem(int iIndex);

   void SetImage(Image* pImage);
   void SetImage(UINT nResourceID);
   void SetImage(LPCTSTR pstrName);


   void SetItemHeight(int nHeight);
   int GetItemHeight();

   void SetDropBoxHeight(int nHeight);
   int GetDropBoxHeight();

   bool IsHasVScroll();
private:
	bool Add(CUIControl* pControl);
protected:
   int m_iCurSel;
   int m_cxWidth;
   SIZE m_szDropBox;
   RECT m_rcButton;
   UINT m_uButtonState;

   Image *m_pImageOwner;

   int m_nItemHeight;

   int m_nDropBoxHeight;
   bool m_bIsHasVScroll;

public:
   CUIComboBoxWnd* m_pWindow;
};


#endif // !defined(AFX_UICOMBO_H__20060218_C01D_1618_FBA5_0080AD509054__INCLUDED_)

