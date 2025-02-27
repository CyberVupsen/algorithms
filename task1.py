class Solution:
    def findMedianSortedArrays(self, nums1, nums2):
            nums1 += nums2
            nums1 = sorted(nums1)
            
            if len(nums1) % 2 == 0:
                n = len(nums1) // 2
                return (nums1[n - 1] + nums1[n]) / 2
            else:
                n = (len(nums1) // 2) + 1
                return nums1[n - 1]
        

nums1 = list(map(int, input().split()))
nums2 = list(map(int, input().split()))
s = Solution()
print(s.findMedianSortedArrays(nums1, nums2))