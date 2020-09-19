/*
    How Swiss system pair works, the essence of this program

    This system will never allow two players to compete more than once, so 7 players can only play
    7/2*(7-1)=21 times. After that, we will try to match two players with the same score. If turns out
    there's none, we will try to match this player with another player who has the closest score.
    We also need to balance the amount of playing as white with playing as black per player.
    If the amount of rounds are more than 21. I will add "second ply", and one ply consists of 21 matches.
    After these 21 match, second ply will allow players to compete more than once and we can match
    players again.
*/
#include <iostream>
#include <fstream>
#include <filesystem>
#include <stdlib.h>
#include <time.h>
#include <cmath>
using namespace std;

//maximum
const short int slotmax=10, playermax=50, matchmax=200, byemax=30;
const short int list=7; //amount of list in config file, which is slot number, name, score, white, black, wplayer, bplayer, playerbye
//point gained
const short winp=2, byep=2, drawp=1,losep=0;
short int player;       //current player
short int playernum;    //number of players
short int rnd,rndnum,rndmax,slot,ply;
short int score[playermax], black[playermax], white[playermax];
short int match=0, matchnum, matchperrnd, matchperply, rndperply;
short int wplayer[matchmax], bplayer[matchmax], playerbye[byemax];
string name[playermax];
bool alwayssave, bye, deletedplayer[playermax];
string input;

