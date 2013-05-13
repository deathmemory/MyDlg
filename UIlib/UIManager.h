#if !defined(AFX_UICONTROLS_H__20050423_DB94_1D69_A896_0080AD509054__INCLUDED_)
#define AFX_UICONTROLS_H__20050423_DB94_1D69_A896_0080AD509054__INCLUDED_

#pragma once

#include <gdiplus.h>
using namespace Gdiplus;
#include <map>
#include <stdarg.h>
#include <vector>

/////////////////////////////////////////////////////////////////////////////////////
//

class CUIControl;


/////////////////////////////////////////////////////////////////////////////////////
//

typedef enum UIEVENTTYPE
{
   UIEVENT__FIRST = 0,
   UIEVENT_MOUSEMOVE,
   UIEVENT_MOUSELEAVE,
   UIEVENT_MOUSEENTER,
   UIEVENT_MOUSEHOVER,
   UIEVENT_KEYDOWN,
   UIEVENT_KEYUP,
   UIEVENT_CHAR,
   UIEVENT_SYSKEY,
   UIEVENT_KILLFOCUS,
   UIEVENT_SETFOCUS,
   UIEVENT_BUTTONDOWN,
   UIEVENT_BUTTONUP,
   UIEVENT_DBLCLICK,
   UIEVENT_CONTEXTMENU,
   UIEVENT_VSCROLL,
   UIEVENT_HSCROLL,
   UIEVENT_SCROLLWHEEL,
   UIEVENT_WINDOWSIZE,
   UIEVENT_SETCURSOR,
   UIEVENT_MEASUREITEM,
   UIEVENT_DRAWITEM,
   UIEVENT_TIMER,
   UIEVENT_NOTIFY,
   UIEVENT_COMMAND,
   UIEVENT__LAST
};

typedef enum
{
   UIFONT__FIRST = 0,
   UIFONT_NORMAL,
   UIFONT_BOLD,
   UIFONT_CAPTION,
   UIFONT_MENU,
   UIFONT_LINK,
   UIFONT_TITLE,
   UIFONT_HEADLINE,
   UIFONT_SUBSCRIPT,
   UIFONT_SMALL,
   UIFONT__LAST,
} UITYPE_FONT;

typedef enum
{
   UICOLOR__FIRST = 0,
   UICOLOR_WINDOW_BACKGROUND,
   UICOLOR_WINDOW_TEXT,

   UICOLOR_DIALOG_BACKGROUND,
   UICOLOR_DIALOG_TEXT_NORMAL,
   UICOLOR_DIALOG_TEXT_DARK,

   UICOLOR_BUTTON_BACKGROUND_NORMAL,
   UICOLOR_BUTTON_BACKGROUND_DISABLED,
   UICOLOR_BUTTON_BACKGROUND_PUSHED,
   UICOLOR_BUTTON_TEXT_NORMAL,
   UICOLOR_BUTTON_TEXT_PUSHED,
   UICOLOR_BUTTON_TEXT_DISABLED,
   UICOLOR_BUTTON_BORDER_LIGHT,
   UICOLOR_BUTTON_BORDER_DARK,
   UICOLOR_BUTTON_BORDER_DISABLED,
   UICOLOR_BUTTON_BORDER_FOCUS,

   UICOLOR_CONTROL_BACKGROUND_NORMAL,
   UICOLOR_CONTROL_BACKGROUND_SELECTED,
   UICOLOR_CONTROL_BACKGROUND_DISABLED,
   UICOLOR_CONTROL_BACKGROUND_READONLY,
   UICOLOR_CONTROL_BACKGROUND_HOVER,
   UICOLOR_CONTROL_BACKGROUND_SORTED,
   UICOLOR_CONTROL_BACKGROUND_EXPANDED,
   UICOLOR_CONTROL_BORDER_NORMAL,
   UICOLOR_CONTROL_BORDER_SELECTED,
   UICOLOR_CONTROL_BORDER_DISABLED,

   UICOLOR_CONTROL_TEXT_NORMAL,
   UICOLOR_CONTROL_TEXT_SELECTED,
   UICOLOR_CONTROL_TEXT_DISABLED,
   UICOLOR_CONTROL_TEXT_READONLY,

  
   UICOLOR_EDIT_BACKGROUND_NORMAL,
   UICOLOR_EDIT_BACKGROUND_HOVER,
   UICOLOR_EDIT_BACKGROUND_DISABLED,
   UICOLOR_EDIT_BACKGROUND_READONLY,
   UICOLOR_EDIT_TEXT_NORMAL,
   UICOLOR_EDIT_TEXT_DISABLED,
   UICOLOR_EDIT_TEXT_READONLY,

   UICOLOR_EDIT_BORDER_NORMAL,
   UICOLOR_EDIT_BORDER_NORMAL_DARK,
   UICOLOR_EDIT_BORDER_SELECTED,
   UICOLOR_EDIT_BORDER_SELECTED_DARK,
   UICOLOR_EDIT_BORDER_DISABLED,
   UICOLOR_EDIT_BORDER_DISABLED_DARK,

   UICOLOR_LINK_TEXT_HOVER,
   UICOLOR_LINK_TEXT_NORMAL,

   UICOLOR_HEADER_BACKGROUND,
   UICOLOR_HEADER_BORDER,
   UICOLOR_HEADER_SEPARATOR,
   UICOLOR_HEADER_TEXT,


   UICOLOR_STANDARD_BLACK,
   UICOLOR_STANDARD_YELLOW,
   UICOLOR_STANDARD_RED,
   UICOLOR_STANDARD_GREY,
   UICOLOR_STANDARD_LIGHTGREY,
   UICOLOR_STANDARD_WHITE,
   UICOLOR_STANDARD_BLUE,

   UICOLOR_TAB_BACKGROUND_NORMAL,
   UICOLOR_TAB_BACKGROUND_SELECTED,
   UICOLOR_TAB_FOLDER_NORMAL,
   UICOLOR_TAB_FOLDER_SELECTED,
   UICOLOR_TAB_BORDER,
   UICOLOR_TAB_TEXT_NORMAL,
   UICOLOR_TAB_TEXT_SELECTED,
   UICOLOR_TAB_TEXT_DISABLED,


   UICOLOR__LAST,
   UICOLOR__INVALID,
} UITYPE_COLOR;

