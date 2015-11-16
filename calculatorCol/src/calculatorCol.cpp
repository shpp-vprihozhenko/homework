#include <iostream>
#include "gobjects.h"
//#include <stack>
#include <stdlib.h>
#include <math.h>
#include "mycollections.cpp"

using namespace std;

// expression example: 2.2+5*(6-2)*cos(30+30)*2^3+14=96.2
// graphic example: y=x*sin(x)

const double PI=3.14159265358979323846;
const int wSizeX=1000, wSizeY=1000;

bool isFunction(string obj);
float countExpression(string exp);

// define operator level
int operatorLevel(char op){
    if((op=='+')||(op=='-'))
        return 1;
    else if((op=='*')||(op=='/'))
        return 2;
    else if(op=='^')
        return 3;
    return 3;
}

// check, if obj is operator?
bool isOperator(string obj){
    if ((obj=="+") || (obj=="-") || (obj=="*") || (obj=="/") || (obj=="^"))
        return true;
    return false;
}

// check, if obj is operand?
bool isOperand(string obj){
    for(int i=0;i<obj.size();i++){
        string chr=obj.substr(i,1);
        if(chr==".")
            continue;
        if((chr<"0") || (chr>"9"))
            return false;
    }
    return true;
}

// cut next token from the string
string getNextToken(string exp){
    string res="";

    string fs=exp.substr(0,1);

    if(isOperator(fs))
        return fs;
    if(isOperand(fs)){
        res=fs;
        for(int i=1;i<exp.size();i++){
            string tmpStr=exp.substr(i,1);
            if(isOperand(tmpStr))
                res+=tmpStr;
            else
                break;
        }
    }
    if(fs=="("){
        int bracketsCounter=1, posCloseBracket=0;
        res=fs;
        for(int i=1;i<exp.size();i++){
            string tmpStr=exp.substr(i,1);
            res+=tmpStr;
            if(tmpStr=="("){
                bracketsCounter++;
            } else if (tmpStr==")") {
                bracketsCounter--;
                if (bracketsCounter==0){
                    posCloseBracket=i;
                    break;
                }
            }
        }
        if(bracketsCounter>0)
            cout << "Error. Wrong opening brackets number.";
    }
    if(isFunction(fs)){
        int bracketsCounter=0, posCloseBracket=0;
        res=fs;
        for(int i=1;i<exp.size();i++){
            string tmpStr=exp.substr(i,1);
            res+=tmpStr;
            if(tmpStr=="("){
                bracketsCounter++;
            } else if (tmpStr==")") {
                bracketsCounter--;
                if (bracketsCounter==0){
                    posCloseBracket=i;
                    break;
                }
            }
        }
        if(bracketsCounter>0)
            cout << "Error. Wrong opening brackets number.";
    }
    return res;
}

// cut some left symbols from string exp
string leftTrimExp(string exp, int trimmedLength){
    return exp.substr(trimmedLength);
}

// make some operation (oper) on two operand - n1 & n2.
float makeOperation(float n1, float n2, char operat){
    float res=0;
    if (operat=='+')
        res=n1+n2;
    else if (operat=='-')
        res=n1-n2;
    else if (operat=='*')
        res=n1*n2;
    else if (operat=='/')
        res=n1/n2;
    else if (operat=='^'){
        res=pow(n1,n2);
    }
    return res;
}

// check token for function
bool isFunction(string obj){
    bool res=false;
    if((obj[0]>='a')&&(obj[0]<='z')){
        res=true;
    }
    return res;
}

// count and return result of function
float countFunction(string obj){
    float res=0;
    string fn="";
    int posFirstBracket=0;
    for(int i=0;i<obj.size();i++){
        string tmpStr=obj.substr(i,1);
        if(tmpStr=="("){
            posFirstBracket=i;
            break;
        } else {
            fn+=tmpStr;
        }
    }
    string param=obj.substr(posFirstBracket+1,obj.size()-posFirstBracket-2);
    float fparam=countExpression(param)*PI/180; // convert to radians
    if(fn=="sin")
        res=sin(fparam);
    else if (fn=="cos")
        res=cos(fparam);
    else if (fn=="tan")
        res=tan(fparam);
    else if (fn=="sqrt")
        res=sqrt(fparam);
    return res;
}

