#include "signature.h"
#include <unistd.h>

int axis_sign(FILE *fildes, bool check){
  unsigned char original_sign[]={0x71,0x62,0x08,0x09};
  unsigned char target_sign[4];
  int c=0;
  if(check){
    if(fread(target_sign,1,4,fildes)<4) c=-1;
    else{
      for(c=0; c<4; c++){
        if(original_sign[c]!=target_sign[c]){
          c=-1;
          break;
        };
      };
    };
  }
  else{
    if(fwrite(original_sign,1,4,fildes)<4) c=-1;
  };
  return c;
}