typedef enum
{
	BUTTON_TYPE_NORMAL = 0,
	BUTTON_TYPE_CHECKBUTTON,
	BUTTON_TYPE_CHECKBOX,
	BUTTON_TYPE_RADIOBOX,
	BUTTON_TYPE_TAB
} BUTTON_TYPE;

typedef enum
{
	UI_NOTIFY_CLICK = 0,
	UI_NOTIFY_DBCLICK,

	UI_NOTIFY_MENU_CLICK,

	UI_NOTIFY_SETFOCUS,
	UI_NOTIFY_KILLFOCUS,

	UI_NOTIFY_LINK,

	UI_NOTIFY_DROPDOWN,
	UI_NOTIFY_TEXT_CHANGE,

	UI_NOTIFY_ITEM_CLICK,
	UI_NOTIFY_ITEM_ACTIVATE,
	UI_NOTIFY_ITEM_SELECT_CHANGE,
	UI_NOTIFY_ITEM_EXPAND,
	UI_NOTIFY_ITEM_COLLAPSE,

	UI_NOTIFY_HEADER_CLICK,
	UI_NOTIFY_HEADER_DRAGGING,
	UI_NOTIFY_HEADER_DRAGGED,

	UI_NOTIFY_WINDOW_INIT,

	UI_NOTIFY_TIMER,

	UI_NOTIFY_SLIDER_POS_CHANGED,

	UI_NOTIFY_VSCROLL_POS_CHANGED,

	UI_NOTIFY__LAST
} UI_NOTIFY_TYPE;



/////////////////////////////////////////////////////////////////////////////////////
//

// Styles for the DoPaintFrame() helper
#define UIFRAME_ROUND        0x00000001
#define UIFRAME_FOCUS        0x00000002

// Styles for the DoPaintArcCaption() helper
#define UIARC_GRIPPER        0x00000001
#define UIARC_EXPANDBUTTON   0x00000002
#define UIARC_COLLAPSEBUTTON 0x00000004

// Flags for CUIControl::GetControlFlags()
#define UIFLAG_TABSTOP       0x00000001
#define UIFLAG_SETCURSOR     0x00000002
#define UIFLAG_WANTRETURN    0x00000004

// Flags for FindControl()
#define UIFIND_ALL           0x00000000
#define UIFIND_VISIBLE       0x00000001
#define UIFIND_ENABLED       0x00000002
#define UIFIND_HITTEST       0x00000004
#define UIFIND_ME_FIRST      0x80000000
#define UIFIND_POINT_CUSTOM	 0x00000003

// Flags for Draw Style
#define UIDRAWSTYLE_INPLACE  0x00000001
#define UIDRAWSTYLE_FOCUS    0x00000002

