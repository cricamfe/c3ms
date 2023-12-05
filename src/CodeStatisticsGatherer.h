/* Copyright (c) 2009, 2010 Basilio B. Fraguela. Universidade da CoruÃ±a */

#ifndef CODESTATISTICSGATHERER_H
#define CODESTATISTICSGATHERER_H

//#include <stack>

#include "CodeStatistics.h"

class CodeStatisticsGatherer : public CodeStatistics {
public:
  CodeStatisticsGatherer()
  : CodeStatistics()
  {
    //cbracketDepth = 0;
    //ignoreMode.push(true);
  }
  
  void openBracket()  { op("{"); }
  void closeBracket() { }

private:

  //unsigned int cbracketDepth;
  
  //std::stack<bool> ignoreMode;  
};

#endif //CODESTATISTICSGATHERER_H
