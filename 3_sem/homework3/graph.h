#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <unordered_set>
#include <functional>
#include <stack>
#include <queue>
#include <exception>

#include <limits>

enum Color
{
    WHITE = 0,
    GREY,
    BLACK
};

/*
 * 1) Цикл для undir с помощью visited.
 * 2) Проверка существования маршрута между 2 вершинами.
 * 3) Проверка принадлежности 2 вершин одной компоненте связности.
 * 4) Вывод маршрута между 2 вершинами.
 *
*/

inline bool undir_has_cycle(const std::vector<std::vector<int>> &g)
{
    std::vector<Color> color(g.size(),WHITE);
    std::function<bool(int,int)> dfs = [&](int v,int parent_v)
    {
        color[v] = GREY;
        for(int u : g[v])
            if( u != parent_v && (color[u] == GREY || dfs(u,v)) )
                return true;
        color[v] = BLACK;
        return false;
    };
    for(int v = 0; v < g.size() ; ++v)
        if(color[v] == WHITE && dfs(v,-1))
            return true;
    return false;
}


inline bool dir_has_cycle(const std::vector<std::vector<int>> &g)
{
    std::vector<Color> color(g.size(),WHITE);
    std::function<bool(int)> dfs = [&](int v)
    {
        color[v] = GREY;//v = 2
        for(int u : g[v])
            if(color[u] == GREY || dfs(u))
                return true;
        color[v] = BLACK;
        return false;
    };
    for(int v = 0; v < g.size() ; ++v)
        if(color[v] == WHITE && dfs(v))
            return true;
    return false;
}

inline std::vector<int> traversal_forest(const std::vector<std::vector<int>> &g)
{
    std::vector<bool> visited(g.size());
    std::vector<int> parent(g.size(),-1);
    std::function<void(int,int)> dfs = [&](int v, int parent_v)
    {
        visited[v] = true;
        parent[v] = parent_v;
        for(int u : g[v])
            if(!visited[u])
                dfs(u,v);
    };
    for(int v = 0; v < g.size() ; ++v)
        if(!visited[v])
            dfs(v,-1);
    return parent;
}

inline bool has_way(const std::vector<std::vector<int>> &g, int v0 , int v1)
{
    std::vector<bool> visited(g.size());
    std::function<bool(int)> dfs = [&](int v)
    {
        if(v == v1)
            return true;
        visited[v] = true;
        for(int u : g[v])
            if(!visited[u] && dfs(u))
                return true;
        return false;
    };
    return dfs(v0);
}

inline std::vector<int> way_from_tree(const std::vector<int>&parent ,
                                      int v0,
                                      int v1)
{
    if(v0 == v1)
        return {v0,};
    std::vector<int> way;
    int v = v1;
    while(v != v0)
    {
        way.push_back(v);
        v = parent[v];
    }
    way.push_back(v);
    std::reverse(way.begin(),way.end());
    return way;
}

inline std::vector<int> dfs_way(const std::vector<std::vector<int>> &g, int v0 , int v1)
{
    std::vector<bool> visited(g.size());
    std::vector<bool> parent(g.size(),-1);
    std::function<bool(int,int)> dfs = [&](int v,int parent_v)
    {
        visited[v] = true;
        parent[v] = parent_v;
        if(v == v1)
            return true;
        for(int u : g[v])
            if(!visited[u] && dfs(u,v))
                return true;
        return false;
    };    
    if(dfs(v0,-1))
    {
        std::vector<int> way;
        int v = v1;
        while(v != v0)
        {
            way.push_back(v);
            v = parent[v];
        }
        way.push_back(v);
        std::reverse(way.begin(),way.end());
        return way;
    }
    return {};
}


inline bool has_way_V1(const std::vector<std::vector<int>> &g, int v0 , int v1)
{
    std::unordered_set<int> visited;//std::set
    std::function<bool(int)> dfs = [&](int v)
    {
        if(v == v1)
            return true;
        visited.insert(v);
        for(int u : g[v])
            if(!visited.count(u) && dfs(u))
                return true;
        return false;
    };
    return dfs(v0);
}

