# Definition for singly-linked list.
# class ListNode:
#     def __init__(self, val=0, next=None):
#         self.val = val
#         self.next = next
class Solution:
    def deleteDuplicates(self, head: Optional[ListNode]) -> Optional[ListNode]:
        
        if not head or not head.next:
            return head

        _head = ListNode(-1)
        _head.next = head
        current = _head

        while current.next and current.next.next:
            if current.next.val == current.next.next.val:
                duplicate_val = current.next.val

                while current.next and current.next.val == duplicate_val:
                    current.next = current.next.next
            else:
                current = current.next

        return _head.next
