#define TESTING 1 // Comment to disable tests
#include "uniq.h"

int main(){
  // uniq::SILENT_TESTS = 1; // omit success report
  quick_exit( uniq::runTests() );
}