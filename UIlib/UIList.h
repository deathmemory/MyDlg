#if !defined(AFX_UILIST_H__20060218_929F_BDFF_55AA_0080AD509054__INCLUDED_)
#define AFX_UILIST_H__20060218_929F_BDFF_55AA_0080AD509054__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <map>
#include <vector>
using namespace std;

/////////////////////////////////////////////////////////////////////////////////////
//
class CUIListHeader;

typedef struct tagTListInfoUI
{
   COLORREF Text;
   COLORREF Background;
   COLORREF SelText;
   COLORREF SelBackground;
   Image *SelImage;
   int ItemHeight;
   vector<RECT> vColumnRectList;
   bool bExpandable;
} TListInfoUI;
/////////////////////////////////////////////////////////////////////////////////////
//

class IUIListCallback
{
public:
   virtual LPCTSTR GetItemText(CUIControl* pList, int iItem, int iSubItem) = 0;
   virtual int CompareItem(CUIControl* pList, CUIControl* pItem1, CUIControl* pItem2) = 0;
};

class IUIListOwner
{
public:
   virtual int GetCurSel() const = 0;
   virtual bool SetCurSel(int iIndex) = 0;
   virtual bool SetHotItem(int iIndex) = 0;
   virtual void Event(TEventUI& event) = 0;
};

class IUIList : public IUIListOwner
{
public:
   virtual CUIListHeader* GetHeader() const = 0;
   virtual CUIContainer* GetList() const = 0;
   virtual const TListInfoUI* GetListInfo() const = 0;
   virtual bool ExpandItem(int iIndex, bool bExpand = true) = 0;
   virtual int GetExpandedItem() const = 0;
};

class IListItemUI
{
public:
   virtual int GetIndex() const = 0;
   virtual void SetIndex(int iIndex) = 0;
   virtual void SetOwner(CUIControl* pOwner) = 0;
   virtual CUIControl* GetOwner() = 0;
   virtual bool IsSelected() const = 0;
   virtual bool Select(bool bSelect = true) = 0;
   virtual bool IsExpanded() const = 0;
   virtual bool Expand(bool bExpand = true) = 0;
   virtual void DrawItem(HDC hDC, const RECT& rcItem, UINT uStyle) = 0;
   virtual bool SetHot(bool bHot = true) = 0;
   virtual DWORD_PTR GetItemData() = 0;
   virtual bool	  SetItemData(DWORD_PTR dwData) = 0;
};

/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API CUIListHeader : public CHorizontalLayoutUI
{
public:
	CUIListHeader();

	LPCTSTR GetClass() const;
	LPVOID GetInterface(LPCTSTR pstrName);

	SIZE EstimateSize(SIZE szAvailable);
	void DoPaint(HDC hDC, const RECT& rcPaint);
};


/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API CUIListHeaderItem : public CUIControl
{
public:
	CUIListHeaderItem();

	LPCTSTR GetClass() const;
	UINT GetControlFlags() const;

	void SetText(LPCTSTR pstrText);

	void Event(TEventUI& event);

	SIZE EstimateSize(SIZE szAvailable);
	void DoPaint(HDC hDC, const RECT& rcPaint);

	void SetWidth(int cxWidth);
	void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

	RECT GetThumbRect(RECT rc) const;

protected:
	int m_cxWidth;
	POINT ptLastMouse;
	UINT m_uDragState;
};


/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API CUIListElement : public CUIControl, public IListItemUI
{
public:
   CUIListElement();

   UINT GetControlFlags() const;
   LPVOID GetInterface(LPCTSTR pstrName);

   int GetIndex() const;
   void SetIndex(int iIndex);

   void SetOwner(CUIControl* pOwner);
   CUIControl* GetOwner();

   bool IsSelected() const;
   bool Select(bool bSelect = true);
   bool IsExpanded() const;
   bool Expand(bool bExpand = true);

   bool Activate();

   void Event(TEventUI& event);
   void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

   bool SetHot(bool bHot = true);

   DWORD_PTR GetItemData();
   bool	  SetItemData(DWORD_PTR dwData);

protected:
   int m_iIndex;
   bool m_bSelected;
   IUIListOwner* m_pOwner;

   DWORD_PTR		 m_dwData;
};


/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API CUIListLabelElement : public CUIListElement
{
public:
   CUIListLabelElement();

   LPCTSTR GetClass() const;

   void SetWidth(int nWidth);
   void SetHeight(int nHeight);
   void SetTextStyle(UINT uStyle);

   void Event(TEventUI& event);
   SIZE EstimateSize(SIZE szAvailable);
   void DoPaint(HDC hDC, const RECT& rcPaint);

   void DrawItem(HDC hDC, const RECT& rcItem, UINT uStyle);

   void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
   void SetImage(Image *pImage);

protected:
   int m_cxWidth;
   int m_nHeight;
   UINT m_uTextStyle;
   UINT m_uButtonState;

   Image* m_pImage;
};


