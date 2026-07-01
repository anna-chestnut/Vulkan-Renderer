#include "first_app.hpp"

//std
#include <cstdlib>
#include <iostream>
#include <stdexcept>

int main(){

    try{
        vr::FirstApp app{};
        app.run();
    } catch (const std::exception &e){
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}