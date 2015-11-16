#include <iostream>
#include "gobjects.h"
#include <fstream>
#include <queue>
#include <algorithm>

using namespace std;

// data part

const double ovalSize=15;
const int lSize=30;
const int llSize=lSize*lSize;
char lab[lSize][lSize];

const int fromX=1, fromY=1,
          toX=28, toY=28;

int wSizeX=600,
    wSizeY=600;

class Noda{
public:
    bool used;
    int from; // index from node
    vector<int> adjacentNodes; // смежные ноды
    int Fdist, Gdist;
    Noda(){
        from=-1;
        used=false;
    }
    Noda(const string &initAN){
        from=-1;
        used=false;
        stringstream strstream(initAN);
        int x;
        while( !strstream.eof() ){
            strstream >> x;
            adjacentNodes.push_back(x);
        }
    }
    Noda(const vector<int> &initAN){
        from=-1;
        used=false;
        int x;
        for(int i=0; i<initAN.size();i++){
            x=initAN[i];
            adjacentNodes.push_back(x);
        }
    }
};

Noda* graph[llSize];

// methods

int absPos(int y, int x){
    return y*lSize+x;
}

vector <int> findFreeNeighbor(int startY,int startX){
    vector <int> res;
    for(int i=-1;i<2;i++){
        int checkingY=startY+i;
        if((checkingY<0)||(checkingY>=lSize))
            continue;
        for(int j=-1;j<2;j++){
            int checkingX=startX+j;
            if((checkingX<0)||(checkingX>=lSize)||((startX==checkingX)&&(startY==checkingY)))
                continue;
            if(lab[checkingY][checkingX]==' '){
                int toAdd=absPos(checkingY,checkingX); //checkingY*lSize+checkingX;
                res.push_back(toAdd);
            }
        }
    }
    return res;
}

void loadLab(const string fname){
    ifstream wfile(fname);
    if(!wfile.is_open())
        return;

    char ch, lastCh=' ';
    int charPosX=0,
        charPosY=0;

    while (wfile >> ch){
        if(ch=='1'){
            lab[charPosY][charPosX]=ch;
        } else {
            if(lastCh=='1') {
                lastCh=ch;
                continue;
            }
            lab[charPosY][charPosX]=' ';
        }
        lastCh=ch;
        charPosX++;
        if(charPosX==lSize) {
            charPosY++; charPosX=0;
        }
    }
}

void drawLab(auto &gw){
    for(int i=0;i<lSize;i++){
        for(int j=0;j<lSize;j++){
            double yPos=i*ovalSize;
            double xPos=j*ovalSize;
            GOval* gov=new GOval(xPos,yPos,ovalSize,ovalSize);
            gov->setFilled(true);
            if(((i==fromY)&&(j==fromX))||(i==toY)&&(j==toX))
                gov->setFillColor(0xff0000);
            else if(lab[i][j]=='1'){
                gov->setFillColor(0x00ff00);
                gov->setColor(0x00ff00);
            } else
                gov->setColor(0xffffff);
            gw.add(gov);
        }
    }
}

void createGraphAsList(){
    for(int i=0;i<llSize;i++){
        int y=i/lSize; int x=i-y*lSize;
        if(lab[y][x]=='1'){
            graph[i]=new Noda();
            graph[i]->from=-2;} // wall flag
        else
            graph[i]=new Noda(findFreeNeighbor(y,x));
    }
}

void coutGraph(){
    for(int i=0; i<llSize;i++){
        Noda* el=graph[i];
        cout << i<<": ";
        for(int j=0; j<el->adjacentNodes.size();j++)
            cout << el->adjacentNodes[j] <<" ";
        cout << endl;
    }
}

void drawWay(int absCoord, auto &gw, int color=0x0000ff){
    int y=absCoord/lSize; int x=absCoord-y*lSize;
    double yPos=y*ovalSize;
    double xPos=x*ovalSize;
    GOval* gov=new GOval(xPos,yPos,ovalSize,ovalSize);
    gov->setFilled(true);
    gov->setFillColor(color);
    gw.add(gov);
}

