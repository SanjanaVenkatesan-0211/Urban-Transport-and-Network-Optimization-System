#include <iostream>
#include <vector>
#include <algorithm>
#include <cctype>
#include <map>
#include <queue>
#include <climits>
#include <unordered_map>
#include <fstream>
using namespace std;

// ================= TRIE =================
class TrieNode {
public:
    map<char, TrieNode*> children;
    bool isEnd = false;
};

class Trie {
    TrieNode* root;
public:
    Trie() { root = new TrieNode(); }

    void insert(string word) {
        TrieNode* node = root;
        for (char c : word) {
            if (!node->children[c])
                node->children[c] = new TrieNode();
            node = node->children[c];
        }
        node->isEnd = true;
    }

    void suggest(TrieNode* node, string prefix) {
        if (node->isEnd)
            cout << prefix << endl;

        for (auto &p : node->children)
            suggest(p.second, prefix + p.first);
    }

    void searchPrefix(string prefix) {
        TrieNode* node = root;
        for (char c : prefix) {
            if (!node->children[c]) {
                cout << "No suggestions found\n";
                return;
            }
            node = node->children[c];
        }
        cout << "Suggestions:\n";
        suggest(node, prefix);
    }
};

// ================= GRAPH =================
class Graph {
    int V;
    vector<vector<pair<int,int>>> adj;

public:
    Graph(int V) {
        this->V = V;
        adj.resize(V);
    }

    void addEdge(int u, int v, int w) {
        adj[u].push_back({v, w});
        adj[v].push_back({u, w});
    }

    void removeEdge(int u, int v) {
        adj[u].erase(remove_if(adj[u].begin(), adj[u].end(),
            [v](auto &x){ return x.first == v; }), adj[u].end());

        adj[v].erase(remove_if(adj[v].begin(), adj[v].end(),
            [u](auto &x){ return x.first == u; }), adj[v].end());
    }

    void updateTraffic(int u, int v, int w) {
        for (auto &x : adj[u]) if (x.first == v) x.second = w;
        for (auto &x : adj[v]) if (x.first == u) x.second = w;
    }

    void display(vector<string>& idToName) {
        for (int i = 0; i < idToName.size(); i++) {
            cout << idToName[i] << " -> ";
            for (auto x : adj[i]) {
                cout << "(" << idToName[x.first] << ", " << x.second << ") ";
            }
            cout << endl;
        }
    }

    void dijkstraPath(int src, int dest, vector<string>& idToName) {
        vector<int> dist(V, INT_MAX), parent(V, -1);

        priority_queue<pair<int,int>, vector<pair<int,int>>, greater<pair<int,int>>> pq;
        dist[src] = 0;
        pq.push({0, src});

        while (!pq.empty()) {
            int u = pq.top().second;
            pq.pop();

            for (auto &edge : adj[u]) {
                int v = edge.first;
                int w = edge.second;

                if (dist[v] > dist[u] + w) {
                    dist[v] = dist[u] + w;
                    parent[v] = u;
                    pq.push({dist[v], v});
                }
            }
        }

        if (dist[dest] == INT_MAX) {
            cout << "No path exists\n";
            return;
        }

        cout << "Shortest Distance: " << dist[dest] << endl;

        vector<int> path;
        for (int v = dest; v != -1; v = parent[v])
            path.push_back(v);

        reverse(path.begin(), path.end());

        cout << "Path: ";
        for (int x : path)
            cout << idToName[x] << " -> ";
        cout << "END\n";
    }

    vector<vector<pair<int,int>>> getAdj() { return adj; }
};

// ================= MAIN =================
int main() {

    Graph g(100); // max size
    Trie t;

    unordered_map<string, int> nameToId;
    vector<string> idToName;

    int nextId = 0;

    // -------- LOAD FROM FILE --------
    ifstream fin("data.txt");
    string a, b;
    int w;

    while (fin >> a >> b >> w) {
        if (nameToId.find(a) == nameToId.end()) {
            nameToId[a] = nextId++;
            idToName.push_back(a);
            t.insert(a);
        }
        if (nameToId.find(b) == nameToId.end()) {
            nameToId[b] = nextId++;
            idToName.push_back(b);
            t.insert(b);
        }
        g.addEdge(nameToId[a], nameToId[b], w);
    }
    fin.close();

    int choice;

    while (true) {
        cout << "\n--- Smart Transport Menu ---\n";
        cout << "1. Add Road\n";
        cout << "2. Remove Road\n";
        cout << "3. Update Traffic\n";
        cout << "4. Display Network\n";
        cout << "5. Exit\n";
        cout << "6. Add Location\n";
        cout << "7. Search Location\n";
        cout << "8. Find Shortest Path\n";
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {

        case 1:
        {
            cout << "Enter location1 location2 weight: ";
            cin >> a >> b >> w;

            transform(a.begin(), a.end(), a.begin(), ::tolower);
            transform(b.begin(), b.end(), b.begin(), ::tolower);

            if (nameToId.find(a) == nameToId.end()) {
                nameToId[a] = nextId++;
                idToName.push_back(a);
                t.insert(a);
            }

            if (nameToId.find(b) == nameToId.end()) {
                nameToId[b] = nextId++;
                idToName.push_back(b);
                t.insert(b);
            }

            g.addEdge(nameToId[a], nameToId[b], w);
            break;
        }

        case 2:
        {
            cout << "Enter locations: ";
            cin >> a >> b;
            g.removeEdge(nameToId[a], nameToId[b]);
            break;
        }

        case 3:
        {
            cout << "Enter locations and new weight: ";
            cin >> a >> b >> w;
            g.updateTraffic(nameToId[a], nameToId[b], w);
            break;
        }

        case 4:
            g.display(idToName);
            break;

        case 5:
        {
            ofstream fout("data.txt");

            auto adj = g.getAdj();
            for (int i = 0; i < idToName.size(); i++) {
                for (auto &x : adj[i]) {
                    fout << idToName[i] << " " << idToName[x.first] << " " << x.second << endl;
                }
            }

            fout.close();
            cout << "Data saved!\n";
            return 0;
        }

        case 6:
        {
            string location;
            cout << "Enter location: ";
            cin.ignore();
            getline(cin, location);

            transform(location.begin(), location.end(), location.begin(), ::tolower);

            if (nameToId.find(location) == nameToId.end()) {
                nameToId[location] = nextId++;
                idToName.push_back(location);
                t.insert(location);
                cout << "Location added\n";
            }
            break;
        }

        case 7:
        {
            string prefix;
            cout << "Enter prefix: ";
            cin >> prefix;

            transform(prefix.begin(), prefix.end(), prefix.begin(), ::tolower);
            t.searchPrefix(prefix);
            break;
        }

        case 8:
        {
            cout << "Enter source and destination: ";
            cin >> a >> b;

            g.dijkstraPath(nameToId[a], nameToId[b], idToName);
            break;
        }

        default:
            cout << "Invalid choice\n";
        }
    }
}