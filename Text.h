/**
 * @author Dmitry Lunin
 * @date 19.09.2018
 * @version 1.0
 * \file
 */
#include "UTF-8Decoder.h"
#include <algorithm>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>


const int MAX_PATH_SIZE = 100;
const int MAX_COMMAND_SIZE = 100;


/**
 * @breif class interpreter of FILE* which close itself in destructor
 */
class CleverFile {
 private:
  
  FILE* file_ = nullptr;
  const char* FileName_;
 public:
 
  /**
   * @breif open file to read
   */
  void FileToRead(const char* FileName) {
 		
    file_ = fopen(FileName, "r");
    if (!file_) {
      fprintf (stderr, "Can not open file %s\n", FileName);
      assert(file_);
    }
    FileName_ = FileName;
  }
 

  /**
   * @breif open file to write in it
   */  
  void FileToWrite(const char* FileName) {

    file_ = fopen(FileName, "w");
    if (!file_) {
      fprintf (stderr, "Can not open file %s\n", FileName);
      assert(file_);
    }
    FileName_ = FileName;
  }


  /**
   * @return pointer to file
   */
  FILE* GetFile() {

    return file_;
  }  

  
  size_t GetFileSize() const { 
    
    struct stat stat_buf;
    int exit = stat(FileName_, &stat_buf); 
    if (exit != 0) {
      fprintf (stderr, "Can not open file %s\n", FileName_);
      assert(exit == 0);
    }
    
    return stat_buf.st_size;
  }


  /**
   * @breif close file in the end
   */
  ~CleverFile() {
    
    fclose(file_);
    file_ = nullptr;
  }

};


/**
 * @breif class-helper to work with lines of text
 */
class TextLine {
 private:

  const char* symbol_parts_;
  size_t size_;
 public:


  /**
   * @breif copy pointer from parameter and culculate size of line by itself
   * @warning the starting char must be the start of a symbol 
   */
  void CreateLine(const char* str) {
   
    if (!IsStartedByte(str[0])) {
      fprintf (stderr, "Wrong start of a line\n");
      assert(IsStartedByte(str[0]));
    }
    symbol_parts_ = str;
    size_         = 0;
    for (size_t i = 0; str[i]; ++i) {
	    ++size_;
    }
  }


  /**
   * @breif copy pointer from parameter and copy size from parameter
   */
  void CreateLine(const char* str, size_t size) {
    
    if (!IsStartedByte(str[0])) {
      fprintf (stderr, "Wrong start of a line\n");
      assert(IsStartedByte(str[0]));
    }
    symbol_parts_   = str;
    size_           = size;
  }


  Symbol operator [](size_t ind) const {
    Symbol curr(symbol_parts_);
    for (size_t i = 0; i < ind; ++i) {
      if(!curr.GetNext(&curr)) {
        fprintf (stderr, "Access to an inaccesible piece of memory in operator [] of TextLine\n");
        assert(0);
      }
    }
    return curr;
  }


  /**
   * @return pointer to the starting char
   */
  const char* GetStartByte() const {
    
    return symbol_parts_;
  }


  /**
   * @return size of line in bytes
   */
  size_t GetTextLineByteSize() const {
  
    return size_;
  }
  

  bool IsEmpty() const {
    
    for (int i = 0; i < size_; ++i) {
      
      if(IsStartedByte(symbol_parts_[i])) {
        Symbol curr(symbol_parts_ + i);
        if (IsLetter(curr)) {
          return false;
        }
      }
    }
    return true;
  } 
};


