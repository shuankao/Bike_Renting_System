#include<iostream>
#include<fstream>
#include<string>
#include<sstream>

using namespace std;
// users <= 100,000
// bikes <= 10,000
// bike types <= 50
// station <= 1,000
# define NUM_USER 100000
# define NUM_BIKES 10000
# define BIKETYPES 50 // less than 50 types of bike
# define NUM_STATIONS 1000



void basic(string selectedCase);
void advanced(string selectedCase);

class Bike{
public:
    Bike(){};
    Bike(string type,int bikeID,string stationID,double price,int count, int availTime){
        bikeType = type;
        bikeId = bikeID;
        stationId = stationID;
        rentalPrice = price;
        rentalCount = count;
        sameTypeNext = NULL;
        // in the beginnig, all bikes available at their station (availableTime=0)
        availableTime = availTime;
    };
    string bikeType;
    int bikeId;
    string stationId;
    double rentalPrice;
    int rentalCount;
    // Bike* nextType;
    Bike* sameTypeNext;
    // track when is the bike aviable after rented
    int availableTime;
};

class StationBikes{
    public:
    StationBikes(){  
    }
    Bike* bikes[BIKETYPES] = {NULL};//50 types of bike
};

class Stations{
    public:
    Stations(){
        for(int i=0;i<NUM_STATIONS;i++){
            station[i] = NULL;
        }
    }
    StationBikes* station[NUM_STATIONS];
    void Initialize();
    void AddBike(string type,int bikeID,string stationID,double rentalPrice,int rentalCount, int availTime=0);
    void DeleteBike(Bike* bike);
    void PrintStation();
};


// at most 1000 station
//store stationId and dist
class Graph{
    public:
    Graph(){
        for(int i=0;i<NUM_STATIONS;i++){
            for(int j=0;j<NUM_STATIONS;j++){
                if(i!=j) shortestPath[i][j] = INT_MAX;
                else shortestPath[i][j] = 0;
            }
        }
    }
    //StationForGraph* AdjList[NUM_STATIONS];
    void AddEdge(int id1,int id2,int e);
    void PrintGraph();
    void FloydWarshall();
    int shortestPath[NUM_STATIONS][NUM_STATIONS];
}; 

class User{
    public:
    User(){}
    User(string userid,string acceptType,int starttime,int endtime,string startpoint,string endpoint):
    userId(userid),startTime(starttime),endTime(endtime),startPoint(startpoint),endPoint(endpoint){
        stringstream ss(acceptType);
        string tok;
        int idx=0;
        // for(int i=0;i<BIKETYPES;i++){
        //     acceptBikeType[i] = "";
        // }
        while(getline(ss,tok,',')){
            acceptBikeType[idx++] = tok;
        }
        revenue = 0;

    }
    string userId;
    string acceptBikeType[BIKETYPES]; // 50 types of bike at most
    int startTime;
    int endTime;
    string startPoint;
    string endPoint;
    int revenue; // shoulde be round down to integer
};

// for output user_result.txt
// row index is userId, when outputing file, add 'U' before userId
// last two column for transfer_log's start and end station
extern int user_result[NUM_USER][7];
// for station_status.txt
extern Bike* stationStatus[NUM_BIKES];

extern int numOfUsers; // for SortUsers() and InsertionSort()
extern int numOfStation;

extern double depDiscPrice; // depreciationDiscountPrice
extern int rentalCountLimit;

void ReadMap(string selectedCase,Graph* g);
void ReadBike(string selectedCase,Stations* s);
void ReadUser(string selectedCase,User** users,int& numOfUsers);
void ReadBikeInfo(string selectedCase, double& depDiscPrice,int& rentalCountLimit);

void PrintUsers(User** users);
void SortUsers(User** users,const int numOfUsers);
void SortUsersById(User** users,const int numOfUsers);
void RentBikes(User** users, Graph* graph, Stations* stations);
void NumOfStations(Graph* graph);
void WriteUserResult(string selectedCase);
void SortBikeById(Stations* stations);
void WriteStationStatus(string selectedCase);
void WriteTransferLog(string selectedCase);

void validation(string selectedCase,string fileName);