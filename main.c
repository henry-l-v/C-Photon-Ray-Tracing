#include <stdio.h>

#define OUTFILEPATH "out.png"
#define CONFIG_FILE_PATH "config.yaml"

void yaml_to_object_stings(char filename[32], char *out_paths, char *out_values){
  //flags
  char comment_flag = 0;
  char path_data_toggle_flag = 0;

  //data
  char working_char;
  char yaml_path[16][32];
  char yaml_value[32];

  //counters
  int whitespace_count = 0;
  int yaml_path_place = 0;
  int yaml_value_place = 0;

  //open input file
  FILE *fp;
  fp = fopen(filename, "r");

  //loop until all chars from the input file have been read
  while(1){
    //get next char
    working_char = fgetc(fp);

    //check for EOF
    if(feof(fp)){
      break;
    }

    //check for comment char
    if(working_char == '#'){
      comment_flag = 1;
    //check for newline
    }else if(working_char == '\n'){
      //end yaml_value and yaml_path with null chars
      yaml_path[whitespace_count/2][yaml_path_place] = 0;
      yaml_value[yaml_value_place] = 0;

      //check if yaml_value has data if so then print yaml_path data and yaml_value data to screen     
      if(yaml_value_place != 0){
        int i = 0;
        //loop though all used paths
        while(i <= whitespace_count/2){
          printf("%s", yaml_path[i]);
          if(i <= whitespace_count/2 - 1){
            printf(".");
          }
          i++;
        }
        printf(": %s\n", yaml_value);
      }
      
      //reset flags
      comment_flag = 0;
      path_data_toggle_flag = 0;

      //reset counters
      whitespace_count = 0;
      yaml_path_place = 0;
      yaml_value_place = 0;
    //check for space char and increment whitespace_count
    }else if(!comment_flag && working_char == ' '){
      if(!path_data_toggle_flag){
        whitespace_count++;
      }
    //check for : and set path_data_toggle_flag to true
    }else if(!comment_flag && working_char == ':'){
      path_data_toggle_flag = 1;
    //process general chars
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

  //close input file
  fclose(fp);
}

int main(){
  return 0;
}