inline std::vector< int > undir_components(const std::vector<std::vector<int>> &g)
{
    std::vector<bool> visited(g.size());
    std::vector<int> components(g.size());
    int ci = 0;
    std::function<void(int)> dfs = [&](int v)
    {
        visited[v] = true;
        components[v] = ci;
        for(int u : g[v])
            if(visited[u] == false)
                dfs(u);
    };
    for(int v = 0; v < g.size() ; ++v)
    {
        if(visited[v] == false)
        {
            dfs(v);
            ++ci;
        }
    }
    return components;
}


inline void dir_vertex_times(const std::vector<std::vector<int>> &g,
                             std::vector<int> &open_times,
                             std::vector<int> &close_times)
{
    std::vector<Color> color(g.size(),WHITE);
    open_times.resize(g.size());
    close_times.resize(g.size());
    int time = 0;
    std::function<void(int)> dfs = [&](int v)
    {
        open_times[v] = time++;
        color[v] = GREY;
        for(int u : g[v])
            if(color[u] == WHITE)
                dfs(u);
        color[v] = BLACK;
        close_times[v] = time++;
    };
    for(int v = 0; v < g.size() ; ++v)
        if(color[v] == WHITE)
            dfs(v);
}


inline std::vector<int> topological_sort(const std::vector<std::vector<int>> &g, bool check_cycle = true)
{
    if(check_cycle && dir_has_cycle(g))
        return {};
    std::vector<bool> visited(g.size());
    std::vector<int> sorted;
    std::function<void(int)> dfs = [&](int v)
    {
        visited[v] = true;
        for(int u : g[v])
            if(!visited[u])
                dfs(u);
        sorted.push_back(v);
    };
    for(int v = 0; v < g.size() ; ++v)
        if(!visited[v])
            dfs(v);
    std::reverse(sorted.begin(),sorted.end());
    return sorted;
}


inline std::vector<std::vector<int>> transposed(const std::vector<std::vector<int>> &g)
{
    std::vector<std::vector<int>> tg(g.size());
    for(int v = 0; v < g.size() ; ++v)
        for(int u : g[v])
            tg[u].push_back(v);
    return tg;
}


inline std::vector< int > dir_strong_components(const std::vector<std::vector<int>> &g)
{
    auto sorted = topological_sort(transposed(g),false);
    std::vector<bool> visited(g.size());
    std::vector<int> components(g.size());
    int ci = 0;
    std::function<void(int)> dfs = [&](int v)
    {
        visited[v] = true;
        components[v] = ci;
        for(int u : g[v])
            if(visited[u] == false)
                dfs(u);
    };
    for(int i = 0; i < sorted.size() ; ++i)
    {
        int v = sorted[i];
        if(visited[v] == false)
        {
            dfs(v);
            ++ci;
        }
    }
    return components;
}


inline std::vector< int > dir_strong_components_tar(const std::vector<std::vector<int>> &g)
{
    std::vector<Color> color(g.size());
    std::vector<int> ot(g.size());
    std::vector<int> rt(g.size());
    std::vector<int> components(g.size());
    std::stack<int> comp_st;
    int ci = 0;
    int time = 1;//or 0
    std::function<void(int)> dfs = [&](int v)
    {
        color[v] = GREY;
        ot[v] = time++;
        rt[v] = ot[v];
        comp_st.push(v);
        for(int u : g[v])
        {
            if(color[u] == GREY)
                rt[v] = std::min(rt[v],ot[u]);
            else if(color[u] == WHITE)
            {
                dfs(u);
                rt[v] = std::min(rt[v],rt[u]);
            }
        }
        color[v] = BLACK;
        ++time;
        if(rt[v] == ot[v])
        {
            while(v != comp_st.top())
            {
                components[comp_st.top()] = ci;
                comp_st.pop();
            }
            components[comp_st.top()] = ci;
            comp_st.pop();
            ++ci;
        }
    };
    for(int v = 0; v < g.size() ; ++v)
        if(color[v] == WHITE)
            dfs(v);
    return components;
}


