#include "./nthu_bike.h"

using namespace std;

// for output user_result.txt
// row index is userId, when outputing file, add 'U' before userId
// last two column for transfer_log's start and end station
int user_result[NUM_USER][7];
// for station_status.txt
Bike* stationStatus[NUM_BIKES];
static int total_revenue; // for comparing with advance version

int numOfUsers; // for SortUsers() and InsertionSort()

double depDiscPrice; // depreciationDiscountPrice
int rentalCountLimit;

void basic(string selectedCase){
    // insert your code here
    cout << "start your basic version of data structure final from here!" << endl;

    Graph* graph = new Graph(); 
    ReadMap(selectedCase,graph);
    // graph->PrintGraph();
    Stations* stations = new Stations(); // store station info
    ReadBike(selectedCase,stations);
    // stations->PrintStation();
    User* users[NUM_USER]={0};
    ReadUser(selectedCase,users,numOfUsers);
    SortUsers(users,numOfUsers);
    SortUsersById(users,numOfUsers);
    // PrintUsers(users);
    ReadBikeInfo(selectedCase,depDiscPrice,rentalCountLimit);
    // floyd-warshall find all-pair shortest paths
    graph->FloydWarshall();
    // read through users (already sorted by starttime and userId)
    // 1. check currentTime < startTime && startPoint to endPoint's shortest path smaller than endTime-startTime
    // 2. check if biketype available
    // 3. choose among biketypes which has highest rental price
    // 4. calculate this user request revenue before moving (since insert into end stations sorted by price)
    // 5. move bike from source to destination(array 'station' in class Stations), update bike stationId
    // 6. rented bike rental price discounted, rental count +1, check if exceed rental count limit(delete if exceeds)
    
    // 7. write to files "transfer_log.txt"
    // 8. store user request info in user_result array
    RentBikes(users,graph,stations);
    
    // reach final state, write to file "station_status.txt" after sorted by stationid and bikeid 
    // reach final state, write to file "user_result.txt" sorted by userid(when processing user requests, userids are not in ascending order(because sorted by price))
    WriteUserResult(selectedCase);
    WriteTransferLog(selectedCase);

    // prepare for station_status.txt
    SortBikeById(stations);
    WriteStationStatus(selectedCase);
    // change the second parameter to validate output results
    // user_result, station_status, transfer_log
    validation(selectedCase, "user_result.txt");
}

