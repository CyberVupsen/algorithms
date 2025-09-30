#include <iostream>
#include <vector>
#include <stdexcept>
#include <queue>
#include <utility>


class Vertex {
public:
    virtual ~Vertex() {}
    virtual int num_inputs() const = 0;
    virtual void set_input(int inp_idx, double inp_val) = 0;
    virtual int num_outputs() const = 0;
    virtual double get_output(int out_idx) = 0;
    virtual void calc_value() = 0;
};

struct Edge {
    int out_vertex_id;
    int out_port_id;
    int inp_vertex_id;
    int inp_port_id;
};

class PlusOperator : public Vertex {
    double m_inp_val[2];
    double m_out;
public:
    PlusOperator() { m_inp_val[0]=m_inp_val[1]=0.0; m_out=0.0; }
    int num_inputs() const override {
        return 2;
    }
    void set_input(int inp_idx, double inp_val) override {
        if (inp_idx < 0 || inp_idx >= 2)
            throw std::out_of_range("Bad inp_idx in the PlusOperator::set_input");
        m_inp_val[inp_idx] = inp_val;
    }
    int num_outputs() const override { return 1; }
    double get_output(int out_idx) override {
        if (out_idx != 0)
            throw std::out_of_range("Bad out_idx in the PlusOperator::get_output");
        return m_out;
    }
    void calc_value() override {
        m_out = m_inp_val[0] + m_inp_val[1];
    }
};

class ConstOperator : public Vertex {
    double m_val;
public:
    explicit ConstOperator(double v) : m_val(v) {}
    int num_inputs() const override { return 0; }
    void set_input(int, double) override {
        throw std::out_of_range("ConstOperator has no inputs");
    }
    int num_outputs() const override { return 1; }
    double get_output(int out_idx) override {
        if (out_idx != 0)
            throw std::out_of_range("ConstOperator::get_output");
        return m_val;
    }
    void calc_value() override {}
};

class CalcGraph : public Vertex {
    std::vector<Vertex*> m_vertices;
    std::vector<Edge> m_edges;

    std::vector<std::vector<Edge>> m_out_edges_by_vertex;
    std::vector<int> m_topo_order;

    std::vector<std::pair<int,int>> m_free_inputs_map;
    std::vector<std::pair<int,int>> m_free_outputs_map;

    std::vector<double> m_graph_input_values;
    std::vector<double> m_graph_output_values;

    bool m_valid = false;

    void build() {
        int n = (int)m_vertices.size();
        m_out_edges_by_vertex.assign(n, {});
        std::vector<std::vector<char>> inp_used(n);
        for (int i=0;i<n;++i) inp_used[i].assign(m_vertices[i]->num_inputs(), 0);

        for (const auto &e : m_edges) {
            if (e.out_vertex_id < 0 || e.out_vertex_id >= n ||
                e.inp_vertex_id < 0  || e.inp_vertex_id >= n)
                throw std::out_of_range("Edge has bad vertex id");

            if (e.out_port_id < 0 || e.out_port_id >= m_vertices[e.out_vertex_id]->num_outputs())
                throw std::out_of_range("Edge has bad out_port_id");

            if (e.inp_port_id < 0 || e.inp_port_id >= m_vertices[e.inp_vertex_id]->num_inputs())
                throw std::out_of_range("Edge has bad inp_port_id");

            if (inp_used[e.inp_vertex_id][e.inp_port_id])
                throw std::runtime_error("Multiple edges to same input port");
            inp_used[e.inp_vertex_id][e.inp_port_id] = 1;

            m_out_edges_by_vertex[e.out_vertex_id].push_back(e);
        }

        // топологическая сортировка
        std::vector<int> indeg(n,0);
        for (const auto &e : m_edges) indeg[e.inp_vertex_id]++;

        std::queue<int> q;
        for (int i=0;i<n;++i) if (indeg[i]==0) q.push(i);

        m_topo_order.clear();
        while (!q.empty()) {
            int v = q.front(); q.pop();
            m_topo_order.push_back(v);
            for (const auto &e : m_out_edges_by_vertex[v]) {
                indeg[e.inp_vertex_id]--;
                if (indeg[e.inp_vertex_id] == 0) q.push(e.inp_vertex_id);
            }
        }
        if ((int)m_topo_order.size() != n)
            throw std::runtime_error("Graph has cycle");

        m_free_inputs_map.clear();
        for (int vid=0; vid<n; ++vid) {
            int nin = m_vertices[vid]->num_inputs();
            for (int ip=0; ip<nin; ++ip) {
                bool connected = false;
                for (const auto &e : m_edges) {
                    if (e.inp_vertex_id == vid && e.inp_port_id == ip) { connected = true; break; }
                }
                if (!connected) m_free_inputs_map.emplace_back(vid, ip);
            }
        }

        m_free_outputs_map.clear();
        for (int vid=0; vid<n; ++vid) {
            int nout = m_vertices[vid]->num_outputs();
            for (int op=0; op<nout; ++op) {
                bool connected = false;
                for (const auto &e : m_edges) {
                    if (e.out_vertex_id == vid && e.out_port_id == op) { connected = true; break; }
                }
                if (!connected) m_free_outputs_map.emplace_back(vid, op);
            }
        }

        m_graph_input_values.assign(m_free_inputs_map.size(), 0.0);
        m_graph_output_values.assign(m_free_outputs_map.size(), 0.0);
        m_valid = true;
    }

public:
    CalcGraph() = default;
    ~CalcGraph() = default;

