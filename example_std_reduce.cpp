
#include <algorithm>
#include <iostream>
#include <numeric>
#include <execution>

struct Reduce{
  int sum=0;  
  Reduce(){std::cout << "+";}
};

struct Reduce_op{
  auto operator() (Reduce &r,int x)     ->Reduce&{ r.sum += x; return r;}
  auto operator() (int x,Reduce &r)     ->Reduce&{r.sum += x; return r;}
  auto operator() (int y ,   int x)     ->Reduce {Reduce r; r.sum = x+y; return r;}
  auto operator() (Reduce &a,Reduce &b) ->Reduce&{a.sum += b.sum; return a;} 
};

int main(int,char**){
  std::vector<int> x = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
  Reduce red;
  auto z =  std::reduce(std::execution::par, x.begin(), x.end(), red , Reduce_op() );
  std::cout << z.sum << std::endl;
}



