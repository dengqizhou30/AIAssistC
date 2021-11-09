#include "pch.h"
#include "AssistWorker.h"


//初始化静态成员变量
AssistConfig* AssistWorker::m_AssistConfig = AssistConfig::GetInstance();


AssistWorker::AssistWorker()
{
    //先重新计算检测区域相关数据
    m_AssistConfig->ReCalDetectionRect();

    //创建队列
    //鼠标操作队列长度设置为1，目的是只处理最新检测结果。
    fireQueue = new BlockQueue<DETECTRESULTS>(1);
    moveQueue = new BlockQueue<DETECTRESULTS>(1);

    drawQueue = new BlockQueue<DRAWRESULTS>(10);
    outDrawQueue = new BlockQueue<Mat>(10);

    //创建线程
    detectThread = new thread(std::bind(&AssistWorker::DetectWork, this));
    detectThread->detach();

    fireThread = new thread(std::bind(&AssistWorker::FireWork, this));
    fireThread->detach();

    moveThread = new thread(std::bind(&AssistWorker::MoveWork, this));
    moveThread->detach();

    drawThread = new thread(std::bind(&AssistWorker::DrawWork, this));
    drawThread->detach();

    //创建图片检测和鼠标操作对象
    //imageDetection = new ImageDetection();
    imageDetection = new ImageDetectionTensorflow();
    mouseKeyboard = new MouseKeyboard();

    return;
}

AssistWorker::~AssistWorker()
{
    m_stopFlag = true;
    
    if (imageDetection != NULL)
        delete imageDetection;
    if (mouseKeyboard != NULL)
        delete mouseKeyboard;

    if (drawQueue != NULL)
        delete drawQueue;
    if (fireQueue != NULL)
        delete fireQueue;
    if (moveQueue != NULL)
        delete moveQueue;

    if (detectThread != NULL)
        delete detectThread;
    if (fireThread != NULL)
        delete fireThread;
    if (moveThread != NULL)
        delete moveThread;
    if (drawThread != NULL)
        delete drawThread;

    return;
}

//修改配置后，需要重新初始化一些对象
void AssistWorker::ReInit() {
    
    //先停止所有工作线程
    Pause();
    Sleep(100);

    //先重新计算检测区域相关数据
    m_AssistConfig->ReCalDetectionRect();

    //清空所有队列
    drawQueue->Clear();
    outDrawQueue->Clear();
    fireQueue->Clear();
    moveQueue->Clear();

    //重建需要重建的对象
    if (imageDetection != NULL) {
        imageDetection->ReInit();
    }
    {
        //新建对象
        //imageDetection = new ImageDetection();
        imageDetection = new ImageDetectionTensorflow();
    }

    //重启工作线程
    Start();

    return;
}

void AssistWorker::DetectWork()
{
    while (!m_stopFlag)
    {
        if (m_detectPauseFlag)
        {
            //根据线程标志进行线程阻塞
            unique_lock<mutex> locker(m_detectMutex);
            while (m_detectPauseFlag)
            {
                m_detectCondition.wait(locker); // Unlock _mutex and wait to be notified
            }
            locker.unlock();
        }
        else {
            //图像检测
            double duration;
            clock_t start, finish;
            start = clock();

            //屏幕截屏图像检测
            imageDetection->getScreenshot();
            DETECTRESULTS detectResult = imageDetection->detectImg();

            finish = clock();
            duration = (double)(finish - start) * 1000 / CLOCKS_PER_SEC;

            if (detectResult.classIds.size() > 0) {
                //有检查到人类，结果放到队列中,并通知处理线程消费检测结果
                //开枪和鼠标移动操作放在不同线程，导致操作割裂，先放回同一个线程处理
                //fireQueue->PushBackForce(detectResult);
                moveQueue->PushBackForce(detectResult);
            }

            //然后复制mat对象，用于前端显示
            Mat mat = imageDetection->getImg();

            DRAWRESULTS  drawResult{detectResult, mat, duration};
            bool push = drawQueue->PushBack(drawResult);

            //如果队列满没有推送成功，则手工释放clone的mat对象
            if (!push) {
                mat.release();
                mat = NULL;
            }
            
        }
    }

    return;
}

void AssistWorker::FireWork()
{
    while (!m_stopFlag)
    {
        if (m_firePauseFlag)
        {
            //根据线程标志进行线程阻塞
            unique_lock<mutex> locker(m_fireMutex);
            while (m_firePauseFlag)
            {
                m_fireCondition.wait(locker); // Unlock _mutex and wait to be notified
            }
            locker.unlock();
        }
        else {
            
            //获取队列中存放的检测结果
            DETECTRESULTS detectResult;
            bool ret = fireQueue->PopFront(detectResult);
            if (ret) {
                //执行自动开枪操作
                //先检查是否设置了自动开枪标志
                if (m_AssistConfig->autoFire) {
                    //在检查是否已经瞄准了
                    bool isInTarget = mouseKeyboard->IsInTarget(detectResult);
                    //如果已经瞄准，执行自动开枪操作
                    if (isInTarget) {
                        //开枪和鼠标移动操作放在不同线程，导致操作割裂，先放回同一个线程处理
                        //mouseKeyboard->AutoFire(detectResult);
                    }
                }
            }
            
        }
    }

    return;
}