    void set_data(const std::vector<Vertex*> &vertices, const std::vector<Edge> &edges) {
        m_vertices = vertices;
        m_edges = edges;
        build();
    }

    int num_inputs() const override {
        if (!m_valid)
            return 0;
        return (int)m_free_inputs_map.size();
    }
    void set_input(int inp_idx, double inp_val) override {
        if (!m_valid)
            throw std::runtime_error("Graph not initialized");
        if (inp_idx < 0 || inp_idx >= (int)m_graph_input_values.size())
            throw std::out_of_range("CalcGraph::set_input");
        m_graph_input_values[inp_idx] = inp_val;
    }
    int num_outputs() const override {
        if (!m_valid)
            return 0;
        return (int)m_free_outputs_map.size();
    }
    double get_output(int out_idx) override {
        if (!m_valid)
            throw std::runtime_error("Graph not initialized");
        if (out_idx < 0 || out_idx >= (int)m_graph_output_values.size())
            throw std::out_of_range("CalcGraph::get_output");
        return m_graph_output_values[out_idx];
    }

    void calc_value() override {
        if (!m_valid)
            throw std::runtime_error("Graph not initialized");
        int n = (int)m_vertices.size();

        for (int i=0;i<n;++i) {
            int ni = m_vertices[i]->num_inputs();
            for (int j=0;j<ni; ++j) m_vertices[i]->set_input(j, 0.0);
        }

        for (size_t k=0; k<m_free_inputs_map.size(); ++k) {
            auto pr = m_free_inputs_map[k];
            m_vertices[pr.first]->set_input(pr.second, m_graph_input_values[k]);
        }

        for (int vid : m_topo_order) {
            m_vertices[vid]->calc_value();
            for (const auto &e : m_out_edges_by_vertex[vid]) {
                double val = m_vertices[vid]->get_output(e.out_port_id);
                m_vertices[e.inp_vertex_id]->set_input(e.inp_port_id, val);
            }
        }

        for (size_t k=0; k<m_free_outputs_map.size(); ++k) {
            auto pr = m_free_outputs_map[k];
            m_graph_output_values[k] = m_vertices[pr.first]->get_output(pr.second);
        }
    }
};

// int main() {
//     std::vector<Vertex*> vertices;
//     vertices.push_back(new PlusOperator()); // id = 0
//     vertices.push_back(new PlusOperator()); // id = 1

//     std::vector<Edge> edges;
//     edges.push_back(Edge{0,0,1,0});

//     CalcGraph cg;
//     cg.set_data(vertices, edges);

//     std::cout << "num_inputs = " << cg.num_inputs() << std::endl;
//     std::cout << "num_outputs = " << cg.num_outputs() << std::endl;

//     cg.set_input(0, 1.0);
//     cg.set_input(1, 2.0);
//     cg.set_input(2, 3.0);

//     cg.calc_value();

//     std::cout << "out0 = " << cg.get_output(0) << std::endl;

//     delete vertices[0];
//     delete vertices[1];
//     return 0;
// }
