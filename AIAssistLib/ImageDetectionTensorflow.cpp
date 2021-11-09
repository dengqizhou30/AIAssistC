#include "pch.h"
#include "ImageDetectionTensorflow.h"


//静态成员初始化
AssistConfig* ImageDetectionTensorflow::m_AssistConfig = AssistConfig::GetInstance();


//类方法实现
ImageDetectionTensorflow::ImageDetectionTensorflow()
{
    initImg();

    //加载AI模型
    initDnn();
}

ImageDetectionTensorflow::~ImageDetectionTensorflow()
{
    //图像资源释放
    releaseImg();

    //释放资源网络
    try {
        if (m_net != NULL) {
            delete m_net;
            m_net = NULL;
        }
    }
    catch (Exception ex) {
        string msg = "";
    }
}

//修改配置后，需要重新初始化一些对象
void ImageDetectionTensorflow::ReInit() {
    releaseImg();
    initImg();
}

//初始化图像资源
void ImageDetectionTensorflow::initImg() {
    //注意屏幕缩放后的情况
    //注意抓取屏幕的时候使用缩放后的物理区域坐标，抓取到的数据实际是逻辑分辨率坐标
    cv::Rect detectRect = m_AssistConfig->detectRect;
    cv::Rect detectZoomRect = m_AssistConfig->detectZoomRect;


    // 获取屏幕 DC
    m_screenDC = GetDC(HWND_DESKTOP);
    m_memDC = CreateCompatibleDC(m_screenDC);
    // 创建位图
    m_hBitmap = CreateCompatibleBitmap(m_screenDC, detectRect.width, detectRect.height);
    SelectObject(m_memDC, m_hBitmap);

    //分析位图信息头
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
    m_Bitmapinfo = new BITMAPINFO{ {sizeof(BITMAPINFOHEADER), detectRect.width, -detectRect.height, 1, wBitCount, BI_RGB },{0,0,0,0} };

    //创建存放图像数据的mat
    //m_mat.create(detectRect.height, detectRect.width, CV_8UC4);
    //m_mat3.create(detectRect.height, detectRect.width, CV_8UC3);
}

//释放图像资源
void ImageDetectionTensorflow::releaseImg() {

    //资源释放
    try {
        m_mat_src.release();
        m_mat_3.release();
        img_mat.release();

        if (m_Bitmapinfo != NULL)
            delete m_Bitmapinfo;
        DeleteObject(m_hBitmap);
        DeleteDC(m_memDC);
        ReleaseDC(HWND_DESKTOP, m_screenDC);
    }
    catch (Exception ex) {
        string msg = "";
    }

    m_Bitmapinfo = NULL;
    m_hBitmap = NULL;
    m_memDC = NULL;
    m_screenDC = NULL;
}


/* 初始化模型 */
void ImageDetectionTensorflow::initDnn() {
    //设置gpu资源限制配置，使用10%gpu内存，其他内存用于游戏
   // Read more to see how to obtain the serialized options
    std::vector<uint8_t> config{ 0x32,0x9,0x9,0x9a,0x99,0x99,0x99,0x99,0x99,0xb9,0x3f };
    // Create new options with your configuration
    TFE_ContextOptions* options = TFE_NewContextOptions();
    //TFE_ContextOptionsSetConfig(options, config.data(), config.size(), cppflow::context::get_status());
    TFE_ContextOptionsSetConfig(options, config.data(), config.size(), cppflow::context::get_status());
    // Replace the global context with your options
    cppflow::get_global_context() = cppflow::context(options);

    // 加载模型文件
    //opencv的dnn模块(NVIDIA GPU的推理模块)
    m_net = new cppflow::model(ModelFile);

    // 加载分类标签
    ifstream fin(LabelFile);
    if (fin.is_open())
    {
        string className = "";
        while (std::getline(fin, className))
            m_classLabels.push_back(className);
    }


    // 设置CUDA加速
    //(*m_net).setPreferableBackend(dnn::DNN_BACKEND_OPENCV);
    //(*m_net).setPreferableTarget(dnn::DNN_TARGET_CPU);

    return;
}


