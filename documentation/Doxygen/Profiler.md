Documentation {#mainpage}
=============
[TOC]

# Introduction {#Introduction}

This is a profiling tool written in C++ and designed for use with the Unified
Model and LFRic.
The current implementation is a work in progress.

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
    void get_a(){return a_;}
};
~~~~~~~~~~~~~~~~~~~~~~~

## Communication Efficiency {#comme}
TBD
 