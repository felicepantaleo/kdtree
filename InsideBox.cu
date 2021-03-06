//
//  InsideBox.cu
//
//
//  Created by Ann-Christine Vossberg on 6/3/15.
//
//

#include <stdio.h>
#include <iostream>
#include "InsideBox.hpp"


// Here you can set the device ID that was assigned to you
#define MYDEVICE 0

// Simple utility function to check for CUDA runtime errors
void checkCUDAError(const char *msg);

template <typename T>
__device__
void traverseTreeRecursiveIF( T *treeArray_values, int *treeArray_ID, int *treeArray_results, T *box, int pos, int startOfTree, int endOfTree, int number_of_dimensions, int startOfBox, int startOfResults){
    
    if(startOfTree + pos - 1 <= endOfTree){
        
        //calculate which tree level we are on to know which dimension was sorted
        int level = ceil(log2(double(pos+1))-1);
        int level_of_dimension = level%number_of_dimensions;
        int lastLevel = ceil(log2(double(endOfTree+1))-1);
        //a mod b = a - floor(a / b) * b
        
        //cout << "global position: " << startOfTree+pos -1 << " und ID: " << treeArray_ID[startOfTree+pos-1] << " level: " << level <<  " levelofDimension: " << level_of_dimension << endl;
        
        
        //check wether invalid encountered, continue search:
        if(treeArray_ID[startOfTree+pos-1] != 0){
            
            //if node has sorted dimension in box, continue both branches:
            //cout << box[2*level_of_dimension] <<  " <= " << treeArray_values[startOfTree*number_of_dimensions+number_of_dimensions*(pos-1)+level_of_dimension] << " <= " << box[2*level_of_dimension+1] << endl;
            
            if(treeArray_values[startOfTree*number_of_dimensions+number_of_dimensions*(pos-1)+level_of_dimension] >= box[startOfBox + 2*level_of_dimension] && treeArray_values[startOfTree*number_of_dimensions+number_of_dimensions*(pos-1)+level_of_dimension] <= box[startOfBox + 2*level_of_dimension+1]){
                bool inside = true;
                //cout << "number of nodes traversed " << number_of_nodes_traversed << endl;
                
                //check wether the node is inside the box:
                for(int i=0; i<number_of_dimensions; i++){
                    if(i == level_of_dimension) continue;
                    //cout << treeArray_values[startOfTree*number_of_dimensions+number_of_dimensions*(pos-1)+i] << " >= " << box[2*i] <<  " && " << treeArray_values[startOfTree*number_of_dimensions+number_of_dimensions*(pos-1)+i] << " <= " << box[2*i+1] << endl;
                    
                    if( treeArray_values[startOfTree*number_of_dimensions+number_of_dimensions*(pos-1)+i] >= box[startOfBox + 2*i] && treeArray_values[startOfTree*number_of_dimensions+number_of_dimensions*(pos-1)+i] <= box[startOfBox + 2*i+1] ){
                        //entirely inside box for all dimensions
                        
                    }
                    else{
                        //not totally inside box
                        //printf("\n thread %d is changing ID %d of tree starting at %d, exact position: %d", threadIdx.x, treeArray_ID[startOfTree+pos-1], startOfTree, startOfTree+pos-1);
                        //cout << "not inside \t ID: " << treeArray_ID[startOfTree+pos-1] <<" bei pos: " << startOfTree+pos-1 << " weg! BEIDE BRANCHES" << endl;
                        inside = false;
                    }
                }
                if(inside){
                    treeArray_results[startOfResults+pos-1] = treeArray_ID[startOfTree+pos-1];
                    //cout << "yes inside \t ID: " << treeArray_ID[startOfTree+pos-1] << " BEIDE BRANCHES" << endl;
                }
                
                //continue both branches:
                if(level != lastLevel){
                    //cout << "ID: " << treeArray_ID[startOfTree+pos-1] <<" bei pos: " << startOfTree+pos-1 << " BEIDE BRANCHES" << endl;
                    //left child:
                    pos *= 2;
                    traverseTreeRecursiveIF(treeArray_values, treeArray_ID,treeArray_results, box, pos, startOfTree, endOfTree, number_of_dimensions, startOfBox, startOfResults);
                    
                    //right child:
                    pos += 1;
                    traverseTreeRecursiveIF(treeArray_values, treeArray_ID,treeArray_results, box, pos, startOfTree, endOfTree, number_of_dimensions, startOfBox, startOfResults);
                }
                
            }
            //if sorted dimension is larger than box follow branch of smaller child = left child
            else if(treeArray_values[startOfTree*number_of_dimensions+number_of_dimensions*(pos-1)+level_of_dimension] > box[startOfBox + 2*level_of_dimension+1]){
                //cout << "ID: " << treeArray_ID[startOfTree+pos-1] << " bei pos: " << startOfTree+pos-1 << " LINKES KIND" << endl;
                //cout << "number of nodes traversed " << number_of_nodes_traversed << endl;
                //left child:
                if(level != lastLevel){
                    pos *= 2;
                    traverseTreeRecursiveIF(treeArray_values, treeArray_ID,treeArray_results, box, pos, startOfTree, endOfTree, number_of_dimensions, startOfBox, startOfResults);
                }
            }
            //if sorted dimension is smaller than box, follow branch of larger child = right child
            else{
                //cout << "ID: " << treeArray_ID[startOfTree+pos-1] << " bei pos: " << startOfTree+pos-1 << " RECHTES KIND" << endl;
                //cout << "number of nodes traversed " << number_of_nodes_traversed << endl;
                if(level != lastLevel){
                    //right child:
                    pos *= 2;
                    pos += 1;
                    traverseTreeRecursiveIF(treeArray_values, treeArray_ID, treeArray_results, box, pos, startOfTree, endOfTree, number_of_dimensions, startOfBox, startOfResults);
                }
            }
        }
    }
}

