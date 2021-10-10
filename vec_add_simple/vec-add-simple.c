#include <stdio.h>
#include <stdlib.h>

/* OpenCL Header file */
#include <CL/cl.h>

/* OpenCL Kernel function */
const char *program_source =
"__kernel void vec_add (__global int *a, __global int *b, __global int *c) { \n"
"    int idx = get_global_id (0); \n"
"    c[idx] = a[idx] + b[idx];       \n"
"} \n";

//#define ERR(msg) { fprintf(stderr,"%s:%s#%d:%s",__FILE__, __func__, __LINE__, msg); exit(1); }
#define ERR(format, ...) { fprintf(stderr,"%s:%s#%d:",__FILE__, __func__, __LINE__); fprintf(stderr, format, ##__VA_ARGS__); fprintf(stderr,"\n"); exit(1); }   
//#define ERR(format, ...) { fprintf(stderr, format, ##__VA_ARGS__); exit(1); }   
//#define ERR(format, ...) { ERR_IMPL(format, __FILE__, __func__, __LINE__, ##__VA_ARGS__); }

int main () {
    cl_int err=0;
    /* data on host */
    int *a = NULL;
    int *b = NULL;
    int *c = NULL;
    int *c_r=NULL;

    /* element numbers of each array */
    auto num_elements = (unsigned long long)(3);

    /* array size */
    size_t data_size = sizeof (int) * num_elements;
    printf("num_elements: %d\n", num_elements);
    /* allocate mem for array */
    a = (int *) malloc (data_size);
    b = (int *) malloc (data_size);
    c = (int *) malloc (data_size);
    c_r = (int *) malloc (data_size);

    /* init array */
    for (int i = 0; i < num_elements; i++) {
        a[i] = i+1;
        b[i] = i+1;
        c[i] = a[i] + b[i];
    }
    for (int i = 0; i < num_elements; i++)
        {
         printf("%d ", c[i]);
        }
    printf("\n");
    /* get platform number of OpenCL */
    cl_uint  num_platforms = 0;
    if(clGetPlatformIDs (0, NULL, &num_platforms)!=CL_SUCCESS) {
        //printf("clGetPlatformIDs failed");
        ERR("clGetPlatformIDs failed");
    }
    
    if(num_platforms==0)
    {
        // printf("num_platforms: %d\n", (int)num_platforms);
        // exit(-1);
        ERR("num_platforms: %d\n", (int)num_platforms);
    }
    else
    {
        printf("num_platforms: %d\n", (int)num_platforms);
    }
    /* allocate a segment of mem space, so as to store supported platform info */
    cl_platform_id *platforms = (cl_platform_id *) malloc (num_platforms * sizeof (cl_platform_id));

    if(clGetPlatformIDs (num_platforms, platforms, NULL)!=CL_SUCCESS) {
        //printf("clGetPlatformIDs failed");
        ERR("clGetPlatformIDs failed");
        
    }
    

    /* get device number on platform */
    cl_uint num_devices = 0;
    err=clGetDeviceIDs (platforms[0], CL_DEVICE_TYPE_CPU, 0, NULL, &num_devices);
    if(err!=CL_SUCCESS) {
        //printf("clGetDeviceIDs failed");
        switch(err)
        {
            case CL_INVALID_PLATFORM:
                ERR("CL_INVALID_PLATFORM");
                break;
            case CL_INVALID_DEVICE_TYPE:
                ERR("CL_INVALID_DEVICE_TYPE");
                break;
            case CL_INVALID_VALUE:
                ERR("CL_INVALID_VALUE");
                break;
            case CL_DEVICE_NOT_FOUND:
                ERR("CL_DEVICE_NOT_FOUND");
                break;
            default:
                ERR("Unknown error");
                break;
        }        
    }
    //clGetDeviceIDs (platforms[0], CL_DEVICE_TYPE_GPU, 0, NULL, &num_devices);

    /* allocate a segment of mem space, to store device info, supported by platform */
    cl_device_id *devices;
    devices = (cl_device_id *) malloc (num_devices * sizeof (cl_device_id));

    /* get device info */
    if(clGetDeviceIDs (platforms[0], CL_DEVICE_TYPE_CPU, num_devices, devices, NULL)!=CL_SUCCESS) {
        //printf("clGetDeviceIDs failed");
        ERR("clGetDeviceIDs failed");
    }
    //clGetDeviceIDs (platforms[0], CL_DEVICE_TYPE_GPU, num_devices, devices, NULL);

    if(num_devices==0)
    {
        // printf("num_devices: %d\n", (int)num_devices);
        // exit(-1);
        ERR("num_devices: %d\n", (int)num_devices);
    }
    else
    {
        printf("num_devices: %d\n", (int)num_devices);
    }


    /* create OpenCL context, and make relation with device */
    cl_context context = clCreateContext (NULL, num_devices, devices, NULL, NULL, &err);
    if(err!=CL_SUCCESS) {
        // printf("clCreateContext failed");
        // exit(-1);
        ERR("clCreateContext failed");
    }
    //cl_context context = clCreateContext (NULL, num_devices, devices, NULL, NULL, NULL);

    /* create cmd queue, and make relation with device */
    cl_command_queue cmd_queue = clCreateCommandQueue (context, devices[0], 0, &err);    
    //cl_command_queue cmd_queue = clCreateCommandQueue (context, devices[0], 0, NULL);
    if(err!=CL_SUCCESS) {
        // printf("clCreateCommandQueue failed");
        // exit(-1);
        ERR("clCreateCommandQueue failed");
    }



    /* create 3 buffer object, to store data of array */
    cl_mem buffer_a, buffer_b, buffer_c, buffer_d;    
    //buffer_a = clCreateBuffer (context, CL_MEM_READ_ONLY, data_size, NULL, NULL);
    buffer_a = clCreateBuffer (context, CL_MEM_READ_ONLY, data_size, NULL, &err);
    if(err!=CL_SUCCESS) {
        // printf("clCreateBuffer failed");
        // exit(-1);
        ERR("clCreateBuffer failed");
    }
    
    //buffer_b = clCreateBuffer (context, CL_MEM_READ_ONLY, data_size, NULL, NULL);
    buffer_b = clCreateBuffer (context, CL_MEM_READ_ONLY, data_size, NULL, &err);
    if(err!=CL_SUCCESS) {
        // printf("clCreateBuffer failed");
        ERR("clCreateBuffer failed");
    }

    //buffer_c = clCreateBuffer (context, CL_MEM_READ_WRITE, data_size, NULL, NULL);
    buffer_c = clCreateBuffer (context, CL_MEM_READ_WRITE, data_size, NULL, &err);
    if(err!=CL_SUCCESS) {
        //printf("clCreateBuffer failed");
        ERR("clCreateBuffer failed");
    }
    //buffer_d = clCreateBuffer (context, CL_MEM_READ_WRITE, data_size, NULL, NULL);

    /* copy array a and b to buffer_a and buffer_b */
    //clEnqueueWriteBuffer (cmd_queue, buffer_a, CL_FALSE, 0, data_size, a, 0, NULL, NULL);
    if(clEnqueueWriteBuffer (cmd_queue, buffer_a, CL_FALSE, 0, data_size, a, 0, NULL, NULL)!=CL_SUCCESS) {
        // printf("clEnqueueWriteBuffer failed");
        // exit(-1);
        ERR("clEnqueueWriteBuffer failed");
    }

    //clEnqueueWriteBuffer (cmd_queue, buffer_b, CL_FALSE, 0, data_size, b, 0, NULL, NULL);
    if(clEnqueueWriteBuffer (cmd_queue, buffer_b, CL_FALSE, 0, data_size, b, 0, NULL, NULL)!=CL_SUCCESS) {
        // printf("clEnqueueWriteBuffer failed");
        // exit(-1);
        ERR("clEnqueueWriteBuffer failed");
    }
       

    /* create OpenCL program from source code */
    //cl_program program = clCreateProgramWithSource (context, 1, (const char **) &program_source, NULL, NULL);
    cl_program program = clCreateProgramWithSource (context, 1, (const char **) &program_source, NULL, &err);
    if(err!=CL_SUCCESS) {
        switch (err) {
            case CL_INVALID_CONTEXT:
                //printf("CL_INVALID_CONTEXT\n");
                ERR("CL_INVALID_CONTEXT\n");
                break;
            case CL_INVALID_VALUE:
                //printf("CL_INVALID_VALUE\n");
                ERR("CL_INVALID_VALUE\n");
                break;
            case CL_OUT_OF_HOST_MEMORY:
                //printf("CL_OUT_OF_HOST_MEMORY\n");
                ERR("CL_OUT_OF_HOST_MEMORY\n");
                break;
            default:
                //printf("clCreateProgramWithSource failed\n");
                ERR("clCreateProgramWithSource failed\n");
                break;
        }
    }

    /* build compilation program for OpenCL device */
    //clBuildProgram (program, num_devices, devices, NULL, NULL, NULL);
    err=clBuildProgram (program, num_devices, devices, NULL, NULL, NULL);
    if(err!=CL_SUCCESS) {
        // printf("clBuildProgram failed");
        // exit(-1);
        switch (err) {
            case CL_INVALID_PROGRAM:
                //printf("CL_INVALID_PROGRAM\n");
                ERR("CL_INVALID_PROGRAM\n");
                break;
            case CL_INVALID_VALUE:
                //printf("CL_INVALID_VALUE\n");
                ERR("CL_INVALID_VALUE\n");
                break;
            case CL_INVALID_DEVICE:
                //printf("CL_INVALID_DEVICE\n");
                ERR("CL_INVALID_DEVICE\n");
                break;
            case CL_INVALID_BINARY:
                //printf("CL_INVALID_BINARY\n");
                ERR("CL_INVALID_BINARY\n");
                break;
            case CL_INVALID_BUILD_OPTIONS:
                //printf("CL_INVALID_BUILD_OPTIONS\n");
                ERR("CL_INVALID_BUILD_OPTIONS\n");
                break;
            case CL_INVALID_OPERATION:
                //printf("CL_INVALID_OPERATION\n");
                ERR("CL_INVALID_OPERATION\n");
                break;
            case CL_COMPILER_NOT_AVAILABLE:
                //printf("CL_COMPILER_NOT_AVAILABLE\n");
                ERR("CL_COMPILER_NOT_AVAILABLE\n");
                break;
            case CL_BUILD_PROGRAM_FAILURE:
                //printf("CL_BUILD_PROGRAM_FAILURE\n");
                ERR("CL_BUILD_PROGRAM_FAILURE\n");
                break;
            case CL_OUT_OF_HOST_MEMORY:
                //printf("CL_OUT_OF_HOST_MEMORY\n");
                ERR("CL_OUT_OF_HOST_MEMORY\n");
                break;
            default:
                //printf("clBuildProgram failed
                ERR("clBuildProgram failed\n");
                break;
        }

    }

    /* create OpenCL kernel, which is used to make vector addition */
    //cl_kernel kernel = clCreateKernel (program, "vec_add", NULL);
    cl_kernel kernel = clCreateKernel (program, "vec_add", &err);
    if(err!=CL_SUCCESS) {
        // printf("clCreateKernel failed");
        // exit(-1);
        ERR("clCreateKernel failed");
    }

    /* parameter transport from host to OpenCL kernel function */
    //clSetKernelArg (kernel, 0, sizeof (cl_mem), &buffer_a);
    if(clSetKernelArg (kernel, 0, sizeof (cl_mem), &buffer_a)!=CL_SUCCESS) {
        // printf("clSetKernelArg failed");
        // exit(-1);
        ERR("clSetKernelArg failed");
    }
    //clSetKernelArg (kernel, 1, sizeof (cl_mem), &buffer_b);
    if(clSetKernelArg (kernel, 1, sizeof (cl_mem), &buffer_b)!=CL_SUCCESS) {
        // printf("clSetKernelArg failed");
        // exit(-1);
        ERR("clSetKernelArg failed");
    }
    //clSetKernelArg (kernel, 2, sizeof (cl_mem), &buffer_c);
    if(clSetKernelArg (kernel, 2, sizeof (cl_mem), &buffer_c)!=CL_SUCCESS) {
        // printf("clSetKernelArg failed");
        // exit(-1);
        ERR("clSetKernelArg failed");
    }

    /* define index space (number of work-items), here only use 1 work group */
    size_t global_work_size[1];

    /* The number of work-items in work-group is `num_elements` */
    global_work_size[0] = num_elements;

    //clEnqueueNDRangeKernel (cmd_queue, kernel, 1, NULL, global_work_size, NULL, 0, NULL, NULL);
    if(clEnqueueNDRangeKernel (cmd_queue, kernel, 1, NULL, global_work_size, NULL, 0, NULL, NULL)!=CL_SUCCESS) {
        // printf("clEnqueueNDRangeKernel failed");
        // exit(-1);
        ERR("clEnqueueNDRangeKernel failed");
    }
    //clEnqueueBarrier(cmd_queue);
    if(clEnqueueBarrier(cmd_queue)!=CL_SUCCESS) {
        // printf("clEnqueueBarrier failed");
        // exit(-1);
        ERR("clEnqueueBarrier failed");
    }

    // int i=1;
    // while(i--){
    //     /* execute kernel compute */
    //     clEnqueueNDRangeKernel (cmd_queue, kernel, 1, NULL, global_work_size, NULL, 0, NULL, NULL);
    //     clEnqueueBarrier(cmd_queue);
    //     // /* execute kernel compute */
    //     // clEnqueueNDRangeKernel (cmd_queue, kernel, 1, NULL, global_work_size, NULL, 0, NULL, NULL);
    //     // clEnqueueBarrier(cmd_queue);
    //     // /* execute kernel compute */
    //     // clEnqueueNDRangeKernel (cmd_queue, kernel, 1, NULL, global_work_size, NULL, 0, NULL, NULL);
    //     // clEnqueueBarrier(cmd_queue);
    //     // /* execute kernel compute */
    //     // clEnqueueNDRangeKernel (cmd_queue, kernel, 1, NULL, global_work_size, NULL, 0, NULL, NULL);
    //     // clEnqueueBarrier(cmd_queue);
    // }
    /* copy `buffer_c` to `c` in host */
    //clEnqueueReadBuffer (cmd_queue, buffer_c, CL_TRUE, 0, data_size, c_r, 0, NULL, NULL);
    if(clEnqueueReadBuffer (cmd_queue, buffer_c, CL_TRUE, 0, data_size, c_r, 0, NULL, NULL)!=CL_SUCCESS) {
        // printf("clEnqueueReadBuffer failed");
        // exit(-1);
        ERR("clEnqueueReadBuffer failed");
    }

    for (int i = 0; i < num_elements; i++) {
        printf("%d ", c_r[i]);
    }
    printf("\n");

    /* verify result */
    for (int i = 0; i < num_elements; i++) {        
        if (c_r[i] != c[i]) {
            printf ("Output is incorrect\n");
            break;
        }
    } 
    
    /* clean device resources */
    clReleaseKernel (kernel);
    clReleaseProgram (program);
    clReleaseCommandQueue (cmd_queue);
    clReleaseMemObject(buffer_a);
    clReleaseMemObject(buffer_b);
    clReleaseMemObject(buffer_c);
    clReleaseContext (context);

    /* clean host resources */
    free (a);
    free (b);
    free (c);
    free(c_r);
    free (platforms);
    free (devices);

    return 0;
}
