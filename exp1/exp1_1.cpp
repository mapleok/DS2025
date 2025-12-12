#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <ctime>
using namespace std;

struct Complex {
    double re, im;
    Complex(double r=0, double i=0): re(r), im(i) {}
    double mag() const { return sqrt(re*re + im*im); }
    bool operator==(const Complex &o) const { return re==o.re && im==o.im; }
};

ostream& operator<<(ostream &os, const Complex &c) {
    os << "(" << c.re << (c.im>=0?"+":"") << c.im << "i)";
    return os;
}

bool complexLess(const Complex &a, const Complex &b) {
    double ma=a.mag(), mb=b.mag();
    if (ma!=mb) return ma<mb;
    return a.re<b.re;
}

vector<Complex> genRandom(int n,int range=10){
    vector<Complex> v;
    for(int i=0;i<n;i++){
        double re=(rand()% (range*10))/10.0;
        double im=(rand()% (range*10))/10.0;
        if(!v.empty() && rand()%5==0) v.push_back(v[rand()%v.size()]);
        else v.push_back(Complex(re,im));
    }
    return v;
}

void printVec(const vector<Complex>&v,string name="vec"){
    cout<<name<<": ";
    for(size_t i=0;i<v.size();i++){
        cout<<v[i];
        if(i+1<v.size()) cout<<", ";
    }
    cout<<"\n";
}

int findComplex(const vector<Complex>&v,const Complex&t){
    for(size_t i=0;i<v.size();i++)
        if(v[i]==t) return (int)i;
    return -1;
}

void insertComplex(vector<Complex>&v,const Complex&t,int pos){
    if(pos<0||pos>(int)v.size()) v.push_back(t);
    else v.insert(v.begin()+pos,t);
}

bool deleteComplex(vector<Complex>&v,const Complex&t){
    int i=findComplex(v,t);
    if(i==-1) return false;
    v.erase(v.begin()+i);
    return true;
}

void uniqueify(vector<Complex>&v){
    vector<Complex>res;
    for(auto &x:v){
        bool seen=false;
        for(auto &y:res) if(x==y){seen=true;break;}
        if(!seen) res.push_back(x);
    }
    v=res;
}

void bubbleSort(vector<Complex>&v){
    for(size_t i=0;i<v.size();i++){
        bool swapped=false;
        for(size_t j=0;j+1<v.size()-i;j++){
            if(!complexLess(v[j],v[j+1])){
                swap(v[j],v[j+1]);
                swapped=true;
            }
        }
        if(!swapped) break;
    }
}

void merge(vector<Complex>&v,int l,int m,int r){
    int n1=m-l+1,n2=r-m;
    vector<Complex>L(v.begin()+l,v.begin()+m+1);
    vector<Complex>R(v.begin()+m+1,v.begin()+r+1);
    int i=0,j=0,k=l;
    while(i<n1&&j<n2){
        if(complexLess(L[i],R[j])) v[k++]=L[i++];
        else v[k++]=R[j++];
    }
    while(i<n1)v[k++]=L[i++];
    while(j<n2)v[k++]=R[j++];
}
void mergeSortRec(vector<Complex>&v,int l,int r){
    if(l>=r)return;
    int m=(l+r)/2;
    mergeSortRec(v,l,m);
    mergeSortRec(v,m+1,r);
    merge(v,l,m,r);
}
void mergeSort(vector<Complex>&v){if(!v.empty())mergeSortRec(v,0,v.size()-1);}

vector<Complex> intervalSearch(const vector<Complex>&v,double m1,double m2){
    vector<Complex>res;
    for(auto &x:v){
        double m=x.mag();
        if(m>=m1&&m<m2) res.push_back(x);
        else if(m>=m2) break;
    }
    return res;
}

int main(){
    srand((unsigned)time(NULL));
    vector<Complex> v=genRandom(10,8);
    printVec(v,"原始向量");

    random_shuffle(v.begin(),v.end());
    printVec(v,"置乱后");

    Complex t=v[rand()%v.size()];
    cout<<"查找元素"<<t<<"，下标="<<findComplex(v,t)<<"\n";

    Complex ins(1.1,2.2);
    insertComplex(v,ins,2);
    printVec(v,"插入后");

    deleteComplex(v,t);
    printVec(v,"删除后");

    uniqueify(v);
    printVec(v,"唯一化后");

    mergeSort(v);
    printVec(v,"按模排序后");

    vector<Complex> sub=intervalSearch(v,1.0,3.0);
    printVec(sub,"模在[1,3)的子向量");
    return 0;
}

