//
// Created by Freewings on 2025/5/6.
//

#ifndef PAIR_HPP
#define PAIR_HPP
namespace sjtu{
  template <typename T1,typename T2>
class pair{
  public:
    T1 first;
    T2 second;
  public:
    pair(){}

    pair(const pair<T1,T2> &p){
      first=p.first;
      second=p.second;
    }
    pair(const T1 &t1,const T2 &t2){
      this->first=t1;
      this->second=t2;
    }
    friend bool operator==(const pair<T1,T2> &p1,const pair<T1,T2> &p){
      return p1.first==p.first && p1.second==p.second;
    }
    friend bool operator!=(const pair<T1,T2> &p1,const pair<T1,T2> &p){
      return !(p1==p);
    }

    friend bool operator<(const pair<T1,T2> &p1,const pair<T1,T2> &p){
      if(p1.first != p.first){
        return p1.first < p.first;
      }
      else{
        return p1.second < p.second;
      }
    }
    friend bool operator>(const pair<T1,T2> &p1,const pair<T1,T2> &p){
      if(p1.first != p.first){
        return p.first < p1.first;
      }
      else{
        return p.second  < p1.second;
      }
    }
    pair<T1,T2> operator=(const pair<T1,T2> &p){
      if( *this != p){
        first=p.first;
        second=p.second;
      }
      return *this;
    }
  };

}


#endif //PAIR_HPP