inline std::vector< std::pair<int,int> > bridges(const std::vector<std::vector<int>> &g)
{
    std::vector<Color> color(g.size());
    std::vector<int> ot(g.size());//open_times
    std::vector<int> rt(g.size());//result
    std::vector< std::pair<int,int> > bridges;
    int time = 1;//or 0
    std::function<void(int,int)> dfs = [&](int v,int parent_v)
    {
        color[v] = GREY;
        ot[v] = time++;
        rt[v] = ot[v];
        for(int u : g[v])
        {
            if(u == parent_v)
                continue;
            if(color[u] == GREY)
                rt[v] = std::min(rt[v],ot[u]);
            else if(color[u] == WHITE)
            {
                dfs(u,v);
                rt[v] = std::min(rt[v],rt[u]);
                if(ot[v] < rt[u])
                    bridges.push_back({v,u});
            }
        }
        color[v] = BLACK;
    };
    for(int v = 0; v < g.size() ; ++v)
        if(color[v] == WHITE)
            dfs(v,-1);//v - root
    return bridges;
}

inline std::vector< int > bi_edge_components(const std::vector<std::vector<int>> &g){
    std::vector< std::pair<int,int> > br = ::bridges(g);
    using edge = std::pair<int,int>;
    std::unordered_set<edge, std::function<std::size_t(edge)>> briges(br.begin(),br.end(),0,[](edge p){
        return p.first * 31 + p.second;
    });
    std::vector<bool> visited(g.size());
    std::vector<int> components(g.size());
    int ci = 0;
    std::function<void(int)> dfs = [&](int v)
    {
        visited[v] = true;
        components[v] = ci;
        for(int u : g[v])
            if(visited[u] == false && briges.count({v,u}) == 0 && briges.count({u,v}) == 0)
                dfs(u);
    };
    for(int v = 0; v < g.size() ; ++v)
    {
        if(visited[v] == false)
        {
            dfs(v);
            ++ci;
        }
    }
    return components;
}



inline std::unordered_set< int > articulation_points(const std::vector<std::vector<int>> &g)
{
    std::vector<Color> color(g.size());
    std::vector<int> ot(g.size());//open_times
    std::vector<int> rt(g.size());//result
    std::unordered_set< int > articulation_points;
    int time = 0;//or 1
    std::function<void(int,int)> dfs = [&](int v,int parent_v)
    {
        color[v] = GREY;
        ot[v] = time++;
        rt[v] = ot[v];
        int dfs_count = 0;
        for(int u : g[v])
        {
            if(u == parent_v)
                continue;
            if(color[u] == GREY)
                rt[v] = std::min(rt[v],ot[u]);
            else if(color[u] == WHITE)
            {
                dfs(u,v);
                ++dfs_count;
                rt[v] = std::min(rt[v],rt[u]);
                if(ot[v] <= rt[u] && parent_v != -1)
                    articulation_points.insert(v);
            }
        }
        if(dfs_count >= 2 && parent_v == -1)
            articulation_points.insert(v);
        color[v] = BLACK;
        ++time;
    };
    for(int v = 0; v < g.size() ; ++v)
        if(color[v] == WHITE)
            dfs(v,-1);//v - root
    return articulation_points;
}

bool is_eiler_graph_undir(const std::vector<std::vector<int>> &g)
{
    int odd_vertex = 0;
    for(auto &a : g)
        if(a.size() % 2)
            ++odd_vertex;
    if(odd_vertex > 2)
        return false;
    std::vector<bool> visited(g.size());
    std::function<void(int)> dfs = [&](int v)
    {
        visited[v] = true;
        for(int u: g[v])
            if(!visited[u])
                dfs(u);
    };
    for(int v = 0; v < g.size() ; ++v)
    {
        if(g[v].size())
        {
            dfs(v);
            break;
        }
    }
    for(int v = 0; v < g.size() ; ++v)
        if(g[v].size() && !visited[v])
            return false;
    return true;
}


