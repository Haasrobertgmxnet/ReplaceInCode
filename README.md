   * ReplaceInCode modifies a bunch of cpp-Files. 
   * Some Patterns will be searched for and afterwards replaced by a hard-coded code stub.
   * Originally used to modify the example cpp-Files of the examples repo of MLPack
   * First an own header file "Header.h" is added by #include "Header.h" directly after #include <mlpack.hpp>
   * Secondly, after main() Helper::Timer tim; is inserted, wehere Helper::Timer is defined in Header.h
   * Then, std::string fname{}; is inserted to get the filename later.
   * Finally, data::Load(...) is replaced by
      1. fname = Helper::findFileAboveCurrentDirectory("<...>.csv").value();
      2. data::Load(fname, <...>, true);
      3. // data::Load(...); ... the original code line after comment slashes

         
