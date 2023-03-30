#include "./nthu_bike.h"

using namespace std;

int numOfStation; // used in graph.cpp


void ReadMap(string selectedCase,Graph* graph){
    //Graph* graph = *g;
    //path is relative to current directory (DS_FINAL_PROJECT)
    string path = "./testcases/" + selectedCase+ "/map.txt";
    string line;
    ifstream infile;
    infile.open(path);
    if(infile.is_open()){
        while(getline(infile,line)){
            stringstream stream(line);
            string s1,s2;
            int dist;
            getline(stream,s1,' '); //space as delimiter
            getline(stream,s2,' ');
            stream >> dist; // can read integer using operator >>
            // string station1="";
            // string station2="";
            // string dist="";
            // while(line.find(" ")!=string::npos){
            //     if(station1=="") station1 = line.substr(0,line.find(" "));
            //     else if(station2 == "") station2 = line.substr(0,line.find(" "));
            //     else if(dist == "") dist = line.substr(0,line.find(" "));
            //     line = line.substr(line.find(" ")+1);
            // }
            graph->AddEdge(stoi(s1.substr(1)),stoi(s2.substr(1)),dist);
            graph->AddEdge(stoi(s2.substr(1)),stoi(s1.substr(1)),dist);
        }
        infile.close();
    }else{
        cout << "not open" << endl;
    }
    NumOfStations(graph);
}

// calculate number of stations
void NumOfStations(Graph* graph){
    int count = 0;
    for(int i=0;i<NUM_STATIONS;i++){
        int hasNode = 0;
        for(int j=0;j<NUM_STATIONS;j++){
            if(i!=j && graph->shortestPath[i][j]!=INT_MAX){
                hasNode = 1;
                break;
            }
        }
        if(hasNode) count++;
    }
    numOfStation = count;
}

// since we use FloydWarshall, store the map by matrix
void
Graph::AddEdge(int id1, int id2, int e){
    shortestPath[id1][id2] = e;
    shortestPath[id2][id1] = e;
}

// void
// Graph::AddEdge(int id1,int id2,int e){
//     StationForGraph* newNode = new StationForGraph(id2,e);
//     StationForGraph* cur = AdjList[id1];
//     StationForGraph* prev = cur;
//     if(!cur){
//         AdjList[id1] = newNode;
//     } else if(cur->stationId>id2){//newNode shoulde be place a first pos, important!
//         newNode->next = cur;
//         AdjList[id1] = newNode;
//     }else{
//         while(cur){
//             if(cur->stationId<=id2){
//                 prev = cur;
//                 cur = cur->next;
//             }else{
//                 prev->next = newNode;
//                 newNode->next = cur;
//                 break; 
//             }
//         }
//         if(!cur){ // newnode should be added to tail
//                 prev->next = newNode;
//         }
//     }
    
// }

void
Graph::PrintGraph(){
    // cout << numOfStation << endl;
    for(int i=0;i<numOfStation;i++){
        cout << "Path " << i << " " << endl;
        for(int j=0;j<numOfStation;j++){
            
            if(i!=j && shortestPath[i][j] != INT_MAX){
                cout << "to " << j << ": " << shortestPath[i][j] << endl;
            }
        }
    }
}

void Graph::FloydWarshall(){
    for(int k=0;k<numOfStation;k++){
        for(int i=0;i<numOfStation;i++){
            for(int j=0;j<numOfStation;j++){
                // be careful with overflow
                if(shortestPath[i][k] != INT_MAX && shortestPath[k][j] != INT_MAX\
                && shortestPath[i][j] > shortestPath[i][k]+shortestPath[k][j]){
                    shortestPath[i][j] = shortestPath[i][k]+shortestPath[k][j];
                }
            }
        }
    }
    //cout << "shortestPath[9][1]: " << shortestPath[9][1] << endl;
}