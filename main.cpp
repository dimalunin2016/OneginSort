#include "OneginSort.h"
#include "CommandParams.h"

int main(int argc, char **argv) {
  char InputFile[MAX_PATH_SIZE] = "";
  char Out1[MAX_PATH_SIZE] = ""; 
  char Out2[MAX_PATH_SIZE] = ""; 
  char Out3[MAX_PATH_SIZE] = "";
  bool is_any_output = false;
  GetCommandParams(argc, argv, InputFile, Out1, Out2, Out3, &is_any_output);
  OneginSortClass ans; 
  if (!is_any_output) {
    
    ans.CreateTask(InputFile);
  } else {
    
    ans.CreateTask(InputFile, Out1, Out2, Out3);
  }
  ans.DoTask();
  return 0;
}
