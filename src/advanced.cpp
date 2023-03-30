#include "./nthu_bike.h"

using namespace std;
static void Ad_RentBikes(string selectedCase,User** users, Graph* graph, Stations* stations);
static int total_revenue;
static void Ad_SortUsersByEndTime(User** users,const int numOfUsers);


void advanced(string selectedCase){
    // insert your code here
    cout << "start your advanced version of data structure final from here!" << endl;

    Graph* graph = new Graph(); 
    ReadMap(selectedCase,graph);
    // graph->PrintGraph();
    Stations* stations = new Stations(); // store station info
    ReadBike(selectedCase,stations);
    // stations->PrintStation();
    User* users[NUM_USER]={0};
    ReadUser(selectedCase,users,numOfUsers);
    SortUsers(users,numOfUsers);
    //SortUsersById(users,numOfUsers);
    Ad_SortUsersByEndTime(users, numOfUsers);
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
    Ad_RentBikes(selectedCase,users,graph,stations);
    
    // reach final state, write to file "station_status.txt" after sorted by stationid and bikeid 
    // reach final state, write to file "user_result.txt" sorted by userid(when processing user requests, userids are not in ascending order(because sorted by price))
    WriteUserResult(selectedCase);
    //WriteTransferLog(selectedCase);

    // prepare for station_status.txt
    SortBikeById(stations);
    WriteStationStatus(selectedCase);
    // change the second parameter to validate output results

    // no need to validate since not going to be the same

    // user_result, station_status, transfer_log
    //validation(selectedCase, "station_status.txt");
    delete graph;
    delete stations;
    // not yet free bikes and stations (StationBikes*, Bike*)
}

// sorting users by startime and endtime, implement differently from basic version
// sort by endtime since we hope to accept more requests
static void Ad_SortUsersByEndTime(User** users,const int numOfUsers){
    for(int i=1;i<numOfUsers;i++){
        int targetEndTime = users[i]->endTime;
        User* target = users[i];
        int idx = i-1;
        // only if starttime is the same will they compare userid
        while(idx>=0 && users[idx]->startTime == target->startTime \
        && users[idx]->endTime > targetEndTime){
            users[idx+1] = users[idx];
            idx--;
        }
        if(idx < 0){
            users[0] = target;
        }
        else{
            users[idx+1] = target;
        }
    }

}


static void Ad_RentBikes(string selectedCase, User** users, Graph* graph, Stations* stations){
    Bike* chooseThisBike = NULL;
    // only transfer_log needs to include dynamic info: free transfer service
    ofstream f;
    f.open("./result/" + selectedCase + "/transfer_log.txt");
    if(!f.is_open()){
        cout << "file not open" << endl;
        return;
    }
    for(int i=0;i<numOfUsers;i++){
        int startPointNum = stoi(users[i]->startPoint.substr(1));
        int endPointNum = stoi(users[i]->endPoint.substr(1));
        if((users[i]->endTime - users[i]->startTime) >
        graph->shortestPath[startPointNum][endPointNum]){
            // check if available biketype
            chooseThisBike = NULL;
            // highestPrice needs to be type double! or 30.5 and 30 would be seen as the same
            double highestPrice = INT_MIN;
            // keep track of the bike prev to chooseThisBike for remove purpose
            Bike* prevBike = NULL;
            Bike* prevChosenBike = NULL;
            int userStartPointNum = stoi(users[i]->startPoint.substr(1));
            
            // iterate through all other stations to find the highest price of all accept bike types 
            for(int bikeStation=0;bikeStation<numOfStation;bikeStation++){
                // check if at start stations certain biketype available
                // iterate through all biketypes
                int acceptTypeNum = 0;
                while(users[i]->acceptBikeType[acceptTypeNum]!=""){
                // for(int j=0;j<acceptTypeNum;j++){
                    int bikeTypeNum = stoi(users[i]->acceptBikeType[acceptTypeNum].substr(1));
                    if(stations->station[bikeStation]->bikes[bikeTypeNum]==NULL){
                        acceptTypeNum++;
                        continue;
                    }
                    Bike* sameTypeBikeCur = stations->station[bikeStation]->bikes[bikeTypeNum];
                    // set prevBike to head of sametype
                    prevBike = sameTypeBikeCur;
                    // iterate through the list of sametype bikes
                    while(sameTypeBikeCur!=NULL){
                        
                        // check if time available
                        // wait for <= 40 sec
                        int delay = sameTypeBikeCur->availableTime + graph->shortestPath[stoi(sameTypeBikeCur->stationId.substr(1))][startPointNum] \
                            - users[i]->startTime;
                        if(delay <= 40 && (users[i]->endTime - users[i]->startTime) > (graph->shortestPath[startPointNum][endPointNum] + delay)){
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
                    acceptTypeNum++;
                }
            }
            if(!chooseThisBike){
                // reject this request
            }    
            else {
                int chosenBikeStation = stoi(chooseThisBike->stationId.substr(1));
                if(startPointNum!= chosenBikeStation){
                // transfer the bike to startPoint

                    if(users[i]->startTime >= chooseThisBike->availableTime + graph->shortestPath[chosenBikeStation][startPointNum]){
                        // can move bike to startPoint before starttime with no delay
                        // starttime dont need to postpone
                    }
                    else{
                        // calculate the delay in order to update starttime
                        users[i]->startTime = chooseThisBike->availableTime + graph->shortestPath[chosenBikeStation][startPointNum];
                    }
                    // dont update choosedbike station, or prevChosenBikeId will be wrong
                    //chooseThisBike->stationId = users[i]->startPoint;
                    // use old chooseThisBike->availableTime
                    f << chooseThisBike->bikeId << " " << chooseThisBike->stationId << " " << users[i]->startPoint \
                    << " " << chooseThisBike->availableTime << " " \
                    << chooseThisBike->availableTime + graph->shortestPath[chosenBikeStation][startPointNum] << " -1" << endl;
                }
                else if(users[i]->startTime < chooseThisBike->availableTime){
                    users[i]->startTime = chooseThisBike->availableTime;
                }
                    // update bike's available time
                    // (not counted from last available time since might not be rented right after returned by others)
                chooseThisBike->availableTime = users[i]->startTime + graph->shortestPath[startPointNum][endPointNum];
                

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
                f << chooseThisBike->bikeId << " " << users[i]->startPoint << " " << users[i]->endPoint \
                << " " << users[i]->startTime << " " << chooseThisBike->availableTime << " U" << idx << endl;
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
                stations->DeleteBike(chooseThisBike);
                chooseThisBike = NULL;
                
            }
        }
        // else reject user request
    }
    f.close();
    cout << "total_revenue for advance version: " << total_revenue << endl;
}