// OpencvTest.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include<string>
#include<fstream>
#include<cmath>
#include <windows.h>
#include <time.h>

#include <Tlhelp32.h>
#include<opencv.hpp>
#include<imgproc.hpp>

#include "tensorflow/cc/ops/const_op.h"
#include "tensorflow/cc/ops/image_ops.h"
#include "tensorflow/cc/ops/standard_ops.h"
#include "tensorflow/core/framework/graph.pb.h"
#include "tensorflow/core/framework/tensor.h"
#include "tensorflow/core/graph/default_device.h"
#include "tensorflow/core/graph/graph_def_builder.h"
#include "tensorflow/core/lib/core/errors.h"
#include "tensorflow/core/lib/core/stringpiece.h"
#include "tensorflow/core/lib/core/threadpool.h"
#include "tensorflow/core/lib/io/path.h"
#include "tensorflow/core/lib/strings/stringprintf.h"
#include "tensorflow/core/platform/env.h"
#include "tensorflow/core/platform/init_main.h"
#include "tensorflow/core/platform/logging.h"
#include "tensorflow/core/platform/types.h"
#include "tensorflow/core/public/session.h"
#include "tensorflow/core/util/command_line_flags.h"

#include "tensorflow/cc/client/client_session.h"
#include "tensorflow/cc/framework/gradients.h"


using namespace tensorflow;
using namespace tensorflow::ops;
using tensorflow::Flag;
using tensorflow::Tensor;
using tensorflow::Status;
using tensorflow::string;
using tensorflow::int32;


using namespace cv;
using namespace std;


/**
https://blog.csdn.net/yx123919804/article/details/107042822
https://blog.csdn.net/dycljj/article/details/118408400
https://www.jianshu.com/p/42589012c6f1
**/
void Mat_to_Tensor(Mat img, Tensor* output_tensor, int input_rows, int input_cols)
{
    //图像进行resize处理
    resize(img, img, cv::Size(input_cols, input_rows));
    //imshow("resized image",img);

    //归一化
    //img.convertTo(img,CV_32FC1);
    //img=1-img/255;

    //创建一个指向tensor的内容的指针
    float* p = output_tensor->flat<float>().data();

    cv::Mat tempMat(input_rows, input_cols, CV_32FC3, p);   //注意转换的图像为彩色图还是灰度图
    img.convertTo(tempMat, CV_32FC3);
}

int Tensor_to_Mat(const tensorflow::Tensor& inputTensor, cv::Mat& output)
{
    tensorflow::TensorShape inputTensorShape = inputTensor.shape();
    if (inputTensorShape.dims() != 4)
    {
        return -1;
    }

    int height = inputTensorShape.dim_size(1);
    int width = inputTensorShape.dim_size(2);
    int depth = inputTensorShape.dim_size(3);

    output = cv::Mat(height, width, CV_32FC(depth));
    auto inputTensorMapped = inputTensor.tensor<float, 4>();
    float* data = (float*)output.data;
    for (int y = 0; y < height; ++y)
    {
        float* dataRow = data + (y * width * depth);
        for (int x = 0; x < width; ++x)
        {
            float* dataPixel = dataRow + (x * depth);
            for (int c = 0; c < depth; ++c)
            {
                float* dataValue = dataPixel + c;
                *dataValue = inputTensorMapped(0, y, x, c);
            }
        }
    }
    return 0;
}



