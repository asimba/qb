#include "archive.h"
#include "etc.h"
#include <getopt.h>
#include "globfunction.h"
#ifndef VERSION
  #include "version.h"
#endif

#ifndef _WIN32
  #include <pthread.h>
  #include <signal.h>
  #include <sys/mman.h>
  #include "vars.h"
#else
  #include <windows.h>
#endif

#define PROGRAM_NAME "qb"
#define AUTHORS "Alexey V. Simbarsky"

char *program_name;

static struct option const long_options[]=
{
  {"pack", no_argument, NULL, 'p'},
  {"unpack", optional_argument, NULL, 'u'},
  {"list", optional_argument, NULL, 'l'},
  {"ignore", no_argument, NULL, 'n'},
  {"input", required_argument, NULL, 'i'},
  {"output", required_argument, NULL, 'o'},
  {"help", no_argument, NULL, 'h'},
  {"version", no_argument, NULL, 'v'},
  {NULL, 0, NULL, 0}
};

void usage(int status){
  if(status) fprintf(stderr, "Try `%s --help' for more information.\n", program_name);
  else{
    fprintf(stdout,"\nCube (qb). This program provides simple compression of the user data\n\
\nUsage  : %s <OPTIONS>\nExamples:\
\n     qb --pack -i ./ > packed.qb\
\n     qb --pack --input=./source.in --output=./packed.qb\
\n     qb --pack -i ./src1 -i ./src2 -o ./packed.qb\
\n     qb --unpack -i ./source.qb\
\n     qb --unpack -i ./source.qb -o ./\
\n     qb -p -i ./source.in -o ./packed.qb\
\n     qb -u -i ./source.qb\
\n     qb --list -i ./source.qb\
\n     qb -l -i ./source.qb\n\
Options:\n\
  -p, --pack                    pack input data\n\
  -u, --unpack                  unpack input data\n\
  -u0,--unpack=0                unpack input data\n\
                                but without writing - a kind of check\n\
  -l, --list                    list contents of the qb-archive\n\
  -l0,--list=0                  list contents of the qb-archive,\n\
                                but without printing - a kind of check\n\
  -n, --ignore                  ignore impossibility of change file\n\
                                ownership/permissions or access/modification\n\
                                times (ONLY for \"unpack\" MODE)\n\
  -i NAME  , --input=NAME       input file or directory\n\
                                (wildcards accepted ONLY for \"pack\" MODE)\n\
                                NOTE: when using wildcards - put them in\n\
                                double quotes\n\
  -o NAME  , --output=NAME      output file or directory - ONLY ONE FILENAME\n\
                                or PATH\n\
  -h, --help                    display this help and exit\n\
  -v, --version                 output version information and exit\n",
  program_name);
    version(stdout,PROGRAM_NAME,VERSION,AUTHORS);
  };
}