// move bikes when rented, update availalbe time to rentTime + shortestPath
void RentBikes(User** users, Graph* graph, Stations* stations){
    for(int i=0;i<numOfUsers;i++){
        int startPointNum = stoi(users[i]->startPoint.substr(1));
        int endPointNum = stoi(users[i]->endPoint.substr(1));
        if((users[i]->endTime - users[i]->startTime)
        > graph->shortestPath[startPointNum][endPointNum]){
            // check if available biketype
            // highestPrice needs to be type double! or 30.5 and 30 would be seen as the same
            double highestPrice = INT_MIN;
            Bike* chooseThisBike = NULL;
            // keep track of the bike prev to chooseThisBike for remove purpose
            Bike* prevBike = NULL;
            Bike* prevChosenBike = NULL;
            int userStartPointNum = stoi(users[i]->startPoint.substr(1));
            // iterate through all biketypes
            
            int j=0;
            while(users[i]->acceptBikeType[j]!=""){
                int bikeTypeNum = stoi(users[i]->acceptBikeType[j].substr(1));
                // check if at start stations certain biketype available
                Bike* sameTypeBikeCur = stations->station[userStartPointNum]->bikes[bikeTypeNum];
                // set prevBike to head of sametype
                prevBike = sameTypeBikeCur;
                // iterate through the list of sametype bikes
                while(sameTypeBikeCur!=NULL){
                    // check if time available
                    if(sameTypeBikeCur->availableTime <= users[i]->startTime){
                        // check if rental count not yet reached, if reached, the bike is retired and stayed at the station
                        // check if rental price < 0, if yes, retired (higestPrice might be INT_MIN, negative int)
                        if(sameTypeBikeCur->rentalPrice >= highestPrice && sameTypeBikeCur->rentalCount < rentalCountLimit \
                        && sameTypeBikeCur->rentalPrice >= 0){
                            // if same highest price, compare by bikeId
                            if(sameTypeBikeCur->rentalPrice == highestPrice \
                            && chooseThisBike && sameTypeBikeCur->bikeId > chooseThisBike->bikeId){
                                prevBike = sameTypeBikeCur;
                                sameTypeBikeCur = sameTypeBikeCur->sameTypeNext;
                                // continue or break is fine
                                break;
                            }
                            // bike chosen
                            highestPrice = sameTypeBikeCur->rentalPrice;
                            prevChosenBike = prevBike;
                            chooseThisBike = sameTypeBikeCur;
                            // found this type highest price and available, move on to next type
                            break;
                        }
                        // if < highest price, no need to continue since sorted by price
                        else break;
                    }else{ // time not available, see next bike of the same type
                        prevBike = sameTypeBikeCur;
                        sameTypeBikeCur = sameTypeBikeCur->sameTypeNext;
                    }
                }
                j++;
            }
            if(!chooseThisBike){
                // reject user request
                //cout << "reject user request" << endl;
                // int idx = stoi(users[i]->userId.substr(1));
                // for(int j=0;j<6;j++){
                //     user_result[idx][j] = 0;
                // }
            }
            else{
                // update revenue before deprecation of price
                // update bike's rentalPrice for comparison when inserting into end stations
                // update rentalCount for checking should delete bike or move to end stations
                // cout << "move the selected bikeID: " << chooseThisBike->bikeId << endl;
                // cout << "move the selected bike rental price: " << chooseThisBike->rentalPrice << endl;
                // cout << "graph->shortestPath[startPointNum][endPointNum]): " << graph->shortestPath[startPointNum][endPointNum] << endl; 
                
                users[i]->revenue = int(chooseThisBike->rentalPrice*(graph->shortestPath[startPointNum][endPointNum]));
                // cout << "revenue from user "<< users[i]->userId << " request: " << users[i]->revenue << endl;
                total_revenue += users[i]->revenue;

                chooseThisBike->rentalPrice -= depDiscPrice; 
                chooseThisBike->rentalCount++;
                // update bike's available time
                // (not counted from last available time since might not be rented right after returned by others)
                chooseThisBike->availableTime = users[i]->startTime + graph->shortestPath[startPointNum][endPointNum];
                
                // fill in user_result array
                int idx = stoi(users[i]->userId.substr(1));
                user_result[idx][0] = 1; // request accepted
                user_result[idx][1] = chooseThisBike->bikeId;
                user_result[idx][2] = users[i]->startTime;
                user_result[idx][3] = chooseThisBike->availableTime;
                user_result[idx][4] = users[i]->revenue;
                // for tranfer_log
                user_result[idx][5] = stoi(users[i]->startPoint.substr(1));
                user_result[idx][6] = stoi(users[i]->endPoint.substr(1));

                //debug
                // cout << chooseThisBike->bikeId << " " << users[i]->startPoint << " " << users[i]->endPoint;
                // cout << " " << users[i]->startTime << " " << user_result[idx][3] << " U" << idx << endl;
                // move bike
                // check if remove head of sametype list
                if(prevChosenBike == chooseThisBike){ // remove head
                    //cout << "chooseThisBike->bikeId: " << chooseThisBike->bikeId << "  chooseThisBike->sameTypeNext:" << chooseThisBike->sameTypeNext << endl;
                    stations->station[stoi(prevChosenBike->stationId.substr(1))]->bikes[stoi(prevChosenBike->bikeType.substr(1))] = chooseThisBike->sameTypeNext;
                }
                else{ // remove middle and tail
                    prevChosenBike->sameTypeNext = chooseThisBike->sameTypeNext;
                }
                
                // move bike to end station
                // if reach rentalCountLimit, dont delete this bike since nedd to be printed out in station_status.txt
                // AddBike update availableTime
                chooseThisBike->stationId = users[i]->endPoint;
                stations->AddBike(chooseThisBike->bikeType, chooseThisBike->bikeId,\
                chooseThisBike->stationId, chooseThisBike->rentalPrice, \
                chooseThisBike->rentalCount, chooseThisBike->availableTime);
                delete chooseThisBike;
            }
        }
        else{
            // reject user request
            //cout << "reject user request" << endl;
        }
    }
    cout << "total_revenue for basic version: " << total_revenue << endl;
    
}

