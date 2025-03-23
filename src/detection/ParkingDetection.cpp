#include "ParkingDetection.h"

bool Parking::process(vector<PredictResult> predict)
    {
        switch (step)
        {
        case Step::init: // 初始化：起点斑马线屏蔽
            countSes++;
            for (int i = 0; i < predict.size(); i++)
            {
                if (predict[i].type == LABEL_CROSSWALK) // AI识别标志
                {
                    if ((predict[i].y + predict[i].height) > ROWSIMAGE * 0.2) // 标志距离计算
                    {
                        countSes = 0;//若检测到斑马线位于图像下方20%区域，持续重置计数器，保持在 init 状态
                        break;// 发现有效斑马线后跳出循环
                    }
                }
            }
            if (countSes > 50) // 若连续50帧未检测到有效斑马线，认为已离开起点，进入检测状态
            {
                countSes = 0;
                step = Step::det;
            }
            break;

        case Step::det: // AI未识别
            for (int i = 0; i < predict.size(); i++)
            {
                if (predict[i].type == LABEL_CROSSWALK) // AI识别标志
                {
                    if ((predict[i].y + predict[i].height) > ROWSIMAGE * 0.4) // 标志距离计算
                    {
                        countRec++;// 有效检测计数器递增
                        break;
                    }
                }
            }
            if (countRec) // 识别AI标志后开始场次计数
                countSes++;

            if (countSes >= 8)
            {
                if (countRec >= 4)
                {
                    step = Step::enable;
                }

                countRec = 0;
                countSes = 0;
            }
            break;

        case Step::enable: // 场景使能: 检测斑马线标识丢失，启动停车逻辑
            countSes++;
            for (int i = 0; i < predict.size(); i++)
            {
                if (predict[i].type == LABEL_CROSSWALK) // AI识别标志
                {
                    if (predict[i].y > ROWSIMAGE * 0.8) // 标志距离计算
                    {
                        countSes = 0;
                        break;
                    }
                }
            }
            if (countSes > 2)
            {
                countExit = 0;// 重置停车倒计时
                step = Step::stop;//进入停车状态
            }
            break;

        case Step::stop: // 准备停车
            bool _find_corss_walk = false;// 检查是否仍存在斑马线
            for(auto _res : predict){ //如果还检测到就break
                if(_res.type == LABEL_CROSSWALK){
                    _find_corss_walk = true;
                    break;
                }
            }
            if(_find_corss_walk){// 若检测到斑马线，重置倒计时并启动计时器
                countExit = 0;
                parking_timer.tic();// 记录当前时间（用于超时判断）
                
                break;
            }
            // 未检测到斑马线，开始停车倒计时
            park = true;
            countExit++; // 停车倒计时
            break;
        }

        // 输出场景状态结果
        if (step == Step::init || step == Step::det)
            return false;
        else
            return true;
    }
    void Parking::drawImage(Mat &img)
    {
        if (step == Step::enable)
            putText(img, "[5] PARK - ENABLE", Point(COLSIMAGE / 2 - 30, 10), cv::FONT_HERSHEY_TRIPLEX, 0.3, cv::Scalar(0, 255, 0), 1, CV_AA);
    }