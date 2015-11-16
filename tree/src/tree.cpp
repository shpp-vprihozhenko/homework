#include <iostream>
#include "console.h"
#include <vector>
#include "gobjects.h"
#include "cstdlib"
#include <ctime>
#include <math.h>

#define PI 3.14159265

using namespace std;

const int numNewBranchesPerLevel=30;
const int numLevels=3;
const int gwWidth=1000;
const int gwHeight=700;
const int maxAngleShift=55;
const int colorBlack=0;
const int colorGreen=0x00ff00;

// data
struct Pnt{
    int x,y;
};

struct Branch{
    int width, length, angle;
    Pnt start, end;
};

vector <Branch> branches;
int baseColor=colorBlack;

// functions

void drawBranch(GWindow &gw, Branch &branch){
    GLine* br_line=new GLine(branch.start.x,branch.start.y,branch.end.x, branch.end.y);
    br_line->setColor(baseColor);
    if(branch.width==0){
        cout << "w=0"<< endl;
        branch.width=1;
    }
    br_line->setLineWidth(branch.width);
    gw.add(br_line);
}

// count coordinates end point of branch depending to
// souce x & y, angle and length of branch
Pnt& countPoint(int x, int y, int angle, int shift){
    Pnt res;
    res.x=x+shift*cos((angle*PI/180));
    res.y=y-shift*sin(angle*PI/180);
    return res;
}

// new branch randomly attached to old, has new length, width
Branch& defineNewBranch(Branch &currentBranch){
    Branch newBranch;
    int hPos=currentBranch.length*0.1 + rand() % ((int)(1+currentBranch.length*0.9));
    newBranch.start=countPoint(currentBranch.start.x, currentBranch.start.y, \
                            currentBranch.angle, hPos);
    newBranch.length=1+(currentBranch.length-hPos)/2;
    int angleShift=rand() % maxAngleShift;
    if(rand()%2==0)
        angleShift=-angleShift;
    newBranch.angle=currentBranch.angle+angleShift;
    newBranch.end=countPoint(newBranch.start.x, newBranch.start.y, newBranch.angle, newBranch.length);
    newBranch.width=1+currentBranch.width*0.7*hPos/(currentBranch.length);

    return newBranch;
}

int main() {
    srand(time(0));

    GWindow gw;
    gw.setCanvasSize(gwWidth,gwHeight);

    Branch firstBranch;
    firstBranch.angle=90;
    firstBranch.width=numLevels;
    firstBranch.length=gwHeight*0.9;
    firstBranch.start.x=gwWidth/2;
    firstBranch.start.y=gwHeight*0.95;
    firstBranch.end.x=firstBranch.start.x;
    firstBranch.end.y=gwHeight*0.05;

    branches.push_back(firstBranch);
    drawBranch(gw,firstBranch);

    vector<Branch> newBranches;
    for(int i=0;i<numLevels;i++){
        if(i==2) baseColor=colorGreen;
        newBranches.clear();
        for(int k=0;k<branches.size();k++){
            Branch currentBranch=branches[k];
            for(int j=0; j<(numNewBranchesPerLevel-i*2);j++){
                 Branch newBranch=defineNewBranch(currentBranch);
                 drawBranch(gw, newBranch);
                 newBranches.push_back(newBranch);
            }
        }
        branches=newBranches;
    }

    return 0;
}
