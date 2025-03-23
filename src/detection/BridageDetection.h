#pragma once 
#include "common.hpp"
#include "../element/Findline.h"
#include "../include/detection.hpp"

class Bridge
{
public:
    bool process(Findline &track, vector<PredictResult> predict);//接收track引用和含有坐标、尺寸等信息的结构体中的变量predict

    /**
     * @brief 识别结果图像绘制
     *
     */
    void drawImage(Mat &image,Findline track);//绘制图像函数，Mat 是 OpenCV 库中用于表示图像矩阵的类
    bool bridgeEnable = false;   // 桥区域使能标志


private:
    uint16_t counterSession = 0; // 图像场次计数器,记录在桥梁区域内的持续帧数
    uint16_t counterRec = 0;     // 加油站标志检测计数器(有效)
    uint16_t counterRec_all=0;
};
