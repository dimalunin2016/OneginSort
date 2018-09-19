#include "OneginSort.h"

int main() {
  Text text("Onegin.txt");
  text.PrintCommonInfo("CommonInfo.txt");
  LinesArray res = text.GetPointerToCurrentText();
  text.Sort(Comporator);
  res.PrintOnlyPoems("Encyclopedia.txt");
  text.Sort(RhymeComporator);
  res.PrintOnlyPoems("Rhyme.txt");
  text.ReturnCurrentTextToOriginal();
  const LinesArray orig = text.GetPointerToOriginalText();
  orig.Print("RealOnegin.txt");
  return 0;
}
