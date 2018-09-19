#include "UTF-8Decoder.h"
#include <algorithm>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>


class CleverFile {
 private:
  
  FILE* file_ = nullptr;
 public:
  
  void FileToRead(const char* FileName) {
 		
		file_ = fopen(FileName, "r");
    assert(file_);
  }
 
   
  void FileToWrite(const char* FileName) {

    file_ = fopen(FileName, "w");
    assert(file_);
  }

  
  FILE* GetFile() {

    return file_;
  }  


  ~CleverFile() {
    
    fclose(file_);
    file_ = nullptr;
  }
};


struct TextLine {
 private:

  const char* symbol_parts_;
	size_t size_;
 public: 
  void CreateLine(const char* str) {
    
    assert(IsStartedByte(str[0]));
    symbol_parts_ = str;
		size_         = 0;
		for (size_t i = 0; str[i]; ++i) {
			++size_;
	  }
  }


  void CreateLine(const char* str, size_t size) {
    
    assert(IsStartedByte(str[0]));
    symbol_parts_   = str;
		size_           = size;
  }


  const char* GetStartByte() const {
    
    return symbol_parts_;
  }

 
  size_t Size() const {
  
    return size_;
  }
  
};


class Text;


class LinesArray {
 private:

  friend Text;
  TextLine* lines_;
  size_t num_of_lines_;
  bool is_copied_ = false; 
  
  LinesArray(TextLine* lines, size_t num_of_lines) {
    lines_        = lines;
    num_of_lines_ = num_of_lines;
  }


  void copy(TextLine* lines, size_t num_of_lines) {
    lines_ = new TextLine[num_of_lines];
    num_of_lines_ = num_of_lines;
    for (int i = 0; i < num_of_lines_; ++i) {
      lines_[i] = lines[i];
    }
    is_copied_ = true;
  }
 
 public:

  LinesArray() {}


  void Print(int k, const char* FileName) const {
    CleverFile write_file;
    write_file.FileToWrite(FileName);
    fprintf(write_file.GetFile(), "%s\n", lines_[k].GetStartByte());
  }


  void Print(const char* FileName) const {
    
    CleverFile write_file;
    write_file.FileToWrite(FileName);
    for (size_t i = 0; i < num_of_lines_; ++i) {
      
      fprintf(write_file.GetFile(), "%s\n", 
          lines_[i].GetStartByte());
    }
  }
 
  
  void PrintOnlyPoems(const char* FileName) const {

    CleverFile write_file;
    write_file.FileToWrite(FileName);
    for (size_t i = 0; i < num_of_lines_; ++i) {
      const char* a = lines_[i].GetStartByte();
      if (a[0] == '\t') {
        fprintf(write_file.GetFile(), "%s\n", 
          lines_[i].GetStartByte());
      }
    }   
  }

  
  ~LinesArray() {
    if (is_copied_) {
      delete[] lines_;
    }
  }
};


class Text {
 private:

	char* text_             = nullptr;
	TextLine* origin_lines_ = nullptr;
  TextLine* curr_lines_   = nullptr;
  size_t num_of_lines_;
  size_t text_size_in_bytes_;
  size_t num_of_symbols_;
  size_t num_of_letters_;


  void DecodeText(const char* FileName) {
    char first_command[100] = "";
    char second_command[100] = "";
    strcat(first_command, "enconv ");
    strcat(first_command, FileName);
    system(first_command);
    strcat(second_command, "dos2unix ");
    strcat(second_command, FileName);
    system(second_command);
  }


  void CopyText(const char* FileName) {
    
		struct stat stat_buf;
    int exit = stat(FileName, &stat_buf);
    printf("%s -- %d\n", strerror(errno), exit);
    assert(exit == 0);
    
    size_t size = stat_buf.st_size;
    CleverFile read_file;
    read_file.FileToRead(FileName);
 		text_ = new char[size + 2];
    fread(text_, sizeof(char), size, read_file.GetFile());
    
    text_[size]         = '\n';
    text_[size + 1]     = '\0';
    text_size_in_bytes_ = size;
  }
  

  void CalcNumOfLines() {
   
    num_of_letters_ = 0; 
    num_of_lines_   = 0;
    num_of_symbols_ = 0;
    for (size_t i = 0; text_[i]; ++i) {
      if (text_[i] == '\n') {
        
        ++num_of_lines_;
      }

      if (IsStartedByte(text_[i])) {
        Symbol curr(text_ + i);

        if (IsLetter(curr)) {
          ++num_of_letters_;
        }
        ++num_of_symbols_;
      }
    }
    --num_of_symbols_;
  }


  void PushLineInArr(
      size_t curr_index_of_line, 
      size_t curr_start_of_line,
      size_t size) {
    
    origin_lines_[curr_index_of_line].CreateLine(
      text_ + curr_start_of_line, size);

    curr_lines_[curr_index_of_line].CreateLine(
        text_ + curr_start_of_line, size);
  }


  void PrepareTextAfterCopyng() {
    
    CalcNumOfLines();
    origin_lines_   = new TextLine[num_of_lines_];
    curr_lines_     = new TextLine[num_of_lines_];
    
    size_t curr_start_of_line = 0;
    size_t curr_index_of_line = 0;
    size_t size               = 0;
    
    for (size_t i = 0; text_[i]; ++i) {
      
      if (text_[i] == '\n') {
        text_[i]           = '\0';
        size               = i - curr_start_of_line;
        PushLineInArr(curr_index_of_line, curr_start_of_line, size);
        ++curr_index_of_line;
        curr_start_of_line = i + 1;
      }
    }
  }
  

  void PrintInfoHelper(FILE *f_open) {
    
    const int NUM_BYTE_IN_KILO = 1024;
    fprintf(f_open, "File size: %zu Kilobytes\n", 
        text_size_in_bytes_ / NUM_BYTE_IN_KILO);
    fprintf(f_open, "Number of lines: %zu\n", num_of_lines_);
    fprintf(f_open, "Number of symbols: %zu\n", num_of_symbols_);
    fprintf(f_open, "Number of letters: %zu\n", num_of_letters_);
  } 

 public:


	Text(const char* FileName) {
    DecodeText(FileName);      
    CopyText(FileName);
    PrepareTextAfterCopyng();
	}
  
  
  void PrintCommonInfo() {
    PrintInfoHelper(stdout);
  }

  
  void PrintCommonInfo(const char* FileName) {
    

    CleverFile write_file;
    write_file.FileToWrite(FileName);
    PrintInfoHelper(write_file.GetFile());
  }

  LinesArray GetPointerToCurrentText() {
    
    LinesArray res(curr_lines_, num_of_lines_);
    return res;
  }

  const LinesArray GetPointerToOriginalText() const {
    
    return LinesArray(origin_lines_, num_of_lines_);
  }
   
  LinesArray GetCurrentText() const {
    
    LinesArray res;
    res.copy(curr_lines_, num_of_lines_);
    return res;
  }


  void ReturnCurrentTextToOriginal() {
    
    for (int i = 0; i < num_of_lines_; ++i) {

      curr_lines_[i] = origin_lines_[i];
    }
  }

  
  void Sort(bool (*comp)(const TextLine& a, const TextLine& b)) {

    std::sort(curr_lines_, curr_lines_ + num_of_lines_, comp); 
  }

  ~Text() {
    delete[]      curr_lines_;
    delete[]      origin_lines_;
    delete[]      text_;
    text_         = nullptr;
    curr_lines_   = nullptr;
    origin_lines_ = nullptr;
  }
};