int main()
{
    HDC m_screenDC;
    HDC m_memDC;
    HBITMAP m_hBitmap;

    int width = 320, height = 320;

    // 获取屏幕 DC
    m_screenDC = GetDC(HWND_DESKTOP);
    m_memDC = CreateCompatibleDC(m_screenDC);
    // 创建位图
    m_hBitmap = CreateCompatibleBitmap(m_screenDC, width, height);
    SelectObject(m_memDC, m_hBitmap);


    cv::Mat mat, mat2;
    mat.create(height, width, CV_8UC4);
    //mat.create(height, width, CV_32FC4);

    //std::cout << "mat.type() = " << mat.type() << std::endl;
    //std::cout << "mat2.type() = " << mat2.type() << std::endl;

    //cv::namedWindow("opencvwindows", WINDOW_AUTOSIZE);

    //cv::imshow("opencvwindows", mat);
    //cout << "show empty mat\n";
    //waitKey(2000);




    double duration1, duration2;
    clock_t start1, finish1, start2, finish2;

    start1 = clock();


    // 得到位图的数据
    // 使用BitBlt截图，性能较低，后续修改为DXGI
    //Windows8以后微软引入了一套新的接口，叫“Desktop Duplication API”，应用程序，可以通过这套API访问桌面数据。
    //由于Desktop Duplication API是通过Microsoft DirectX Graphics Infrastructure (DXGI)来提供桌面图像的，速度非常快。
    bool opt = BitBlt(m_memDC, 0, 0, width, height, m_screenDC, 300, 250, SRCCOPY);

    int iBits = GetDeviceCaps(m_memDC, BITSPIXEL) * GetDeviceCaps(m_memDC, PLANES);
    WORD wBitCount;
    if (iBits <= 1)
        wBitCount = 1;
    else if (iBits <= 4)
        wBitCount = 4;
    else if (iBits <= 8)
        wBitCount = 8;
    else if (iBits <= 24)
        wBitCount = 24;
    else
        wBitCount = 32;
    BITMAPINFOHEADER bi = { sizeof(bi), width, -height, 1, wBitCount, BI_RGB };


    //int rows = GetDIBits(m_screenDC, m_hBitmap, 0, detectRect.height, m_mat.data, (BITMAPINFO*)&bi, DIB_RGB_COLORS);
    int rows = GetDIBits(m_memDC, m_hBitmap, 0, height, mat.data, (BITMAPINFO*)&bi, DIB_RGB_COLORS);

    //cv::namedWindow("opencvwindows", WINDOW_AUTOSIZE);
    //cv::imshow("opencvwindows", mat);
    cout << "show GetDIBits mat\n";
    //waitKey(10000);

    // 创建mat对象
    //注意参数顺序为 行、列、图像格式、bitmap数据
    //Mat screenshot = Mat(detectRect.height, detectRect.width, CV_8UC4, m_screenshotData);
    //Mat screenshot(detectRect.width, detectRect.height, CV_8UC4, m_screenshotData);
    //Mat screenshot(detectRect.height, detectRect.width, CV_32FC4, m_screenshotData);

    //屏幕截图和视频截图格式不一样，需要进行图像格式转换
    //去掉alpha 值(透明度)通道，转换为CV_8UC3格式
    cv::cvtColor(mat, mat2, COLOR_RGBA2RGB);

    finish1 = clock();
    duration1 = (double)(finish1 - start1) * 1000 / CLOCKS_PER_SEC;

    //std::cout << "mat.type() = " << mat.type() << std::endl;
    //std::cout << "mat2.type() = " << mat2.type() << std::endl;

    //检测图像格式
    //std::cout << "mat格式：" << mat.depth() << "," << mat.type() << "," << mat.channels() << "," << std::endl;
    //std::cout << "mat2格式：" << mat2.depth() << "," << mat2.type() << "," << mat2.channels() << "," << std::endl;

    cv::imshow("opencvwindows", mat2);
    cout << "show cvtColor mat\n";
    waitKey(10000);




    /**
    const string ConfigFile = "../../Data/model/efficientdet/d0.pbtxt";
    const string ModelFile = "../../Data/model/efficientdet/d0.pb";
    const string LabelFile = "../../Data/model/efficientdet/coco.names";
    vector<string> m_classLabels; //类标签
    const float MinConfidence = 0.7; //最小置信度
    const int PersonClassId = 0; //分类标签列表中人员的位置
    vector<String> m_outputsNames; //模型输出层的名字
   */


   //const string ConfigFile = "../../Data/model/mobilenet/ssd_mobilenet_v3.pbtxt";
   //const string ModelFile = "../../Data/model/mobilenet/ssd_mobilenet_v3.pb";
    const string ConfigFile = "../../Data/model/mobilenet/ssd_mobilenet_v3_small.pbtxt";
    const string ModelFile = "../../Data/model/mobilenet/ssd_mobilenet_v3_small.pb";
    const string LabelFile = "../../Data/model/mobilenet/coco.names";
    vector<string> m_classLabels; //类标签
    const float MinConfidence = 0.7; //最小置信度
    const int PersonClassId = 0; //分类标签列表中人员的位置
    //cv::dnn::DetectionModel m_net;
    vector<String> m_outputsNames; //模型输出层的名字



    //get the names of all the layers in the network
    //vector<String> layersNames = m_net.getLayerNames();

    //m_net.setPreferableBackend(dnn::DNN_BACKEND_OPENCV);
    //m_net.setPreferableTarget(dnn::DNN_TARGET_CPU);

    // 加载分类标签
    ifstream fin(LabelFile);
    if (fin.is_open())
    {
        string className = "";
        while (std::getline(fin, className))
            m_classLabels.push_back(className);
    }




    //创建tensorflow会话
    Session* session;
    cout << "start initalize session" << "\n";
    Status status = tensorflow::NewSession(SessionOptions(), &session);
    if (!status.ok()) {
        cout << status.ToString() << "\n";
        return 1;
    }

    GraphDef graph_def;
    status = ReadBinaryProto(Env::Default(), ModelFile, &graph_def);
    //MNIST_MODEL_PATH为模型的路径，即model_frozen.pb的路径
    if (!status.ok()) {
        cout << status.ToString() << "\n";
        return 1;
    }
    for (int i = 0; i < graph_def.node_size(); i++)
    {
        std::string name = graph_def.node(i).name();
        std::cout << name << std::endl;
    }

    status = session->Create(graph_def);
    if (!status.ok()) {
        cout << status.ToString() << "\n";
        return 1;
    }
    cout << "tensorflow模型加载成功" << "\n";


    //转换图片
    int input_height = 320;
    int input_width = 320;
    Tensor input_tensor1(DT_FLOAT, TensorShape({ 1,input_height,input_width,3 }));
    //将Opencv的Mat格式的图片存入tensor
    Mat_to_Tensor(mat2, &input_tensor1, input_height, input_width);
    cout << input_tensor1.DebugString() << endl;
    


    //前向运行，输出结果一定是一个tensor的vector
    vector<tensorflow::Tensor> outputs;
    string input_tensor_name = "normalized_input_image_tensor";
    string output_node = "detection_out";
    //cout << "Session Running......" << endl;
    Status status_run = session->Run({ { input_tensor_name, input_tensor1 } }, { output_node }, {}, &outputs);
    //cout << "Session complet......" << endl;
    if (!status_run.ok()) {
        cout << "ERROR: RUN failed..." << std::endl;
        cout << status_run.ToString() << "\n";
        exit(-1);
    }
    session->Close();



    Mat show_image;
    //vector<tensorflow::Tensor> outputs;
    for (int i = 0; i < outputs.size(); ++i)
    {
        cout << outputs[i].DebugString() << endl;

        Tensor_to_Mat(outputs[i], show_image);

    }

    cv::imshow("opencvwindows", show_image);
    cout << "show cvtColor mat\n";
    waitKey(10000);


    /*
    string times = format("%.2f, %.2f", duration1, duration2);
    putText(mat2, times, Point(10, 20), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0, 0, 255), 1);

    for (int i = 0; i < classIds.size(); i++) {
        rectangle(mat2, boxes[i], Scalar(255, 178, 50), 3);

        //Get the label for the class name and its confidence
        string label = format("%.2f", confidences[i]);
        if (!m_classLabels.empty())
        {
            //label = m_classLabels[classIds[i]-1] + ":" + label;
            label = to_string(classIds[i]) + "," + label;
        }

        //Display the label at the top of the bounding box
        int baseLine;
        Size labelSize = getTextSize(label, FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);
        int top = max(boxes[i].y, labelSize.height);
        putText(mat2, label, Point(boxes[i].x, top), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0, 0, 255), 1);
    }
    */

    cv::namedWindow("opencvwindows", WINDOW_AUTOSIZE);
    cv::imshow("opencvwindows", mat2);


    waitKey(0);

    //cv::destroyAllWindows();


}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
