#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>


static const char *optString = "I:O:";


void InCaseOfInput(
    char** InputFileName, 
    const char* param_str, 
    int* num_of_curr_input_files) {


  if (*num_of_curr_input_files >= 2) {
    fprintf (stderr, "Wrong number of input parameters\n");
    assert(false);
  }

  strcpy(*InputFileName, param_str);
  ++(*num_of_curr_input_files);
}


void InCaseOfOutput(
    char** OutputFileName1,
    char** OutputFileName2,
    char** OutputFileName3,
    const char* param_str,
    int* num_of_curr_output_files) {

  switch (*num_of_curr_output_files) {
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
        
  ++(*num_of_curr_output_files);
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
  int num_of_curr_output_files = 1;
  int num_of_curr_input_files = 1;

  while ((opt = getopt (argc, argv, optString)) != -1) {
    switch (opt) {
      case 'I':
        InCaseOfInput(&InputFileName, optarg, &num_of_curr_input_files);
        break;
      
      case 'O':
        *is_any_output = true; 
        InCaseOfOutput(
            &OutputFileName1, 
            &OutputFileName2, 
            &OutputFileName3,
            optarg, &num_of_curr_output_files);
        break;

      case '?':
        InCaseOfQuestion(optopt);
        break;

      default:
        abort ();
    }
  }

  if (!(num_of_curr_input_files == 2 && 
        (num_of_curr_output_files == 1 || 
         num_of_curr_output_files == 4))) {
    fprintf (stderr, "Wrong number of parameters\n"); 
    assert(false);
  }
}