// Flags for DoAnumateWindow()
#define UIANIM_FADE          0x00000001
#define UIANIM_HIDE          0x00000002

// Flags for the CDialogLayout stretching
#define UISTRETCH_NEWGROUP   0x00000001
#define UISTRETCH_NEWLINE    0x00000002
#define UISTRETCH_MOVE_X     0x00000004
#define UISTRETCH_MOVE_Y     0x00000008
#define UISTRETCH_SIZE_X     0x00000010
#define UISTRETCH_SIZE_Y     0x00000020

// Flags used for controlling the paint
#define UISTATE_FOCUSED      0x00000001
#define UISTATE_SELECTED     0x00000002
#define UISTATE_DISABLED     0x00000004
#define UISTATE_HOT          0x00000008
#define UISTATE_PUSHED       0x00000010
#define UISTATE_CHECKED      0x00000020
#define UISTATE_READONLY     0x00000040
#define UISTATE_CAPTURED     0x00000080



/////////////////////////////////////////////////////////////////////////////////////
//

// Structure for notifications from the system
// to the control implementation.
typedef struct tagTEventUI
{
   int Type;
   CUIControl* pSender;
   DWORD dwTimestamp;
   POINT ptMouse;
   TCHAR chKey;
   WORD wKeyState;
   WPARAM wParam;
   LPARAM lParam;
} TEventUI;

// Structure for notifications to the outside world
typedef struct tagTNotifyUI 
{
   UI_NOTIFY_TYPE nType;
   CUIControl* pSender;
   DWORD dwTimestamp;
   POINT ptMouse;
   WPARAM wParam;
   LPARAM lParam;
} TNotifyUI;

// Structure for adding alpha bitmaps on top of the window
typedef struct tagTPostPaintUI
{
   HBITMAP hBitmap;
   RECT rc;
   BYTE iAlpha;
} TPostPaintUI;

// System settings
typedef struct tagTSystemSettingsUI
{
   bool bShowKeyboardCues;
   bool bScrollLists;
} TSystemSettingsUI;

// Various system settings
typedef struct tagTSystemMetricsUI
{
   int cxvscroll;
   int cyvscroll;
} TSystemMetricsUI;

// Listener interface
class INotifyUI
{
public:
   virtual void Notify(TNotifyUI& msg) = 0;
};

// MessageFilter interface
class IMessageFilterUI
{
public:
   virtual LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled) = 0;
};

// Various system settings
typedef struct tagIconInfo
{
	UINT nWindth;
	Image* pImage;

	tagIconInfo()
	{
		nWindth = 16;
		pImage = NULL;
	}
} IconInfo;


