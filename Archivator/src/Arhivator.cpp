#include <bitset>
#include <iostream>
#include <vector>
#include <algorithm>
#include <map>
#include "console.h"
#include "filelib.h"

using namespace std;

// describe data part
class Node {
public:
    int top,left,right;
    char elem;
    int frequence;
    Node(char el='\0', int freq=0){
        top=-1; left=-1; right=-1;
        elem=el;
        frequence=freq;
    }
    Node(const Node& sourceNode){
        top=sourceNode.top; left=sourceNode.left; right=sourceNode.right;
        elem=sourceNode.elem;
        frequence=sourceNode.frequence;
    }
    bool operator==(const Node &node2){
        bool res=false;
        if((elem==node2.elem)&&(frequence==node2.frequence)&&
           (left==node2.left)&&(right==node2.right)&&(top==node2.top))
                res=true;

        return res;
    }
};
vector <Node> nodes;
long initNodesSize=0;
map <char,string> binaryTable;

// functions

// param - array of chars, length - it size
void defineLettersFrequece(auto &param,long length){
    for(int i=0;i<length;i++){
        char currentChar=param[i];
        bool found=false;
        for(int j=0; j<nodes.size(); j++){
            if(nodes[j].elem==currentChar){
                found=true;
                nodes[j].frequence++;
                break;
            }
        }
        if(!found){
            Node node(currentChar,1);
            nodes.push_back(node);
        }
    }
}

// comparation function to sort nodes vector
bool compareFunction (Node elem1,Node elem2) {
    return (elem1.frequence < elem2.frequence);
}

// define index of node in nodes vector
int nodePos(Node node){
    for(int i=0; i<nodes.size(); i++)
        if(nodes[i]==node)
            return i;
    return -1;
}

void buildBinaryTree(){
    vector <Node> binaryTree;
    for(Node node:nodes){
        Node tmp=node;
        binaryTree.push_back(tmp);
        initNodesSize++;
    }
    while(binaryTree.size()>1){
        sort (binaryTree.begin(), binaryTree.end(), compareFunction);

        Node leftElem=binaryTree[0];
        Node rightElem=binaryTree[1];

        Node newNode;
        newNode.left=nodePos(leftElem);
        newNode.right=nodePos(rightElem);
        newNode.frequence=leftElem.frequence+rightElem.frequence;

        nodes.push_back(newNode);
        nodes[newNode.left].top=nodes.size()-1;
        nodes[newNode.right].top=nodes.size()-1;

        binaryTree.erase(binaryTree.begin(),binaryTree.begin()+2);
        binaryTree.push_back(newNode);
    }
}

// build map. Key-source char, data-it string 1&0 association
void buildElemBinaryTable(){
    for(int i=0; i<nodes.size(); i++){
        if(i==initNodesSize)
            break;
        Node startNode=nodes[i];
        string res="";
        while (1) {
            Node nextNode=nodes[startNode.top];
            if(nodes[nextNode.left]==startNode)
                res="1"+res;
            else
                res="0"+res;
            if(nextNode.top==-1)
                break;
            startNode=nextNode;
        }
        binaryTable.insert(pair <char,string> ((char)nodes[i].elem,res));
    }
}

// convert source file to the string of 0 & 1
void convertToBinaryStringFile(auto &buffer, long length, string &outSFile){
    for(int i=0; i<length;i++){
        string bss=binaryTable[(char)buffer[i]];
        outSFile+=bss;
    }
}

// convert string of 0&1 to array of bitset
void convertToBitFile(string &outBSFile, auto &outCFile){
    int bfSize=outBSFile.size();
    int elemSize=8;
    for(int i=0; i<bfSize;i+=elemSize){
        int readingLength=elemSize;
        if((bfSize-i)<elemSize)
            readingLength=bfSize-i;
        string binStr="";
        for(int j=0;j<readingLength;j++)
            binStr+=outBSFile[i+j];
        if(binStr.size()<elemSize){
            for(int k=0; k<(elemSize-binStr.size());k++)
                binStr+="1";
        }
        bitset<8> bs (binStr);
        outCFile[i/elemSize]=bs;
    }
}

// complements string to bit length of standart char
void addZeroToByteSize(string &s){
    while(s.size()<8)
        s+="0";
}

// write packed file to disk with extension .myarc
void writeArchivedFile(string &filename,auto &vbsData){
    int posExt=filename.find('.');

    string sourceExt=filename.substr(posExt+1);

    if (posExt>0)
        filename=filename.substr(0,posExt);
    filename+=".myarc";
    if(fileExists(filename))
        deleteFile(filename);

    ofstream fout(filename, ofstream::binary);

    fout << sourceExt << "|"; // save source extension

    // save bin. table
    for(auto elem:binaryTable){
        string s8bit=elem.second;
        char sourceBits=s8bit.size();
        addZeroToByteSize(s8bit);
        string addBits="";
        if(s8bit.size()>8){
            addBits=s8bit.substr(8);
            s8bit=s8bit.substr(0,8);
            addZeroToByteSize(addBits);
            bitset<8> bitElem (s8bit);
            bitset<8> bitElem2 (addBits);
            fout << sourceBits << elem.first << (unsigned char) bitElem.to_ulong() << (unsigned char) bitElem2.to_ulong();
        } else {
            bitset<8> bitElem (s8bit);
            fout << sourceBits << elem.first << (unsigned char) bitElem.to_ulong();
        }
    }
    fout << "|"; // delimiter
    // save data
    for(auto elem:vbsData){
        fout << (unsigned char) elem.to_ulong();
    }
    fout.close();
}

