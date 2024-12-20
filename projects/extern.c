#include "extern.h"

int some_var =
    10;  // because extern did not allocate memory for this ting, untill defined
         // here (different from the non-extern declarations)
