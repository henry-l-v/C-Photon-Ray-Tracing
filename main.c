#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define OUTFILEPATH "out.png"
#define CONFIG_FILE_PATH "config.yaml"

struct Vector3d {
  float x;
  float y;
  float z;
};

float Vector3d_dot_product(struct Vector3d a, struct Vector3d b){
  return a.x*b.x + a.y*b.y + a.z*b.z;
};

struct Vector3d Vector3d_cross_product(struct Vector3d a, struct Vector3d b){
  struct Vector3d out;

  out.x = a.y*b.z - a.z*b.y;
  out.y = a.z*b.x - a.x*b.z;
  out.z = a.x*b.y - a.y*b.x;

  return out;
}

float Vector3d_magnitude(struct Vector3d a){
  return sqrt(pow(a.x, 2) + pow(a.y, 2) + pow(a.z, 2));
}

void vector3d_print(struct Vector3d vector3d_to_print){
  printf("<%f, %f, %f>", vector3d_to_print.x, vector3d_to_print.y, vector3d_to_print.z);
}

struct Point3d {
  float x;
  float y;
  float z;
};

void point3d_print(struct Point3d point3d_to_print){
  printf("[%f, %f, %f]", point3d_to_print.x, point3d_to_print.y, point3d_to_print.z);
}

struct Ray {
  struct Point3d origin;
  struct Vector3d direction;
};

struct Triangle {
  struct Point3d point1;
  struct Point3d point2;
  struct Point3d point3;
};

void triangle_print(struct Triangle triangle_to_print){
  printf("^");
  point3d_print(triangle_to_print.point1);
  printf(", ");
  point3d_print(triangle_to_print.point2);
  printf(", ");
  point3d_print(triangle_to_print.point3);
  printf("^");
}

int read_obj_file(char filename[32], struct Triangle out_triangles[]){
  int num_triangles = 0;
  char *line_buffer;
  size_t line_buffer_size;
  size_t line_size;
  struct Point3d points[65536];
  struct Triangle triangles[65536];
  int num_points = 0;

  int i = 2;
  int arg_place = 0;
  char arg_value[64];
  int arg_value_place = 0;

  FILE *fp;
  fp = fopen(filename, "r");

  if(!fp){
    return 0;
  }

  while(1){
    line_size = getline(&line_buffer, &line_buffer_size, fp);

    if(line_size == -1){
      break;
    }

    if(line_buffer[0] == 'v'){
      i = 2;
      arg_place = 0;
      arg_value_place = 0;
      while(i < line_size){
        if(line_buffer[i] == ' '){
          arg_value[arg_value_place] = 0;
          if(arg_place == 0){
            points[num_points].x = atof(arg_value);
          }else if(arg_place == 1){
            points[num_points].y = atof(arg_value);
          }else if(arg_place == 2){
            points[num_points].z = atof(arg_value);
          }
          arg_place++;
          arg_value_place = 0;
        }else{
          arg_value[arg_value_place] = line_buffer[i];
          arg_value_place++;
        }
        i++;
      }
      arg_value[arg_value_place] = 0;
      if(arg_place == 0){
        points[num_points].x = atof(arg_value);
      }else if(arg_place == 1){
        points[num_points].y = atof(arg_value);
      }else if(arg_place == 2){
        points[num_points].z = atof(arg_value);
      }
      num_points++;
    }else if(line_buffer[0] == 'f'){
      i = 2;
      arg_place = 0;
      arg_value_place = 0;
      while(i < line_size){
        if(line_buffer[i] == ' '){
          arg_value[arg_value_place] = 0;
          if(arg_place == 0){
            triangles[num_triangles].point1 = points[atoi(arg_value) - 1];
          }else if(arg_place == 1){
            triangles[num_triangles].point2 = points[atoi(arg_value) - 1];
          }else if(arg_place == 2){
            triangles[num_triangles].point3 = points[atoi(arg_value) - 1];
          }
          arg_place++;
          arg_value_place = 0;
        }else{
          arg_value[arg_value_place] = line_buffer[i];
          arg_value_place++;
        }
        i++;
      }
      arg_value[arg_value_place] = 0;
      if(arg_place == 0){
        triangles[num_triangles].point1 = points[atoi(arg_value) - 1];
      }else if(arg_place == 1){
        triangles[num_triangles].point2 = points[atoi(arg_value) - 1];
      }else if(arg_place == 2){
        triangles[num_triangles].point3 = points[atoi(arg_value) - 1];
      }

      num_triangles++;
    }
  }
  
  fclose(fp);
  
  i = 0;
  while(i < num_triangles){
    out_triangles[i] = triangles[i];
    i++;
  }
  return num_triangles;
}

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