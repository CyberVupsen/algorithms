s = list(map(int, input().split()))
pairs = 0
for j in range(1, len(s)):
    for i in range(j):
        if s[i] > s[j]:
            pairs += 1
            
print(pairs)