void restart(int i){
    score[i]=0;
    black[i]=0;
    white[i]=0;
}
void displaydata(){
    system("CLS");
        cout << "Player list: " << endl;
        for(int i=0; i < playernum; i++){
            cout << i+1 << ". ";
            cout << name[i]<<endl;
        }
    cout<<"Number of round(s): "<<rndnum<<endl;
    cout<<"Number of matches: "<<matchnum<<endl;
}
void readfromcfg(string dataln, string datatype){
/*  confing line syntax:
    name a b c d e f g
    name is the data type, a is 0th player data, etc.
    name is -1th player data, which will not be stored in the array.
    a is 0th player data.
    b is 1st player data.
    This applies to name, score, white, black, wplayer, and bplayer.
*/
    string dataword;
/*  Processing stages
    for name: nameln -> nameword=name
    else    : dataln -> dataword -> atoi(convert str to int) -> data
    dataln  : data stored in string format, per line
    dataword: data stored in string format, per player
    data    : data stored in int format, per player
*/
    int w=-1, length=dataln.length();
    for(int i=0; i <= length; i++){
        if(dataln[i]=='/'||i==length){
            if(w>=0){
                if(datatype=="slot"&&w==0)    rnd=stoi(dataword);
                else if(datatype=="slot"&&w==1)rndnum=stoi(dataword);
                else if(datatype=="name"){
                    name[w]=dataword;
                    playernum++;
                }
                else if(datatype=="score")    score[w]=stoi(dataword);
                else if(datatype=="white")    white[w]=stoi(dataword);
                else if(datatype=="black")    black[w]=stoi(dataword);
                else if(datatype=="wplayer"){
                    wplayer[w]=stoi(dataword);
                    matchnum++;
                }
                else if(datatype=="bplayer")  bplayer[w]=stoi(dataword);
                else if(datatype=="playerbye")playerbye[w]=stoi(dataword);
            }
            w++;               //slash indicates a new word
            dataword="";        //restart word
        }
        else dataword+=dataln[i];
    }
}
void load(){
    string slotln, nameln, scoreln, whiteln, blackln, wplayerln, bplayerln, playerbyeln;  //data stored in the config file, line by line
    ifstream config("swiss.cfg");
    playernum=0;
    matchnum=0;
/*  Config file structure:
    Slot 0      0th line
    name        1st line
    score       2nd line
    white
    black
    Slot 1
    name etc.
    To access Slot 1, skip 0th to 5th line.
    To access Slot 2, skip 0th to 10th line, etc.
*/
    for(int i=0; i <list*slot; i++){
        getline(config, nameln);
    }
    //and then access the desired data, store it into arrays.
    getline(config, slotln);
    readfromcfg(slotln, "slot");
    getline(config, nameln);
    readfromcfg(nameln, "name");
    getline(config, scoreln);
    readfromcfg(scoreln, "score");
    getline(config, whiteln);
    readfromcfg(whiteln, "white");
    getline(config, blackln);
    readfromcfg(blackln, "black");
    getline(config, wplayerln);
    readfromcfg(wplayerln, "wplayer");
    getline(config, bplayerln);
    readfromcfg(bplayerln, "bplayer");
    getline(config, playerbyeln);
    readfromcfg(playerbyeln, "playerbye");
    config.close();
    displaydata();
}
void storetocfg(string datatype, ofstream &configtemp){
    configtemp << datatype;
    if(datatype=="Slot")configtemp <<" "<< slot << "/" << rnd << "/" << rndnum;
    if(datatype=="name")for(int i=0; i < playernum; i++)configtemp << "/" << name[i];
    else if(datatype=="score")for(int i=0; i < playernum; i++)configtemp << "/" << score[i];
    else if(datatype=="white")for(int i=0; i < playernum; i++)configtemp << "/" << white[i];
    else if(datatype=="black")for(int i=0; i < playernum; i++)configtemp << "/" << black[i];
    else if(datatype=="wplayer")for(int i=0; i < matchnum; i++)configtemp << "/" << wplayer[i];
    else if(datatype=="bplayer")for(int i=0; i < matchnum; i++)configtemp << "/" << bplayer[i];
    else if(datatype=="playerbye")for(int i=0; i < rndnum; i++)configtemp << "/" << playerbye[i];
}
void save(){
    string line;
    ifstream config("swiss.cfg");
    ofstream configtemp("swisstemp.cfg");
    cin.ignore();
    for(int i=0; i<list*slotmax; i++){
        getline(config, line);
        if(i==slot*list)storetocfg("Slot", configtemp);
        else if(i==slot*list+1)storetocfg("name", configtemp);
        else if(i==slot*list+2)storetocfg("score", configtemp);
        else if(i==slot*list+3)storetocfg("white", configtemp);
        else if(i==slot*list+4)storetocfg("black", configtemp);
        else if(i==slot*list+5)storetocfg("wplayer", configtemp);
        else if(i==slot*list+6)storetocfg("bplayer", configtemp);
        else if(i==slot*list+7)storetocfg("playerbye", configtemp);
        else configtemp << line;
        configtemp << endl;
    }
    config.close();
    configtemp.close();
    remove("swiss.cfg");
    rename("swisstemp.cfg", "swiss.cfg");
}
void resulttoscore(){
    if(input=="w")score[wplayer[match]]+=winp;
    if(input=="b")score[bplayer[match]]+=winp;
    if(input=="d"){
        score[wplayer[match]]+=drawp;
        score[bplayer[match]]+=drawp;
    }
}
void executebye(){
    bool canbye[playernum];
    short int nbye=playernum, r;                        //r stands for random
    for(int i=0;i<playernum;i++){
        canbye[i]=true;                                 //suppose everyone can bye
    }
    for(int i=rndperply*ply;i<rndperply*(ply+1);i++){   //scan this ply
        if(playerbye[i]>=0){
            canbye[playerbye[i]]=false;                     //if has bye once, don't bye again.
            nbye--;
        }
    }
    playerbye[rnd]=-1;
    srand(time(NULL));
    r=rand()%nbye+1;
/*  suppose nbye=5, r=2
    canbye={true, false, false, true,...}
*/
    for(int i=0; r>0; i++){
        if(canbye[i]){
            r--;
            playerbye[rnd]=i;
        }
    }
}
bool hasplayed(int player){
    for(int i=matchperrnd*rnd; i<matchperrnd*(rnd+1);i++){
            if(wplayer[i]==player||bplayer[i]==player)return true;
        }
    return false;
}
void makepair(){
/*
    How Swiss pairing system works:
    1. You can't play yourself
    2. This system will never allow two players to compete more than once in a ply.
    3. Byed player and player that has played in a round can't play
    4. Opponents which have too far score compared to player can't match with player.
    5. Balance black and white
*/
    if(bye)executebye();//amount of players might be odd, so everyone might not play once at a time.
    bool candidate[playernum];//List of candidates, highest value of candidate(s) will be chosen randomly
    short int diff[playernum], mindiff, nmatch, r;
    match=matchperrnd*rnd;
    //let's do pairing per player
    for(int i=0; i<playernum; i++){
        mindiff=SHRT_MAX;
        if(i!=playerbye[rnd]&&!hasplayed(i)){        //#3
            nmatch=playernum;
            for(int j=0; j<playernum; j++){
                candidate[j]=true;                                  //anyone can play against another
                if(i==j){               //#1
                    candidate[i]=false;
                    nmatch--;
                }                        
                else if(hasplayed(j)){//#2 
                    candidate[j]=false;
                    nmatch--;
                }      
                else if(j==playerbye[rnd]){//#3
                    candidate[playerbye[rnd]]=false;
                    nmatch--;           
                }
                diff[j]=SHRT_MAX;
                if(candidate[j]){
                    diff[j]=abs(score[j]-score[i]);
                    if(diff[j]<mindiff)mindiff=diff[j];
                }
            }
            for(int j=0; j<playernum; j++){
                if(diff[j]!=mindiff&&candidate[j]){
                    candidate[j]=false;
                    nmatch--;
                }
            }
            srand(time(NULL)+i);
            r=rand()%nmatch+1;
            //r=3=3rd true match
            //j should be 5
            //candidate[m]={false, true, true, false, false, true,...}
            int j=-1;
            do{
                j++;
                if(candidate[j]==true)r--;
            }while(r>0);
            if(white[i]==black[j]){  //#5
                srand(2*time(NULL)+i+1);
                r=rand()%2;
                if(r==0){
                    wplayer[match]=i;white[i]++;
                    bplayer[match]=j;black[j]++;
                }
                if(r==1){
                    wplayer[match]=j;white[j]++;
                    bplayer[match]=i;black[i]++;
                }
            }
            else if(white[i]>black[j]){
                wplayer[match]=j;white[j]++;
                bplayer[match]=i;black[i]++;
            }
            else{
                wplayer[match]=i;white[i]++;
                bplayer[match]=j;black[j]++;
            }
            match++;      
        }
    }
}
void addplayer(){
    playernum++;
    cout<<"Player name: ";
    cin.ignore();
    getline(cin, input);
    player=playernum-1;
    name[player]=input;
    restart(player);
}
void delplayer(){
    cout<<"Player name: ";
    cin.ignore();
    getline(cin, input);
    cin>>player;
    name[player]="";
    restart(player);
}
void namedit(){
    name[player]="";
    cout<<"Player no.: ";
    cin>>player;
    cout<<"Player name: ";
    cin.ignore();
    getline(cin, input);
    name[player-1]=input;
}
void rndedit(){
    cout<<"Number of round(s): ";
    cin>>rndnum;
    name[playernum+1]=input;
    matchperrnd=floor(playernum/2);
    matchnum=rndnum*matchperrnd;
}
void edit(){
    cout<<"Press c to confirm, a to add player, d to delete player, n to change name, r to change round.\n";
    cout<<"Your answer: ";
    cin>>input;
    while(input!="c"&&input!="a"&&input!="d"&&input!="n"&&input!="r"){
        cout<<"Invalid!"<<endl;
        cout<<"Your answer: ";
        cin>>input;
    }
    if(input=="a")addplayer();
    else if(input=="d")delplayer();
    else if(input=="n")namedit();
    else if(input=="r")rndedit();
}
int inputresult(){
    int inputint;
    while(inputint!=-2){
        cout<<"Enter match number to edit result manually, "<<endl
        <<"0. edit result sequentially "<<endl
        <<"-1. to edit something,"<<endl
        <<"-2. to end match: "<<endl;
        cin>>inputint;
        if(inputint==-1)edit();
        else if(inputint==0){
            cout <<"w. white wins"<<endl
            <<"b. black wins"<<endl
            <<"d. draw"<<endl
            <<"f. forfeit"<<endl;
            for(match=matchperrnd*rnd; match<matchperrnd*(rnd+1); match++){
                cout<<match+1<<". "<<name[wplayer[match]]<<" vs "<<name[bplayer[match]]<<endl;
                cout<<"Result: ";
                cin>>input;
                resulttoscore();
            }
        }
        else if(inputint>0){
            cout <<"w. white wins"<<endl
            <<"b. black wins"<<endl
            <<"d. draw"<<endl
            <<"f. forfeit"<<endl;
            match=inputint-1;
            cout<<match+1<<". "<<name[wplayer[match]]<<" vs "<<name[bplayer[match]]<<endl;
            cout<<"Result: ";
            cin>>input;
            resulttoscore();
        }
    }
    return 0;
}
void sort(bool display){ //selection sort descending
    string namesort[playernum];
    short int scoresort[playernum], scoremax=SHRT_MIN, k=0;
    for(int i=0;i<playernum; i++)if(score[i]>scoremax)scoremax=score[i];
    for(int j=scoremax;j>=0;j--)for(int i=0;i<playernum;i++){
        if(score[i]==j){
            namesort[k]=name[i];
            scoresort[k]=score[i];
            k++;
        }
    }
    if(display){
        for(int i=0; i<playernum; i++){
            cout<<i+1<<". "<<namesort[i]<<"      "<<scoresort[i]<<endl;
        }
    }
}
void finalresult(){
    cout<<"Leaderboards: "<<endl;
    sort(true);
    cout<<"Congratulations for the winner!!! Enter y to exit...";
    cin>>input;
}
void rndexecute(){
/*
    rnd     match    matchperrnd=3, players are 6 or 7
    0       0
            1
            2
    1       3
            4
            5
    rndnum  matchnum
*/
    matchperrnd=floor(playernum/2);    //match per round=floor(number of player / 2)
    matchperply=playernum*(playernum-1)/2;
    rndperply=matchperply/matchperrnd;
    if(playernum%2==1)bye=true;
    for(rnd=rnd; rnd < rndnum; rnd++){
        ply=floor(rnd*matchperrnd/matchperply);
        makepair();
        system("CLS");
        cout<<"Round "<<rnd+1<<"!!!"<<endl;
        cout<<"Match no. White vs Black"<<endl;
        for(match=matchperrnd*rnd; match<matchperrnd*(rnd+1); match++){
            cout<<match+1<<". "<<name[wplayer[match]]<<" vs "<<name[bplayer[match]]<<endl;
        }
        if(bye){
            score[playerbye[rnd]]+=byep;
            cout<<"Bye, "<<name[playerbye[rnd]]<<"! "
            <<name[playerbye[rnd]]<<" gets a bonus by "<<byep<<" point(s)."<<endl;
        }
        inputresult();
        if(rnd<rndnum-1){
            system("CLS");
            cout<<"Round "<<rnd+1<<" results: "<<endl;
            sort(true);
            cout<<"Enter y to continue..."<<endl;
            cin>>input;
        }
        if(alwayssave==true)save;
    }
}
void manualinput(){
    cout << "How many players? (3 ~ " << playermax << "): ";
    cin >> playernum;
    while(playernum<3 || playernum>playermax){
        cout << "Invalid!" << endl;
        cout << "How many players? ";
        cin >> playernum;
    }
    cin.ignore();
    cout << "Name of the player?" << endl;
    for(int i=0; i < playernum; i++){
        restart(i);
        cout << i+1 << ". ";
        input="";
        getline(cin, input);
        name[i]=input;
    }
}
int interface(){
    cout << "Swiss pairing by John Ruben Saragi" << endl;
    cout << "Do you want to enter player's name (y) or load a slot (n)?: " << endl;
    cin >> input;
    if(input=="y")manualinput();
    else if(input=="n"){
        cout << "Enter 0 to 9 to load: ";
        cin >> slot;
        alwayssave=true;
        load();
        return 0;
    }
    matchperrnd=floor(playernum/2);
    rndmax=floor(matchmax/floor(playernum/2));
    for(int i=0; i<byemax; i++)playerbye[i]=-1;
    cout << "How many round(s)? (1 ~ " << rndmax << "): "; //accidentally sad face ):
    cin >> rndnum;
    while(rndnum<1 || rndnum>rndmax){
        cout << "Invalid!" << endl;
        cout << "How many round(s)? (1 ~ " << rndmax << "): ";
        cin >> rndnum;
    }
    matchnum=rndnum*matchperrnd;
    for(int i=0; i<matchnum; i++){
        wplayer[i]=-1;
        bplayer[i]=-1;
    }
    input="n";
    while (input!="c"){
        displaydata();
        cout<<"Confirm? Option:"<<endl;
        edit();
    }
    cout << "Do you want to save this? (y/n)";
    cin >> input;
    if(input=="y"){
        cout << "Enter 0 to 9 to save, existing slot will be overwrited: ";
        cin >> slot;
        alwayssave=true;
    }
    if(alwayssave)save();
    return 0;
}

int main(){
    remove("swisstemp.cfg");
    ifstream config("swiss.cfg");
    if(!config){
        config.close();
        ofstream config("swiss.cfg");
        for(int i=0; i < slotmax; i++){
            config << "Slot " << i << endl 
            << "name " << endl 
            << "score " << endl 
            << "white " << endl 
            << "black " << endl
            << "wplayer " << endl
            << "bplayer " << endl;
        }
    }
    config.close();
    interface();
    rndexecute();
    finalresult();
    return 0;
}