inline std::vector<int> eiler_path_undir(const std::vector<std::vector<int>> &g)
{
    std::vector<int> path;
    if(!is_eiler_graph_undir(g))
        return path;
    std::vector<std::unordered_set<int>> visited(g.size());
    std::function<void(int)> dfs = [&](int v)
    {
        for(int u: g[v])
        {
            if(!visited[v].count(u))
            {
                visited[v].insert(u);
                visited[u].insert(v);
                dfs(u);
            }
        }
        path.push_back(v);
    };

    for(int v = 0; v < g.size() ; ++v)
    {
        if(g[v].size() % 2)
        {
            dfs(v);
            return path;
        }
    }

    for(int v = 0; v < g.size() ; ++v)
    {
        if(g[v].size())
        {
            dfs(v);
            return path;
        }
    }

    return path;
}


inline std::vector<int> eiler_path_undir_V1(const std::vector<std::vector<int>> &g)
{
    std::vector<int> path;
    if(!is_eiler_graph_undir(g))
        return path;
    using edge = std::pair<int,int>;
    std::unordered_set<edge, std::function<std::size_t(edge)>> visited(0,[](edge p){
        return p.first * 31 + p.second;
    });
    std::function<void(int)> dfs = [&](int v)
    {
        for(int u: g[v])
        {
            if(!visited.count({v,u}))
            {
                visited.insert({u,v});
                visited.insert({v,u});
                dfs(u);
            }
        }
        path.push_back(v);
    };

    for(int v = 0; v < g.size() ; ++v)
    {
        if(g[v].size() % 2)
        {
            dfs(v);
            return path;
        }
    }

    for(int v = 0; v < g.size() ; ++v)
    {
        if(g[v].size())
        {
            dfs(v);
            return path;
        }
    }

    return path;
}



inline std::vector<int> bfs_tree(const std::vector<std::vector<int>> &g,int v0)
{
    std::vector<Color> colors(g.size());
    std::vector<int> parent(g.size(),-1);//p[v] == -1  v - root
    std::queue<int> q;
    parent[v0] = -1;
    q.push(v0);
    colors[v0] = GREY;
    while(!q.empty())
    {
        int v = q.front();
        for(int u : g[v])
        {
            if(colors[u] == WHITE)
            {
                parent[u] = v;
                q.push(u);
                colors[u] = GREY;
            }
        }
        q.pop();
        colors[v] = BLACK;
    }
    return parent;
}

inline std::vector<int> bfs_dist(const std::vector<std::vector<int>> &g,int v0)
{
    std::vector<int> dist(g.size(),-1);//-1 is infinity distance
    std::queue<int> q;
    q.push(v0);
    dist[v0] = 0;
    while(!q.empty())
    {
        int v = q.front();
        for(int u : g[v])
        {
            if(dist[u] == -1)//color[u] == WHITE
            {
                q.push(u);
                dist[u] = dist[v] + 1;
            }
        }
        q.pop();
    }
    return dist;
}

inline int bfs_dist(const std::vector<std::vector<int>> &g,int v0,int v1)
{
    std::vector<int> dist(g.size(),-1);//-1 is infinity distance

    std::queue<int> q;
    q.push(v0);
    dist[v0] = 0;
    while(!q.empty())
    {
        int v = q.front();
        if(v == v1)
            return dist[v];
        for(int u : g[v])
        {
            if(dist[u] == -1)//color[u] == WHITE
            {
                q.push(u);
                dist[u] = dist[v] + 1;
            }
        }
        q.pop();
    }
    return -1;
}


