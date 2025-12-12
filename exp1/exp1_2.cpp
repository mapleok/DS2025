#include <iostream>
#include <vector>
#include <stack>
#include <string>
#include <sstream>
#include <cctype>
using namespace std;

int precedence(char op){
    if(op=='+'||op=='-') return 1;
    if(op=='*'||op=='/') return 2;
    return 0;
}
bool isOp(char c){return c=='+'||c=='-'||c=='*'||c=='/';}

vector<string> infixToPostfix(const string&s,bool&ok){
    ok=true;
    vector<string>out;
    stack<char>ops;
    for(size_t i=0;i<s.size();){
        char c=s[i];
        if(isspace(c)){i++;continue;}
        if(isdigit(c)||c=='.'){
            size_t j=i;
            while(j<s.size()&&(isdigit(s[j])||s[j]=='.')) j++;
            out.push_back(s.substr(i,j-i));
            i=j;
        }else if(c=='('){ops.push(c);i++;}
        else if(c==')'){
            bool found=false;
            while(!ops.empty()){
                char t=ops.top();ops.pop();
                if(t=='('){found=true;break;}
                out.push_back(string(1,t));
            }
            if(!found){ok=false;return out;}
            i++;
        }else if(isOp(c)){
            while(!ops.empty()&&isOp(ops.top())&&precedence(ops.top())>=precedence(c)){
                out.push_back(string(1,ops.top()));ops.pop();
            }
            ops.push(c);i++;
        }else{ok=false;return out;}
    }
    while(!ops.empty()){
        if(ops.top()=='('||ops.top()==')'){ok=false;return out;}
        out.push_back(string(1,ops.top()));ops.pop();
    }
    return out;
}

bool evalPostfix(const vector<string>&post,double &res){
    stack<double>st;
    for(auto &tok:post){
        if(isdigit(tok[0])||tok[0]=='.'){
            st.push(stod(tok));
        }else if(isOp(tok[0])){
            if(st.size()<2)return false;
            double b=st.top();st.pop();
            double a=st.top();st.pop();
            double r=0;
            if(tok[0]=='+')r=a+b;
            else if(tok[0]=='-')r=a-b;
            else if(tok[0]=='*')r=a*b;
            else if(tok[0]=='/'){if(b==0)return false;r=a/b;}
            st.push(r);
        }else return false;
    }
    if(st.size()!=1)return false;
    res=st.top();
    return true;
}

bool calcExpr(const string&s,double&res){
    bool ok;
    auto post=infixToPostfix(s,ok);
    if(!ok)return false;
    return evalPostfix(post,res);
}

int main(){
    vector<string> tests={
        "1+2*3",
        "(1+2)*3",
        "3 + 4 * 2 / ( 1 - 5 )",
        "10/2+3.5",
        "1/0",
        "((2+3)*2"
    };
    for(auto &e:tests){
        double ans;
        bool ok=calcExpr(e,ans);
        cout<<"表达式 "<<e<<" => ";
        if(ok) cout<<ans<<"\n";
        else cout<<"无效表达式\n";
    }
    return 0;
}