template <typename T>
__device__
void traverseTreeIterative( T *treeArray_values, int *treeArray_ID, int *treeArray_results, T *box, int pos, int startOfTree, int endOfTree, int number_of_dimensions, int startOfBox, int startOfResults){
//void traverseTreeIterative( T *treeArray_values, int *treeArray_ID, int *treeArray_results, T *box, int *queue, int pos, int startOfTree, int endOfTree, int number_of_dimensions, int startOfBox, int startOfResults){
    
    int lastLevel = ceil(log2(double(endOfTree+1))-1);
    int queue[sizeof(treeArray_results)/sizeof(int)];
    queue[startOfResults] = pos;
    int queueFront = startOfResults;
    int queueRear = startOfResults+1;
    int queueSize = 1;
    int numberOfMightHits = 0;
    
    
    while(queueSize != 0){
        queueSize--;
        pos = queue[queueFront++];
        
        int level = ceil(log2(double(pos+1))-1);
        int level_of_dimension = level%number_of_dimensions;
        
        //cout << "position " << pos-1 << endl;
        
        //if sorted dimension inside box continue with both branches
        if(treeArray_values[startOfTree*number_of_dimensions+number_of_dimensions*(pos-1)+level_of_dimension] >= box[startOfBox + 2*level_of_dimension] && treeArray_values[startOfTree*number_of_dimensions+number_of_dimensions*(pos-1)+level_of_dimension] <= box[startOfBox + 2*level_of_dimension+1]){
            //put left and right child in queue:
            //cout << "ID: " << treeArray_ID[startOfTree+pos-1] <<" bei pos: " << startOfTree+pos-1 << " BEIDE BRANCHES" << endl;
            
            //nested ifs - bad!
            if(level != lastLevel){
                queue[queueRear++] = pos*2;
                queue[queueRear++] = pos*2+1;
                queueSize+=2;
            }
            
            //and check if node is totally inside box - then right it to results
            //possibility(?) can this be checked after tree traversed? Can an extra thread check this? 2 threads per tree?
            //write pos to array? What about size of array on GPU? Size of tree must be allocated? Is there so much space?
            //per warp needed memory: 3*TreeSize - NO !!! can be put into results :)
            //toCheckIfInside.push_back(pos);
            //these results have to be checked again!!!
            treeArray_results[startOfResults+numberOfMightHits] = pos; //treeArray_ID[startOfTree+pos-1];
            numberOfMightHits++;
            
        }
        //else if sorted dimensions > inside box continue with left child
        else if(treeArray_values[startOfTree*number_of_dimensions+number_of_dimensions*(pos-1)+level_of_dimension] > box[startOfBox + 2*level_of_dimension+1]
                && level != lastLevel){
            //cout << "ID: " << treeArray_ID[startOfTree+pos-1] << " bei pos: " << startOfTree+pos-1 << " LINKES KIND" << endl;
            queue[queueRear++] = pos*2;
            queueSize++;
        }
        //else sorted dimension < inside box continue with right child
        else if(level != lastLevel){
            //cout << "ID: " << treeArray_ID[startOfTree+pos-1] << " bei pos: " << startOfTree+pos-1 << " RECHTES KIND" << endl;
            queue[queueRear++] = pos*2+1;
            queueSize++;
        }
    }
    
    //check nodes, that might be inside box:
    for(int j = 0; j<=numberOfMightHits;j++){
        pos = treeArray_results[startOfResults+j];
        treeArray_results[startOfResults+j] = 0;
        
        bool inside = true;
        for(int i=0; i<number_of_dimensions; i++){
            if( treeArray_values[startOfTree*number_of_dimensions+number_of_dimensions*(pos-1)+i] >= box[startOfBox + 2*i] && treeArray_values[startOfTree*number_of_dimensions+number_of_dimensions*(pos-1)+i] <= box[startOfBox + 2*i+1] ){
                //entirely inside box for all dimensions
            }
            else{
                inside = false;
            }
        }
        if(inside){
            treeArray_results[startOfResults+pos-1] = treeArray_ID[startOfTree+pos-1];
        }
    }
    delete[] queue;
}

