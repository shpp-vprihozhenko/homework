#ifndef MYCOLLECTIONS_H
#define MYCOLLECTIONS_H

template <typename T>
class myStack{
    int aSize, aCapasity;
    T *arElem;
    void reallocMem(){
        aCapasity*=2;
        T *newArr=new T[aCapasity];
        for(long i=0; i<aSize; i++)
            newArr[i]=arElem[i];
        delete[] arElem;
        arElem=newArr;
    }
public:
    myStack(){
        aCapasity=100; aSize=0;
        arElem = new T[aCapasity];
    }
    ~myStack(){
        delete[] arElem;
    }
    unsigned long size(){
        return aSize;
    }
    void push(const T &elem){
        if(aSize==aCapasity){
            reallocMem();
        }
        arElem[aSize]=elem;
        aSize++;
    }
    T& top() const{
        return arElem[aSize-1];
    }
    T& pop() {
        aSize--;
        return arElem[aSize];
    }
};

template <typename T>
class myQueue{
    int aSize, aCapasity;
    T *arElem;
    void reallocMem(){
        aCapasity*=2;
        T *newArr=new T[aCapasity];
        for(long i=0; i<aSize; i++)
            newArr[i]=arElem[i];
        delete[] arElem;
        arElem=newArr;
    }
public:
    myQueue(){
        aCapasity=100; aSize=0;
        arElem = new T[aCapasity];
    }
    ~myQueue(){
        delete[] arElem;
    }
    unsigned long size(){
        return aSize;
    }
    void enqueue(const T &elem){
        if(aSize==aCapasity){
            reallocMem();
        }
        arElem[aSize]=elem;
        aSize++;
    }
    void add(const T &elem){
        enqueue(elem);
    }
    T& dequeue(){
        aSize--;
        return arElem[aSize];
    }
};

template <typename T>
class myVector{
    long aSize, aCapasity;
    T *arElem;

    void reallocMem(){
        aCapasity*=2;
        T *newArr=new T[aCapasity];
        for(long i=0; i<aSize; i++)
            newArr[i]=arElem[i];
        delete[] arElem;
        arElem=newArr;
    }
public:
    myVector(long startCapacity=100){
        aCapasity=startCapacity; aSize=0;
        arElem = new T[aCapasity];
    }
    ~myVector(){
        delete[] arElem;
    }
    myVector(const myVector &sourceVec){
        aCapasity=sourceVec.aCapasity; aSize=sourceVec.aSize;
        arElem = new T[aCapasity];
        for(int i=0;i<aSize;i++)
            arElem[i]=sourceVec[i];
    }
    myVector& operator= (const myVector& sourceVec){
        myVector newVec;
        aCapasity=sourceVec.aCapasity; aSize=sourceVec.aSize;
        arElem = new T[aCapasity];
        for(int i=0;i<aSize;i++)
            arElem[i]=sourceVec[i];
        return *this;
    }

    unsigned long size(){
        return aSize;
    }

    void push_back(const T &elem){
        if(aSize==aCapasity){
            reallocMem();
        }
        arElem[aSize]=elem;
        aSize++;
    }
    void add(const T &elem){push_back(elem);}

    T& operator[] (const int i) const{
        return arElem[i];
    }
    T& operator[] (const int i){
        if(i>=aSize){
            aSize=i+1;
            while(aSize>aCapasity)
                reallocMem();
        }
        return arElem[i];
    }

    int begin(){return 0;}

    void erase(int fromIndex, int toIndex){
        toIndex--; // to compatiblity with stl vector
        for(long i=(toIndex+1);i<aSize;i++)
            arElem[(fromIndex+(i-(toIndex+1)))]=arElem[i];
        aSize=aSize-(toIndex-fromIndex+1);
    }

    void sort(bool (*compareFunc)(T elem1, T elem2)){
        for(int i=0;i<aSize;i++){
            T minEl=arElem[i];
            long posMin=i;
            for(int j=i+1;j<aSize;j++){
                if(compareFunc(arElem[j],minEl)){
                    posMin=j;
                    minEl=arElem[j];
                }
            }
            if(i<posMin){
                T tmp=arElem[i];
                arElem[i]=minEl;
                arElem[posMin]=tmp;
            }
        }
    }

    void clear(){
        aSize=0;
    }
};

template <typename TKeys, typename TData>
class myUnorderedMap{
    long aSize, aCapasity;
    TKeys *arElemKeys;
    TData *arElemData;

    void reallocMem(){
        aCapasity*=2;
        TKeys *newArrKeys=new TKeys[aCapasity];
        TData *newArrData=new TData[aCapasity];
        for(long i=0; i<aSize; i++){
            newArrKeys[i]=arElemKeys[i];
            newArrData[i]=arElemData[i];
        }
        delete[] arElemKeys;
        delete[] arElemData;
        arElemKeys=newArrKeys;
        arElemData=newArrData;
    }
public:
    myUnorderedMap(long startCapacity=100){
        aCapasity=startCapacity; aSize=0;
        arElemKeys = new TKeys[aCapasity];
        arElemData = new TData[aCapasity];
    }
    ~myUnorderedMap(){
        delete[] arElemKeys;
        delete[] arElemData;
    }

    unsigned long size(){
        return aSize;
    }

    void insert(const TKeys &key, const TData &data){
        if(aSize==aCapasity){
            reallocMem();
        }
        arElemKeys[aSize]=key;
        arElemData[aSize]=data;
        aSize++;
    }

    TData& operator[] (const TKeys &key) const {
        int i;
        for(i=0;i<aSize;i++)
            if(arElemKeys[i]==key)
                break;
        return arElemData[i];
    }

    TKeys& key (int i) const {
        return arElemKeys[i];
    }
    TData& data (int i) const {
        return arElemData[i];
    }

    void erase(const TKeys &key){
        int i;
        for(i=0;i<aSize;i++)
            if(arElemKeys[i]==key)
                break;
        if(i<aSize){
            for(long j=(i+1);j<aSize;j++){
                arElemKeys[j-1]=arElemKeys[j];
                arElemData[j-1]=arElemData[j];
            }
            aSize--;
        }
    }

};

class myList{
   class noda{
   public:
       noda *next;
       int value;
   };
   noda *begin, *end, *current;
public:
   myList(){begin=end=current=nullptr;}
   void push_back(int i){
       noda *tmp=new noda;
       tmp->value=i;
       tmp->next=nullptr;
       if(begin==nullptr){
           begin=end=current=tmp;
       }else{
           end->next=tmp;
           end=tmp;
       }
   }
   void to_begin(){current=begin;}
   void to_next(){current=current->next;}
   noda* nextPtr(){return current->next;}
   int giveValue(){return current->value;}
};

#endif // MYCOLLECTIONS_H