/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API CUIListTextElement : public CUIListLabelElement
{
public:
   CUIListTextElement();

   LPCTSTR GetClass() const;
   UINT GetControlFlags() const;

   void SetOwner(CUIControl* pOwner);

   void Event(TEventUI& event);
   SIZE EstimateSize(SIZE szAvailable);
   void DoPaint(HDC hDC, const RECT& rcPaint);

   void DrawItem(HDC hDC, const RECT& rcItem, UINT uStyle);
   void SetSubItemText(int iSubIndex, LPCTSTR pstrSubText);
   LPCTSTR GetSubItemText(int iSubIndex);

   void SetItemHeight(int iHeight);

protected:
   int m_iItemHeight;
   int m_nLinks;
   RECT m_rcLinks[8];
   IUIList* m_pOwner;
   map<int, CStdString> m_mSubItemList;
};


/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API CUIListExpandElement : public CUIListTextElement, public IContainerUI
{
public:
   CUIListExpandElement();
   virtual ~CUIListExpandElement();

   LPCTSTR GetClass() const;

   void SetPos(RECT rc);
   void Event(TEventUI& event);
   SIZE EstimateSize(SIZE szAvailable);
   void DoPaint(HDC hDC, const RECT& rcPaint);

   CUIControl* GetItem(int iIndex) const;
   int GetCount() const;
   bool Add(CUIControl* pControl);
   bool Remove(CUIControl* pControl);
   void RemoveAll();

   bool IsExpanded() const;
   bool Expand(bool bExpand = true);

   void DrawItem(HDC hDC, const RECT& rcItem, UINT uStyle);

protected:
   bool m_bExpanded;
   int m_cyExpanded;
   CUIContainer* m_pContainer;
};


class UILIB_API CUISubMenu : public CUIListElement
{
public:
	CUISubMenu();

	LPCTSTR GetClass() const;

	void SetText(LPCTSTR pstrText);

	void SetWidth(int cxWidth);
	void SetTextStyle(UINT uStyle);

	void Event(TEventUI& event);
	SIZE EstimateSize(SIZE szAvailable);
	void DoPaint(HDC hDC, const RECT& rcPaint);

	void DrawItem(HDC hDC, const RECT& rcItem, UINT uStyle);

	void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
	bool Activate();
	bool SetHot(bool bHot = true);
protected:
	int m_cxWidth;
	UINT m_uTextStyle;
	UINT m_uButtonState;
};
/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API CUIList : public CVerticalLayoutUI, public IUIList
{
public:
	CUIList();	

	//Columns
	virtual void InsertColumn(LPCTSTR lpszItem, int nWidth);
	void SetColumnHeaderVisible(bool bVisible);
	
	//Items Add
	int InsertItem(UINT uIndex, LPCTSTR pstrText);
	bool SetItemText(UINT uIndex, UINT uSubItem, LPCTSTR pstrText);
	bool SetItemToolTip(UINT uIndex, LPCTSTR pstrText);
	LPCTSTR GetItemText(UINT uIndex);

	//Item Data
	DWORD_PTR GetItemData(int nItem);
	bool	  SetItemData(int nItem, DWORD_PTR dwData);

	//Item Remove
	bool Remove(UINT uIndex);
	void RemoveAll();

	//Item Select

	int GetCurSel() const;
	bool SetCurSel(int iIndex);

	//Item count
	int GetCount() const;

	//Item Color and Image
	void SetTextColor(COLORREF acrTextColor);
	void SetTextSelColor(COLORREF acrTextSelColor);
	void SetTextBkColor(COLORREF acrBkColor);
	void SetTextBkSelColor(COLORREF acrBkSelColor);
	void SetTextBkSelImage(Image *pImage);

	void SetItemHeight(int iHeight);


	LPCTSTR GetClass() const;
	UINT GetControlFlags() const;
	LPVOID GetInterface(LPCTSTR pstrName);

	CUIListHeader* GetHeader() const;
	CUIContainer* GetList() const;
	const TListInfoUI* GetListInfo() const;

	void EnsureVisible(int iIndex);
	void Scroll(int dx, int dy);

	void SetExpanding(bool bExpandable);

	int GetExpandedItem() const;
	bool ExpandItem(int iIndex, bool bExpand = true);

	void SetPos(RECT rc);
	void Event(TEventUI& event);
	void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

	bool SetHotItem(int iIndex);
private:
	bool Add(CUIControl* pControl);
	bool Remove(CUIControl* pControl);
	CUIControl* GetItem(int iIndex) const;
	void ResetColumnRect();
	void ResetItemIndexList();

protected:
	int m_iCurSel;
	int m_iExpandedItem;
	CVerticalLayoutUI* m_pControlsList;
	CUIListHeader* m_pListHeader;
	map<int, int> m_mItemIndexList;
	TListInfoUI m_oListInfo;
};


/////////////////////////////////////////////////////////////////////////////////////
//


#endif // !defined(AFX_UILIST_H__20060218_929F_BDFF_55AA_0080AD509054__INCLUDED_)
