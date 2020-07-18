/**
  * ������������Լ����ﻯ����
  */

#ifndef CONVOLUTION_H
#define CONVOLUTION_H

#include <windows.h>

// ==================== �ߴ綨�� ===================
#define MAP_SIDE_MAX 224     // ����ͼ�߳������
#define MAP_CHANNEL_DEFULT 3 // ����ͼͨ��Ĭ������ RGB
#define KERNEL_SIDE 3        // �����˹̶��߳�
#define KERNEL_MAX_COUNT 32  // �������������
#define MAX_LAYER 32         // ���Ĳ�����32����128��


/**
 * �����̶���С����ά����
 * �ǵ��ֶ�delete[]������ڴ�й©
 * ���Ҫ�Ż��ٶȣ����԰�Z���������ѭ��
 */
INT8*** create3D(int y, int x, int z)
{
    INT8*** bits = new INT8**[y];
    for (int i = 0; i < y; i++)
    {
        bits[i] = new INT8*[x];
        for (int j = 0; j < x; j++)
        {
            bits[i][j] = new INT8[z];
            for (int k = 0; k < z; k++)
                bits[i][j][k] = 0;
        }
    }
    return bits;
}


/**
 * ͼ��
 * �����˱�ź�ͼ�Ĳ���
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
//        memcpy(this->map, map->map, sizeof(INT8)*side*side*channel); // Ī���ı���
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

    int kernel = 0;     // kernel ��š�������ͼ����Ҫ����
    int side = 0;       // ͼ�ı߳��������Σ�
    int channel = 0;    // ͼ��channel����
    INT8 ***map = NULL; // ͼ��Ϊ�˱������㣬Ϊ��map[channel][side][side]

    /**
     * ��ʼ��ͼ��ȫ����Ĭ��0
     * @param m ��������ͼ�����ΪNULL��ȫ�����ó�0
     */
    void initMap(INT8***m = NULL)
    {
        map = m ? m : create3D(side, side, channel);
    }
};


/**
 * ��������
 */
struct Kernel {
    Kernel(): side(3), channel(3) {}
    Kernel(int side, int channel)
        : side(side), channel(channel)
    {
        initKernel();
    }
    int side;    // �߳� side * side
    int channel; // ���ڵ�ǰ��������ͼ��channel����
    INT8 ***bits = NULL; // ÿһλ��ֵ

    /**
     * ��ʼ��kernel
     * @param k ���ΪNULL����ȫ��Ϊ0
     */
    void initKernel(INT8*** k = NULL)
    {
        bits = k ? k : create3D(side, side, channel);
    }
};


/**
 * �̴߳��ݲ�����
 * kernel.channel == image.channel
 * kernel.filter = ��һ�� image.channel
 */
struct ConvThreadArg {
    ConvThreadArg(){}
    ConvThreadArg(int layer, int k, FeatureMap *img, Kernel *kernel)
        : layer(layer), k_indx(k), map(img), kernel(kernel)
    {}
    int layer = 0;          // ��ǰ�ǵڼ���
    int k_indx = 0;         // �˵������������������ˣ���˳��
    FeatureMap *map = NULL; // ͼ�Ķ���ָ��
    Kernel *kernel;         // �����˵ı߳�
};


/**
 * ���о����ļ��㺯��
 */
FeatureMap* convolution(FeatureMap *image, Kernel *kernel)
{
    int new_side = image->side - kernel->side + 1;
    FeatureMap* result = new FeatureMap(image->kernel, new_side, 1);
    INT8*** map = result->map;

    // �ۼӣ�ע�⣺�������귴�ŵģ�����y������x��
    for (int y = 0; y < new_side; y++)
    {
        for (int x = 0; x < new_side; x++)
        {
            // ��ͼ��λ�ã�map[y][x][ch]
            // TODO��������ԼӸ��������ӿ��ٶ�
            INT8& v = map[y][x][0];
            for (int i = 0; i < kernel->side; i++)
                for (int j = 0; j < kernel->side; j++)
                    for (int k = 0; k < kernel->channel; k++)
                        v += image->map[i][j][k];
        }
    }
    return result;
}

#endif // CONVOLUTION_H