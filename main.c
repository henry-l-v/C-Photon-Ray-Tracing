#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <png.h>
#include <stdint.h>

#define CONFIG_FILE_PATH "config.yaml"
#define EPSILON 1.192093e-07f

//this code is from https://www.lemoda.net/c/write-png/
typedef struct
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
}
pixel_t;
    
typedef struct
{
    pixel_t *pixels;
    size_t width;
    size_t height;
}
bitmap_t;

static pixel_t * pixel_at (bitmap_t * bitmap, int x, int y)
{
    return bitmap->pixels + bitmap->width * y + x;
}

static int save_png_to_file (bitmap_t *bitmap, const char *path)
{
    FILE * fp;
    png_structp png_ptr = NULL;
    png_infop info_ptr = NULL;
    size_t x, y;
    png_byte ** row_pointers = NULL;
    /* "status" contains the return value of this function. At first
       it is set to a value which means 'failure'. When the routine
       has finished its work, it is set to a value which means
       'success'. */
    int status = -1;
    /* The following number is set by trial and error only. I cannot
       see where it it is documented in the libpng manual.
    */
    int pixel_size = 3;
    int depth = 8;
    
    fp = fopen (path, "wb");
    if (! fp) {
        goto fopen_failed;
    }

    png_ptr = png_create_write_struct (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_ptr == NULL) {
        goto png_create_write_struct_failed;
    }
    
    info_ptr = png_create_info_struct (png_ptr);
    if (info_ptr == NULL) {
        goto png_create_info_struct_failed;
    }
    
    /* Set up error handling. */

    if (setjmp (png_jmpbuf (png_ptr))) {
        goto png_failure;
    }
    
    /* Set image attributes. */

    png_set_IHDR (png_ptr,
                  info_ptr,
                  bitmap->width,
                  bitmap->height,
                  depth,
                  PNG_COLOR_TYPE_RGB,
                  PNG_INTERLACE_NONE,
                  PNG_COMPRESSION_TYPE_DEFAULT,
                  PNG_FILTER_TYPE_DEFAULT);
    
    /* Initialize rows of PNG. */

    row_pointers = png_malloc (png_ptr, bitmap->height * sizeof (png_byte *));
    for (y = 0; y < bitmap->height; y++) {
        png_byte *row = 
            png_malloc (png_ptr, sizeof (uint8_t) * bitmap->width * pixel_size);
        row_pointers[y] = row;
        for (x = 0; x < bitmap->width; x++) {
            pixel_t * pixel = pixel_at (bitmap, x, y);
            *row++ = pixel->red;
            *row++ = pixel->green;
            *row++ = pixel->blue;
        }
    }
    
    /* Write the image data to "fp". */

    png_init_io (png_ptr, fp);
    png_set_rows (png_ptr, info_ptr, row_pointers);
    png_write_png (png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

    /* The routine has successfully written the file, so we set
       "status" to a value which indicates success. */

    status = 0;
    
    for (y = 0; y < bitmap->height; y++) {
        png_free (png_ptr, row_pointers[y]);
    }
    png_free (png_ptr, row_pointers);
    
 png_failure:
 png_create_info_struct_failed:
    png_destroy_write_struct (&png_ptr, &info_ptr);
 png_create_write_struct_failed:
    fclose (fp);
 fopen_failed:
    return status;
}

//borrowed code ends

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

struct Vector3d vector3d_multiply(struct Vector3d a, struct Vector3d b){
  struct Vector3d out;

  out.x = a.x * b.x;
  out.y = a.y * b.y;
  out.z = a.z * b.z;

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

struct Vector3d vector3d_to_unit_vector(struct Vector3d v){
  struct Vector3d out;
  float vMag = Vector3d_magnitude(v);

  out.x = v.x / vMag;
  out.y = v.y / vMag;
  out.z = v.z / vMag;

  return out;
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

int ray_triangle_intersection(struct Ray ray, struct Triangle triangle, struct Vector3d *outPoint, struct Vector3d *outNormal){
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
  struct Vector3d rayPlaneIntersection = vector3d_subtract(vector3d_multiply_float(ray.direction, rayPlaneDistance), ray.origin);

  *outPoint = rayPlaneIntersection;
  *outNormal = vector3d_to_unit_vector(triangleNormal);

  //Check if ray plane intersection is inside triangle
  struct Vector3d planePerpendicular;
  
  struct Vector3d edge1 = vector3d_subtract(triangle.point2, triangle.point1);
  struct Vector3d vp1 = vector3d_subtract(rayPlaneIntersection, triangle.point1);
  planePerpendicular = Vector3d_cross_product(edge1, vp1);
  if (Vector3d_dot_product(triangleNormal, planePerpendicular) < 0){
    return 1;
  }

  struct Vector3d edge2 = vector3d_subtract(triangle.point3, triangle.point2);
  struct Vector3d vp2 = vector3d_subtract(rayPlaneIntersection, triangle.point2);
  planePerpendicular = Vector3d_cross_product(edge2, vp2);
  if (Vector3d_dot_product(triangleNormal, planePerpendicular) < 0){
    return 1;
  }

  struct Vector3d edge3 = vector3d_subtract(triangle.point1, triangle.point3);
  struct Vector3d vp3 = vector3d_subtract(rayPlaneIntersection, triangle.point3);
  planePerpendicular = Vector3d_cross_product(edge3, vp3);
  if (Vector3d_dot_product(triangleNormal, planePerpendicular) < 0){
    return 1;
  }

  return 2;
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

static int pix (int value, int max)
{
    if (value < 0) {
        return 0;
    }
    return (int) (256.0 *((double) (value)/(double) max));
}

int main(){
  printf("Starting...\n");

  int seed = time(NULL);
  srand(seed);
  printf("Using seed %d for random\n", seed);

  int output_width;
  int output_height;
  int num_objects;
  int camera_exposure;
  int num_lights;
  int max_ray_depth;
  char object_name[16];
  struct Vector3d object_offset;
  struct Triangle triangles[65536];
  int num_triangles;
  struct Vector3d camera_position;
  float camera_width;
  float camera_height;
  char output_file[64];
  struct Vector3d camera_point_position;

  printf("Reading config file...\n");
  //config reader
  //config reader variables
  char config_paths[256][128];
  char config_values[256][32];
  int config_num;

  int config_map_length = 19;
  char config_path_map[][128] = {"camera.output.size.width", "camera.output.size.height", "objects.count", "camera.misc.exposure", "lights.count", "camera.misc.max_ray_depth", "object.name", "object.position.x", "object.position.y", "object.position.z", "camera.position.x", "camera.position.y", "camera.position.z", "camera.size.width", "camera.size.height", "camera.output.file", "camera.focus_point.x", "camera.focus_point.y", "camera.focus_point.z"};
  void *config_pointer_map[] =  {&output_width, &output_height, &num_objects, &camera_exposure, &num_lights, &max_ray_depth, &object_name, &object_offset.x, &object_offset.y, &object_offset.z, &camera_position.x, &camera_position.y, &camera_position.z, &camera_width, &camera_height, &output_file, &camera_point_position.x, &camera_point_position.y, &camera_point_position.z};
  //type map: i = int, f = float, s = char[]
  char config_type_map[] = {'i', 'i', 'i', 'i', 'i', 'i', 's', 'f', 'f', 'f', 'f', 'f', 'f', 'f', 'f', 's', 'f', 'f', 'f'};

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
        //char[]
        else if(config_type_map[j] == 's'){
          char *value = config_pointer_map[j];
          strcpy(value, config_values[i]);
        }
        //float
        else if(config_type_map[j] == 'f'){
          float *value = config_pointer_map[j];
          *value = atof(config_values[i]);
        }
      }
      j++;
    }
    i++;
  }

  //setup output buffer
  printf("Creating output buffer...\n");
  float output_buffer[output_width][output_height];
  int x;
  int y;

  while(x < output_width){
    y = 0;
    while(y < output_height){
      output_buffer[x][y] = 0;
      y++;
    }
    x++;
  }

  //process triangles
  printf("Processing triangles...\n");

  printf("  Reading object \"%s\"...\n", object_name);
  char object_file_path[128];
  sprintf(object_file_path, "objects/%s/%s.obj", object_name, object_name);
  num_triangles = read_obj_file(object_file_path, triangles);

  printf("  Adding camera triangles...\n");
  struct Triangle cameraTriangle1 = {
    camera_position.x - camera_width, camera_position.y - camera_height, camera_position.z,
    camera_position.x + camera_width, camera_position.y - camera_height, camera_position.z,
    camera_position.x - camera_width, camera_position.y + camera_height, camera_position.z,
  };
  struct Triangle camreaTriangle2 = {
    camera_position.x + camera_width, camera_position.y + camera_height, camera_position.z,
    camera_position.x - camera_width, camera_position.y + camera_height, camera_position.z,
    camera_position.x + camera_width, camera_position.y - camera_height, camera_position.z,
  };;
  int cameraId = num_triangles;

  triangles[num_triangles] = cameraTriangle1;
  num_triangles++;
  triangles[num_triangles] = camreaTriangle2;
  num_triangles++;

  //process lights
  printf("Processing lights...\n");
  char light_path[128];
  struct Vector3d light_points[num_lights];
  float light_brightnesses[num_lights];

  i = 0;
  while(i < num_lights){
    sprintf(light_path, "lights.lights.%d.position.x", i);
    int j = 0;
    while(j < config_num){
      if(!strcmp(light_path, config_paths[j])){
        light_points[i].x = atof(config_values[j]);
      }
      j++;
    }

    sprintf(light_path, "lights.lights.%d.position.y", i);
    j = 0;
    while(j < config_num){
      if(!strcmp(light_path, config_paths[j])){
        light_points[i].y = atof(config_values[j]);
      }
      j++;
    }

    sprintf(light_path, "lights.lights.%d.position.z", i);
    j = 0;
    while(j < config_num){
      if(!strcmp(light_path, config_paths[j])){
        light_points[i].z = atof(config_values[j]);
      }
      j++;
    }

    sprintf(light_path, "lights.lights.%d.brightness", i);
    j = 0;
    while(j < config_num){
      if(!strcmp(light_path, config_paths[j])){
        light_brightnesses[i] = atof(config_values[j]);
      }
      j++;
    }

    printf("  %f %%              \r", (100 * (float) i) / (float) num_lights);
    i++;
  }


  //run ray tracing loop
  printf("Starting ray tracing loop..\n");
  i = 0;
  struct Ray ray;
  struct Vector3d p;
  struct Vector3d normal;
  struct Vector3d camera_normal = {0, 0, 1};
  int rays_traced = 0;
  while(i < camera_exposure){
    //generate random ray
    double direction_x = ((double)rand())/RAND_MAX * M_PI - 1.0f / 2.0f * M_PI;
    double direction_y = ((double)rand())/RAND_MAX * 2 * M_PI;

    ray.direction.x = cos(direction_y);
    ray.direction.y = sin(direction_x);
    ray.direction.z = sin(direction_y);

    ray.origin = light_points[(i * num_lights) / camera_exposure];

    //loop though bounces
    int j = 0;
    int k = 0;
    while(j < max_ray_depth){
      k = 0;
      while(k < num_triangles){
        rays_traced++;
        if(ray_triangle_intersection(ray, triangles[k], &p, &normal) == 2){
          if((k == cameraId || k == cameraId + 1) && j > 0){

            ray.direction = vector3d_subtract(camera_point_position, ray.origin);
            rays_traced++;
            ray_triangle_intersection(ray, triangles[k], &p, &normal);

            int outX = (int) ((p.x * 25) + output_width / 2);
            int outY = (int) ((p.y * 25) + output_height / 2);
            
            if(outX >= 0 && outY >= 0 && outX < output_width && outY < output_height){
              output_buffer[outX][outY] = 255;
            }

            j = max_ray_depth;
            break;
          }
          direction_x = ((double)rand())/RAND_MAX * M_PI - 1.0f / 2.0f * M_PI;
          direction_y = ((double)rand())/RAND_MAX * 2 * M_PI;

          ray.direction.x = cos(direction_y);
          ray.direction.y = sin(direction_x);
          ray.direction.z = sin(direction_y);
          
          ray.origin = p;
          //ray.direction = vector3d_subtract(ray.direction, vector3d_multiply_float(normal, Vector3d_dot_product(ray.direction, normal) * 2));
          break;
        }
        k++;
      }
      if(!(k < num_triangles)){
        break;
      }
      j++;
    }
    
    if(i % 100 == 0){
      printf("  %f %%              \r", (100 * (float) i) / (float) camera_exposure);
    }
    i++;
  }
  printf("  Traced %d rays\n", rays_traced);

  printf("Outputing image...\n");

  bitmap_t output_image;
  output_image.width = output_width;
  output_image.height = output_height;
  output_image.pixels = calloc (output_width * output_height, sizeof (pixel_t));

  if (!output_image.pixels) {
    printf("  Error creating output image...\n");
    return 0;
  }

  printf("  Processing image...\n");

  x = 0;
  while(x < output_width){
    y = 0;
    while(y < output_height){
      pixel_t * pixel = pixel_at (&output_image, x, y);

      uint8_t c = output_buffer[x][y];
      if(output_buffer[x][y] > 255){
        c = 255;
      }else if(output_buffer[x][y] < 0){
        c = 0;
      }

      pixel->red = c;
      pixel->green = c;
      pixel->blue = c;

      y++;
    }
    x++;
  }

  printf("  Writing image to \"%s\"...\n", output_file);
  save_png_to_file(&output_image, output_file);

  printf("Done.\n");
  return 0;
}