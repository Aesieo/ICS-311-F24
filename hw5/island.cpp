#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <queue>
#include <limits>
#include <stack>
#include <fstream>
#include <cmath>
#include <iomanip>

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
    }

    void addRoute(const std::string& from, const std::string& to, int distance) {
        int u = index[from];
        int v = index[to];
        adjMatrix[u][v] = distance;
    }

    void specialResource(const std::string& source){
        int sourceIdx = index[source];
        std::priority_queue<std::pair<int,int>, std::vector<std::pair<int,int>>, std::greater<std::pair<int,int>>> queue;
        std::vector<int> parent(islandCount, -1);
        std::vector<int> key(islandCount, std::numeric_limits<int>::max());
        std::vector<bool> visited(islandCount, false);

        queue.push({0, sourceIdx});
        key[sourceIdx] = 0;

        while(!queue.empty()){
            int island = queue.top().second;
            queue.pop();

            if(visited[island] == true) continue;
            visited[island] = true;

            for(int adjacent = 0; adjacent < islandCount; adjacent++){
                if(adjMatrix[island][adjacent] != std::numeric_limits<int>::max()){
                    int neighbor = adjacent;
                    int distance = adjMatrix[island][adjacent];

                    if(visited[neighbor] == false && key[neighbor] > distance){
                        key[neighbor] = distance;
                        queue.push({key[neighbor], neighbor});
                        parent[neighbor] = island;
                    }
                }
            }
        }

        std::cout << "Paths from " << source << std::endl;
        for(int i = 0; i < islandCount; i++){
            if(i != sourceIdx && parent[i] != -1){
                std::stack<int> routeStack;
                int current = i;
                while(current != -1){
                    routeStack.push(current);
                    current = parent[current];
                }
                routeStack.pop();
                std::cout << std::setw(3) << index[source];
                while(!routeStack.empty()){
                    std::cout << std::setw(3) << "->" << std::setw(3);
                    std::cout << routeStack.top();
                    routeStack.pop();
                }
            std::cout << std::endl;
            }
        }
    }

    void shareKnowledge(const std::string& start){
        int startIdx = index[start];
        std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, std::greater<std::pair<int,int>>> queue;
        std::vector<int> shortest_distance(islandCount, std::numeric_limits<int>::max());
        std::vector<int> previous(islandCount, -1);

        shortest_distance[startIdx] = 0;
        queue.push({0, startIdx});
        
        std::cout << "Island\t\t\tPopulation\tTotal Distance\tRoute\n";
        std::cout << "-----------------------------------------------------------------\n";

        while(!queue.empty()){
            int distance = queue.top().first;
            int island = queue.top().second;
            queue.pop();

            if(distance > shortest_distance[island]) continue;

            std::cout << islandName[island] << "\t\t\t" << populations[island] << "\t\t" << distance << "\t\t" << island;
            std::cout << std::endl;

            for(int adjacent = 0; adjacent < islandCount; adjacent++){
                if(adjMatrix[island][adjacent] != std::numeric_limits<int>::max()){
                    int new_distance = distance + adjMatrix[island][adjacent];

                    if(new_distance < shortest_distance[adjacent]){
                        shortest_distance[adjacent] = new_distance;
                        previous[adjacent] = island;
                        queue.push({new_distance,adjacent});
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
            graph.addRoute(islands[y], islands[x], std::round(haversine(latitudes[x], longitudes[x], latitudes[y], longitudes[y])));
        }
    }

    // Connected graph for region of Samoa
    for(int x = 8; x < 14; x++){
        for(int y = 0; y < 14; y++){
            graph.addRoute(islands[x], islands[y], std::round(haversine(latitudes[x], longitudes[x], latitudes[y], longitudes[y])));
            graph.addRoute(islands[y], islands[x], std::round(haversine(latitudes[x], longitudes[x], latitudes[y], longitudes[y])));
        }
    }

    // Connected graph for region of Tonga
    for(int x = 14; x < 20; x++){
        for(int y = 0; y < 20; y++){
            graph.addRoute(islands[x], islands[y], std::round(haversine(latitudes[x], longitudes[x], latitudes[y], longitudes[y])));
            graph.addRoute(islands[y], islands[x], std::round(haversine(latitudes[x], longitudes[x], latitudes[y], longitudes[y])));
        }
    }

    // Connected graph for region of French Polynesia
    for(int x = 20; x < 25; x++){
        for(int y = 20; y < 25; y++){
            graph.addRoute(islands[x], islands[y], std::round(haversine(latitudes[x], longitudes[x], latitudes[y], longitudes[y])));
            graph.addRoute(islands[y], islands[x], std::round(haversine(latitudes[x], longitudes[x], latitudes[y], longitudes[y])));
        }
    }

    // Connected graph for region of Cook Islands
    for(int x = 25; x < 32; x++){
        for(int y = 25; y < 32; y++){
            graph.addRoute(islands[x], islands[y], std::round(haversine(latitudes[x], longitudes[x], latitudes[y], longitudes[y])));
            graph.addRoute(islands[y], islands[x], std::round(haversine(latitudes[x], longitudes[x], latitudes[y], longitudes[y])));
        }
    }

    // Connected graph for region of New Zealand
    for(int x = 32; x < 35; x++){
        for(int y = 32; y < 35; y++){
            graph.addRoute(islands[x], islands[y], std::round(haversine(latitudes[x], longitudes[x], latitudes[y], longitudes[y])));
            graph.addRoute(islands[y], islands[x], std::round(haversine(latitudes[x], longitudes[x], latitudes[y], longitudes[y])));
        }
     }

    // Connected graph for region of Tokelau
    for(int x = 36; x < 39; x++){
        for(int y = 36; y < 39; y++){
            graph.addRoute(islands[x], islands[y], std::round(haversine(latitudes[x], longitudes[x], latitudes[y], longitudes[y])));
            graph.addRoute(islands[y], islands[x], std::round(haversine(latitudes[x], longitudes[x], latitudes[y], longitudes[y])));
        }
    }

    // Connected graph for region of Tuvalu
    for(int x = 40; x < 44; x++){
        for(int y = 40; y < 44; y++){
            graph.addRoute(islands[x], islands[y], std::round(haversine(latitudes[x], longitudes[x], latitudes[y], longitudes[y])));
            graph.addRoute(islands[y], islands[x], std::round(haversine(latitudes[x], longitudes[x], latitudes[y], longitudes[y])));
        }
    }

    //Misc adjacencies per island
    graph.addRoute(islands[0], islands[12], 4046.00);
    graph.addRoute(islands[0], islands[18], 4477.29);
    graph.addRoute(islands[0], islands[20], 4191.51);
    graph.addRoute(islands[0], islands[31], 3184.46);
    graph.addRoute(islands[0], islands[36], 3627.47);

    graph.addRoute(islands[6], islands[18], 4498.23);
    graph.addRoute(islands[6], islands[42], 3998.79);

    graph.addRoute(islands[8], islands[19], 315.93);

    graph.addRoute(islands[9], islands[19], 293.56);
    graph.addRoute(islands[9], islands[38], 493.86);
    graph.addRoute(islands[9], islands[41], 956.76);

    graph.addRoute(islands[10], islands[32], 3075.72);
    graph.addRoute(islands[10], islands[35], 533.81);

    graph.addRoute(islands[12], islands[0], 4046.00);
    graph.addRoute(islands[12], islands[20], 2171.97);
    graph.addRoute(islands[12], islands[30], 794.48);

    graph.addRoute(islands[15], islands[23], 2537.46);
    graph.addRoute(islands[15], islands[30], 1146.19);
    graph.addRoute(islands[15], islands[35], 436.54);

    graph.addRoute(islands[18], islands[0], 4477.29);
    graph.addRoute(islands[18], islands[41], 848.31);

    graph.addRoute(islands[19], islands[9], 293.56);

    graph.addRoute(islands[20], islands[0], 4191.51);
    graph.addRoute(islands[20], islands[12], 2171.97);
    graph.addRoute(islands[20], islands[28], 950.06);

    graph.addRoute(islands[23], islands[15], 2537.46);
    graph.addRoute(islands[23], islands[27], 821.93);

    graph.addRoute(islands[24], islands[39], 2613.40);

    graph.addRoute(islands[27], islands[23], 821.93);
    graph.addRoute(islands[27], islands[39], 4917.36);

    graph.addRoute(islands[29], islands[38], 1126.38);

    graph.addRoute(islands[30], islands[12], 794.48);
    graph.addRoute(islands[30], islands[15], 1146.19);
    graph.addRoute(islands[30], islands[35], 717.30);
    graph.addRoute(islands[30], islands[41], 2071.56);

    graph.addRoute(islands[31], islands[0], 3184.46);

    graph.addRoute(islands[32], islands[10], 3075.72);
    graph.addRoute(islands[32], islands[25], 3080.43);

    graph.addRoute(islands[35], islands[10], 533.81);
    graph.addRoute(islands[35], islands[15], 436.54);
    graph.addRoute(islands[35], islands[30], 717.30);

    graph.addRoute(islands[36], islands[0], 3627.47);

    graph.addRoute(islands[38], islands[9], 493.86);
    graph.addRoute(islands[38], islands[29], 1126.38);

    graph.addRoute(islands[39], islands[27], 4917.36);

    graph.addRoute(islands[41], islands[9], 965.76);
    graph.addRoute(islands[41], islands[18], 848.31);
    graph.addRoute(islands[41], islands[30], 2071.56);

    graph.addRoute(islands[42], islands[6], 3998.79);

    // Algorithms
    graph.shareKnowledge(islands[0]);
    graph.specialResource(islands[6]);
    graph.specialResource(islands[1]);
    return 0;
}