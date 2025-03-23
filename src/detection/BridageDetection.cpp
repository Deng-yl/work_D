#include "BridageDetection.h"

bool Bridge::process(Findline &track, vector<PredictResult> predict)
    {
        if (!ai_params->bridge_change_scheme){
            if (bridgeEnable) // 坡道处理，直接降低前瞻，见motion.json
            {
                // if (track.pointsEdgeLeft.size() > ROWSIMAGE / 2 && track.pointsEdgeRight.size() > ROWSIMAGE / 2) // 切行，防止错误前瞻引发转向
                // {
                //     track.pointsEdgeLeft.resize(track.pointsEdgeLeft.size() / 2);
                //     track.pointsEdgeRight.resize(track.pointsEdgeRight.size() / 2);
                // }
                counterSession++;
                std::cout << "session" << counterSession <<"\n";
                if (counterSession > CarParams->bridge_count_exit){//大于40帧
                    counterRec = 0;
                    counterSession = 0;
                    bridgeEnable = false;// 关闭桥梁状态
                    std::cout<<"****退出桥****"<<std::endl;
                    return false; // 返回“未处于桥梁状态”
                }
                return true;// 保持桥梁状态
            }
            else // 检测坡道
            {
                for (int i = 0; i < predict.size(); i++)
                {   
                    if (predict[i].type == LABEL_BRIDGE){
                        counterRec_all++;
                    }
                    //满足三个条件认定坡道
                    if (predict[i].type == LABEL_BRIDGE && 
                        predict[i].score > CarParams->bridge_score && //置信度大于0.9
                        (predict[i].y + predict[i].height) > ROWSIMAGE * ai_params->bridge_block_row)//240*0.2位置位于底部20%区域
                    {
                        counterRec++;
                        std::cout<<"识别坡道:"<<counterRec<<std::endl;
                        break;
                    }
                    if(counterRec_all>20&&counterRec<CarParams->bridge_count_enter){
                        counterRec = 0;
                        counterRec_all=0;
                        //std::cout<<"坡道误判"<<std::endl;
                    }
                }
                if (counterRec>=CarParams->bridge_count_enter)
                {
                    counterRec = 0;
                    bridgeEnable = true; // 检测到桥标志
                    std::cout<<"进入桥状态"<<std::endl;
                    return true;
                }

                return false;
            }
        }


        else{//备用方案                
            // 与主方案类似，但条件更严格：
           // 1. 检测到非桥梁预测时立即重置计数器
          // 2. 使用不同的进入阈值 `ai_params->bridge_count_enter_changed`
            if (bridgeEnable) // 坡道处理，直接降低前瞻，见motion.json
            {
                counterSession++;
                std::cout << "session" << counterSession <<"\n";
                if (counterSession > CarParams->bridge_count_exit){
                    counterRec = 0;
                    counterSession = 0;
                    bridgeEnable = false;
                    std::cout<<"****退出桥****"<<std::endl;
                    return false;
                }
                return true;
            }
            else // 检测坡道
            {
                for (int i = 0; i < predict.size(); i++)
                {
                    if (predict[i].type == LABEL_BRIDGE &&
                     predict[i].score > CarParams->bridge_score && 
                     (predict[i].y + predict[i].height) > ROWSIMAGE * ai_params->bridge_block_row)
                    {
                        counterRec++;
                        std::cout<<"识别坡道:"<<counterRec<<std::endl;
                        break;
                    }
                    else{
                        counterRec = 0;
                    }
                }
                if (counterRec >= ai_params->bridge_count_enter_changed)//大于10帧
                {
                    counterRec = 0;
                    bridgeEnable = true; // 检测到桥标志
                    std::cout<<"进入桥状态"<<std::endl;
                    return true;
                }

                return false;
            }
        }
    }


void Bridge::drawImage(Mat &image,Findline track)
    {
        // 赛道边缘
        for (int i = 0; i < track.pointsEdgeLeft.size(); i++)
        {//调用circle函数，参数是image，Point对象，半径1，颜色Scalar(0,255,0)，厚度-1(代表实心点)。坐标(x,y)=(col，row)
            circle(image, Point(track.pointsEdgeLeft[i].col, track.pointsEdgeLeft[i].row), 1,//调用circle函数，参数是image，Point对象，半径1，颜色Scalar(0,255,0)，厚度-1。Point的参数是col和row
                   Scalar(0, 255, 0), -1); // 绿色点
        }
        for (int i = 0; i < track.pointsEdgeRight.size(); i++)
        {
            circle(image, Point(track.pointsEdgeRight[i].col, track.pointsEdgeRight[i].row), 1,
                   Scalar(0, 255, 255), -1); // 黄色点
        }

        if (bridgeEnable)
            putText(image, "[1] BRIDGE - ENABLE", Point(COLSIMAGE / 2 - 30, 10), cv::FONT_HERSHEY_TRIPLEX, 0.3, cv::Scalar(0, 255, 0), 1, CV_AA);
    }//在图像上显示提示“桥状态”，位置为图像的水平中心偏左 30 像素，垂直方向距离顶部 10 像素，字体为...，字体大小为 0.3，颜色为绿色，线宽为 1，使用抗锯齿渲染。
