// duplicated for eecs398 hw11
//  main.cpp
//  project2
//
//  Created by Haoxuan Zhu on 10/8/18.
//  Copyright © 2018 Zhu Haoxuan. All rights reserved.
//

#include <iostream>
#include <getopt.h>
#include <algorithm>
#include <string>
#include <vector>
#include <iomanip>
#include "P2random.h"
#include <queue>
#include <deque>


using namespace std;

struct Coord{
    unsigned int row;
    unsigned int col;
};

struct Tile {
    unsigned int row;
    unsigned int col;
    short rubble;
    bool visited;
    
};

class Grid {
public:
    void get_options(int argc, char** argv);
    void test_grid();
    void create_map();
    
    unsigned int nStats;
    bool medianOn;
    bool statsOn;
    bool verboseOn;
    
    char mapMode;
    unsigned int mapSize;
    unsigned int startRow;
    unsigned int startCol;
    
    vector<vector<Tile>> gridmap;
};

//void print_grid(Grid grid);//test function

struct compare{
    bool operator()(const Tile &left, const Tile &right) const{
        if(left.rubble != right.rubble){
            return right.rubble < left.rubble;
        }
        else{
            if(left.col != right.col){
                return right.col < left.col;
            }
            else{
                //if(left.row != right.row){
                return right.row < left.row;
                //}
            }
        }//else
    }//operator
};//compare

struct reverse_compare{
    bool operator()(const Tile &left, const Tile &right) const{
        if(left.rubble != right.rubble){
            return left.rubble < right.rubble;
        }
        else{
            if(left.col != right.col){
                return left.col < right.col;
            }
            else{
                //if(left.row != right.row){
                return left.row < right.row;
                //}
            }
        }//else
    }//operator
};//reverse_compare


double calMedians(double &med, short &x, priority_queue<short> &s,priority_queue<short,vector<short>,greater<short> > &g);




