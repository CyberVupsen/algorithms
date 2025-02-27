def is_allowable(k: int, mid: int, allowed_points) -> bool:
    count = 1
    start_index = 0
    for i in range(1, len(allowed_points)):
        if allowed_points[i] - allowed_points[start_index] > mid:
            if i == start_index + 1:
                return False
            else:
                count += 1
                start_index = i - 1
    return count <= k

def get_allowed_points(n, a):
    allowed_points = [0]
    for i in range(1, n):
        if a[i - 1] == '1':
            allowed_points.append(i)
    allowed_points.append(n)
    return allowed_points

def search_jumps(n, k, a):
    allowed_points = get_allowed_points(n, a)

    left = 1
    right = n
    answer = n

    while left <= right:
        mid = (left + right) // 2
        if is_allowable(k, mid, allowed_points):
            answer = mid
            right = mid - 1
        else:
            left = mid + 1

    return answer

n, k = map(int, input().split())
a = input()
print(search_jumps(n, k, a))