template <typename T>
__global__
void insideBox(T *treeArray_values, int *treeArray_ID, int *treeArray_results, T *box, int tree_size, int number_of_dimensions){
//void insideBox(T *treeArray_values, int *treeArray_ID, int *treeArray_results, T *box, int *queue, int tree_size, int number_of_dimensions){
    //__shared__ int s[]
    //for each thread has it's own tree starting here
    //TODO: STARTOFTREE falsch.. ist die gesamte position, ohne berücksichtigung der number_of_dimensions. Die müssen berücksichtigt werden!!!
    int startOfTree = threadIdx.x%32 * tree_size;
    int endOfTree = startOfTree + (tree_size - 1);
    int startOfBox = (blockIdx.x*blockDim.x + threadIdx.x)/32*number_of_dimensions*2;
    int startOfResults = (blockIdx.x*blockDim.x + threadIdx.x)*tree_size;
    
    //if(startOfResults != startOfTree){printf("\n not same! Global Index: %d, startOfTree: %d, startOfResults %d ", blockIdx.x*blockDim.x + threadIdx.x, startOfTree, startOfResults);}
    //if(blockIdx.x*blockDim.x + threadIdx.x < 63){ printf("\n box nr: %d for gloabl index: %d",(blockIdx.x*blockDim.x + threadIdx.x)/32, blockIdx.x*blockDim.x + threadIdx.x );}
    //if(blockIdx.x >= 6 && threadIdx.x > 900){printf("\n Global Index: %d, blockDim: %d, startOfTree: %d, startOfResults %d ", blockIdx.x*blockDim.x + threadIdx.x, blockDim.x, startOfTree, startOfResults);}
    //if(blockIdx.x*blockDim.x + threadIdx.x == 0) {printf("\t \t \t INDEX IS ZERO!!!! ");}
    
    
    
    /*if(startOfResults <= sizeof(treeArray_results)/sizeof(int)){
        traverseTreeRecursiveIF(treeArray_values, treeArray_ID, treeArray_results, box, 1, startOfTree, endOfTree, number_of_dimensions, startOfBox, startOfResults);
     
    }*/
    if(startOfResults <= sizeof(treeArray_results)/sizeof(int)){
        traverseTreeIterative(treeArray_values, treeArray_ID, treeArray_results, box, 1, startOfTree, endOfTree, number_of_dimensions, startOfBox, startOfResults);
        //traverseTreeIterative(treeArray_values, treeArray_ID, treeArray_results, box, queue, 1, startOfTree, endOfTree, number_of_dimensions, startOfBox, startOfResults);
    }
}