inline std::vector<int> bfs_way(
        const std::vector<std::vector<int>> &g,
        int v0,
        int v1)
{
    std::vector<int> dist(g.size(),-1);//dist
    std::vector<int> parents(g.size(),-1);//parents
    std::queue<int> q;
    dist[v0] = 0;
    q.push(v0);
    while(!q.empty())
    {
        int v = q.front();
        q.pop();
        if(v == v1)
            return way_from_tree(parents,v0,v1);
        for(int u : g[v])
        {
            if(dist[u] == -1)
            {
                dist[u] = dist[v] + 1;
                parents[u] = v;
                q.push(u);
            }
        }
    }
    return {};
}


inline std::vector<long long> bfs_way(std::function<std::vector<long long>(long long)> g,
                                      long long v0,long long v1,long long dist_limit = 100000)
{
    std::vector<long long> adj_v0 = g(v0);
    std::vector<long long> dist(1<<64);
}



inline std::vector<int> dijkstra_way(
        const std::vector<std::vector<int>> &g,
        const std::vector<std::vector<int>> &w,
        int v0, int v1)
{
    std::vector<int> dist(g.size(),-1);//dist
    std::vector<int> parents(g.size(),-1);//parents
    std::priority_queue<std::pair<int,int>> q;//max_heap   std::pair<int,int> == std::tuple<int,int>
    dist[v0] = 0;
    q.push({-dist[v0], v0});
    while(!q.empty())
    {
        int v = q.top().second;
        int dv = -q.top().first;
        q.pop();
        if(v == v1)
            return way_from_tree(parents,v0,v1);
        if(dv >= dist[v])
            continue;
        for(int iu = 0 ; iu < g[v].size() ; ++iu)
        {
            int u = g[v][iu];
            int wvu = w[v][iu];
            if(dist[u] == -1 || dist[v] + wvu < dist[u])
            {
                dist[u] = dist[v] + wvu;
                parents[u] = v;
                q.push({-dist[u],u});
            }
        }
    }
    return {};
}


inline std::vector<int> astar_way(
        const std::vector<std::vector<int>> &g,
        const std::vector<std::vector<int>> &w,
        const std::vector<int> &h,
        int v0, int v1)
{
    std::vector<int> dist(g.size(),-1);//dist
    std::vector<int> parents(g.size(),-1);//parents
    std::priority_queue<std::tuple<int,int,int>> q;//max_heap
    dist[v0] = 0;
    q.push({-dist[v0] - h[v0],-dist[v0],v0});
    while(!q.empty())
    {
        int v = std::get<2>(q.top());
        int dv = -std::get<1>(q.top());
        q.pop();
        if(v == v1)
            return way_from_tree(parents,v0,v1);
        if(dv >= dist[v])
            continue;
        for(int iu = 0 ; iu < g[v].size() ; ++iu)
        {
            int u = g[v][iu];
            int wvu = w[v][iu];
            if(dist[u] == -1 || dist[v] + wvu < dist[u])
            {
                dist[u] = dist[v] + wvu;
                parents[u] = v;
                q.push({-dist[u]-h[u],-dist[u],u});
            }
        }
    }
    return {};
}


inline std::vector<int> astar_way(
        const std::vector<std::vector<int>> &g,
        const std::vector<std::vector<int>> &w,
        std::function<int(int,int)> H,
        int v0, int v1)
{
    std::vector<int> h(g.size());
    for(int v = 0; v < g.size() ; ++v)
        h[v] = H(v,v1);
    return astar_way(g,w,h,v0,v1);
}


inline std::vector<int> ford_bellman_way(const std::vector<std::vector<int>> &g,
                                 const std::vector<std::vector<int>> &w,
                                 int v0, int v1)
{
    std::vector<int> dist(g.size());
    std::vector<int> parents(g.size(),-1);
    std::vector<bool> inf(g.size(),true);
    inf[v0] = false;
    for(int i = 0; i < g.size() - 1 ; ++i)
    {
        for(int v = 0; v < g.size() ; ++v)
        {
            if(inf[v])
                continue;
            for(int iu = 0; iu < g[v].size() ; ++iu)
            {
                int u = g[v][iu];
                int wvu = w[v][iu];
                if(inf[u] || dist[v] + wvu < dist[u])
                {
                    dist[u] = dist[v] + wvu;
                    inf[u] = false;
                    parents[u] = v;
                }
            }
        }
    }

    for(int v = 0; v < g.size() ; ++v)
    {
        if(inf[v])
            continue;
        for(int iu = 0; iu < g[v].size() ; ++iu)
        {
            int u = g[v][iu];
            int wvu = w[v][iu];
            if(!inf[u] || dist[v] + wvu < dist[u])
                return {};
        }
    }
    return way_from_tree(parents,v0,v1);
}


