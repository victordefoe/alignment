
#### Have been archived

> It have been archived.

### Hole Shaft Alignment

Hole shaft alignment detection technology is widely used in equipment installation, daily maintenance and repair in engineering, power, petrochemical, aerospace and many other industries. Computer vision detection technology has the advantages of non-contact, full field of view, high speed, high precision and so on, and is widely used in military industry, machinery, electronics, agriculture, medicine and other fields. Applying the relevant principles and algorithms of computer vision detection technology to the detection of hole shaft alignment can effectively improve the measurement efficiency and measurement accuracy of the system. The digital image processing applied in this field is the core of computer vision inspection and plays a crucial role in the inspection process.

The ellipse detector is according to the paper blow:  
1. Fornaciari M, Prati A, Cucchiara R. A fast and effective ellipse detector for embedded vision applications[J]. Pattern Recognition, 2014, 47(11): 3693-3708.

## Practical Problem: Preliminary Solution to Hole Shaft Alignment Problem
#### 实际问题：孔轴对中问题的初步方案

For specific questions and analysis reports, please refer to the "纸质说明" folder（Only Chinese version provided）    
（其实是个大作业，具体问题和分析报告参看 “纸质说明”文件夹）

（这个大作业从分析问题、选定方案与算法、代码实现、设计调参到最终结果汇报共计耗时大约4天，实际效果一般，可以做个参考）

With the help of engineering code from "A fast and effective ellipse detector for embedded vision applications" paper.    

（借助了“A fast and effective ellipse detector for embedded vision applications ”论文的工程代码）

The project is based on Opencv 3.1.0 (or higher version? maybe ), make sure you have Opencv 3.1.0 (C++ version) installed.

Run way: in the project root directory    
（运行方式:在工程根目录下）

``` bash
bash ./build.sh

./main
```

Hough_Main is a comparison that uses only the Hough circle transformation.  
（Hough_Main是只使用hough圆变换的对比）

``` bash
bash ./build.sh
./Hough
```

The result will be saved as the video of `write_xxx.avi` in the current directory.    
（结果会保存成当前目录下`write_xxx.av`i的视频）

Some detailed data is displayed in the upper left corner of the video       
* **center shift**: distance between two ellipses    
* **rate shift**: the difference between the minor axis/major axis of the two ellipses   
* **rads shift**: the major axis offset angle of the two ellipses   


（一些细节数据展示在视频的左上角
视频左上角：
* **centre shift**:两个椭圆之间的距离    
* **rate shift**: 两个椭圆 短轴/长轴 的 差值    
* **rads shift**: 两个椭圆的长轴偏移角度    ）

Notes in the code：   
（代码里面的注意事项）：

The method of selecting the inner and outer ellipses: Select only the ellipse with the largest and the smallest major axis as the determined inner and outer ellipse [this selection method is too simple and has its limitations, which is also one of the reasons why the effect is not so good]    
（挑选内外椭圆的方法：只选取长轴最大的和最小的椭圆 作为检测得到的内外椭圆【该选取办法太简单、有局限性，这也是造成效果没那么好的原因之一】）

