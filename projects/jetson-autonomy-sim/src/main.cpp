#include "autonomy/mission.hpp"
#include <iomanip>
#include <iostream>
int main(int argc,char**argv){
    using namespace autonomy; double duration=20.0;if(argc>1)duration=std::stod(argv[1]);
    MissionRuntime rt;rt.configure({{{20,0,8},1.5},{{20,20,10},1.5},{{0,20,6},1.5},{{0,0,3},1.5}});
    constexpr double dt=0.0025; double next_print=0;
    while(rt.truth().t<duration&&!rt.complete()){
        rt.step(dt);
        if(rt.truth().t>=next_print){const auto&s=rt.truth();const auto&e=rt.estimate();std::cout<<std::fixed<<std::setprecision(2)<<"t="<<s.t<<" wp="<<rt.waypoint_index()<<" truth="<<s.position<<" est="<<e.position<<" err="<<norm(s.position-e.position)<<"\n";next_print+=0.5;}
    }
    std::cout<<"mission_complete="<<std::boolalpha<<rt.complete()<<" t="<<rt.truth().t<<" final="<<rt.truth().position<<"\n";
}
