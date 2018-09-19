#include <stdio.h>
#include <assert.h>


bool IsStartedByte(char a) {
  
  static const int check = -2;
  if ((a >> 6) != check) {
    return true;
  }
  return false;
}


class Symbol {
 private:
  const char *start_;
  size_t size_; 

 public:

  Symbol(const char* str) {
    char a  = str[0];
    size_   = 0;
    assert(IsStartedByte(a));
    for (int i = 7; i > 3; --i) {
      
      if ((a & (1 << i)) != 0) {
        
        ++size_;
      } else {
        
        break;
      }
    }
  
    size_   = (size_ == 0) ? 1 : size_;
    start_  = str;
  }


  bool operator < (const Symbol& another) const {
    
    for (int i = 0; i < size_; ++i) {
      
      if (another.size_ <= i || another.start_[i] < start_[i]) {
        
        return false;
      }
      if (start_[i] < another.start_[i]) {
        
        return true;
      }
    }

    return false;
  }


  bool operator == (const Symbol& another) const {
    
    if(size_ != another.size_) {
      
      return false;
    }
    
    for (int i = 0; i < size_; ++i) {
      
      if (start_[i] != another.start_[i]) {
        
        return false;
      }
    }

    return true;
  }
  
  
  bool operator !=(const Symbol& another) const {
    
    return !((*this) == another);
  }


  bool operator >(const Symbol& another) const {

    return !((*this) < another || (*this) == another);
  }

  
  bool operator <=(const Symbol& another) const {
    
    return !((*this) > another);
  }


  bool operator >=(const Symbol& another) const {
    
    return !((*this) < another);
  }

 
  bool GetNext(Symbol* next) {
      
    if (start_ == '\0') {
      return false;
    }

    *next = Symbol(start_ + size_);
    return true;
  }

  
  size_t GetSymbolByteSize() const {
    
    return size_;
  }


  const char* GetStartByte() const {
    
    return start_;
  }

};


bool IsLetter(const Symbol& curr) {
  
  Symbol A_r("А"), a_r("а");
  Symbol Z_r("Я"), z_r("я");
  Symbol zero("0"), nine("9");
  Symbol A_e("A"), Z_e("Z");
  Symbol a_e("a"), z_e("z");
  return 
    (curr >= A_r && curr <= Z_r)   ||
    (curr >= a_r && curr <= z_r)   ||
    (curr >= zero && curr <= nine) || 
    (curr >= A_e && curr <= Z_e)   ||
    (curr >= a_e && curr <= z_e);
}