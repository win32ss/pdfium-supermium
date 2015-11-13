// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef _XFA_LAYOUT_APPADAPTER_H_
#define _XFA_LAYOUT_APPADAPTER_H_
class CXFA_TraverseStrategy_PageAreaContainerLayoutItem {
 public:
  static inline CXFA_ContainerLayoutItem* GetFirstChild(
      CXFA_ContainerLayoutItem* pLayoutItem) {
    if (pLayoutItem->m_pFormNode->GetClassID() == XFA_ELEMENT_PageSet) {
      return (CXFA_ContainerLayoutItem*)pLayoutItem->m_pFirstChild;
    }
    return NULL;
  }
  static inline CXFA_ContainerLayoutItem* GetNextSibling(
      CXFA_ContainerLayoutItem* pLayoutItem) {
    return (CXFA_ContainerLayoutItem*)pLayoutItem->m_pNextSibling;
  }
  static inline CXFA_ContainerLayoutItem* GetParent(
      CXFA_ContainerLayoutItem* pLayoutItem) {
    return (CXFA_ContainerLayoutItem*)pLayoutItem->m_pParent;
  }
};
class CXFA_TraverseStrategy_ContentAreaContainerLayoutItem {
 public:
  static inline CXFA_ContainerLayoutItem* GetFirstChild(
      CXFA_ContainerLayoutItem* pLayoutItem) {
    for (CXFA_LayoutItem* pChildItem = pLayoutItem->m_pFirstChild; pChildItem;
         pChildItem = pChildItem->m_pNextSibling) {
      if (pChildItem->IsContentLayoutItem()) {
        continue;
      }
      return (CXFA_ContainerLayoutItem*)pChildItem;
    }
    return NULL;
  }
  static inline CXFA_ContainerLayoutItem* GetNextSibling(
      CXFA_ContainerLayoutItem* pLayoutItem) {
    for (CXFA_LayoutItem* pChildItem = pLayoutItem->m_pNextSibling; pChildItem;
         pChildItem = pChildItem->m_pNextSibling) {
      if (pChildItem->IsContentLayoutItem()) {
        continue;
      }
      return (CXFA_ContainerLayoutItem*)pChildItem;
    }
    return NULL;
  }
  static inline CXFA_ContainerLayoutItem* GetParent(
      CXFA_ContainerLayoutItem* pLayoutItem) {
    return (CXFA_ContainerLayoutItem*)pLayoutItem->m_pParent;
  }
};
class CXFA_TraverseStrategy_ContentLayoutItem {
 public:
  static inline CXFA_ContentLayoutItem* GetFirstChild(
      CXFA_ContentLayoutItem* pLayoutItem) {
    return (CXFA_ContentLayoutItem*)pLayoutItem->m_pFirstChild;
  }
  static inline CXFA_ContentLayoutItem* GetNextSibling(
      CXFA_ContentLayoutItem* pLayoutItem) {
    return (CXFA_ContentLayoutItem*)pLayoutItem->m_pNextSibling;
  }
  static inline CXFA_ContentLayoutItem* GetParent(
      CXFA_ContentLayoutItem* pLayoutItem) {
    return (CXFA_ContentLayoutItem*)pLayoutItem->m_pParent;
  }
};
FX_DWORD XFA_GetRelevant(CXFA_Node* pFormItem, FX_DWORD dwParentRelvant);
void XFA_ReleaseLayoutItem(CXFA_LayoutItem* pLayoutItem);
#endif
