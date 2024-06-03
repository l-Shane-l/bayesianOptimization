
cmake_minimum_required(VERSION 3.10)
project(HelloWorldEigen)

set(CMAKE_CXX_STANDARD 17)

# Find Eigen3 package
find_package(Eigen3 3.3 REQUIRED NO_MODULE)

find_package(dlib REQUIRED)


find_package(Boost REQUIRED)


# Include directories for BayesOpt
include_directories("/nix/store/jivykd5ixdpc2ydz1amrsi2rk0gnx16k-source/include")
include_directories("/nix/store/faj2xh4s7aqw7kla2pyknigmzjj8lif6-nlopt-2.7.1/include")
link_directories("/nix/store/5y188nyrzfcl86kf34j7bs1qm6hxap8x-bayesopt/lib")
link_directories("/nix/store/faj2xh4s7aqw7kla2pyknigmzjj8lif6-nlopt-2.7.1/lib")

# Add the executable
add_executable(bayesian main.cpp)

# Link libraries
target_link_libraries(bayesian Eigen3::Eigen dlib::dlib ${Boost_LIBRARIES} bayesopt nlopt)