int main(int argc, char * argv[]) {
    ios_base::sync_with_stdio(false);
    cout<<std::fixed<<std::setprecision(2);
#ifdef __APPLE__
    //std::string infile = "/Users/zhuhaoxuan/Documents/UM_EECS/EECS281/project2/project2/project2/test.txt";
    std::string infile = "/Users/zhuhaoxuan/Documents/UM_EECS/EECS281/project2/project2/project2/spec-M.txt";
    //std::string infile = "/Users/zhuhaoxuan/Documents/UM_EECS/EECS281/project2/project2/project2/spec-R.txt";
    if(!freopen(infile.c_str(), "r", stdin)){
        std::cerr << "Unable to open input file" << std::endl;
        exit(1);
    }
    //freopen("/Users/zhuhaoxuan/Documents/UM_EECS/EECS281/Project1/project1/project1/output.txt","w",stdout);
#endif
    
    
    Grid grid;
    grid.get_options(argc, argv);
    grid.create_map();
    int amount=0;
    int num=0;
    
    //for median
    //vector<unsigned int> medVec;
    double med;
    priority_queue<short> medHeap_l ;
    priority_queue<short, vector<short>, greater<short>> medHeap_h;
    //end median
    
    //for stats
    priority_queue<Tile, vector<Tile>, compare> stats_easy;
    priority_queue<Tile, vector<Tile>,reverse_compare> stats_hard;
    vector<Tile> sequence;
    deque<Tile> sequence2;
    
    //end stats
    
    
    priority_queue<Tile, vector<Tile>, compare> pq;
    priority_queue<Tile, vector<Tile>, compare> tntpq;
    
    Coord curr, currTnt;
    
    //initial state
    pq.push(grid.gridmap[grid.startRow][grid.startCol]);
    
    //loop
    do{
        curr.row=pq.top().row;
        curr.col=pq.top().col;
        pq.pop();
        while(grid.gridmap[curr.row][curr.col].visited==true){
            curr.row=pq.top().row;
            curr.col=pq.top().col;
            pq.pop();
        }
        grid.gridmap[curr.row][curr.col].visited=true;
        
        
        if(grid.gridmap[curr.row][curr.col].rubble != -1){
            if(grid.gridmap[curr.row][curr.col].rubble!=0){
                amount+=grid.gridmap[curr.row][curr.col].rubble;
                num++;
                if(grid.verboseOn){
                    cout<<"Cleared: "<<grid.gridmap[curr.row][curr.col].rubble<<" at ["<<curr.row<<","<<curr.col<<"]\n";
                }
                if(grid.medianOn){
                    med=calMedians(med, grid.gridmap[curr.row][curr.col].rubble, medHeap_l, medHeap_h);
                    cout<<"Median difficulty of clearing rubble is: "<<med<<"\n";
                }
                if(grid.statsOn){
                    stats_easy.push(grid.gridmap[curr.row][curr.col]);
                    stats_hard.push(grid.gridmap[curr.row][curr.col]);
                    if(sequence.size()<grid.nStats){
                        sequence.push_back(grid.gridmap[curr.row][curr.col]);
                        sequence2.push_front(grid.gridmap[curr.row][curr.col]);
                    }
                    else{
                        sequence2.pop_back();
                        sequence2.push_front(grid.gridmap[curr.row][curr.col]);
                    }
                }
            }
            
            grid.gridmap[curr.row][curr.col].rubble=0;
            
            if(curr.row >0){
                pq.push(grid.gridmap[curr.row-1][curr.col]);
            }
            if(curr.row +1 <grid.mapSize){
                pq.push(grid.gridmap[curr.row+1][curr.col]);
            }
            if(curr.col >0){
                pq.push(grid.gridmap[curr.row][curr.col-1]);
            }
            if(curr.col +1 <grid.mapSize){
                pq.push(grid.gridmap[curr.row][curr.col+1]);
            }
        }
        else{//grid.gridmap[curr.row][curr.col].rubble == -1
            tntpq.push(grid.gridmap[curr.row][curr.col]);
            
            while(!tntpq.empty()){
                currTnt.col=tntpq.top().col;
                currTnt.row=tntpq.top().row;
                tntpq.pop();
                
                if(grid.gridmap[currTnt.row][currTnt.col].rubble == -1){
                    if(grid.verboseOn){
                        cout<<"TNT explosion at ["<<currTnt.row<<","<<currTnt.col<<"]!\n";
                    }
                    if(grid.statsOn){
                        stats_easy.push(grid.gridmap[currTnt.row][currTnt.col]);
                        stats_hard.push(grid.gridmap[currTnt.row][currTnt.col]);
                        if(sequence.size()<grid.nStats){
                            sequence.push_back(grid.gridmap[currTnt.row][currTnt.col]);
                            sequence2.push_front(grid.gridmap[currTnt.row][currTnt.col]);
                        }
                        else{
                            sequence2.pop_back();
                            sequence2.push_front(grid.gridmap[currTnt.row][currTnt.col]);
                        }
                    }
                    
                    grid.gridmap[currTnt.row][currTnt.col].rubble=0;//clear the rubble
                    
                    if(currTnt.row > 0){
                        tntpq.push(grid.gridmap[currTnt.row-1][currTnt.col]);
                    }
                    if(currTnt.row +1 <grid.mapSize){
                        tntpq.push(grid.gridmap[currTnt.row+1][currTnt.col]);
                    }
                    if(currTnt.col >0){
                        tntpq.push(grid.gridmap[currTnt.row][currTnt.col-1]);
                    }
                    if(currTnt.col +1 <grid.mapSize){
                        tntpq.push(grid.gridmap[currTnt.row][currTnt.col+1]);
                    }
                }
                else{
                    if(grid.gridmap[currTnt.row][currTnt.col].rubble !=0){
                        amount+=grid.gridmap[currTnt.row][currTnt.col].rubble;
                        num++;
                        if(grid.verboseOn){
                            cout<<"Cleared by TNT: "<<grid.gridmap[currTnt.row][currTnt.col].rubble<< " at ["<<currTnt.row<<","<<currTnt.col<<"]\n";
                        }
                        if(grid.medianOn){
                            med=calMedians(med, grid.gridmap[currTnt.row][currTnt.col].rubble, medHeap_l, medHeap_h);
                            cout<<"Median difficulty of clearing rubble is: "<<med<<"\n";
                        }
                        if(grid.statsOn){
                            stats_easy.push(grid.gridmap[currTnt.row][currTnt.col]);
                            stats_hard.push(grid.gridmap[currTnt.row][currTnt.col]);
                            if(sequence.size()<grid.nStats){
                                sequence.push_back(grid.gridmap[currTnt.row][currTnt.col]);
                                sequence2.push_front(grid.gridmap[currTnt.row][currTnt.col]);
                            }
                            else{
                                sequence2.pop_back();
                                sequence2.push_front(grid.gridmap[currTnt.row][currTnt.col]);
                            }
                        }
                    }
                    
                    grid.gridmap[currTnt.row][currTnt.col].rubble=0;//clear the rubble
                    
                    if(!grid.gridmap[currTnt.row][currTnt.col].visited){//newly added for speed
                        pq.push(grid.gridmap[currTnt.row][currTnt.col]);
                    }
                }
                
            }//while tntpq
        }
        // if(curr.col == 0 or curr.row ==0 or curr.col == grid.mapSize-1 or curr.row == grid.mapSize-1){break;}
    }while(curr.col != 0 and curr.row !=0 and curr.col != grid.mapSize-1 and curr.row != grid.mapSize-1);
    
    cout<<"Cleared "<<num<<" tiles containing "<<amount<<" rubble and escaped.\n";
    
    
    //stats
    if(grid.statsOn){
        cout<<"First tiles cleared:\n";
        for(auto i=sequence.cbegin();i!=sequence.cend();++i){
            if(i->rubble != -1){
                cout<<i->rubble<<" at ["<<i->row<<","<<i->col<<"]\n";
            }
            else{
                cout<<"TNT at ["<<i->row<<","<<i->col<<"]\n";
            }
        }
        
        cout<<"Last tiles cleared:\n";
        for(auto i=sequence2.cbegin();i!=sequence2.cend();++i){
            
            if(i->rubble != -1){
                cout<<i->rubble<<" at ["<<i->row<<","<<i->col<<"]\n";
            }
            else{
                cout<<"TNT at ["<<i->row<<","<<i->col<<"]\n";
            }
        }
        
        cout<<"Easiest tiles cleared:\n";
        unsigned int dest=static_cast<unsigned int>(min((int)(grid.nStats),(int)(stats_easy.size())));
        for(unsigned int i=0; i<dest; ++i){
            if(stats_easy.top().rubble != -1){
                cout<<stats_easy.top().rubble<<" at ["<<stats_easy.top().row<<","<<stats_easy.top().col<<"]\n";
            }
            else{
                cout<<"TNT at ["<<stats_easy.top().row<<","<<stats_easy.top().col<<"]\n";
            }
            stats_easy.pop();
        }
        
        cout<<"Hardest tiles cleared:\n";
        dest=static_cast<unsigned int>(min((int)(grid.nStats),(int)(stats_hard.size())));
        for(unsigned int i=0; i<dest; ++i){
            if(stats_hard.top().rubble != -1){
                cout<<stats_hard.top().rubble<<" at ["<<stats_hard.top().row<<","<<stats_hard.top().col<<"]\n";
            }
            else{
                cout<<"TNT at ["<<stats_hard.top().row<<","<<stats_hard.top().col<<"]\n";
            }
            stats_hard.pop();
        }
    }
    
    return 0;
}



