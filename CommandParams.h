#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>


static const char *optString = "I:O:";


void InCaseOfInput(
    char** InputFileName, 
    const char* param_str, 
    int* curr_input_file) {


  if (*curr_input_file >= 2) {
    fprintf (stderr, "Wrong number of input parameters\n");
    assert(false);
  }

  strcpy(*InputFileName, param_str);
  ++(*curr_input_file);
}


void InCaseOfOutput(
    char** OutputFileName1,
    char** OutputFileName2,
    char** OutputFileName3,
    const char* param_str,
    int* curr_output_file) {

  switch (*curr_output_file) {
    case 1:

      strcpy(*OutputFileName1, param_str);
      break;
    case 2:
      
      strcpy(*OutputFileName2, param_str);
      break;
    case 3:
      
      strcpy(*OutputFileName3, param_str);
      break;
    default:

      fprintf (stderr, "Wrong number of output parameters\n");
      assert(false);
      break;
  } 
        
  ++(*curr_output_file);
}


void InCaseOfQuestion(char opt) {
  
  if (opt == 'I' || opt == 'O')
    fprintf (stderr, "Option -%c requires an argument.\n", opt);
  else if (isprint (opt))
    fprintf (stderr, "Unknown option `-%c'.\n", opt);
  else
    fprintf (stderr,
             "Unknown option character `\\x%x'.\n",
             opt);
  assert(false);
}

void GetCommandParams(int argc, char **argv, 
    char* InputFileName, 
    char* OutputFileName1, 
    char* OutputFileName2,
    char* OutputFileName3, bool* is_any_output) {
  
  int opt = 0;
  int curr_output_file = 1;
  int curr_input_file = 1;

  while ((opt = getopt (argc, argv, optString)) != -1) {
    switch (opt) {
      case 'I':
        InCaseOfInput(&InputFileName, optarg, &curr_input_file);
        break;
      
      case 'O':
        *is_any_output = true; 
        InCaseOfOutput(
            &OutputFileName1, 
            &OutputFileName2, 
            &OutputFileName3,
            optarg, &curr_output_file);
        break;

      case '?':
        InCaseOfQuestion(optopt);
        break;

      default:
        abort ();
    }
  }

  if (!(curr_input_file == 2 && 
        (curr_output_file == 1 || 
         curr_output_file == 4))) {
    fprintf (stderr, "Wrong number of parameters\n"); 
    assert(false);
  }
}
