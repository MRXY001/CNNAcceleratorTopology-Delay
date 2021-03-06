/**
  * 卷积的相关类以及柯里化方法
  */

#ifndef CONVOLUTION_H
#define CONVOLUTION_H

#include <QApplication>
#ifdef Q_OS_WIN
#include <windows.h>
#else
    #define INT8 char
#endif


/**
 * 创建固定大小的三维数组
 * 记得手动delete[]，免得内存泄漏
 * 如果要优化速度，可以把Z放在最外层循环
 */
INT8*** create3D(int y, int x, int z);


/**
 * 图类
 * 包含了标号和图的层数
 */
struct FeatureMap {
    FeatureMap(){}
    FeatureMap(int k, int side, int channel) : FeatureMap(side, channel)
    {
        this->kernel = k;
    }
    FeatureMap(int side, int channel) : side(side), channel(channel)
    {
        initMap();
    }
    FeatureMap(int k, FeatureMap *map)
    {
        this->kernel = k;
        this->side = map->side;
        this->channel = map->channel;
        initMap();
//        printf("initMap finished: %d, %d, %d  %d~%d\n", side, side, channel, this->map[side-1][side-1][channel-1], map->map[side-1][side-1][channel-1]);
//        memcpy(this->map, map->map, sizeof(INT8)*side*side*channel); // 莫名的崩溃
//        printf("memcpy finished\n");
        for (int y = 0; y < side; y++)
            for (int x = 0; x < side; x++)
                for (int z = 0; z < channel; z++)
                    this->map[y][x][z] = map->map[y][x][z];
    }
    ~FeatureMap()
    {
        if (map)
        {
            for (int y = 0; y < side; y++)
            {
                for (int x = 0; x < side; x++)
                {
                    delete[] map[y][x];
                }
                delete map[y];
            }
            delete[] map;
        }
    }

    int kernel = 0;     // kernel 标号。被滑的图不需要这项
    int side = 0;       // 图的边长（正方形）
    int channel = 0;    // 图的channel数量
    INT8 ***map = NULL; // 图：为了遍历方便，为：map[channel][side][side]

    /**
     * 初始化图，全部都默认0
     * @param m 传进来的图，如果为NULL则全部设置成0
     */
    void initMap(INT8***m = NULL)
    {
        map = m ? m : create3D(side, side, channel);
    }
};


/**
 * 卷积核类
 */
struct Kernel {
    Kernel(): side(3), channel(3) {}
    Kernel(int side, int channel)
        : side(side), channel(channel)
    {
        initKernel();
    }
    int side;    // 边长 side * side
    int channel; // 等于当前被滑动的图的channel数量
    INT8 ***bits = NULL; // 每一位的值

    /**
     * 初始化kernel
     * @param k 如果为NULL，则全部为0
     */
    void initKernel(INT8*** k = NULL)
    {
        bits = k ? k : create3D(side, side, channel);
    }
};


/**
 * 线程传递参数类
 * kernel.channel == image.channel
 * kernel.filter = 下一层 image.channel
 */
struct ConvThreadArg {
    ConvThreadArg(){}
    ConvThreadArg(int layer, int k, FeatureMap *img, Kernel *kernel)
        : layer(layer), k_indx(k), map(img), kernel(kernel)
    {}
    int layer = 0;          // 当前是第几层
    int k_indx = 0;         // 核的索引，最终与其他核（按顺序）
    FeatureMap *map = NULL; // 图的对象指针
    Kernel *kernel;         // 卷积核的边长
};


/**
 * 进行卷积的计算函数
 */
FeatureMap* convolution(FeatureMap *image, Kernel *kernel);

#endif // CONVOLUTION_H