/////////////////////////////////////////////////////////////////////////
////////////////////////////Grid Member Funcion//////////////////////////
/////////////////////////////////////////////////////////////////////////
////////////////////////////start     here///////////////////////////////
/////////////////////////////////////////////////////////////////////////

void Grid::get_options(int argc, char **argv){
    int option_index = 0, option = 0;
    
    struct option longOpts[] = {
        { "stats", required_argument, nullptr, 's' },
        { "median", no_argument, nullptr, 'm'},
        { "verbose", no_argument, nullptr, 'v'},
        { "help", no_argument, nullptr, 'h'},
        { nullptr, 0, nullptr, '\0' }
    };
    
    while ((option = getopt_long(argc, argv, "s:mvh", longOpts, &option_index)) != -1) {
        switch (option) {
            case 's':
                statsOn = true;
                nStats = static_cast<unsigned int>(stoi(optarg));
                break;
            case 'm':
                medianOn = true;
                break;
            case 'v':
                verboseOn = true;
                break;
            case 'h':
                cout<<"**********HELP MANUAL**********\n"
                << "\t[--stats | -s N]​: print extra summarization statistics upon termination.\n"
                <<"\t[--median | -m]:​ print the median difficulty of clearing a rubble tile that the miner has seen.\n"
                <<"\t[--verbose | -v]:​ print out every rubble value (or TNT) as a tile is being cleared.\n"
                <<"\t[--help | -h]: show the manual of the program\n" <<endl;
                exit(0);
        }//switch
    }//while
}//get_options


