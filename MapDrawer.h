#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <nlohmann/json.hpp>
#include <mutex>
#include <condition_variable>

using namespace std;

typedef struct lparam {
	cv::Scalar col;
	int thk;
	cv::Point pos;
}lanepar;

typedef struct linenf {
	int idline;
}linf;

enum ltypes {
	STRAIGHT_LINE, LINE_AND_DOT
};

class _Mdraw {
protected:
	vector<lparam> vimg;
public:
	cv::Mat img;// default image
	_Mdraw(const cv::Mat& srs);
	linf addlane(cv::Scalar col, int thk, cv::Point fpos);
	cv::Mat drawline(linf line, cv::Point npos, int type);
};

class MapDrawer : private _Mdraw {
	cv::Point2f tlpos, brpos;
	float xdelc, ydelc;
public:
	cv::Mat _getimg();
	MapDrawer(cv::Mat& src, cv::Point2f toplpos, cv::Point2f botrpos);
	linf addline(cv::Scalar col, int thk, cv::Point2f fpos);
	cv::Mat drawrealline(linf line, float xc, float yc, int type);
};

struct tnf{
	linf id;
	cv::Point2f pt;
};

class ThreadMapDrawer : public MapDrawer {
	void draw();
	mutex mut1;
	mutex mut2;
	condition_variable condition;
	std::vector<tnf> lorder;
	unsigned int ct = 0;
	void notify();
	void wait();
public:
	ThreadMapDrawer(cv::Mat& src, cv::Point2f toplpos, cv::Point2f botrpos);
	linf addineT(cv::Scalar col, int thk, cv::Point2f fpos);
	void addlistdraw(linf line, float xc, float yc, int type);
	void drawnow();
	cv::Mat getimg();
};

