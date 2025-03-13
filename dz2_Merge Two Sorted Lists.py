# Definition for singly-linked list.
# class ListNode:
#     def __init__(self, val=0, next=None):
#         self.val = val
#         self.next = next
class Solution:
    def mergeTwoLists(self, list1: Optional[ListNode], list2: Optional[ListNode]) -> Optional[ListNode]:
        
    _node = ListNode(-1)
    current_node = _node

    while l1 and l2:
        if l1.val < l2.val:
            current_node.next = l1
            l1 = l1.next
        else:
            current_node.next = l2
            l2 = l2.next
        current_node = current_node.next

    if l1:
        current_node.next = l1
    elif l2:
        current_node.next = l2

    return _node.next
