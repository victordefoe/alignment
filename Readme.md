#### 实际问题：孔轴对中问题的初步方案

--其实是个大作业，具体问题和分析报告参看 “说明”文件夹

这个大作业从分析问题、选定方案与算法、代码实现、设计调参到最终结果汇报共计耗时大约4天，实际效果一般，可以做个参考



借助了“A fast and effective ellipse detector for embedded vision applications ”论文的工程代码

Opencv 3.1.0

运行方式:在当前目录下

``` bash
bash ./build.sh

./main
```



Hough_Main是只使用hough圆变换的对比，

``` bash
bash ./build.sh
./Hough
```

结果会保存成当前目录下write_xxx.avi的视频

视频左上角：

> centre shift:两个椭圆之间的距离
>
> rate shift: 两个椭圆 短轴/长轴 的 差值
>
> rads shift: 两个椭圆的长轴偏移角度

代码里面的注意事项：

挑选内外椭圆的方法：只选取长轴最大的和最小的椭圆 作为检测得到的内外椭圆【该选取办法太简单、有局限性，这也是造成效果没那么好的原因之一】