/**
 * @breif class, which helps to work with texts from files
 */
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

    char first_command[MAX_COMMAND_SIZE]  = "";
    char second_command[MAX_COMMAND_SIZE] = "";
    strcat(first_command, "enconv ");
    strcat(first_command, FileName);
    system(first_command);
    strcat(second_command, "dos2unix ");
    strcat(second_command, FileName);
    system(second_command);
  }


  void CopyText(const char* FileName) {
    
    CleverFile read_file;
    read_file.FileToRead(FileName);
    size_t size = read_file.GetFileSize();
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
    origin_lines_ = new TextLine[num_of_lines_];
    curr_lines_   = new TextLine[num_of_lines_];
    
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
  

  void PrintInfoHelper(FILE* f_open) {
    
    const int NUM_BYTE_IN_KILO = 1024;
    fprintf(f_open, "Text size: %zu Kilobytes\n", 
        text_size_in_bytes_ / NUM_BYTE_IN_KILO);
    fprintf(f_open, "Number of lines: %zu\n", num_of_lines_);
    fprintf(f_open, "Number of symbols: %zu\n", num_of_symbols_);
    fprintf(f_open, "Number of letters: %zu\n", num_of_letters_);
  } 
  

  void PrintHelper(
      const char* FileName, 
      const TextLine* lines_,
      bool LikePoems, bool write_original = false) const {
    
    CleverFile write_file;
    write_file.FileToWrite(FileName);
    for (size_t i = 0; i < num_of_lines_; ++i) {
      
      const char* a = lines_[i].GetStartByte();

      if (write_original) {
        
          fprintf(write_file.GetFile(), "%s\n", a);
      }
      else if (!(LikePoems && a[0] != '\t')) {
        
        if (!(lines_[i].IsEmpty())) {
          
          fprintf(write_file.GetFile(), "%s\n", a);
        }
      }
    }
  }

 public:
  
  
  Text() {}


  /**
   * @brief decode input file in UTF-8 then copy it and prapare data 
   *    to work with it
   */
  void CreateText(const char* FileName) {
    DecodeText(FileName);      
    CopyText(FileName);
    PrepareTextAfterCopyng();   
  }


  /**
   * @brief decode input file in UTF-8 then copy it and prapare data 
   *    to work with it
   */
	Text(const char* FileName) {
      
    CreateText(FileName);
	}
 

  /**
   * @brief return current text to original
   */
  void ReturnCurrentTextToOriginal() {
    
    for (int i = 0; i < num_of_lines_; ++i) {

      curr_lines_[i] = origin_lines_[i];
    }
  }


  /**
   * @brief print all common information about text in stdout
   */
  void PrintCommonInfo() {
    
    PrintInfoHelper(stdout);
  } 
 
  
  const TextLine& operator [](size_t ind) const {
    
    if (num_of_lines_ <= ind) { 
        fprintf (stderr, "Access to an inaccesible piece of memory in const operator [] of Text\n");
        assert(0);
    }
    return curr_lines_[ind];
  }
  
  
  TextLine& operator [](size_t ind) {
    
    if (num_of_lines_ <= ind) { 
        fprintf (stderr, "Access to an inaccesible piece of memory in operator [] of Text\n");
        assert(0);
    }
    return curr_lines_[ind];
  } 


  TextLine* operator *() {
   
    return curr_lines_;
  }


  size_t GetNumberOfLines() const {
    
    return num_of_lines_;
  }


  /**
   * @brief print all common information about text in file
   */
  void PrintCommonInfo(const char* FileName) {
    CleverFile write_file;
    write_file.FileToWrite(FileName);
    PrintInfoHelper(write_file.GetFile());
  }


  /**
   * @breif print original text in file
   */
  void PrintOriginText(const char* FileName) const {
    
    PrintHelper(FileName, origin_lines_, false, true);
  }
 

  /**
   * @breif print current text in file
   */
  void PrintCurrentText(const char* FileName) const {
    
    PrintHelper(FileName, curr_lines_, false);
  } 


  /**
   * @breif print current text where lines have tabulation in the beginning
   * @note usually tabulation means that this line is part of a poem
   */
  void PrintOnlyPoems(const char* FileName) const {

    PrintHelper(FileName, curr_lines_, true);
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

