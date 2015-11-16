#include <iostream>
#include <stdlib.h>
#include "queue.h"
#include <Vector>
#include "gbufferedimage.h"
#include "filelib.h"

#include "legsCounter.cpp"

using namespace std;

// define data part
string sourceFileName;

struct Pnt{
    int x;
    int y;
};
Vector <Vector <Pnt>> objects;

GBufferedImage* image;
int imgWidth;
int imgHeight;

// procedures and functions

// check color - if dark - is silhouette!
bool isSilhouetteColor(int color){
    // if any color chanel is not enough lite - this point matches to current object
    int redColorChanel=color&0xff0000;
    int greenColorChanel=color&0x00ff00;
    int blueColorChanel=color&0x0000ff;
    if ((redColorChanel<0x5f0000)||(greenColorChanel<0x005f00)||(blueColorChanel<0x00005f))
        return true;
    else
        return false;
}

// get object points to vector
Vector<Pnt> getNewObject(int startX, int startY){
    Vector<Pnt> res;

    Pnt currentPoint;
    currentPoint.x=startX;
    currentPoint.y=startY;

    Queue <Pnt> qWorkingPoints;
    qWorkingPoints.add(currentPoint);
    image->setRGB(currentPoint.x, currentPoint.y, 0xffffff);// repaint point
    while(qWorkingPoints.size()>0){
        currentPoint=qWorkingPoints.dequeue(); // take point to work from queue
        res.add(currentPoint);  // add to object vector
        // look for black points around current point
        int sX=currentPoint.x-1; if(sX<0) sX=0;
        int fX=currentPoint.x+1; if(fX>=imgWidth) fX=imgWidth-1;
        int sY=currentPoint.y-1; if(sY<0) sY=0;
        int fY=currentPoint.y+1; if(fY>=imgHeight)fY=imgHeight-1;
        for(int x=sX;x<=fX;x++)
            for(int y=sY;y<=fY;y++){
                if((currentPoint.x==x)&&(currentPoint.y==y))
                    continue;
                int curColor=image->getRGB(x, y); //<0x0fffff))
                if(isSilhouetteColor(curColor)){
                    Pnt nextPoint;
                    nextPoint.x=x; nextPoint.y=y;
                    qWorkingPoints.enqueue(nextPoint);
                    image->setRGB(x, y, 0xffffff);// hide point
                }
            }
    }
    return res;
}

// find objects on picture
void findObjects(){
    for(int y=0; y<imgHeight-1;y++)
        for(int x=0; x<imgWidth-1;x++){
            if(isSilhouetteColor(image->getRGB(x, y))){
                Vector<Pnt> object = getNewObject(x,y);
                if(object.size()>20)  //  size limit for object.
                    objects.add(object);
            }
        }
}

// define min-max coordinates of object
void findMinMax(auto &obj, int &xMin, int &xMax, int &yMin, int &yMax){
    xMin=-1;
    for(auto pnt:obj){
        if(xMin==-1){
            xMin=pnt.x; xMax=pnt.x; yMin=pnt.y; yMax=pnt.y;
            continue;
        }
        if(pnt.x<xMin) xMin=pnt.x;
        if(pnt.x>xMax) xMax=pnt.x;
        if(pnt.y<yMin) yMin=pnt.y;
        if(pnt.y>yMax) yMax=pnt.y;
    }
}

// create black-white model of object as char array to increase
// working speed
char **createBWModel(int xSize, int ySize, auto &obj, int xMin, int yMin){
    char **bw=new char*[ySize];
    for(int r=0;r<ySize;r++) {
        bw[r]=new char[xSize];
        for(int k=0;k<xSize;k++)
            bw[r][k]=' ';
    }
    for(auto el:obj)
        bw[(el.y-yMin)][(el.x-xMin)]='1';
    return bw;
}

// delete 2d array from memory
void destroyBWModel(char **bwModel, int ySize){
    for(int k=0;k<ySize;k++) {
        delete []bwModel[k];
    }
    delete[] bwModel;
}

int main()
{
    sourceFileName = "2-2.jpg"; //

    image = new GBufferedImage(1000,1000,0);
    image->load(sourceFileName);

    imgWidth = image->getWidth();
    imgHeight = image->getHeight();

    GWindow gw;
    gw.setCanvasSize(imgWidth,imgHeight);
    gw.add(image);

    findObjects();

    cout << "Objects found: " << objects.size() << endl;

    int silhouettes=0;
    for(int i=0; i<objects.size();i++){
        auto obj=objects[i];
        int xMin, xMax, yMin, yMax;
        findMinMax(obj, xMin, xMax, yMin, yMax);
        int xSize=xMax-xMin+1; int ySize=yMax-yMin+1;
        if((xSize<5)||(ySize<5)) // thickness limit for objects
            continue;
        if(((double)xSize/ySize)<0.2) //min. proportion thick/height=20%
            continue;

        cout << "Analizing object Nr " << i << " with coords (" << xMin << "-" << yMin << ")-("<<xMax<<"-"<<yMax<<")\n";

        // convert object to bw model for better speed analize.
        char **bwModel;
        bwModel=createBWModel(xSize, ySize, obj, xMin, yMin);

        // first check - leg's counter
        int legs=countObjLegs(bwModel, ySize, xSize);
        int sil_legs=(legs+1)/2;
        if(legs<=2) // additional proportion checking for objects
            if (!proportionCheking(bwModel, ySize, xSize)){ //check max. proportion thick/height=50%
                cout << "Object don't pass proportion control.\n";
                continue;
            }
        cout << "Silhouette quantity (from legs): " << sil_legs << endl;

        // second check - head's counter

        // third check - center-mass counter

        // here must be analize block to compare different numbers from our checking
        silhouettes+=sil_legs;
        destroyBWModel(bwModel, ySize);
    }

    cout << "Total silhouettes: " << silhouettes << endl;

    return 0;
}
