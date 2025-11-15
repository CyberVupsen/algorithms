from collections import deque

def count_shortest_paths(n, edges, s, t):

    adj = [[] for _ in range(n)]
    for u, v in edges:
        adj[u].append(v)
        adj[v].append(u)

    dist = [-1] * n
    ways = [0] * n

    q = deque([s])
    dist[s] = 0
    ways[s] = 1

    while q:
        u = q.popleft()
        for v in adj[u]:
            if dist[v] == -1:
                dist[v] = dist[u] + 1
                ways[v] = ways[u]
                q.append(v)
            elif dist[v] == dist[u] + 1:
                ways[v] += ways[u]

    return dist[t], ways[t]
