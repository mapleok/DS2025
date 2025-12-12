#include <iostream>
#include <vector>
#include <stack>
#include <cstdlib>
#include <ctime>
using namespace std;

int largestRectangleArea(const vector<int>&h){
    stack<int>st;
    int maxArea=0;
    int n=h.size();
    for(int i=0;i<=n;i++){
        int cur=(i==n?0:h[i]);
        while(!st.empty()&&cur<h[st.top()]){
            int height=h[st.top()];st.pop();
            int left=st.empty()?0:st.top()+1;
            int width=i-left;
            int area=height*width;
            if(area>maxArea)maxArea=area;
        }
        st.push(i);
    }
    return maxArea;
}

int main(){
    srand((unsigned)time(NULL));
    for(int t=0;t<10;t++){
        int len=2+rand()%8;
        vector<int>h(len);
        for(int i=0;i<len;i++)h[i]=rand()%10;
        cout<<"测试"<<t+1<<": heights=[";
        for(int i=0;i<len;i++){cout<<h[i];if(i+1<len)cout<<",";}
        cout<<"] => ";
        cout<<"最大面积="<<largestRectangleArea(h)<<"\n";
    }
    return 0;
}

