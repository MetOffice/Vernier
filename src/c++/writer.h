/* -----------------------------------------------------------------------------
 *  (c) Crown copyright 2021 Met Office. All rights reserved.
 *  The file LICENCE, distributed with this code, contains details of the terms
 *  under which the code may be used.
 * -----------------------------------------------------------------------------
 */

/**
 * @file   writer.h
 * @brief  Contains writer class and handles IO.
 *
 * Contains a strategy method made up of a base IO class + derived classes,
 * aswell as context and writer classes which handle tasks such as creation 
 * of pointers and calling the correct methods.
 *
 */

#include <vector>
#include <memory>
#include <fstream>
#include "hashtable.h"

/**
 * @brief  IO interface class.
 *
 * An interface-style base class which contains a pure virtual write method,
 * which can then be overloaded by derived classes.
 *
 */

class IO_Interface {

    protected:

      std::ofstream output_stream;

    public:

      // Virtual constructor
      virtual ~IO_Interface() = default;

      // Pure virtual "write" method
      virtual void write(std::vector<HashTable>) = 0;

};

/**
 * @brief  Derived class for multiple-file output.
 * 
 * Overloads the pure virtual write method in IO_Interface.
 *
 */

class MultipleFiles : public IO_Interface {

    public:

      void write(std::vector<HashTable> ht) override;
      
};

/**
 * @brief  Context class.
 * 
 * A key part of any strategy method which is responsible for maintaining
 * a reference to one of the interface's derived classes. It has an execution
 * method which calls the appropriate derived class write method.
 *
 */

class IO_StrategyContext {

    private:

      std::unique_ptr<IO_Interface> io_strategy_;

    public:

      // Constructor 
      explicit IO_StrategyContext(std::unique_ptr<IO_Interface> temp_ptr);

      // Methods
      void setStrategy(std::unique_ptr<IO_Interface> temp_ptr);
      void executeStrategy(std::vector<HashTable> ht); 
    
};

/**
 * @brief  Writer class.
 * 
 * A class that picks a strategy and creates a context class object. This 
 * class serves as something for the profiler to interface with. 
 *
 */

class Writer { 

    public:

      void write(std::vector<HashTable> ht);
      
};