/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API CUIPaintManager
{
public:
   CUIPaintManager();
   ~CUIPaintManager();

public:
   void Init(HWND hWnd);
   void UpdateLayout();
   void Invalidate(RECT rcItem);

   HDC GetPaintDC() const;
   HWND GetPaintWindow() const;

   POINT GetMousePos() const;
   SIZE GetClientSize() const;

   void SetMinMaxInfo(int cx, int cy);

   static HINSTANCE GetResourceInstance();
   static HINSTANCE GetLanguageInstance();
   static void SetResourceInstance(HINSTANCE hInst);
   static void SetLanguageInstance(HINSTANCE hInst);

   HPEN GetThemePen(UITYPE_COLOR Index) const;
   HFONT GetThemeFont(UITYPE_FONT Index) const;
   HBRUSH GetThemeBrush(UITYPE_COLOR Index) const;
   COLORREF GetThemeColor(UITYPE_COLOR Index) const;

   const TEXTMETRIC& GetThemeFontInfo(UITYPE_FONT Index) const;
   bool GetThemeColorPair(UITYPE_COLOR Index, COLORREF& clr1, COLORREF& clr2) const;

   bool AttachDialog(CUIControl* pControl);
   bool InitControls(CUIControl* pControl, CUIControl* pParent = NULL);
   void ReapObjects(CUIControl* pControl);

   CUIControl* GetFocus() const;
   void SetFocus(CUIControl* pControl);

   bool SetNextTabControl(bool bForward = true);

   bool SetTimer(CUIControl* pControl, UINT nTimerID, UINT uElapse);
   bool KillTimer(CUIControl* pControl, UINT nTimerID);

   bool AddNotifier(INotifyUI* pControl);
   bool RemoveNotifier(INotifyUI* pControl);   
   void SendNotify(TNotifyUI& Msg);
   void SendNotify(CUIControl* pControl, UI_NOTIFY_TYPE nMessage, WPARAM wParam = 0, LPARAM lParam = 0);

   bool AddMessageFilter(IMessageFilterUI* pFilter);
   bool RemoveMessageFilter(IMessageFilterUI* pFilter);

   bool AddPostPaintBlit(const TPostPaintUI& job);

   CUIControl* FindControl(POINT pt) const;
   CUIControl* FindControl(LPCTSTR pstrName);
   CUIControl* FindControl(UINT uID);

   static void MessageLoop();
   static bool TranslateMessage(const LPMSG pMsg);

   bool MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lRes);
   bool PreMessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lRes);

   TSystemMetricsUI GetSystemMetrics() const;
   TSystemSettingsUI GetSystemSettings() const;
   void SetSystemSettings(const TSystemSettingsUI Config);


	////////////////////////////////////////////////
   static bool SetImage(Image **pImage, UINT nResourceID, HMODULE hResHandle=NULL);
   static bool SetImage(Image **pImage, const BYTE* pData, UINT nSize);

   bool SetNormalSkin(UINT nResourceID, HMODULE hResHandle=NULL);
   bool SetFocusSkin(UINT nResourceID, HMODULE hResHandle=NULL);
   bool SetPushedSkin(UINT nResourceID, HMODULE hResHandle=NULL);
   bool SetDisabledSkin(UINT nResourceID, HMODULE hResHandle=NULL);

   bool SetNormalSkin(Image* apImage);
   bool SetFocusSkin(Image* apImage);
   bool SetPushedSkin(Image* apImage);
   bool SetDisabledSkin(Image* apImage);

   void SetEditBoxBKColor_Normal(COLORREF color);
   void SetEditBoxTextColor_Normal(COLORREF color);

   void SetEditBoxBKColor_Disable(COLORREF color);
   void SetEditBoxTextColor_Disable(COLORREF color);

   void SetEditBoxBoderColor_Normal(COLORREF colorLight, COLORREF colorDark = INVALID__UICOLOR);
   void SetEditBoxBoderColor_Focused(COLORREF colorLight, COLORREF colorDark = INVALID__UICOLOR);
   void SetEditBoxBoderColor_Disabled(COLORREF colorLight, COLORREF colorDark = INVALID__UICOLOR);
   

   static bool SetCommonDialogSkin(UINT nResourceID, HMODULE hResHandle=NULL);
   static bool SetCommonMenuSkin(UINT nResourceID, HMODULE hResHandle=NULL);
   static bool SetCommonComboSkin(UINT nResourceID, HMODULE hResHandle=NULL);
   static bool SetCommonButtonSkin(UINT nResourceID, HMODULE hResHandle=NULL);
   static bool SetCommonCloseButtonSkin(UINT nResourceID, HMODULE hResHandle=NULL);
   static bool SetCommonCheckBoxSkin(UINT nResourceID, HMODULE hResHandle=NULL);
   static bool SetCommonRadioBoxSkin(UINT nResourceID, HMODULE hResHandle=NULL);
   static bool SetCommonVScrollSkin(UINT nResourceID, HMODULE hResHandle=NULL);

   static bool SetCommonIcon(UINT nResourceID, UINT nIndex, UINT nWidth = 16, HMODULE hResHandle=NULL);
   static void ReleaseCommonSKin();

   void EmptyNameHashTable()
   {
	   m_aNameHash.Empty();

	   m_pFocus = NULL;
   }
public:

   Image*						m_pImageSkinNormal;
   Image*						m_pImageSkinFocused;
   Image*						m_pImageSkinPushed;
   Image*						m_pImageSkinDisabled;

   static Image*				m_pImageSkinMenu;
   static Image*				m_pImageSkinCombo;
   static Image*				m_pImageSkinCommonDialog;
   static Image*				m_pImageSkinCommonButton;
   static Image*				m_pImageSkinCloseButton;
   static Image*				m_pImageSkinCheckbox;
   static Image*				m_pImageSkinRadiobox;
   static Image*				m_pImageSkinVScroll;


   static std::map<UINT, IconInfo> m_mapSkinCommonIcon;

   ULONG_PTR					m_gdiplusToken;

private:
   static CUIControl* CALLBACK __FindControlFromNameHash(CUIControl* pThis, LPVOID pData);
   static CUIControl* CALLBACK __FindControlFromCount(CUIControl* pThis, LPVOID pData);
   static CUIControl* CALLBACK __FindControlFromPoint(CUIControl* pThis, LPVOID pData);
   static CUIControl* CALLBACK __FindControlFromTab(CUIControl* pThis, LPVOID pData);
   static CUIControl* CALLBACK __FindControlFromShortcut(CUIControl* pThis, LPVOID pData);

