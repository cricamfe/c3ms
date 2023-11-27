/* Copyright (c) 2009, 2010 Basilio B. Fraguela. Universidade da Coruña */

#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include "CodeStatistics.h"

extern const CodeStatistics& readFile(const char *fname);

namespace {

  const char RCSId[] = "$Date$ $Revision$";
  const char ExecutableType[] = 
#ifdef NDEBUG
  "Production";
#else
  "Development";
#endif
  
  int verbosity = 0;
  bool displayGlobalData = false;
  
  void usage()
  {
    printf("C++ Code Complexity Measurement System %s (%s)\n", RCSId, ExecutableType);
    printf("Copyright (c) 2009-10 Basilio B. Fraguela. Universidade da Coruna\n");
    puts("c3ms [-g] [-v level] <files>");
    puts("-g            display global data");
    puts("-v level      verbosity level");
    exit(EXIT_FAILURE);
  }

  void reportStatistics(const CodeStatistics& cs) {
    printf("  Types      : %7u (%7u unique)\n", cs.getTypes(),       cs.getUniqueTypes());
    printf("  Constants  : %7u (%7u unique)\n", cs.getConstants(),   cs.getUniqueConstants());
    printf("  Identifiers: %7u (%7u unique)\n", cs.getIdentifiers(), cs.getUniqueIdentifiers());
    printf("  Cspecs     : %7u (%7u unique)\n", cs.getCspecs(),      cs.getUniqueCspecs());
    printf("  Keywords   : %7u (%7u unique)\n", cs.getKeywords(),    cs.getUniqueKeywords());
    printf("  Operators  : %7u (%7u unique)\n", cs.getOps(),         cs.getUniqueOps());
    //printf("Conditions : %d\n", cs.getConds());
  }
  
  /// Stores the volume in V
  double computeHalsteadEffort(const CodeStatistics& cs, double& V, bool verbose) {
    unsigned int n1 = cs.getUniqueOperators();
    unsigned int n2 = cs.getUniqueOperands();
    
    unsigned int N1 = cs.getOperators();
    unsigned int N2 = cs.getOperands();
    
    unsigned int n = n1 + n2;
    unsigned int N = N1 + N2;
    
    V = N * log2((double)n);
    
    double Lhat = (2.0 / n1) * (n2 / (double)N2);
    
    double Ehat = V / Lhat;
  
    if (verbose) {
      printf("  n1 (unique operators) = %9u\n", n1);
      printf("  n2 (unique operands)  = %9u\n", n2);
      printf("  N1 (total # operatros)= %9u\n", N1);
      printf("  N2 (total # operands) = %9u\n", N2);
    }
    
    return Ehat;
    
  }
  
  void config(int argc, char *argv[])
  { int i;
    static const char *srchArgs =
#ifdef __linux__
    "+v:g"
#else
    "v:g"
#endif
    ;
    
    while ((i = getopt(argc, argv, srchArgs)) != -1)
      switch(i) {
	case 'g':
	  displayGlobalData = true; 
	  break;
	case 'v':
	  verbosity = (unsigned long long int)strtoll(optarg, (char **)NULL, 10);
	  break;
	case '?':
	default:
	  usage();
      }

    if(argc <= optind)
      usage();
    
  }
  
} //end private namespace

/////////////////////////////////////////////////////////////////////
int main(int argc, char *argv[])
{ CodeStatistics cs;
  struct stat fileStat;
  double globalEhat = 0.0, globalVolume = 0.0;
  CodeStatistics::StatSize totConds = 0;

  config(argc, argv);

  for(int nfile = optind; nfile < argc; ++nfile) {
    double localVolume;
    
    if( stat(argv[nfile], &fileStat) != 0) {
      printf("Error accessing %s errno=%d\n", argv[nfile], errno);
      exit(EXIT_FAILURE);
    }

    if( !S_ISREG(fileStat.st_mode) ) {
      printf("%s is not a regular file: disregarded\n", argv[nfile]);
      continue;
    }

    const CodeStatistics& csfile = readFile(argv[nfile]);
    double localEhat = computeHalsteadEffort(csfile, localVolume, verbosity > 2);
    totConds += csfile.getConds();

    if(verbosity != 0) {
      printf("%25s: %10.2lf  (Vol %10.2lf) (%5d conds)\n", argv[nfile], localEhat, localVolume, csfile.getConds());
      if(verbosity > 1)
	reportStatistics(csfile);
    }
    
    if(displayGlobalData)
      cs += csfile;
    
    globalEhat   += localEhat;
    globalVolume += localVolume;
  }
  
  if(verbosity)
    printf("\n");
  printf("       Total Effort: %10.2lf (Vol %10.2lf) (%5d conds)\n", globalEhat, globalVolume, totConds);

  if(displayGlobalData) {

    if(verbosity > 1) {
      puts("------------------  GLOBAL STATISTICS:");
      reportStatistics(cs);
    }
    
    globalEhat = computeHalsteadEffort(cs, globalVolume, verbosity > 2);
    printf("Total Global Effort: %10.2lf (Vol %10.2lf) (%5d conds)\n", globalEhat, globalVolume, totConds);
  }
  
  exit(EXIT_SUCCESS);
}
