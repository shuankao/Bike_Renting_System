#include "./nthu_bike.h"

using namespace std;

void ReadBike(string selectedCase,Stations* stations){
    //Stations* stations = *s;
    string path = "./testcases/" + selectedCase+ "/bike.txt";
    string line;
    ifstream infile;
    infile.open(path);
    if(infile.is_open()){
        stations->Initialize();
        while(getline(infile,line)){
            stringstream stream(line);
            string biketype,stationid;
            int bikeid,rentalcount;
            double rentalprice;
            stream >> biketype >> bikeid >> stationid >> rentalprice >> rentalcount;
            //cout << biketype << ' '<< bikeid <<' '<< stationid <<' '<< rentalprice <<' '<< rentalcount << endl;
            stations->AddBike(biketype,bikeid,stationid,rentalprice,rentalcount);
        }
        infile.close();
    }else{
        cout << "not open" << endl;
    }
}

void ReadBikeInfo(string selectedCase, double& depDiscPrice,int& rentalCountLimit){
    string path = "./testcases/" + selectedCase+ "/bike_info.txt";
    string line;
    ifstream infile;
    infile.open(path);
    if(infile.is_open()){
        getline(infile,line);
        depDiscPrice = stod(line);
        getline(infile,line);
        rentalCountLimit = stoi(line);
        infile.close();
    }else{
        cout << "not open" << endl;
    }
    cout << depDiscPrice << " " << rentalCountLimit << endl;
}

// called when ReadBike and before AddBike while loop
// initialize array of objects of class StationBikes
void
Stations::Initialize(){
    for(int i=0;i<1000;i++){
        station[i] = new StationBikes;
    }
}

//type and stationID are string type
// need to update availableTime
void
Stations::AddBike(string type,int bikeID,string stationID,double price,int count, int availTime){
    Bike* newBike = new Bike(type,bikeID,stationID,price,count,availTime);
    int sID = stoi(stationID.substr(1)); // convert stationID from string to int
    int t = stoi(type.substr(1)); // convert type from string to int
    Bike* head = station[sID]->bikes[t];
    if(!head){
        station[sID]->bikes[t] = newBike;
    }else{
        Bike* prev = head;
        while(head && newBike->rentalPrice<=head->rentalPrice){ // let bike with higher price be put in front
            //cout <<"newbikeid: " << newBike->bikeId << " price: "<< newBike->rentalPrice << endl;
            //cout <<"headbikeid: " << head->bikeId << " price: "<< head->rentalPrice << endl;
            if(newBike->rentalPrice==head->rentalPrice){ // if price the same, compare bikeId
                if(newBike->bikeId<head->bikeId) break;
            }
            prev = head;
            head = head->sameTypeNext;
        }
        if(!head){// add at tail
            //cout << "here" << endl;
            prev->sameTypeNext = newBike;
        }else{
            newBike->sameTypeNext = head;
            if(prev==head){ // consider inserting at front, important!
                station[sID]->bikes[t] = newBike;
            }
            else prev->sameTypeNext = newBike;
        }
    }
    //cout << station[stoi(stationID.substr(1))]->bikes[stoi(type.substr(1))]->bikeId << endl;
    //if(head) cout << head->bikeId << endl;
}

void
Stations::DeleteBike(Bike* bikeToBeDeleted){
    delete bikeToBeDeleted;
}

//void Stations::PrintStation(){};

void 
Stations::PrintStation(){
    for(int i=0;i<1000;i++){
        // bike types 10000
        for(int j=0;j<BIKETYPES;j++){
            if(station[i]->bikes[j]){
                cout << "stations " << i << " biketype " << j;
                Bike* head = station[i]->bikes[j];
                while(head){
                    cout << "    bikeid " << head->bikeId << " price " << head->rentalPrice;
                    head = head->sameTypeNext;
                }
                cout << endl;
            }
        }
    }
}