template <typename T>
void Cuda_class<T>::cudaInsideBox(int number_of_trees, int tree_size, int number_of_dimensions, T *treeArray_values, int *treeArray_ID, int *treeArray_results, T box[], int numberOfHits){
//void Cuda_class<T>::cudaInsideBox(int number_of_trees, int tree_size, int number_of_dimensions, T *treeArray_values, int *treeArray_ID, int *treeArray_results, T box[], int* queue, int numberOfHits){
    //number_of_warps = numberOfHits
    //numberOfBlocks = numberOfHits/32, because 32 = 1 warp
    
    
    //insideBox<T><<<Anzahl benutzte Blöcke, Anzahl Threads>>> = <<<Anzahl benutzte Blöcke, Anzahl Baeume >>>
    //weil ein Thread == ein Baum, ein warp == eine box
    int warp_size = 32;
    int number_of_blocks = (numberOfHits+32-1)/32;
    
    //std::cout << "Number of Blocks: " << number_of_blocks << "\t number of Threads: " << numberOfHits*warp_size << std::endl;
    
    //insideBox<T><<<number_of_blocks,1024>>>(d_treeArray_values, d_treeArray_ID, d_treeArray_results, d_box, d_queue, tree_size, number_of_dimensions );
    insideBox<T><<<number_of_blocks,1024>>>(d_treeArray_values, d_treeArray_ID, d_treeArray_results, d_box, tree_size, number_of_dimensions );
    
    //YourKernel<<<dimGrid, dimBlock>>>(d_A,d_B); //Kernel invocation
    
    cudaError_t err = cudaGetLastError();
    if (err != cudaSuccess)
        printf("Error after Kernel: %s\n", cudaGetErrorString(err));
}

