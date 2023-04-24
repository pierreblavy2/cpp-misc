#ifndef SLICE_AND_REDUCE_HPP_
#define SLICE_AND_REDUCE_HPP_

#include <algorithm>
#include <thread>


// - Cut the content of the container in chuncks of approximatively equal size
// - create a COPY of Fn_t per thread
// - for each thread, run the corresponding copy of Fn_t operator() for each element of the chunk 
// - for each thread, call reduce_fn(xxx.get() ), where xxx is the Fn_t object correspondig to the thread/
//
// --- example ---
/*
#include <iostream>

struct Fn_test{
  int  sum = 0;
  void operator()(int i)noexcept{sum+=i;}
  int  get(){return sum;}  
};

struct Fn_reduce{
  int  sum = 0;
  void operator()(int i){sum+=i;}
  int  get(){return sum;}  
};


int main(int,char**){
  std::vector<int> x = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
  std::cout <<   slice_and_reduce<Fn_test>(x)           << std::endl;
  std::cout <<   slice_and_reduce<Fn_test,Fn_reduce>(x) << std::endl;
}
*/


template<
  typename Fn_t, typename Fn_reduce=Fn_t , 
  typename Container
>
auto slice_and_reduce(
  const Container &c, 
  unsigned int nb_threads = std::max((unsigned int)(1), std::thread::hardware_concurrency() ),
  Fn_reduce fn_reduce=Fn_reduce()
){

  if(c.size()==0){return fn_reduce.get();}
  if(nb_threads>c.size() ){nb_threads=c.size();}

  struct El{
    std::thread thread;
    Fn_t fn;
    Container::const_iterator b;
    Container::const_iterator e;
  };
  
  std::vector<El> fn_v(nb_threads);
    
  size_t length = c.size() / nb_threads;
  size_t remain = c.size() % nb_threads;
  size_t begin = 0;
  size_t end   = 0;
  for (size_t i = 0; i < nb_threads; ++i){
    end += length + (i < remain);
    El& current = fn_v[i];
    current.b = c.begin() + begin;
    current.e = c.begin() + end;
    current.thread = std::thread([&c, &current](){
      std::for_each( current.b ,current.e,[&](const typename Container::value_type &x ){ current.fn(x);  } );
    });
    begin = end;
  }

  for(size_t i=0; i<nb_threads; ++i){fn_v[i].thread.join();}
  
  for(auto &x : fn_v ){
    fn_reduce(x.fn.get() );
  }

  return fn_reduce.get();
}


#endif
