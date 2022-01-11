# SLIC
### inplementation of SLIC
> using c++ as the coding language

### how to use
    '''
    SLIC slic;  
	slic.SLIC_demo(imagePath, labelMask, len, m);
    '''
- 实例化对象slic
- imagePath 传入要进行超像素分割的图像路径
- labelMask 用以记录传入图像的所有像素对应的超像素标签，大小同传入的图像大小
- len 为slic超像素块的大小
- m 为lab与xy聚类时候的比重，具体见[slic](https://ieeexplore.ieee.org/abstract/document/6205760)论文
