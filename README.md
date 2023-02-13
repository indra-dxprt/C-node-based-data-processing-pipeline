# C++node-based-data-processing-pipeline
Demo project for C++17
## Description
The goal is to design and implement a minimal node-based data processing pipeline. We only consider the fix pipeline below, which has two inputs, with one binary and one unary operator. The operators are applied on the inputs to compute the result. The implementation is done with c++17. CMake as build system.
## Part A:
Inputs
* User Data A: [float] - fix values
* Generator B: [float] - random values
   Binary Node
* Inputs: A: [float], B: [float]
* Output: [add( a, b )] for a, b \in A, B
   Unary Node
* Input: C: [float]
* Output: max(C)
## Part B:
* Add binary operations {subtract, multiply, divide} and unary operations {min, sum} as options
* Add some way to serialize and deserialize the pipeline (e.g., seed, operators)
* Allow to load a pipeline configuration from file specified via cmdargs, e.g.
  $ pipeline.exe dataB.json config1.json > resultB1.txt
* Create test cases for the following pipelines
* BOUNDING BOX - ops: add / sum, seed: 0xc0010ff, user data: 1k floats \in [-100,100]
* ALPHA CHANNEL - ops: multiply / max, seed: 0xbadcaffe, user data: 10k floats \in [0,1]
