def make_strongly_connected(n, edges):
    g = [[] for _ in range(n)]
    gr = [[] for _ in range(n)]
    for u, v in edges:
        g[u].append(v)
        gr[v].append(u)

    used = [False] * n
    order = []

    for start in range(n):
        if used[start]:
            continue
        stack = [(start, 0)]
        used[start] = True
        while stack:
            v, state = stack.pop()
            if state == 0:
                # первый раз входим в v
                stack.append((v, 1))  # после детей
                for to in g[v]:
                    if not used[to]:
                        used[to] = True
                        stack.append((to, 0))
            else:
                # все дети обработаны
                order.append(v)

    comp = [-1] * n
    current = 0

    for v in reversed(order):
        if comp[v] != -1:
            continue

        stack = [v]
        comp[v] = current
        while stack:
            u = stack.pop()
            for to in gr[u]:
                if comp[to] == -1:
                    comp[to] = current
                    stack.append(to)
        current += 1

    if current == 1:
        return 0

    indeg = [0] * current
    outdeg = [0] * current

    for u, v in edges:
        cu, cv = comp[u], comp[v]
        if cu != cv:
            outdeg[cu] += 1
            indeg[cv] += 1

    sources = sum(1 for i in range(current) if indeg[i] == 0)
    sinks   = sum(1 for i in range(current) if outdeg[i] == 0)

    return max(sources, sinks)