// main function, that cout string expression
float countExpression(string exp){
    myStack <float> stOperands;
    myStack <char> stOperators;
    string obj="";
    char lastOperator=0;

    while (exp.size()>0) {
        obj=getNextToken(exp);
        if (isOperand(obj)){
            exp=leftTrimExp(exp,obj.size());
            stOperands.push(atof(obj.c_str()));
        } else if (isOperator(obj)){
            exp=leftTrimExp(exp,obj.size());
            if(stOperators.size()>0){
                lastOperator=stOperators.top();
                while(operatorLevel(obj[0])<=operatorLevel(lastOperator)){
                    float n1=stOperands.top();
                    stOperands.pop();
                    float n2=stOperands.top();
                    stOperands.pop();
                    char operat=stOperators.top();
                    stOperators.pop();
                    if (stOperators.size()>0){
                        char signN2=stOperators.top();
                        if(signN2=='-'){
                            n2=-n2;
                            stOperators.pop();
                            stOperators.push('+');
                        }
                    }
                    float res=makeOperation(n2,n1,operat);
                    stOperands.push(res);
                    if(stOperators.size()>0)
                        lastOperator=stOperators.top();
                    else
                        break;
                }
            }
            stOperators.push(obj[0]);
            lastOperator=obj[0];
        } else if (isFunction(obj)){
            exp=leftTrimExp(exp,obj.size());
            stOperands.push(countFunction(obj));
        } else if (obj[0]=='('){
            string subExp=obj.substr(1,obj.size()-2); // cut brackets
            exp=leftTrimExp(exp,obj.size());
            stOperands.push(countExpression(subExp));
        } else {
            cout << "Wrong token " << obj << endl;
            return 0;
        }

    }
    float res=0;
    while(stOperators.size()>0){
        char operat=stOperators.top();
        stOperators.pop();
        float n1=stOperands.top();
        stOperands.pop();
        if(stOperands.size()==0){
            if(operat=='-')
                res=-n1;
            else
                res=n1;
            break;
        }
        float n2=stOperands.top();
        stOperands.pop();
        if (stOperators.size()>0){
            char signN2=stOperators.top();
            if(signN2=='-'){
                n2=-n2;
                stOperators.pop();
                stOperators.push('+');
            }
        }
        res=makeOperation(n2,n1,operat);
        stOperands.push(res);
    }
    if(stOperands.size()>0){
        res=stOperands.top();
    }
    return res;
}

// prompt user for expression
string enterExpression(){
    string res="";
    cout << "Enter expression to calculate: ";
    cin >> res;
    return res;
}

// replace x in string expression for some number (x coord.) when graphic is drawing
string replaceX(string exprs, int x){
    string sx=to_string(x);
    if(x<0) sx="("+sx+")";
    while (true) {
        int pos=exprs.find("x");
        if(pos==string::npos)
            break;
        exprs.erase(pos,1);
        exprs.insert(pos,sx);
    }
    return exprs;
}

// draw graphic
void drawGraph(string exprs){
    GWindow gw;
    gw.setCanvasSize(wSizeX,wSizeY);
    int offsetX=wSizeX/2, offsetY=wSizeY/2;
    gw.add(new GLine(offsetX, 0, offsetX, wSizeY));
    gw.add(new GLine(0, offsetY, wSizeX, offsetY));
    for(int x=-offsetX;x<offsetX;x++){
        string curExprs=replaceX(exprs,x);
        int y=countExpression(curExprs);
        if(((offsetY+y)<0)||((offsetY+y)>wSizeY))
            continue;
        gw.add(new GOval(offsetX+x,offsetY+y,3,3));
    }
}

int main()
{
    string exprs=enterExpression();

    if (exprs.size()==0)
        return -1;
    if(exprs.substr(0,2)=="y=")
        drawGraph(exprs.substr(2));
    else
        cout << "Result: " << countExpression(exprs) << endl;

    return 0;
}
