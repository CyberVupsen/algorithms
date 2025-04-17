n, k = map(int, input().split())

smallest = []
count = 0

numbers = list(map(int, input().split()))

for x in numbers:
    if count < k:
        smallest.append(x)
        if count == k - 1:
            smallest.sort()
        count += 1
    else:
        if x < smallest[-1]:
            smallest[-1] = x 

            for i in range(k - 2, -1, -1):
                if smallest[i] > smallest[i+1]:
                    smallest[i], smallest[i+1] = smallest[i+1], smallest[i]
                else:
                    break

print(*smallest)