void WriteUserResult(string selectedCase){
    ofstream f;
    f.open("./result/" + selectedCase + "/user_result.txt");
    if(f.is_open()){
        for(int i=0;i<numOfUsers;i++){
            f << "U" << i << " " << user_result[i][0] << " " << user_result[i][1];
            f << " " << user_result[i][2] << " " << user_result[i][3] << " " << user_result[i][4] << "\n";
        }
        f.close();
    }
    else{
        cout << "user_result not open" << endl;
    }
}
// iterate through stations to collect all bikes and sort by bikeid within same station
// use insertion sort since already sorted by stations, most of them only move a little bit
// preparation for station_status.txt
void SortBikeById(Stations* stations){
    int idx=0;
    // iterate through stations to collect all bikes
    for(int i=0;i<NUM_STATIONS;i++){
        // assume at most 50 types of bike
        for(int j=0;j<BIKETYPES;j++){
            Bike* sameTypeCur = stations->station[i]->bikes[j];
            if(sameTypeCur==NULL){
                continue;
            }
            while(sameTypeCur){
                stationStatus[idx++] = sameTypeCur;
                sameTypeCur = sameTypeCur->sameTypeNext;
            }
        }
    }
    // insertion sort
    // iterate through all bikes
    for(int i=1;i<NUM_BIKES;i++){
        Bike* bikeCur = stationStatus[i];
        int idx = i-1;
        // find the correct place for bikeCur
        while(idx>=0 && bikeCur && bikeCur->stationId == stationStatus[idx]->stationId \
        && bikeCur->bikeId < stationStatus[idx]->bikeId){
            // if station id the same and bikeCur id is smaller, move the compared bike one entry back
            stationStatus[idx+1] = stationStatus[idx];
            idx--;
        }
        // put bikeCur int the right place
        stationStatus[idx+1] = bikeCur;
    }
}

void WriteStationStatus(string selectedCase){
    ofstream f;
    f.open("./result/" + selectedCase + "/station_status.txt");
    if(f.is_open()){
        // at most 10000 bikes
        for(int i=0;i<NUM_BIKES;i++){
            Bike* cur = stationStatus[i];
            // cur might be null, causing segmentation fault
            if(cur){
                f << cur->stationId << " " << cur->bikeId << " " << cur->bikeType;
                f << " " << fixed << setprecision(1) << cur->rentalPrice << " " << cur->rentalCount << "\n";
            }
        }
        f.close();
    }
   
    else{
        cout << "station_status not open" << endl;
    }
}

void WriteTransferLog(string selectedCase){
    ofstream f;
    f.open("./result/" + selectedCase + "/transfer_log.txt");
    if(f.is_open()){
        // at most 10,000 users (requests)
        for(int i=0;i<numOfUsers;i++){
            // user_result[i][0] is ith user's AcceptOrNot
            if(user_result[i][0]!=0){
                f << user_result[i][1] << " S" << user_result[i][5];
                f << " S" << user_result[i][6] << " " << user_result[i][2];
                f << " " << user_result[i][3] << " " << "U" << i << "\n";
            }
        }
        f.close();
    }
    else{
        cout << "transfer_log.txt not open" << endl;
    }
}