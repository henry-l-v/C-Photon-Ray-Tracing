#include <stdio.h>

#define OUTFILEPATH "out.png"
#define CONFIG_FILE_PATH "config.yaml"

//yaml map functions
void yaml_to_object_stings(char filename[32], char *out_paths, char *out_values){
  //read input file
  FILE *fp;
  fp = fopen(filename, "r");

  char comment_flag = 0;
  char path_data_toggle_flag = 0;
  int whitespace_count = 0;
  char working_char;
  char yaml_path[16][32];
  int yaml_path_place = 0;
  char yaml_value[32];
  int yaml_value_place = 0;

  while(1){
    working_char = fgetc(fp);

    if(feof(fp)){
      break;
    }

    if(working_char == '#'){
      comment_flag = 1;
    }else if(working_char == '\n'){
      yaml_value[yaml_value_place] = 0;
      yaml_path[whitespace_count/2][yaml_path_place] = 0;
      
      if(yaml_value_place != 0){
        int i = 0;
        while(i <= whitespace_count/2){
          printf("%s", yaml_path[i]);
          if(i <= whitespace_count/2 - 1){
            printf(".");
          }
          i++;
        }
        printf(": %s\n", yaml_value);
      }

      comment_flag = 0;
      path_data_toggle_flag = 0;
      yaml_value_place = 0;
      yaml_path_place = 0;
      whitespace_count = 0;
    }else if(!comment_flag && working_char == ' '){
      if(!path_data_toggle_flag){
        whitespace_count++;
      }
    }else if(!comment_flag && working_char == ':'){
      path_data_toggle_flag = 1;
    }else if(!comment_flag){
      if(path_data_toggle_flag){
        yaml_value[yaml_value_place] = working_char;
        yaml_value_place++;
      }else{
        yaml_path[whitespace_count/2][yaml_path_place] = working_char;
        yaml_path_place++;
      }
    }
  }

  fclose(fp);
}

int main(){
  yaml_to_object_stings(CONFIG_FILE_PATH, NULL, NULL);

  return 0;
}