/* 获取检测区的屏幕截图 */
void ImageDetectionTensorflow::getScreenshot()
{
    //计算屏幕缩放后的，裁剪后的实际图像检查区域
    //注意抓取屏幕的时候使用缩放后的物理区域坐标，抓取到的数据实际是逻辑分辨率坐标
    cv::Rect detectRect = m_AssistConfig->detectRect;
    cv::Rect detectZoomRect = m_AssistConfig->detectZoomRect;


    // 得到位图的数据
    // 使用BitBlt截图，性能较低，后续修改为DXGI
    //Windows8以后微软引入了一套新的接口，叫“Desktop Duplication API”，应用程序，可以通过这套API访问桌面数据。
    //由于Desktop Duplication API是通过Microsoft DirectX Graphics Infrastructure (DXGI)来提供桌面图像的，速度非常快。
    bool opt = BitBlt(m_memDC, 0, 0, detectRect.width, detectRect.height, m_screenDC, detectZoomRect.x, detectZoomRect.y, SRCCOPY);

    //注意在非opencv函数中使用mat时，需要手动调用create申请内存创建数组；重复执行create函数不会导致重复创建数据存放数组；
    m_mat_src.create(detectRect.height, detectRect.width, CV_8UC4);
    //int rows = GetDIBits(m_screenDC, m_hBitmap, 0, detectRect.height, m_mat.data, (BITMAPINFO*)&bi, DIB_RGB_COLORS);
    int rows = GetDIBits(m_memDC, m_hBitmap, 0, detectRect.height, m_mat_src.data, (BITMAPINFO*)m_Bitmapinfo, DIB_RGB_COLORS);

    //cv::namedWindow("opencvwindows", WINDOW_AUTOSIZE);
    //cv::imshow("opencvwindows", m_mat_src);
    //waitKey(2000);

    //屏幕截图和视频截图格式不一样，需要进行图像格式转换
    //去掉alpha 值(透明度)通道，转换为CV_8UC3格式
    cv::cvtColor(m_mat_src, m_mat_3, COLOR_RGBA2RGB);

    //数据格式转换
    m_mat_3.convertTo(img_mat, CV_8UC3, 1.0);

    return;
}


/* AI推理 */
DETECTRESULTS ImageDetectionTensorflow::detectImg()
{
    //计算屏幕缩放后的，裁剪后的实际图像检查区域
    //注意抓取屏幕的时候使用缩放后的物理区域坐标，抓取到的数据实际是逻辑分辨率坐标
    cv::Rect detectRect = m_AssistConfig->detectRect;
    cv::Rect detectZoomRect = m_AssistConfig->detectZoomRect;
    int playerCentX = m_AssistConfig->playerCentX;

    auto classIds = vector<float_t>();
    auto confidences = vector<float_t>();
    auto boxes_float = vector<float_t>();
    auto boxes = vector<Rect>();
    DETECTRESULTS out;

    std::vector<uint8_t > img_data;

    try
    {
        img_data.assign(img_mat.data, img_mat.data + detectRect.width * detectRect.height * 3);
        cppflow::tensor input(img_data, { 1, detectRect.height, detectRect.width, 3 });

        //执行模型推理
        auto model = *m_net;
        auto result = model({ {"serving_default_images:0", input} }, { "StatefulPartitionedCall:0", "StatefulPartitionedCall:1", "StatefulPartitionedCall:2" });

        //处理推理结果
        classIds = result[2].get_data<float_t>();
        confidences = result[1].get_data<float_t>();
        boxes_float = result[0].get_data<float_t>();

        //efficientdet模型的推理结果已经排好序了
        out.maxPersonConfidencePos = 0; //最大置信度所在位置
        for (int i = 0; i < classIds.size() && i < 100; i++) {

            //分析检测结果的类型、置信度和坐标
            int classid = (int)classIds.at(i);
            float confidence = confidences.at(i);
            if (classid == PersonClassId && confidence > MinConfidence) {
                //处理满足条件的检测对象
                //efficientdet-lite object_detection检测box的坐标格式为[ymin , xmin , ymax , xmax] 
                Rect box;
                box.y = boxes_float[i*4];
                box.x = boxes_float[i*4 + 1];
                box.height = boxes_float[i*4 + 2] - box.y;
                box.width = boxes_float[i*4 + 3] - box.x;

                //为保障项目，排除太大或者太小的模型
                if (box.width <= 200 && box.width >= 10 && box.height <= 280 && box.height >= 10)
                {
                    //判断是否是游戏操者本人,模型位置为屏幕游戏者位置
                    //游戏者的位置在屏幕下方靠左一点，大概 860/1920处
                    //另外游戏中左右摇摆幅度较大，所以x轴的兼容值要设置大一些。
                    if (abs(box.x + box.width / 2 - playerCentX) <= 100 &&
                        box.y > detectRect.height * 1 / 2 &&
                        abs(detectRect.height - (box.y + box.height)) <= 10)
                    {
                        //排除游戏者自己
                        //var testi = 0;
                    }
                    else
                    {
                        //保存这个检测到的对象
                        out.classIds.push_back(classid);
                        out.confidences.push_back(confidence);
                        out.boxes.push_back(box);
                    }
                }
            }
        }

    }
    catch (Exception ex) {
        string msg = "";
    }

    return out;
}


cv::Mat ImageDetectionTensorflow::getImg() {
    //克隆mat数据结外部程序使用，这个类自身只重用两个mat对象及他们的数据内存区
    Mat mat = m_mat_3.clone();
    return mat;
}