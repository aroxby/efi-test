#define getNodeData(TYPE, NODE) (*(TYPE*)((NODE)->data))
#define createDataNode(TYPE, DATA) (createNode((&(DATA)), sizeof(TYPE)))
#define insertData(LIST, TYPE, DATA) (insertBefore((LIST), (createDataNode(TYPE, DATA))))

typedef struct _ListNode {
    struct _ListNode *next;
    struct _ListNode *prev;
    void *data;
} ListNode;

ListNode *createList();
ListNode *insertBefore(ListNode *head, ListNode *toInsert);
void removeNode(ListNode *drop);
void deleteNode(ListNode *node);
ListNode *createNode(void *data, UINTN size);