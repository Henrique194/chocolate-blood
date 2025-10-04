/*
 * Copyright (C) 2018, 2022 nukeykt
 *
 * This file is part of Blood-RE.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
#ifndef _GAMEMENU_H_
#define _GAMEMENU_H_

#include "typedefs.h"
#include "inifile.h"
#include "misc.h"
#include "qav.h"
#include "resource.h"

#define kMaxGameMenuItems 32
#define kMaxGameCycleItems 32
#define kMaxPicCycleItems 32

enum {
    kMenuEvent0 = 0,
    kMenuEvent1 = 1,
    kMenuEvent2 = 2,
    kMenuEvent3 = 3,
    kMenuEvent4 = 4,
    kMenuEvent5 = 5,
    kMenuEvent6 = 6,
    kMenuEvent7 = 7,
    kMenuEvent8 = 8,
    kMenuEventInit = 0x8000,
    kMenuEventDeInit = 0x8001,
};

struct CGameMenuEvent {
    ushort at0;
    byte at2;
};

class CMenuTextMgr
{
public:
    int at0;
    char __f_4[16];
    CMenuTextMgr();
    ~CMenuTextMgr();
    void DrawText(const char *pString, int nFont, int x, int y, int nShade, int nPalette, QBOOL shadow );
    void GetFontInfo(int nFont, const char *pString, int *pXSize, int *pYSize);
};

class CGameMenu;

enum {
    kMenuItemFlag0 = 1,
    kMenuItemFlag1 = 2,
    kMenuItemFlag2 = 4,
    kMenuItemFlag3 = 8,
};

class CGameMenuItem {
public:
    CGameMenu *pMenu;
    const char *at4;
    int at8;
    int atc;
    int at10;
    int at14;
    int at18;
    CGameMenuItem();
    virtual void Draw(void) = 0;
    virtual QBOOL Event(CGameMenuEvent &);
    void func_81920(CGameMenu *a1) {
        pMenu = a1;
    }
    void func_81910(char *a1) {
        at4 = a1;
    }
    void func_81900(int a1, int a2, int a3) {
        atc = a1;
        at10 = a2;
        at14 = a3;
    }
    void func_818F0(int a1) {
        at8 = a1;
    }
    QBOOL CanShow(void) {
        return (at18 & kMenuItemFlag0) ? 1 : 0;
    }
    QBOOL CanFocus(void) {
        return (at18 & kMenuItemFlag1) ? 1 : 0;
    }
    QBOOL Can2(void) {
        return (at18 & kMenuItemFlag2) ? 1 : 0;
    }
    QBOOL Can3(void) {
        return (at18 & kMenuItemFlag3) ? 1 : 0;
    }
    void Set0(void) {
        at18 |= kMenuItemFlag0;
    }
    void Set1(void) {
        at18 |= kMenuItemFlag1;
    }
    void Set2(void) {
        at18 |= kMenuItemFlag2;
    }
    void Set3(void) {
        at18 |= kMenuItemFlag3;
    }
    void Clear0(void) {
        at18 &= ~kMenuItemFlag0;
    }
    void Clear1(void) {
        at18 &= ~kMenuItemFlag1;
    }
    void Clear2(void) {
        at18 &= ~kMenuItemFlag2;
    }
    void Clear3(void) {
        at18 &= ~kMenuItemFlag3;
    }
};

class CGameMenuItemText : public CGameMenuItem
{
public:
    int at20;
    CGameMenuItemText();
    CGameMenuItemText(const char *, int, int, int, int);
    virtual void Draw(void);
    virtual QBOOL Event(CGameMenuEvent &);
    void SetVal(int a1) {
        at20 = a1;
    }
};

class CGameMenuItemTitle : public CGameMenuItem
{
public:
    int at20;
    CGameMenuItemTitle();
    CGameMenuItemTitle(const char *, int, int, int, int);
    virtual void Draw(void);
    virtual QBOOL Event(CGameMenuEvent &);
    void SetVal(int a1) {
        at20 = a1;
    }
};

class CGameMenuItemZBool : public CGameMenuItem
{
public:
    static const char *m_pzOnDefault;
    static const char *m_pzOffDefault;
    QBOOL at20;
    const char *at21;
    const char *at25;
    void (*at29)(CGameMenuItemZBool *);
    CGameMenuItemZBool();
    CGameMenuItemZBool(const char *,int,int,int,int,QBOOL,void (*)(CGameMenuItemZBool *),const char *,const char *);
    virtual void Draw(void);
    virtual QBOOL Event(CGameMenuEvent &);
};

class CGameMenuItemChain : public CGameMenuItem
{
public:
    int at20;
    CGameMenu *at24;
    int at28;
    void(*at2c)(CGameMenuItemChain *);
    int at30;
    CGameMenuItemChain();
    CGameMenuItemChain(const char *, int, int, int, int, int, CGameMenu *, int, void(*)(CGameMenuItemChain *), int);
    virtual void Draw(void);
    virtual QBOOL Event(CGameMenuEvent &);
};

class CGameMenuItem7EA1C : public CGameMenuItem
{
public:
    int at20; // text align
    CGameMenu *at24;
    int at28;
    void(*at2c)(CGameMenuItem7EA1C *);
    int at30;
    IniFile *at34;
    char at38[16];
    char at48[16];
    CGameMenuItem7EA1C();
    CGameMenuItem7EA1C(const char *a1, int a2, int a3, int a4, int a5, const char *a6, const char *a7, int a8, int a9, void(*a10)(CGameMenuItem7EA1C *), int a11);
    void Setup(void);
    virtual void Draw(void);
    virtual QBOOL Event(CGameMenuEvent &);
};

class CGameMenuItem7EE34 : public CGameMenuItem
{
public:
    int at20;
    int at24;
    CGameMenu *at28;
    CGameMenu *at2c;
    CGameMenuItem7EE34();
    CGameMenuItem7EE34(const char *a1, int a2, int a3, int a4, int a5, int a6);
    void Setup(void);
    virtual void Draw(void);
    virtual QBOOL Event(CGameMenuEvent &);
};

class CGameMenuItemChain7F2F0 : public CGameMenuItemChain
{
public:
    int at34;
    CGameMenuItemChain7F2F0();
    CGameMenuItemChain7F2F0(const char *a1, int a2, int a3, int a4, int a5, int a6, CGameMenu *a7, int a8, void(*a9)(CGameMenuItemChain *), int a10, int a11);
    virtual void Draw(void);
    virtual QBOOL Event(CGameMenuEvent &);
};

class CGameMenuItemBitmap : public CGameMenuItem
{
public:
    int at20;
    CGameMenuItemBitmap();
    CGameMenuItemBitmap(const char *, int, int, int, int);
    virtual void Draw(void);
    virtual QBOOL Event(CGameMenuEvent &);
};

class CGameMenuItemBitmapLS : public CGameMenuItemBitmap
{
public:
    int at24;
    int at28;
    CGameMenuItemBitmapLS();
    CGameMenuItemBitmapLS(const char *, int, int, int, int);
    virtual void Draw(void);
    virtual QBOOL Event(CGameMenuEvent &);
};

class CGameMenuItemKeyList : public CGameMenuItem
{
public:
    void(*at20)(CGameMenuItemKeyList *);
    int at24;
    int at28;
    int at2c;
    int at30;
    int at34;
    QBOOL at38;
    CGameMenuItemKeyList();
    CGameMenuItemKeyList(const char * a1, int a2, int a3, int a4, int a5, int a6, int a7, void(*a8)(CGameMenuItemKeyList *));
    void Scan(void);
    virtual void Draw(void);
    virtual QBOOL Event(CGameMenuEvent &);
};

class CGameMenuItemSlider : public CGameMenuItem
{
public:
    int *at20;
    int at24;
    int at28;
    int at2c;
    int at30;
    void(*at34)(CGameMenuItemSlider *);
    int at38;
    int at3c;
    CGameMenuItemSlider();
    CGameMenuItemSlider(const char *, int, int, int, int, int, int, int, int, void(*)(CGameMenuItemSlider *), int, int);
    CGameMenuItemSlider(const char *, int, int, int, int, int *, int, int, int, void(*)(CGameMenuItemSlider *), int, int);
    void SetValue(int value)
    {
        at24 = ClipRange(value, at28, at2c);
    }
    virtual void Draw(void);
    virtual QBOOL Event(CGameMenuEvent &);
};

class CGameMenuItemZEdit : public CGameMenuItem
{
public:
    char *at20;
    int at24;
    int at28;
    void(*at2c)(CGameMenuItemZEdit *, CGameMenuEvent *);
    QBOOL at30;
    QBOOL at31;
    QBOOL at32;
    CGameMenuItemZEdit();
    CGameMenuItemZEdit(const char *, int, int, int, int, char *, int, QBOOL, void(*)(CGameMenuItemZEdit *, CGameMenuEvent *), int);
    void AddChar(char);
    void BackChar(void);
    virtual void Draw(void);
    virtual QBOOL Event(CGameMenuEvent &);
};

class CGameMenuItemZEditBitmap : public CGameMenuItem
{
public:
    char *at20;
    int at24;
    int at28;
    CGameMenuItemBitmapLS *at2c;
    void(*at30)(CGameMenuItemZEditBitmap *, CGameMenuEvent *);
    QBOOL at34;
    QBOOL at35;
    QBOOL at36;
    QBOOL at37;
    CGameMenuItemZEditBitmap();
    CGameMenuItemZEditBitmap(const char *, int, int, int, int, char *, int, char, void(*)(CGameMenuItemZEditBitmap *, CGameMenuEvent *), int);
    void AddChar(char);
    void BackChar(void);
    virtual void Draw(void);
    virtual QBOOL Event(CGameMenuEvent &);
};

class CGameMenuItemQAV : public CGameMenuItem
{
public:
    const char *at20;
    DICTNODE *at24;
    QAV *at28;
    int at2c;
    int at30;
    CGameMenuItemQAV();
    CGameMenuItemQAV(const char *, int, int, int, const char *);
    virtual void Draw(void);
    virtual QBOOL Event(CGameMenuEvent &);
    void Reset(void);
};

class CGameMenuItemZCycle : public CGameMenuItem
{
public:
    int m_nItems;
    int at24;
    int at28;
    int at2c;
    const char **at30;
    const char *at34[kMaxGameCycleItems];
    void(*atb4)(CGameMenuItemZCycle *);
    CGameMenuItemZCycle();
    CGameMenuItemZCycle(const char *, int, int, int, int, int, void(*)(CGameMenuItemZCycle *), const char **, int, int);
    ~CGameMenuItemZCycle();
    virtual void Draw(void);
    virtual QBOOL Event(CGameMenuEvent &);
    void Add(const char *, QBOOL);
    void Next(void);
    void Prev(void);
    void Clear(void);
    void SetTextArray(const char **, int, int);
    void SetTextIndex(int);
};

class CGameMenuItemYesNoQuit : public CGameMenuItem
{
public:
    int at20;
    int at24;
    int at28;
    CGameMenuItemYesNoQuit();
    CGameMenuItemYesNoQuit(const char *, int, int, int, int, int, int, int);
    virtual void Draw(void);
    virtual QBOOL Event(CGameMenuEvent &);
};

class CGameMenuItemPicCycle : public CGameMenuItem
{
public:
    int m_nItems;
    int at24;
    int at28;
    int at2c;
    int at30[kMaxPicCycleItems];
    void(*atb0)(CGameMenuItemPicCycle *);
    int atb4;
    CGameMenuItemPicCycle();
    CGameMenuItemPicCycle(int, int, void(*)(CGameMenuItemPicCycle *), int *, int, int);
    virtual void Draw(void);
    virtual QBOOL Event(CGameMenuEvent &);
    void Add(int, QBOOL);
    void Next(void);
    void Prev(void);
    void Clear(void);
    void SetPicArray(int *, int, int);
    void SetPicIndex(int);
};

class CGameMenuItemPassword : public CGameMenuItem
{
public:
    char at20[9];
    char at29[9];
    int at32;
    char at36;
    int at37;
    char at3b[32];
    int at5b;
    CGameMenuItemZBool *at5f;
    CGameMenuItemPassword();
    CGameMenuItemPassword(const char *, int, int, int);
    virtual void Draw(void);
    virtual QBOOL Event(CGameMenuEvent &);
};

class CGameMenu
{
public:
    int m_nItems;
    int m_nFocus;
    int at8;
    QBOOL atc;
    CGameMenuItem *pItemList[kMaxGameMenuItems]; // atd
    CGameMenu();
    CGameMenu(int);
    ~CGameMenu();
    void InitializeItems(CGameMenuEvent &event);
    void Draw(void);
    QBOOL Event(CGameMenuEvent &event);
    void Add(CGameMenuItem *pItem, QBOOL active);
    void SetFocusItem(int nItem);
    QBOOL CanSelectItem(int nItem);
    void FocusPrevItem(void);
    void FocusNextItem(void);
    QBOOL IsFocusItem(CGameMenuItem *pItem);
};

class CGameMenuMgr
{
public:
    static QBOOL m_bInitialized;
    static QBOOL m_bActive;
    CGameMenu *pActiveMenu;
    CGameMenu *pMenuStack[8];
    int nMenuPointer;
    CGameMenuMgr();
    ~CGameMenuMgr();
    void InitializeMenu(void);
    void func_7DF1C(void);
    QBOOL Push(CGameMenu *pMenu, int data = -1);
    void Pop(void);
    void Draw(void);
    void Clear(void);
    void Process(void);
    void Deactivate(void);
    static QBOOL Active() { return m_bActive; };
};

extern CMenuTextMgr gMenuTextMgr;
extern CGameMenuMgr gGameMenuMgr;

#endif