inline std::vector<int> ford_bellman_dist(const std::vector<std::vector<int>> &g,
                                 const std::vector<std::vector<int>> &w,
                                 int v0)
{
    std::vector<int> dist(g.size());
    std::vector<bool> inf(g.size(),true);
    inf[v0] = false;
    for(int i = 0; i < g.size() - 1 ; ++i)
    {
        for(int v = 0; v < g.size() ; ++v)
        {
            if(inf[v])
                continue;
            for(int iu = 0; iu < g[v].size() ; ++iu)
            {
                int u = g[v][iu];
                int wvu = w[v][iu];
                if(inf[u] || dist[v] + wvu < dist[u])
                {
                    dist[u] = dist[v] + wvu;
                    inf[u] = false;
                }
            }
        }
    }

    for(int v = 0; v < g.size() ; ++v)
    {
        if(inf[v])
            continue;
        for(int iu = 0; iu < g[v].size() ; ++iu)
        {
            int u = g[v][iu];
            int wvu = w[v][iu];
            if(!inf[u] && dist[v] + wvu < dist[u])
                return {};
        }
    }
    return dist;
}

inline int dijkstra_dist(const std::vector<std::vector<int>> &g,
                              const std::vector<std::vector<int>> &w,
                              int v0, int v1)
{
    std::vector<int> dist(g.size(),-1);//dist
    std::priority_queue<std::pair<int,int>> q;//max_heap   std::pair<int,int> == std::tuple<int,int>
    dist[v0] = 0;
    q.push({-dist[v0], v0});
    while(!q.empty())
    {
        int v = q.top().second;
        int dv = -q.top().first;
        q.pop();
        if(v == v1)
            return dist[v1];
        if(dv >= dist[v])
            continue;
        for(int iu = 0 ; iu < g[v].size() ; ++iu)
        {
            int u = g[v][iu];
            int wvu = w[v][iu];
            if(dist[u] == -1 || dist[v] + wvu < dist[u])
            {
                dist[u] = dist[v] + wvu;
                q.push({-dist[u],u});
            }
        }
    }
    return {};
}

class djonson_way
{
    std::vector<std::vector<int>> m_g;
    std::vector<std::vector<int>> m_w;
    std::vector<int> m_fi;
public:
    djonson_way(const std::vector<std::vector<int>> &g,
                const std::vector<std::vector<int>> &w):m_g(g),m_w(w)
    {
        m_g.push_back({});
        m_w.push_back({});
        for(int v = 0 ; m_g.size() - 1 ; ++v)
        {
            m_g.back().push_back(v);
            m_w.back().push_back(0);
        }
        m_fi = ford_bellman_dist(m_g,m_w,m_g.size() - 1);
        if(m_fi.empty())
            throw "Neg cycle";
        for(int v = 0; v < g.size() ; ++v)
        {
            for(int iu = 0; iu < g[v].size() ; ++iu)
            {
                int u = g[v][iu];
                m_w[v][iu] += m_fi[v] - m_fi[u];
            }
        }
    }

    int dist(int v0, int v1) const
    {
        return dijkstra_dist(m_g,m_w,v0,v1) + m_fi[v1] - m_fi[v0];
    }

    std::vector<int> operator()(int v0,int v1) const
    {
        return dijkstra_way(m_g,m_w,v0,v1);
    }
};



#endif // GRAPH_H
