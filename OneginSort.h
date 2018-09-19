#include "Text.h"

const size_t NUM_OF_BAD_SYMB            = 30;
const char BadSymbols[NUM_OF_BAD_SYMB] = ".,;:!?«»()—…";


bool InArray(const Symbol& a, const char* check_arr) {
  
  char a_symb_start = *(a.GetStartByte());

  for (int i = 0; check_arr[i];++i) {
    
    if (a_symb_start == check_arr[i]) {
      
      return true;
    }
  }

  return false;
}


bool IsValid(const char* a, int ind) {
  
  if (ind >= 0 && a[ind] != '\0') {
    
    return true; 
  }
  return false;
}


enum Comparison {Bigger, Smaller, Equal};


Comparison CheckSymbolsInComp(const Symbol& a_sym, const Symbol& b_sym) {
  
  if (a_sym < b_sym) { 
    
    return Smaller;
  }
  if (b_sym < a_sym) {
    
    return Bigger;
  } 
  return Equal;
}


void MakeStep(size_t& a_ind, bool reversed) {
  
  size_t step = (reversed) ? -1 : 1;
  a_ind += step;
}


void CreateStartPosiztion(
    const TextLine& a, 
    const char** a_str, 
    size_t& a_ind_begin, 
    bool reversed) {

  *a_str = a.GetStartByte();
  a_ind_begin = (reversed) ? a.Size() - 1 : 0;
}


void SkipBadSymbols(const char* a_str, size_t& a_ind, bool reversed) {
  
    for (;IsValid(a_str, a_ind) && 
        !(IsStartedByte(a_str[a_ind]) && 
        !InArray(Symbol(a_str + a_ind), BadSymbols)); 
        MakeStep(a_ind, reversed)) {}
}


bool CompHelper(const TextLine& a, const TextLine& b, bool reversed) {

  const char* a_str = nullptr;
  const char* b_str = nullptr;
  size_t a_ind;
  size_t b_ind;
  CreateStartPosiztion(a, &a_str, a_ind, reversed);
  CreateStartPosiztion(b, &b_str, b_ind, reversed);
  while (true) { 

    SkipBadSymbols(a_str, a_ind, reversed);
    SkipBadSymbols(b_str, b_ind, reversed);
    
    if (!IsValid(b_str, b_ind)) {
      return false;
    }
    
    if (!IsValid(a_str, a_ind)) {
      return true;
    }

    Symbol a_sym(a_str + a_ind);
    Symbol b_sym(b_str + b_ind); 
    
    Comparison res = CheckSymbolsInComp(a_sym, b_sym);
    
    if (res == Bigger) {
      return false;
    }

    if (res == Smaller) {
      return true;
    }

    MakeStep(a_ind, reversed);
    MakeStep(b_ind, reversed);
  }

  return false; 
}


bool Comporator(const TextLine& a, const TextLine& b) {
  return CompHelper(a, b, false);
}


bool RhymeComporator(const TextLine& a, const TextLine& b) {
  return CompHelper(a, b, true);
}