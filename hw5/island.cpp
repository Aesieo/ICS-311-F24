#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <queue>
#include <limits>
#include <stack>
#include <fstream>
#include <cmath>

class Graph {
private:
    std::vector<std::vector<int> > adjMatrix;
    std::vector<std::string> islandName;
    std::vector<int> populations;
    std::unordered_map<std::string, int> index;
    int islandCount;

public:
    Graph(int n) : islandCount(n) {
        adjMatrix.resize(n, std::vector<int>(n, std::numeric_limits<int>::max()));
        populations.resize(n, 0);
        islandName.resize(n);
        for(int i = 0; i < n; i++){
            adjMatrix[i][i] = 0;
        }
    }

    void addIsland(int id, const std::string& name, int population) {
        index[name] = id;
        islandName[id] = name;
        populations[id] = population;
        std::cout << index[name] << std::endl;
    }

    void addRoute(const std::string& from, const std::string& to, int distance) {
        int u = index[from];
        int v = index[to];
        adjMatrix[u][v] = distance;
    }

    void shareKnowledge(const std::string& start){
        int startIdx = index[start];
        std::priority_queue<std::tuple<int, int, int>> queue;
        std::vector<int> shortest_distance(islandCount, std::numeric_limits<int>::max());
        std::vector<int> previous(islandCount, -1);

        shortest_distance[startIdx] = 0;
        queue.push(std::make_tuple(-populations[startIdx], 0, startIdx));
        
        std::cout << "Island\t\tPopulation\tTotal Distance\tRoute\n";
        std::cout << "-----------------------------------------------------------------\n";

        while(!queue.empty()){
            int distance = std::get<1>(queue.top());
            int island = std::get<2>(queue.top());
            queue.pop();

            if(distance > shortest_distance[island]) continue;

            std::stack<int> routeStack;
            int current = island;
            while(current != -1){
                routeStack.push(current);
                current = previous[current];
            }

            std::cout << islandName[island] << "\t\t" << populations[island] << "\t\t" << distance << "\t\t";
            while(!routeStack.empty()){
                std::cout << islandName[routeStack.top()];
                routeStack.pop();
                if(!routeStack.empty()) std::cout << " -> ";
            }
            std::cout << std::endl;

            for(int neighbor = 0; neighbor < islandCount; neighbor++){
                if(adjMatrix[island][neighbor] != std::numeric_limits<int>::max()){
                    int new_distance = distance + adjMatrix[island][neighbor];

                    if(new_distance > shortest_distance[neighbor]){
                        shortest_distance[neighbor] = new_distance;
                        previous[neighbor] = island;
                        queue.push(std::make_tuple(-populations[neighbor], new_distance, neighbor));
                    }
                }
            }
        }
    }
};

double dtor(double deg){
    return ((deg*M_PI)/180.0);
}

double haversine(double lat1, double lon1, double lat2, double lon2){
    return (2*6371*asin(sqrt((pow((sin(dtor(lon1-lon2))/2), 2.0))+(pow((sin(dtor(lat1-lat2))/2), 2.0))*cos(dtor(lon1))*cos(dtor(lon2)))));
}

int main(){
    int n = 44;
    std::ifstream islandsfile("islands.txt");
    std::ifstream latsfile("lats.txt");
    std::ifstream longsfile("longs.txt");
    std::string island;
    std::string lats;
    std::string longs;
    std::vector<std::string> islands;
    std::vector<double> latitudes;
    std::vector<double> longitudes;
    srand(time(0));
    Graph graph(n);

    while(getline(islandsfile, island)){
        islands.push_back(island);
    }

    while(getline(latsfile, lats)){
        latitudes.push_back((std::stod(lats)));
    }

    while(getline(longsfile, longs)){
        longitudes.push_back((std::stod(longs)));
    }

    for(int i = 0; i < n; i++){
        graph.addIsland(i, islands[i], ((std::rand()%451) + 50));
    }

    // Connected graph for region of Hawai'i
    for(int x = 0; x < 8; x++){
        for(int y = 0; y < 8; y++){
            graph.addRoute(islands[x], islands[y], std::round(haversine(latitudes[x], longitudes[x], latitudes[y], longitudes[y])));
        }
    }
    graph.shareKnowledge("Big Island");

    return 0;
}