void Grid::create_map(){
    cin>>mapMode;
    if(mapMode != 'M' and mapMode != 'R'){//Check Error
        cerr<<"First line character must be 'M' or 'R'!"<<endl;
        exit(1);
    }//if
    
    //get size and start point for M
    string rubbish;
    cin>>rubbish>>mapSize;
    int int_SR, int_SC;
    cin>>rubbish>>int_SR>>int_SC;
    
    if(int_SR<0 or int_SC<0){
        cerr<<"Start location is outside the grid!"<<endl;
        exit(1);
    }
    
    startCol=static_cast<unsigned int>(int_SC);
    startRow=static_cast<unsigned int>(int_SR);
    
    if(startRow>=mapSize or startCol>=mapSize){//Check Error
        cerr<<"Start location is outside the grid!"<<endl;
        exit(1);
    }//if
    
    stringstream ss;
    if(mapMode == 'R'){
        unsigned int seed, maxRubble, tnt;
        cin>>rubbish>>seed;
        cin>>rubbish>>maxRubble;
        cin>>rubbish>>tnt;
        //cout<<mapSize<<", "<<startRow<<", "<<startCol<<", "<<seed<<", "<<maxRubble<<", "<<tnt<<endl;//test
        P2random::PR_init(ss, mapSize, seed, maxRubble, tnt);
    }
    istream &inputStream = (mapMode == 'M') ? cin : ss;
    
    //store rubble
    Tile tile;
    gridmap.resize(mapSize, vector<Tile>(mapSize));
    for(unsigned i = 0; i<mapSize; ++i){
        for(unsigned j=0;j<mapSize;j++){
            tile.row = i;
            tile.col = j;
            inputStream>>tile.rubble;
            tile.visited=0;
            gridmap[i][j]=tile;
        }
    }
    
}//Grid::create_map()


double calMedians(double &med, short &x, priority_queue<short> &s,priority_queue< short,vector<short>,greater<short> > &g){
    if(s.empty() and g.empty()){
        s.push(x);
        return x;
    }
    
    if (s.size() > g.size()){
        if (x < med){
            g.push(s.top());
            s.pop();
            s.push(x);
        }
        else{
            g.push(x);
        }
        med = (s.top() + g.top())/2.0;
    }
    else if (s.size()==g.size()){
        if (x < med){
            s.push(x);
            med = s.top();
        }
        else{
            g.push(x);
            med = g.top();
        }
    }
    else{
        if (x > med){
            s.push(g.top());
            g.pop();
            g.push(x);
        }
        else{
            s.push(x);
        }
        med = (s.top() + g.top())/2.0;
    }
    return med;
}



/*
 //test function
 void print_grid(Grid grid){
 //cout<<"Check create_map: "<<grid.mapMode<<" "<<grid.mapSize<<"\nStart Location: ["<<grid.startRow<<", "<<grid.startCol<<"]"<<endl;//test:
 
 for(unsigned i=0;i<grid.mapSize;i++){
 for(auto j : grid.gridmap[i]){
 cout<<setw(3)<<j.rubble;
 }//for j
 cout<<endl;
 }//for
 }
 
 void Grid::test_grid(){
 cout<<nStats<<" -nStats\n"
 <<medianOn<<" -mode median\n"
 <<statsOn<<" -mode stats\n"
 <<verboseOn<<" -mode verbose\n";
 }
 */

