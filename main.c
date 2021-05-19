#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define OUTFILEPATH "out.png"
#define CONFIG_FILE_PATH "config.yaml"
#define EPSILON 1.192093e-07f

int output_width;
int output_height;

struct Vector3d {
  float x;
  float y;
  float z;
};

struct Vector3d vector3d_add(struct Vector3d a, struct Vector3d b){
  struct Vector3d out;

  out.x = a.x + b.x;
  out.y = a.y + b.y;
  out.z = a.z + b.z;

  return out;
}

struct Vector3d vector3d_subtract(struct Vector3d a, struct Vector3d b){
  struct Vector3d out;

  out.x = a.x - b.x;
  out.y = a.y - b.y;
  out.z = a.z - b.z;

  return out;
}

struct Vector3d vector3d_multiply_float(struct Vector3d a, float b){
  struct Vector3d out;

  out.x = a.x * b;
  out.y = a.y * b;
  out.z = a.z * b;

  return out;
}

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

struct Ray {
  struct Vector3d origin;
  struct Vector3d direction;
};

void ray_print(struct Ray ray_to_print){
  printf("[");
  vector3d_print(ray_to_print.origin);
  printf(", ");
  vector3d_print(ray_to_print.direction);
  printf("]");
}

struct Triangle {
  struct Vector3d point1;
  struct Vector3d point2;
  struct Vector3d point3;
};

void triangle_print(struct Triangle triangle_to_print){
  printf("^");
  vector3d_print(triangle_to_print.point1);
  printf(", ");
  vector3d_print(triangle_to_print.point2);
  printf(", ");
  vector3d_print(triangle_to_print.point3);
  printf("^");
}

int ray_triangle_intersection(struct Ray ray, struct Triangle triangle, struct Vector3d *outPoint){
  //Calculate plane normal
  struct Vector3d v1v2 = vector3d_subtract(triangle.point2, triangle.point1);
  struct Vector3d v1v3 = vector3d_subtract(triangle.point3, triangle.point1);
  struct Vector3d triangleNormal = Vector3d_cross_product(v1v2, v1v3);

  //check if ray and plane are parallel
  float triangleNormalDotRayDirection = Vector3d_dot_product(triangleNormal, ray.direction);
  if(fabs(triangleNormalDotRayDirection) < EPSILON){
    return 0;
  }

  //Calculate Plane origin distance
  float planeOriginDistance = Vector3d_dot_product(triangleNormal, triangle.point1);

  //Calculate ray plane distance
  float rayPlaneDistance = (Vector3d_dot_product(triangleNormal, ray.origin) + planeOriginDistance) / triangleNormalDotRayDirection;

  //Check if triangle is behind ray
  if(rayPlaneDistance < 0){
    return 0;
  }

  //Calculate ray plane intersection point
  struct Vector3d rayPlaneIntersection = vector3d_add(ray.origin, vector3d_multiply_float(ray.direction, rayPlaneDistance));

  //Check if ray plane intersection is inside triangle
  struct Vector3d planePerpendicular;
  
  struct Vector3d edge1 = vector3d_subtract(triangle.point2, triangle.point1);
  struct Vector3d vp1 = vector3d_subtract(rayPlaneIntersection, triangle.point1);
  planePerpendicular = Vector3d_cross_product(edge1, vp1);
  if (Vector3d_dot_product(triangleNormal, planePerpendicular) < 0){
    return 0;
  }

  struct Vector3d edge2 = vector3d_subtract(triangle.point3, triangle.point2);
  struct Vector3d vp2 = vector3d_subtract(rayPlaneIntersection, triangle.point2);
  planePerpendicular = Vector3d_cross_product(edge2, vp2);
  if (Vector3d_dot_product(triangleNormal, planePerpendicular) < 0){
    return 0;
  }

  struct Vector3d edge3 = vector3d_subtract(triangle.point1, triangle.point3);
  struct Vector3d vp3 = vector3d_subtract(rayPlaneIntersection, triangle.point3);
  planePerpendicular = Vector3d_cross_product(edge3, vp3);
  if (Vector3d_dot_product(triangleNormal, planePerpendicular) < 0){
    return 0;
  }

  *outPoint = rayPlaneIntersection;
  return 1;
}

int read_obj_file(char filename[32], struct Triangle out_triangles[]){
  int num_triangles = 0;
  char *line_buffer;
  size_t line_buffer_size;
  size_t line_size;
  struct Vector3d points[65536];
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

int yaml_to_object_stings(char filename[32], char out_paths[256][128], char out_values[256][32]){
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
  int num_values = 0;
  int out_paths_place = 0;

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
          int j = 0;
          while(1){
            if(yaml_path[i][j] == 0){
              break;
            }
            out_paths[num_values][out_paths_place] = yaml_path[i][j];
            out_paths_place++;
            j++;
          }
          if(i <= whitespace_count/2 - 1){
            out_paths[num_values][out_paths_place] = '.';
            out_paths_place++;
          }
          i++;
        }

        int j = 0;
        while(1){
          if(yaml_value[j] == 0){
            break;
          }
          out_values[num_values][j] = yaml_value[j];
          j++;
        }
        num_values++;
        out_paths_place = 0;
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

  return num_values;
}

int main(){
  printf("Starting...\n");
  printf("Reading config file...\n");
  //config reader
  //config reader variables
  char config_paths[256][128];
  char config_values[256][32];
  int config_num;

  int config_map_length = 2;
  char config_path_map[][128] = {"camera.output.size.width", "camera.output.size.height"};
  void *config_pointer_map[] = {&output_width, &output_height};
  //type map: i = int, f = float
  char config_type_map[] = {'i', 'i'};

  //read config file
  config_num = yaml_to_object_stings(CONFIG_FILE_PATH, config_paths, config_values);
  
  printf("  Setting parameters...\n");
  //loop through parameters 
  int i = 0;
  while(i < config_num){
    //loop throght maps
    int j = 0;
    while(j < config_map_length){
      //check if match
      if(!strcmp(config_paths[i], config_path_map[j])){
        printf("    %s = %s\n", config_paths[i], config_values[i]);
        //int
        if(config_type_map[j] == 'i'){
          int *value = config_pointer_map[j];
          *value = atoi(config_values[i]);
        }
      }
      printf("    [%d / %d]          \r", i * config_map_length + j, config_num * config_map_length);
      j++;
    }
    i++;
  }
  printf("\n");

  printf("Done.\n");
  return 0;
}