#include <opencv2/opencv.hpp>
#include <iostream>

int main()
{
    //const std::string video1 = "C:\\Users\\Mr.HONG\\Desktop\\자료모음\\video1.mp4";
    //cv::VideoCapture cap(video1);
    cv::VideoCapture cap(0);  //웹캠 쓰려면 주석풀기


    if (!cap.isOpened()) {
        std::cerr << "ERROR: 영상 파일을 열 수 없습니다.\n";
        return 1;
    }

    cv::Mat frame, gray, blurred;
    cv::Mat prev_gray; // 이전 프레임을 저장할 변수

    while (true) {
        if (!cap.read(frame) || frame.empty()) break;

        // 전처리 (흑백 + 블러) -- 컬러있으면 계산량 큼
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
        cv::GaussianBlur(gray, blurred, cv::Size(21, 21), 0);

        // 첫 프레임일 경우 이전 프레임 변수를 초기화하고 다음으로 넘어감
        if (prev_gray.empty()) {
            prev_gray = blurred.clone();
            continue;
        }

        // 프레임 차분
        cv::Mat diff, thresh;
        // 두 프레임의 차이 계산 (절대값 차이)
        cv::absdiff(prev_gray, blurred, diff);

        // 임계값 처리: 차이가 25 이상인 픽셀만 흰색(255)으로 만듦
        cv::threshold(diff, thresh, 25, 255, cv::THRESH_BINARY);

        // 팽창 연산: 작은 구멍을 메워 덩어리화
        cv::dilate(thresh, thresh, cv::Mat(), cv::Point(-1, -1), 2);

        // 결과 확인
        cv::imshow("Original", frame);
        cv::imshow("Motion Mask (Step 3)", thresh);

        // [핵심] 현재 프레임을 다음 루프를 위한 '이전 프레임'으로 업데이트
        prev_gray = blurred.clone();

        const char key = (char)cv::waitKey(30);
        if (key == 'q' || key == 27) break;
    }

    cap.release();
    cv::destroyAllWindows();
    return 0;
}

