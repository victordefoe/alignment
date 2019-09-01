#!/bin/bash

rm Operam.o Alignment.o parameters.o main common.o EllipseDetectorYaed.o
g++ -c parameters.cpp -std=c++11 
g++ -c Operam.cpp -std=c++11 
g++ -c Alignment.cpp -std=c++11
g++ -c common.cpp -std=c++11
g++ -c EllipseDetectorYaed.cpp -std=c++11
g++ -c Hough_Main.cpp -std=c++11

g++ Alignment.o Operam.o parameters.o common.o EllipseDetectorYaed.o  -o main  -std=c++11 `pkg-config --cflags --libs opencv`

g++ Hough_Main.o Operam.o parameters.o common.o EllipseDetectorYaed.o  -o Hough  -std=c++11 `pkg-config --cflags --libs opencv`

rm Operam.o Alignment.o parameters.o common.o EllipseDetectorYaed.o