void AssistWorker::MoveWork()
{
    while (!m_stopFlag)
    {
        if (m_movePauseFlag)
        {
            //根据线程标志进行线程阻塞
            unique_lock<mutex> locker(m_moveMutex);
            while (m_movePauseFlag)
            {
                m_moveCondition.wait(locker); // Unlock _mutex and wait to be notified
            }
            locker.unlock();
        }
        else {
            //获取队列中存放的检测结果
            DETECTRESULTS detectResult;
            bool ret = moveQueue->PopFront(detectResult);
            if (ret) {
                //执行鼠标操作
                //std::cout << to_string(detectResult.classIds.size());
                //先检查是否设置了自动追踪
                if (m_AssistConfig->autoTrace) {
                    //在检查是否已经瞄准了
                    bool isInTarget = mouseKeyboard->IsInTarget(detectResult);
                    //没有瞄准的情况下，才执行鼠标追踪操作
                    if (isInTarget) {
                        //开枪和鼠标移动操作放在不同线程，导致操作割裂，先放回同一个线程处理
                        mouseKeyboard->AutoFire(detectResult); 
                    }
                    else {
                        mouseKeyboard->AutoMove(detectResult);
                    }
                }
            }
        }
    }

    return;
}

void AssistWorker::DrawWork()
{
    //cv::namedWindow("opencvwindows", WINDOW_AUTOSIZE);
    //cv::startWindowThread();

    while (!m_stopFlag)
    {
        if (m_drawPauseFlag)
        {
            //根据线程标志进行线程阻塞
            unique_lock<mutex> locker(m_drawMutex);
            while (m_drawPauseFlag)
            {
                m_drawCondition.wait(locker); // Unlock _mutex and wait to be notified
            }
            locker.unlock();
        }
        else {
            //获取队列中存放的检测结果
            DrawResults drawResult;
            bool ret = drawQueue->PopFront(drawResult);
            if (ret) {
                //执行绘图操作
                DETECTRESULTS out = drawResult.out;
                Mat mat = drawResult.mat;
                double duration = drawResult.duration;
                if (!mat.empty()) {
                    string times = format("%.2f ms", duration);
                    putText(mat, times, Point(10, 20), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0, 0, 250), 1);

                    //注意游戏屏幕中心，和检测区域的中心位置不一样，检测区域有向上稍微调整1/10
                    Rect center = { mat.cols/2 -5,mat.rows / 2 + mat.rows /10 - 5,10,10 };
                    rectangle(mat, center, Scalar(0, 0, 250), 2);

                    if (out.classIds.size() > 0) {
                        Rect rect = out.boxes[0];
                        rectangle(mat, { rect.x + rect.width/2 -4, rect.y + rect.height / 2 - 4, 8, 8 }, Scalar(255, 178, 50), 2);
                    }

                    for (int i = 0; i < out.classIds.size(); i++) {
                        rectangle(mat, out.boxes[i], Scalar(255, 178, 50), 2);

                        //Get the label for the class name and its confidence
                        string label = format("%.2f", out.confidences[i]);
                        //label = m_classLabels[classIds[i]-1] + ":" + label;
                        label = "" + to_string(out.classIds[i]) + ", " + label;                      

                        //Display the label at the top of the bounding box
                        int baseLine;
                        Size labelSize = getTextSize(label, FONT_HERSHEY_SIMPLEX, 0.5, 2, &baseLine);
                        int top = max(out.boxes[i].y, labelSize.height);
                        putText(mat, label, Point(out.boxes[i].x, top), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0, 0, 250), 1);
                    }
                    
                    //把处理处理好的mat对象放在外部使用的队列中
                    bool push = outDrawQueue->PushBack(mat);
                    //如果队列推送失败，手工清理clone的mat对象
                    if (!push) {
                        mat.release();
                        mat = NULL;
                    }
                   
                    //cv::imshow("opencvwindows", mat);
                    //waitKey(10);
                }
            }
        }
    }

    return;
}

Mat AssistWorker::PopDrawMat() {
    Mat mat;
    outDrawQueue->PopFront(mat);

    //注意返回克隆对象，才能安全地传递图像数据
    Mat mat2 = mat.clone();

    //释放老mat
    mat.release();
    mat = NULL;

    return mat2;
}

void AssistWorker::Start()
{
    m_detectPauseFlag = false;
    m_detectCondition.notify_all();
    
    m_firePauseFlag = false;
    m_fireCondition.notify_all();

    m_movePauseFlag = false;
    m_moveCondition.notify_all();

    m_drawPauseFlag = false;
    m_drawCondition.notify_all();

    return;
}

void AssistWorker::Pause()
{
    m_detectPauseFlag = true;
    m_detectCondition.notify_all();

    m_firePauseFlag = true;
    m_fireCondition.notify_all();

    m_movePauseFlag = true;
    m_moveCondition.notify_all();

    m_drawPauseFlag = true;
    m_drawCondition.notify_all();

    return;
}