template <typename T>
void Cuda_class<T>::cudaCopyToDevice(int number_of_trees_, int tree_size_, T *treeArray_values, int *treeArray_ID, int* treeArray_results, T box[], int number_of_dimensions_, int numberOfHits){
//void Cuda_class<T>::cudaCopyToDevice(int number_of_trees_, int tree_size_, T *treeArray_values, int *treeArray_ID, int* treeArray_results, T box[], int *queue, int number_of_dimensions_, int numberOfHits){
    number_of_trees = number_of_trees_;
    number_of_dimensions = number_of_dimensions_;
    tree_size = tree_size_;
    size_of_forest =  number_of_trees*tree_size;
    
    cudaSetDevice(MYDEVICE);
    std::cout << "sizeof(int): " << sizeof(int) << " sizeof(T) " <<  sizeof(T) << std::endl;
    std::cout << "number of trees: " << number_of_trees << std::endl;
    std::cout << "tree size: " << tree_size << std::endl;
    std::cout << "number of dimensions: " << number_of_dimensions << std::endl;
    std::cout << "size of treeArray_values " << size_of_forest*number_of_dimensions*sizeof(T) << " byte and size of array: " <<  size_of_forest*number_of_dimensions <<  std::endl;
    std::cout << "size of treeArray_ID " << size_of_forest*sizeof(int) << " byte and size of array: " <<  size_of_forest << std::endl;
    std::cout << "size of treeArray_results " << size_of_forest*numberOfHits*sizeof(int) << " byte and size of array: " <<  size_of_forest*numberOfHits << std::endl;
    std::cout << "size of box " << number_of_dimensions*2*numberOfHits*sizeof(T) << " byte and size of array: " <<  number_of_dimensions*2*numberOfHits << std::endl;
    std::cout << "size of queue " << size_of_forest*numberOfHits*sizeof(int) << " byte and size of array: " <<  size_of_forest*numberOfHits << std::endl;
    
    //std::cout << "box: " << box[0] << " " << box[1] << " " << box[2] << " " << box[3] << " " << box[4] << " " << box[5] << std::endl;
    //TODO: int ----> num_t
    
    //allocate memory
    cudaMalloc(&d_treeArray_values, size_of_forest*number_of_dimensions*sizeof(T));
    cudaMalloc(&d_treeArray_ID, size_of_forest*sizeof(int));
    cudaMalloc(&d_treeArray_results, size_of_forest*numberOfHits*sizeof(int));
    cudaMalloc(&d_box, number_of_dimensions*2*numberOfHits*sizeof(T));
    //cudaMalloc(&d_queue, size_of_forest*numberOfHits*sizeof(int));
    
    cudaError_t err = cudaGetLastError();
    if (err != cudaSuccess)
        printf("Error in allocating stuff: %s\n", cudaGetErrorString(err));
    
    std::cout << "before copying to device " << std::endl;
    //send trees to gpu
    cudaMemcpy(d_treeArray_values, treeArray_values, size_of_forest*number_of_dimensions*sizeof(T), cudaMemcpyHostToDevice);
    cudaMemcpy(d_treeArray_ID, treeArray_ID, size_of_forest*sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(d_treeArray_results, treeArray_results, size_of_forest*numberOfHits*sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(d_box, box, number_of_dimensions*2*numberOfHits*sizeof(T), cudaMemcpyHostToDevice);
    //don't have to copy queue... only allocate!
    //cudaMemcpy(d_queue, queue, size_of_forest*numberOfHits*sizeof(int), cudaMemcpyHostToDevice);
    
    std::cout << "finished copying to device " << std::endl;
    
    
    err = cudaGetLastError();
    if (err != cudaSuccess)
        printf("Error in sending stuff: %s\n", cudaGetErrorString(err));
}

template <typename T>
void Cuda_class<T>::cudaCopyToHost(int* treeArray_results, int numberOfHits){
    
    
    cudaError_t err = cudaGetLastError();
    if (err != cudaSuccess)
        printf("Error before copying back to host: %s\n", cudaGetErrorString(err));
    
    cudaMemcpy(treeArray_results, d_treeArray_results, size_of_forest*numberOfHits*sizeof(int), cudaMemcpyDeviceToHost);
    std::cout << "done copying back to host " << std::endl;
    err = cudaGetLastError();
    if (err != cudaSuccess)
        printf("Error after copying back to host: %s\n", cudaGetErrorString(err));
    
    std::cout << "\n Size of forest: " << size_of_forest << std::endl;
    
    //    //print out ID's which are in box:
    //        for(int i = 0; i< number_of_trees*tree_size; i++){
    //            std::cout << "ID: " << treeArray_ID[i]<< std::endl;
    //        }
    
    
    //free space
    cudaFree(d_treeArray_values);
    cudaFree(d_treeArray_ID);
    cudaFree(d_treeArray_results);
    cudaFree(d_box);
    //cudaFree(d_queue);
    
    err = cudaGetLastError();
    if (err != cudaSuccess)
        printf("Error while freeing memory: %s\n", cudaGetErrorString(err));
    else{ std::cout <<"Freed memory successfully " << std::endl; }
    
}

template class Cuda_class<int>;
template class Cuda_class<double>;