private:
   HWND m_hWndPaint;
   HDC m_hDcPaint;
   HDC m_hDcOffscreen;
   HBITMAP m_hbmpOffscreen;
   HWND m_hwndTooltip;
   TOOLINFO m_ToolTip;
   //
   CUIControl* m_pRoot;
   CUIControl* m_pFocus;
   CUIControl* m_pEventHover;
   CUIControl* m_pEventClick;
   CUIControl* m_pEventKey;
   //
   POINT m_ptLastMousePos;
   SIZE m_szMinWindow;
   UINT m_uMsgMouseWheel;
   UINT m_uTimerID;
   bool m_bFirstLayout;
   bool m_bResizeNeeded;
   bool m_bFocusNeeded;
   bool m_bOffscreenPaint;
   bool m_bMouseTracking;
   //
   TSystemMetricsUI m_SystemMetrics;
   TSystemSettingsUI m_SystemConfig;
   //
   CStdPtrArray m_aNotifiers;
   CStdPtrArray m_aNameHash;
   CStdPtrArray m_aTimers;
   CStdValArray m_aPostPaint;
   CStdPtrArray m_aMessageFilters;
   CStdPtrArray m_aDelayedCleanup;
   //
   static HINSTANCE m_hLangInst;
   static HINSTANCE m_hInstance;
   static CStdPtrArray m_aPreMessages;
};


/////////////////////////////////////////////////////////////////////////////////////
//

typedef CUIControl* (CALLBACK* FINDCONTROLPROC)(CUIControl*, LPVOID);

class UILIB_API CUIControl : public INotifyUI
{
public:
   CUIControl();
   virtual ~CUIControl();

public:
   virtual CStdString GetName() const;
   virtual void SetName(LPCTSTR pstrName);

   virtual UINT GetId() const;
   virtual void SetId(UINT nId);
   virtual void SetBoundary(int Num,...);
   virtual void SetBoundary(Point * Points,int Num);

   virtual LPVOID GetInterface(LPCTSTR pstrName);

   virtual bool Activate();
   virtual CUIControl* GetParent() const;

   virtual CStdString GetText() const;
   virtual void SetText(LPCTSTR pstrText);

   virtual CStdString GetToolTip() const;
   virtual void SetToolTip(LPCTSTR pstrText);

   virtual TCHAR GetShortcut() const;
   virtual void SetShortcut(TCHAR ch);

   virtual UINT_PTR GetTag() const;
   virtual void SetTag(UINT_PTR pTag);

   virtual void SetFocus();

   virtual bool IsVisible() const;
   virtual bool IsEnabled() const;
   virtual bool IsFocused() const;
   virtual void SetVisible(bool bVisible = true);
   virtual void SetEnabled(bool bEnable = true);

   virtual CUIControl* FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags);

   virtual bool IsCustomPoint(POINT point);

   virtual CUIPaintManager* GetManager() const;
   virtual void SetManager(CUIPaintManager* pManager, CUIControl* pParent);

   virtual RECT GetPos() const;
   virtual void SetPos(RECT rc);

   virtual UINT GetControlFlags() const;
   
   void Invalidate();
   void UpdateLayout();

   virtual void Init();
   virtual void Event(TEventUI& event);
   virtual void Notify(TNotifyUI& msg);

   virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
   CUIControl* ApplyAttributeList(LPCTSTR pstrList);

   virtual LPCTSTR GetClass() const = 0;
   virtual SIZE EstimateSize(SIZE szAvailable) = 0;
   virtual void DoPaint(HDC hDC, const RECT& rcPaint) = 0;

   bool SetTimer(UINT nTimerID, UINT uElapse);
   bool KillTimer(UINT nTimerID);

protected:
   CUIPaintManager* m_pManager;
   CUIControl* m_pParent;
   TCHAR m_chShortcut;
   CStdString m_sName;
   CStdString m_sText;
   CStdString m_sToolTip;
   UINT_PTR m_pTag;
   RECT m_rcItem;
   std::vector<Point> m_boundaryList;
   bool m_bVisible;
   bool m_bEnabled;
   bool m_bFocused;

   UINT m_nId;
};


#endif // !defined(AFX_UICONTROLS_H__20050423_DB94_1D69_A896_0080AD509054__INCLUDED_)