// for block reading count bytes of source file (size)
long countFileLength(auto &iFileStream){
    iFileStream.seekg (0, iFileStream.end);
    long length = iFileStream.tellg();
    iFileStream.seekg (0, iFileStream.beg);
    return length;
}

// main archivation function
void pack(string filename){

    ifstream iFileStream(filename,ifstream::binary);
    long length = countFileLength(iFileStream);
    cout << "Reading " << length << " source characters... ";
    // read data as a block:
    char *buffer = new char [length];
    iFileStream.read (buffer,length);
    iFileStream.close();

    defineLettersFrequece(buffer,length);
    buildBinaryTree();
    buildElemBinaryTable();

    string outBS;
    convertToBinaryStringFile(buffer,length,outBS);
    long arcSize=outBS.size()/8;
    if(outBS.size()%8>0)
        arcSize++;
    vector<bitset<8>> vbsData (arcSize);
    convertToBitFile(outBS,vbsData);

    writeArchivedFile(filename,vbsData);
    cout << "Archive " << filename << " created. " << endl;
}

// convert array of bits to the string with 0 & 1
string bitsToString(auto bs,int numBits){
    string sBits="";
    for(int i=0;i<numBits;i++)
        if(bs[7-i]==1)
            sBits+="1";
        else
            sBits+="0";
    return sBits;
}

// search binary table for string sBits and return source char
string searchInBinaryTable(string sBits){
    string res="";
    for(auto elem:binaryTable)
        if(elem.second==sBits){
            res=elem.first;
            break;
        }
    return res;
}

// convert string of 0&1 to source data
void convertSBDataToRealData(string& sData, auto &realSourceData){
    string sBits="";
    for(int i=0;i<sData.size();i++){
        sBits+=sData[i];
        string tmp=searchInBinaryTable(sBits);
        if(tmp!=""){
            realSourceData.push_back(tmp[0]);
            sBits="";
        }
    }
}

void writeUnpackedFile(string filename, string sourceExt, auto &realSourceData){
    int posPoint=filename.find('.');
    if (posPoint>0)
        filename=filename.substr(0,posPoint);
    filename+="."+sourceExt;
    if(fileExists(filename))
        deleteFile(filename);
    ofstream fout(filename, ofstream::binary);
    for(auto el:realSourceData)
        fout.write((char*)&el,1);
    fout.close();
}

// first part of archived file is source extension. Reading it.
string readSourceFileExtension(auto &buffer, int &i, int length){
    string res="";
    while(i<length){
        if(buffer[i]=='|')
            break;
        else
            res+=buffer[i];
        i++;
    }
    i++;
    return res;
}

// cut source binary table from buffer array
void readSourceBinaryTable(auto &buffer, int &i, int length){
    int counter=0, numBits=0;
    unsigned char curChar; string sBits="";
    while(i<length){
        unsigned char c=buffer[i];
        if(counter==0)
            if(c=='|')
                break;
            else
                numBits=c;
        else if(counter==1)
            curChar=c;
        else if(counter==2){
            bitset<8> bs(c);
            if(numBits>8){
                bitset<8> bs2(buffer[i+1]); i++;
                sBits=bitsToString(bs,8)+bitsToString(bs2,(numBits-8));
            } else {
                sBits=bitsToString(bs,numBits);
            }
            binaryTable.insert(pair <char,string> (curChar,sBits));
        }
        counter++; if(counter==3) counter=0;
        i++;
    }
    i++;
}

// cut packed data from buffer array and save as string of 0&1
void readArchivedDataAsStringBits(auto &buffer, int &i, int length, string &sData){
    while(i<length){
        char c=buffer[i];
        bitset<8> bs(c);
        sData+=bitsToString(bs,8);
        i++;
    }
}

// main unpacking function
void unpack(string filename){

    ifstream iFileStream(filename,ifstream::binary);
    long length = countFileLength(iFileStream);
    cout << "Reading " << length << " archived characters... ";
    // read data as a block:
    char *buffer = new char [length];
    iFileStream.read (buffer,length);
    iFileStream.close();

    int i=0; // used as pointer to current pos in buffer array

    string sourceExt = readSourceFileExtension(buffer,i,length);
    readSourceBinaryTable(buffer,i,length);
    string sData;
    readArchivedDataAsStringBits(buffer,i,length,sData);
    vector <char> realSourceData;
    convertSBDataToRealData(sData,realSourceData);

    writeUnpackedFile(filename,sourceExt,realSourceData);

    cout << "Archived file unpacked." << endl;
}

// check, if filename is archive? true if yes.
bool isArchive(string filename){
    bool res=false;
    int posExt=filename.find('.');
    if (posExt>0){
        if(filename.substr(posExt+1)=="myarc")
            res=true;
    }
    return res;
}

int main() {
    ifstream inputFileStream;
    string filename = openFileDialog(inputFileStream, "Choose file to archivation: ", "");
    if(filename=="")
        return -1;

    if(isArchive(filename))
        unpack(filename);
    else
        pack(filename);

    return 0;
}
