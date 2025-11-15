from collections import deque

def validArrangement(pairs):
    graph = {}
    out_degree = {}
    in_degree = {}
    
    for u, v in pairs:
        if u not in graph:
            graph[u] = []
        graph[u].append(v)
        
        out_degree[u] = out_degree.get(u, 0) + 1
        in_degree[v] = in_degree.get(v, 0) + 1
    
    start = pairs[0][0]
    all_nodes = set(out_degree.keys()) | set(in_degree.keys())
    
    for node in all_nodes:
        if out_degree.get(node, 0) == in_degree.get(node, 0) + 1:
            start = node
            break
    
    next_edge = {}
    for node in all_nodes:
        next_edge[node] = deque(graph.get(node, []))
    
    path = []
    stack = [start]
    
    while stack:
        current = stack[-1]
        if next_edge.get(current):
            next_node = next_edge[current].popleft()
            stack.append(next_node)
        else:
            path.append(stack.pop())
    
    path.reverse()
    
    return [[path[i], path[i + 1]] for i in range(len(path) - 1)]