void findWayByBSF(auto &nodes, int from, int to, auto &gw){
    queue<int> qNodesIndexesToWork;
    qNodesIndexesToWork.push(from);
    while(qNodesIndexesToWork.size()>0){
        int curNI=qNodesIndexesToWork.front();
        if(curNI==to) break; // found!
        qNodesIndexesToWork.pop();
        for(int i=0; i<nodes[curNI]->adjacentNodes.size();i++){
            int newNodeIndex=nodes[curNI]->adjacentNodes[i];
            if (nodes[newNodeIndex]->used) continue;
            nodes[newNodeIndex]->used=true;
            nodes[newNodeIndex]->from=curNI;
            qNodesIndexesToWork.push(newNodeIndex);
        }
    }
    if(nodes[to]->from==-1)
        cout<< "No way found(";
    else {
        string way=to_string(to);
        while(1){
            int previousNI=nodes[to]->from;
            if(!(previousNI==from))
                drawWay(previousNI, gw);
            way=to_string(previousNI)+" "+way;
            if(previousNI==from) break;
            to=previousNI;
        }
        cout << way;
    }
}

bool sortFunction(int leftElem, int rightElem){
    bool res=false;
    if(graph[leftElem]->Fdist < graph[rightElem]->Fdist)
        res=true;
    return res; // descending order - from max to min
}

int countFdist(int nodeIndex, int to){
    int y1=nodeIndex/lSize;     int x1=nodeIndex-y1*lSize;
    int y2=to/lSize;    int x2=to-y2*lSize;
    int dy=y2-y1; int dx=x2-x1;
    double res=sqrt((double)(dy*dy+dx*dx));
    return (int) res;
}

void findWayBySStar(Noda* nodes[], int from, int to, auto &gw){
    vector<int> qNodesIndexesToWork;
    qNodesIndexesToWork.push_back(from);
    nodes[from]->Gdist=0;
    while(qNodesIndexesToWork.size()>0){

        sort(qNodesIndexesToWork.begin(), qNodesIndexesToWork.end(), sortFunction);

        int curNI=qNodesIndexesToWork[(qNodesIndexesToWork.size()-1)]; //last elem, with min Fdist
        if(curNI==to) break; // found!
        qNodesIndexesToWork.resize(qNodesIndexesToWork.size()-1);
        drawWay(curNI, gw, 0x0f0f0f);

        for(int i=0; i<nodes[curNI]->adjacentNodes.size();i++){
            int newNodeIndex=nodes[curNI]->adjacentNodes[i];
            int newGDist=nodes[curNI]->Gdist+1;
            if (nodes[newNodeIndex]->used)
                if(nodes[newNodeIndex]->Gdist<=newGDist)
                    continue;
            nodes[newNodeIndex]->used=true;
            nodes[newNodeIndex]->from=curNI;
            qNodesIndexesToWork.push_back(newNodeIndex);
            nodes[newNodeIndex]->Fdist=countFdist(newNodeIndex,to);
            nodes[newNodeIndex]->Gdist=newGDist;
        }
    }
    if(nodes[to]->from==-1)
        cout<< "No way found(";
    else {
        string way=to_string(to);
        while(1){
            int previousNI=nodes[to]->from;
            if(!(previousNI==from))
                drawWay(previousNI, gw);
            way=to_string(previousNI)+" "+way;
            if(previousNI==from) break;
            to=previousNI;
        }
        cout << way;
    }
}

int main()
{
    GWindow gw;
    gw.setCanvasSize(wSizeX,wSizeY);

    loadLab("lab.txt");
    drawLab(gw);
    createGraphAsList();
    //coutGraph();

    //findWayByBSF(graph,absPos(fromY, fromX), absPos(toY,toX),gw);
    findWayBySStar(graph,absPos(fromY, fromX), absPos(toY,toX),gw);

    return 0;
}
