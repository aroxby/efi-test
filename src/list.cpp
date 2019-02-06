#ifdef __cplusplus
    extern "C" {
#endif

#include <efi.h>
#include <efilib.h>

#ifdef __cplusplus
}
#endif

#include "list.h"

ListNode *insertBefore(ListNode *head, ListNode *toInsert) {
    if(head->prev) {
        toInsert->prev = head->prev;
        head->prev->next = toInsert;
    }
    head->prev = toInsert;
    toInsert->next = head;
    return toInsert;
}

ListNode *insertAfter(ListNode *tail, ListNode *toInsert) {
    if(tail->next) {
        toInsert->next = tail->next;
        tail->next->prev = toInsert;
    }
    tail->next = toInsert;
    toInsert->prev = tail;
    return toInsert;
}

void removeNode(ListNode *drop) {
    if(drop->prev) {
        drop->prev->next = drop->next;
    }
    if(drop->next) {
        drop->next->prev = drop->prev;
    }
}

void deleteNode(ListNode *node) {
    if(node->data) {
        FreePool(node->data);
    }
    FreePool(node);
}

ListNode *createNode(void *data, UINTN size) {
    ListNode *newNode = AllocateZeroPool(sizeof(ListNode));
    newNode->data = AllocateZeroPool(size);
    CopyMem(newNode->data, data, size);
    return newNode;
}