# Documentation {#mainpage}
=============
[TOC]

# Introduction {#Introduction}

This is a profiling tool written in C++ and designed for use with the Unified
Model and LFRic.
The current implementation is a work in progress.

# Build Options {#Options}

There are a number of options which can be passed to CMake on the command line
or set using ccmake.

Argument | Options (Default **Bold**)| Description
:---------:|:---------:|:------------
 `-DINCLUDE_GTEST` |  ON / **OFF** | Fetches and populates googletest within the project build.


# Metrics  {#metrics}

The metrics to be included are going to be based on Performance Optimisation
and Productivity (<a href="https://pop-coe.eu/node/69">POP</a>) Standard Metrics 
for Parallel Performance Analysis.

## Load Balance {#lb}

~~~~~~~~~~~~~~~~~~~~~~~cpp
class A {
    private:
        int a_;
        
    public:
    // Constructor
    A(int a):a_(a){};
    
    // Destructor
    ~A;
    
    // Getter
    int get_a(){return a_;}
};
~~~~~~~~~~~~~~~~~~~~~~~

## Communication Efficiency {#comme}

TBD