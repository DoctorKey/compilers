#ifndef __IROPT_H_
#define __IROPT_H_

#include "IR.h"

InterCodes ifreduce(InterCodes IRhead); 
InterCodes labelreduce(InterCodes IRhead); 
InterCodes labelreduce2(InterCodes IRhead); 
InterCodes assignreduce(InterCodes IRhead); 
InterCodes constantreduce(InterCodes IRhead); 

InterCodes initbasicblock(InterCodes IRhead); 
#endif
