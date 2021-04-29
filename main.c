#include <stdio.h>

#define OUTFILEPATH "out.png"
#define CONFIG_FILE_PATH "test.yaml"

const char config_name_map[] = "";
const char config_type_map[] = "";
const int config_pointer_map;

//yaml map functions
void yaml_map(char name_map[], char type_map[], int pointer_map, char filename[32]){
  //read input file
  FILE *fp;
  fp = fopen(filename, "r");

  char object_name[64] = "";
  char working_char;
  int comment = 0;
  int indent = 0;
  int line_position = 0;
  int object_name_place = 0;
  int object_name_indent = 0;
  int i;

  while(1){
    working_char = fgetc(fp);

    if(feof(fp)){
      break;
    }

    if(working_char == '#' && line_position == 0){
      comment = 1;
    }else if(working_char == '\n'){
      if(!comment){
        //printf("%s\n", object_name);
      }
      comment = 0;
      line_position = 0;
    }else if(working_char == ':'){
      if(indent<=object_name_indent){
        i = object_name_place;
        while(i > 0){
          printf("%c", object_name[i]);
          i--;
        }
        printf("\n");
      }else{
        object_name_indent = indent;
      }
      indent = 0;
      object_name[object_name_place++] = '.';
    }else if(working_char == ' '){
      indent++;
    }else if(!comment){
      object_name[object_name_place++] = working_char;
      line_position++;
    }
  }
  fclose(fp);
}

int main(){
  yaml_map((char*)config_name_map, (char*)config_type_map, config_pointer_map, CONFIG_FILE_PATH);

  return 0;
}