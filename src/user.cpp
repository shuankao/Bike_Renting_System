#include "./nthu_bike.h"

using namespace std;
void SortUsersById(User** users,const int numOfUsers);

void ReadUser(string selectedCase,User** users,int& numOfUsers){
    string path = "./testcases/" + selectedCase+ "/user.txt";
    string line;
    ifstream infile;
    infile.open(path);
    if(infile.is_open()){
        int idx=0;
        while(getline(infile,line)){
            stringstream stream(line);
            string userid,acceptType,starttime,endtime,startpoint,endpoint;
            getline(stream,userid,' ');
            getline(stream,acceptType,' ');
            getline(stream,starttime,' ');
            getline(stream,endtime,' ');
            getline(stream,startpoint,' ');
            getline(stream,endpoint,' ');
            users[idx++] =  new User(userid,acceptType,stoi(starttime),stoi(endtime),startpoint,endpoint);
            numOfUsers++;
        }
        infile.close();
    }else{
        cout << "not open" << endl;
    }
}

void PrintUsers(User** users){
    for(int i=0;i<NUM_USER;i++){
        if(users[i]){
            int idx=0;
            cout << users[i]->userId << " ";
            while(users[i]->acceptBikeType[idx]!=""){
                cout << users[i]->acceptBikeType[idx++] << " ";
            }
            cout << users[i]->startTime << " ";
            cout << users[i]->endTime << " ";
            cout << users[i]->startPoint << " ";
            cout << users[i]->endPoint << " ";
            cout << endl;
        }
        else break;
    }
}

// sort users by startTime
// bucket sort since startTime, endTime at most 4 digits(1440)
// dealt with smaller userId first when startTime the same with SortUsersById()
void SortUsers(User** users, const int numOfUsers){
    // sort users with bucket sort
    User* bucket[10][numOfUsers]; // time is decimal, so base = 10 as num of rows
    // in SortUsers(), the array "link" stores every round result
    // need to initialize, or segmentation fault when link[j]->startTime (since no object and not NULL)
    User* link[numOfUsers];
    //put users into link
    for(int j=0;j<numOfUsers;j++){
        if(users[j]) {
            link[j] = users[j];
        }
        else break;
    }
    // pow used to extract each digit from least significant digit (LSD)
    int pow=1;
    // 4 digits, so 4 rounds
    for(int i=0;i<4;i++){
        //clean the bucket
        for(int j=0;j<10;j++){
            for(int k=0;k<numOfUsers;k++){
                bucket[j][k] = NULL;
            }
        }
        // put users into buckets
        pow *=10;
        for(int j=0;j<numOfUsers;j++){
            int bucketNum = (link[j]->startTime)*10/pow%10;
            int idx2=0;
            while(bucket[bucketNum][idx2]){
                idx2++;
            }
            bucket[bucketNum][idx2] = link[j];
        }
        // store users' new order to array "link"
        int idx2=0;
        for(int j=0;j<10;j++){
            for(int k=0;k<numOfUsers;k++){
                if(bucket[j][k]){
                    link[idx2++] = bucket[j][k];
                }
            }
        }
    }
    //put link (sorted result) back to users
    for(int j=0;j<numOfUsers;j++){
        if(link[j]) users[j] = link[j];
        else break;
    }
    
}

// after sort user.txt by startTime, sort by userID if two has the same startTime
// use insertion sort since most of entries are already sorted, nearly O(n) time
void SortUsersById(User** users,const int numOfUsers){
    for(int i=1;i<numOfUsers;i++){
        int target_id = stoi((users[i]->userId).substr(1));
        User* target = users[i];
        int idx = i-1;
        // only if starttime is the same will they compare userid
        while(idx>=0 && users[idx]->startTime == target->startTime \
        && stoi((users[idx]->userId).substr(1)) > target_id){
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