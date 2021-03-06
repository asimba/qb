#include "rc32.h"

/***********************************************************************************************************/
//Базовый класс для упаковки/распаковки потока по упрощённому алгоритму Range Coder 32-bit
/***********************************************************************************************************/

void rc32::set_operators(io_operator r, io_operator w, eof_operator e){
  if(r) read=r;
  if(w) write=w;
  if(e) is_eof=e;
}

int32_t rc32::rc32_read(FILE* file, char *buf, int32_t lenght){
  if((read==NULL)||(is_eof==NULL)){
    eof=true;
    return -1;
  }
  if(init){
    if((bufsize=(*read)((void*)file,(char*)pqbuffer,_RC32_BUFFER_SIZE))<0)return -1;
    if((bufsize==0)&&(*is_eof)((void *)file)) finalize=true;
    for(uint16_t i=0;i<sizeof(uint32_t);i++){
      hlp<<=8;
      hlp|=pqbuffer[rbufsize++];
      bufsize--;
    }
    init=false;
  };
  while(lenght--){
    if(finalize&&(hlp==0)){
      eof=true;
      return 1;
    };
    uint16_t i;
    range/=frequency[256];
    uint32_t count=(hlp-low)/range;
    if(count>=frequency[256]) return -1;
    for(i=255; frequency[i]>count; i--) if(!i) break;
    *buf=(uint8_t)i;
    low+=frequency[i]*range;
    range*=frequency[i+1]-frequency[i];
    for(i=(uint8_t)*buf+1; i<257; i++) frequency[i]++;
    if(frequency[256]>=0x10000){
      frequency[0]>>=1;
      for(i=1; i<257; i++){
        frequency[i]>>=1;
        if(frequency[i]<=frequency[i-1]) frequency[i]=frequency[i-1]+1;
      };
    };
    while((range<0x10000)||(hlp<low)){
      if(((low&0xff0000)==0xff0000)&&(range+(uint16_t)low>=0x10000)) range=0x10000-(uint16_t)low;
      hlp<<=8;
      if(finalize==false){
        if(bufsize==0){
          if((bufsize=(*read)((void*)file,(char*)pqbuffer,_RC32_BUFFER_SIZE))<0)return -1;
          if((bufsize==0)&&(*is_eof)((void *)file)) finalize=true;
          rbufsize=0;
        };
        if(bufsize){
          hlp|=pqbuffer[rbufsize++];
          bufsize--;
        };
      };
      low<<=8;
      range<<=8;
    };
    buf++;
  };
  return 1;
}

int32_t rc32::rc32_write(FILE* file, char *buf, int32_t lenght){
  if(write==NULL){
    eof=true;
    return -1;
  }
  if((lenght==0)&&finalize){
    if(bufsize&&((*write)((void*)file,(char*)pqbuffer,bufsize)==0)) return -1;
    bufsize=0;
    for(int i=sizeof(uint32_t);i>0;i--){
      pqbuffer[bufsize++]=(uint8_t)(low>>24);
      low<<=8;
    }
    if((*write)((void*)file,(char*)pqbuffer,bufsize)==0) return -1;
    eof=true;
  }
  else while(lenght--){
    uint16_t i;
    range/=frequency[256];
    low+=frequency[(uint8_t)*buf]*range;
    range*=frequency[(uint8_t)*buf+1]-frequency[(uint8_t)*buf];
    for(i=(uint8_t)*buf+1; i<257; i++) frequency[i]++;
    if(frequency[256]>=0x10000){
      frequency[0]>>=1;
      for(i=1; i<257; i++){
        frequency[i]>>=1;
        if(frequency[i]<=frequency[i-1]) frequency[i]=frequency[i-1]+1;
      };
    };
    while(range<0x10000){
      if(((low&0xff0000)==0xff0000)&&(range+(uint16_t)low>=0x10000)) range=0x10000-(uint16_t)low;
      if(bufsize==_RC32_BUFFER_SIZE){
        if((*write)((void*)file,(char*)pqbuffer,bufsize)==0) return -1;
        bufsize=0;
      };
      pqbuffer[bufsize++]=(uint8_t)(low>>24);
      low<<=8;
      range<<=8;
    };
    buf++;
  };
  return 1;
}

rc32::rc32(){
  frequency=(uint32_t *)calloc(257,sizeof(uint32_t));
  if(frequency){
    pqbuffer=(uint8_t *)calloc(_RC32_BUFFER_SIZE,sizeof(uint8_t));
    if(pqbuffer==NULL){
      free(frequency);
      frequency=NULL;
    };
    for(low=0; low<257; low++) frequency[low]=low;
  };
  range=0xffffffff;
  low=bufsize=rbufsize=hlp=0;
  eof=finalize=false;
  init=true;
  read=NULL;
  write=NULL;
  is_eof=NULL;
}

rc32::~rc32(){
  if(frequency){
    memset(frequency,0,257*sizeof(uint32_t));
    free(frequency);
    frequency=NULL;
    memset(pqbuffer,0,_RC32_BUFFER_SIZE);
    free(pqbuffer);
    pqbuffer=NULL;
  };
  low=0;
  hlp=0;
  range=0;
  init=false;
  bufsize=0;
  rbufsize=0;
  eof=false;
  finalize=false;
}

/***********************************************************************************************************/
