#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *head = malloc(sizeof(struct list_head));
    if (head) {
        INIT_LIST_HEAD(head);
        return head;
    } else {
        return NULL;
    }
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (!l)
        return;
    while (!list_empty(l)) {
        element_t *target = list_first_entry(l, element_t, list);
        list_del(l->next);
        q_release_element(target);
    }
    free(l);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *newh = malloc(sizeof(element_t));
    if (!newh)
        return false;
    int len = strlen(s);
    newh->value = malloc(sizeof(char) * (len + 1));
    if (!newh->value) {
        free(newh);
        return false;
    }
    strncpy(newh->value, s, len + 1);
    list_add(&newh->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *newh = malloc(sizeof(element_t));
    if (!newh)
        return false;
    int len = strlen(s);
    newh->value = malloc(sizeof(char) * (len + 1));
    if (!newh->value) {
        free(newh);
        return false;
    }
    strncpy(newh->value, s, len + 1);
    list_add_tail(&newh->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head) || !sp)
        return NULL;
    element_t *target = list_first_entry(head, element_t, list);
    list_del(head->next);
    int len = strlen(target->value);
    if (len > (bufsize - 1)) {
        strncpy(sp, target->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    } else {
        strncpy(sp, target->value, len);
        sp[len] = '\0';
    }
    return target;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head) || !sp)
        return NULL;
    element_t *target = list_last_entry(head, element_t, list);
    list_del(head->prev);
    int len = strlen(target->value);
    if (len > (bufsize - 1)) {
        strncpy(sp, target->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    } else {
        strncpy(sp, target->value, len);
        sp[len] = '\0';
    }
    return target;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;
    struct list_head *node = NULL;
    int size = 0;
    list_for_each (node, head) {
        size++;
    }
    return size;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    if (!head || list_empty(head))
        return false;
    struct list_head *next_node = head->next, *prev_node = head->prev;
    while (next_node != prev_node && next_node->next != prev_node) {
        next_node = next_node->next;
        prev_node = prev_node->prev;
    }
    element_t *target = list_entry(prev_node, element_t, list);
    list_del(prev_node);
    q_release_element(target);
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return false;
    bool dup = false;
    char *value = NULL;
    element_t *entry = NULL, *safe = NULL;
    list_for_each_entry_safe (entry, safe, head, list) {
        if (value && strcmp(value, entry->value) == 0) {
            list_del(&entry->list);
            q_release_element(entry);
            dup = true;
        } else {
            value = entry->value;
            if (dup) {
                element_t *target =
                    list_entry(entry->list.prev, element_t, list);
                list_del(&target->list);
                q_release_element(target);
                dup = false;
            }
        }
    }
    if (dup) {
        element_t *target = list_entry(head->prev, element_t, list);
        list_del(&target->list);
        q_release_element(target);
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    if (!head || list_is_singular(head))
        return;
    struct list_head *node = head->next, *from = NULL, *to = NULL, *next = NULL;
    while ((node != head) && (node->next != head)) {
        next = node->next;
        from = node->prev;
        to = node->next->next;
        from->next = next;
        next->prev = from;
        next->next = node;
        node->prev = next;
        node->next = to;
        to->prev = node;
        node = to;
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head))
        return;
    struct list_head *node = NULL, *safe = NULL, *temp = NULL;
    list_for_each_safe (node, safe, head) {
        temp = node->next;
        node->next = node->prev;
        node->prev = temp;
    }
    temp = head->next;
    head->next = head->prev;
    head->prev = temp;
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    if (!head || list_empty(head))
        return;
    struct list_head *n, *s, iter, *tmp_head = head;
    int i = 0;
    INIT_LIST_HEAD(&iter);
    list_for_each_safe (n, s, head) {
        i++;
        if (i == k) {
            list_cut_position(&iter, tmp_head, n);
            q_reverse(&iter);
            list_splice_init(&iter, tmp_head);
            i = 0;
            tmp_head = s->prev;
        }
    }
}

/* Merge two sorted lists */
void mergetwolist(struct list_head *head1,
                  struct list_head *head2,
                  struct list_head *head)
{
    if (!head || !head1 || !head2 || (list_empty(head1) && list_empty(head2)))
        return;
    if (list_empty(head1)) {
        list_splice_tail_init(head2, head);
        return;
    }
    if (list_empty(head2)) {
        list_splice_tail_init(head1, head);
        return;
    }
    struct list_head *temp = NULL, *l1 = head1->next, *l2 = head2->next;
    while (l1 != head1 && l2 != head2) {
        if (strcmp(list_entry(l1, element_t, list)->value,
                   list_entry(l2, element_t, list)->value) > 0) {
            temp = l2;
            l2 = l2->next;
            list_move_tail(temp, head);
        } else {
            temp = l1;
            l1 = l1->next;
            list_move_tail(temp, head);
        }
    }
    list_splice_tail_init(head1, head);
    list_splice_tail_init(head2, head);
}

/* Implement the merge sort algorithm */
void mergesort(struct list_head *head, int size)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;
    LIST_HEAD(head1);
    LIST_HEAD(head2);
    int mid = size >> 1, count = 0;
    struct list_head *node = NULL, *safe = NULL;
    list_for_each_safe (node, safe, head) {
        count++;
        if (count == mid) {
            list_cut_position(&head1, head, node);
            list_splice_tail_init(head, &head2);
            break;
        }
    }
    mergesort(&head1, mid);
    mergesort(&head2, size - mid);
    mergetwolist(&head1, &head2, head);
}

/* Sort elements of queue in ascending order */
void q_sort(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;
    mergesort(head, q_size(head));
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return q_size(head);
    struct list_head *curr = head->prev->prev, *node = head->prev;
    while (curr != head) {
        element_t *curr_ele = list_entry(curr, element_t, list);
        if (strcmp(list_entry(node, element_t, list)->value, curr_ele->value) >
            0) {
            list_del(curr);
            q_release_element(curr_ele);
        } else {
            node = curr;
        }
        curr = node->prev;
    }
    return q_size(head);
}

/* Merge all the queues into one sorted queue, which is in ascending order */
int q_merge(struct list_head *head)
{
    if (!head)
        return 0;
    queue_contex_t *curr = NULL, *safe = NULL;
    LIST_HEAD(sorted);
    LIST_HEAD(temp);
    int size = 0;
    list_for_each_entry_safe (curr, safe, head, chain) {
        size += curr->size;
        mergetwolist(&sorted, curr->q, &temp);
        list_splice_init(&temp, &sorted);
        INIT_LIST_HEAD(curr->q);
        curr->size = 0;
    }
    curr = list_first_entry(head, queue_contex_t, chain);
    curr->size = size;
    list_splice(&sorted, curr->q);
    return curr->size;
}