int main(int argc, char *argv[]){
  int errcode=0;
#ifndef _WIN32
  uid=getuid();
  if(uid==0){
    if(mlockall(MCL_FUTURE)<0) errcode=0x08000000;
  };
#endif
  archiver xs;
  xs.set_list_operator(list_print);
  xs.set_list_summary_operator(list_summary_print);
  int optc;
  bool emode=true;
  char *errhlp=NULL;
  unsigned char mode=0;
  const char *input_file=NULL;
  const char *output_file=NULL;
  uint8_t unarc_null=false;
  glob64_t *globbuf=NULL;
  program_name=argv[0];
#ifndef _WIN32
  if(!errcode){
#endif
  while((optc=getopt_long(argc,argv,"pu::l::ni:o:vh",long_options,NULL))!=-1){
    switch(optc){
      case  0 : break;
      case 'p': if(mode){
                  errcode=0x00000020;
                  break;
                };
                mode=1;
                break;
      case 'u': if(mode){
                  errcode=0x00000020;
                  break;
                };
                mode=2;
                unarc_null=0;
                if(optarg!=NULL){
                  if(strcmp(optarg,"0")!=0){
                    errcode=0x00000008;
                    break;
                  }
                  else unarc_null=1;
                };
                break;
      case 'l': if(mode){
                  errcode=0x00000020;
                  break;
                };
                mode=3;
                if(optarg!=NULL){
                  if(strcmp(optarg,"0")!=0){
                    errcode=0x00000008;
                    break;
                  }
                  else{
                    xs.set_list_operator(NULL);
                    xs.set_list_summary_operator(NULL);
                  };
                };
                break;
      case 'i': globbuf=glob_getlist(optarg,errcode);
                if(globbuf!=NULL){
                  for(uint32_t i=0;i<globbuf->gl_pathc;i++){
                    xs.add_name(globbuf->gl_pathv[i]);
                    errcode=xs.errcode;
                    if(errcode) break;
                  };
                  glob_freelist(globbuf);
                }
                else{
                  if(errcode==0) errcode=0x00000040;
                };
                if(errcode) errhlp=optarg;
                input_file=optarg;
                break;
      case 'o': if((output_file!=NULL)){
                  errcode=0x00000020;
                  break;
                };
                output_file=optarg;
                break;
      case 'h': usage(0);
                return errcode;
      case 'v': version(stdout,PROGRAM_NAME,VERSION,AUTHORS);
                fclose(stdout);
                return errcode;
      default : errcode=0x00000010;
    };
    if(errcode) break;
  };
#ifndef _WIN32
  }
#endif
  if(!errcode){
    if(mode){
      if(!errcode){
        if(mode==2){
          if(output_file) xs.set_base(output_file);
          errcode=xs.errcode;
          if(!errcode){
            xs.archive(input_file,true,unarc_null);
          };
          emode=false;
        };
        if(mode==1){
          if(xs.ignore_pt) errcode=0x00000020;
          else{
            if(output_file)
              if(access(output_file,F_OK)==0) errcode=0x00000080;
            if(!errcode){
              xs.archive(output_file,false,unarc_null);
            };
          };
          emode=false;
        };
        if(mode==3){
          if(output_file||xs.ignore_pt) errcode=0x00000020;
          else{
            if(!errcode) xs.archive(input_file,true,(uint8_t)2);
          };
          emode=false;
        };
      };
      if(!errcode && emode) errcode=0x00000008;
    }
    else errcode=0x00000008;
  };
  if((errcode==0)&&(mode!=6)) errcode=xs.errcode;
  switch(errcode){
    case 0x00000000: break;
    case 0x00000001: error("memory allocation failed");
                     break;
    case 0x00000002: error("initialization failed");
                     break;
    case 0x00000004: error("read error");
                     break;
    case 0x00000008: error("wrong mode");
                     usage(1);
                     break;
    case 0x00000010: usage(1);
                     break;
    case 0x00000020: error("too many arguments");
                     usage(1);
                     break;
    case 0x00000040: error("input file/directory is not accessible");
                     if(errhlp){
                       fprintf(stderr,"\nUnable to open file/directory: \"%s\"!\n\n",input_file);
                     };
                     break;
    case 0x00000080: error("output file already exists");
                     break;
    case 0x000000f0: error("output file is not accessible");
                     break;
    case 0x00000100: error("input file is not qb-file");
                     break;
    case 0x00000200: error("write error");
                     break;
    case 0x00000400: error("access failed or bad file");
                     break;
    case 0x00000800: error("broken file or I/O error");
                     break;
    case 0x00000f00: error("unable to create directory");
                     break;
    case 0x00001000: error("unable to change directory");
                     break;
    case 0x00002000: error("no data for archiving/processing");
                     break;
    case 0x00004000: error("input file was changed");
                     break;
    case 0x00010000: error("unable to change file access permissions");
                     break;
    case 0x00020000: error("unable to set file access and modification times");
                     break;
    case 0x00040000: error("unable to change ownership of a file/directory");
                     break;
    case 0x00080000: error("unable to get current directory name");
                     break;
    default        : break;
  };
#ifndef _WIN32
  if(uid==0) munlockall();
#endif
  return errcode;
}
