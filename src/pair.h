//
// Created by LENOVO on 10/10/2024.
//

#ifndef PAIR_H
#define PAIR_H

template<typename F,typename S>
class pair {
    pair(const F& f, const S& s) : first(f), second(s) {}
    F getFirst(){return first;}
    S getSecond(){return second;}
private:
    F first;
    S second;
};

#endif //PAIR_H
