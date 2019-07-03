#include "MapDrawer.h"
#include <fstream>

using namespace cv;
using namespace std;

_Mdraw::_Mdraw(const cv::Mat& srs) {
	img = srs.clone();
}

linf _Mdraw::addlane(cv::Scalar col, int thk, cv::Point fpos)
{
	linf t;
	t.idline = vimg.size();
	lparam l;
	l.col = col;
	l.pos = fpos;
	l.thk = thk;
	vimg.push_back(l);
	return t;
}

cv::Mat  _Mdraw::drawline(linf line, cv::Point npos, int type)
{
	cv::line(img, vimg[line.idline].pos, npos, vimg[line.idline].col, vimg[line.idline].thk);
	vimg[line.idline].pos = npos;
	if (type = ltypes::LINE_AND_DOT) cv::circle(img, npos, 5, cv::Scalar(0, 0, 255), -1);
	return img.clone();
}

cv::Mat MapDrawer::_getimg(){
	return img;
}

MapDrawer::MapDrawer(cv::Mat & src, cv::Point2f toplpos, cv::Point2f botrpos) : _Mdraw(src) {
	tlpos = toplpos;
	brpos = botrpos;
	xdelc = (brpos.x - tlpos.x) / src.cols;
	ydelc = (tlpos.y - brpos.y) / src.rows;
}

linf MapDrawer::addline(cv::Scalar col, int thk, cv::Point2f _fpos)
{
	cv::Point fpos((_fpos.x - tlpos.x) / xdelc, (tlpos.y - _fpos.y) / ydelc);
	return addlane(col, thk, fpos);
}

cv::Mat MapDrawer::drawrealline(linf line, float xc, float yc, int type)
{
	int x = (int)((xc - tlpos.x) / xdelc);
	int y = (int)((tlpos.y - yc) / ydelc);
	return drawline(line, cv::Point(x, y), type);
}

int main() {
	using json = nlohmann::json;

	float xl, yl, xr, yr;
	string defimgname, text;
	
	std::ifstream ifs("config.json");
	json j = json::parse(ifs);
	defimgname = j.at("defimgname").get<string>();
	text = j.at("text").get<string>();
	xl = j.at("xl").get<float>();
	yl = j.at("yl").get<float>();
	xr = j.at("xr").get<float>();
	yr = j.at("yr").get<float>();
	
	Mat image = imread(defimgname);
	ThreadMapDrawer m(image, cv::Point2f(xl, yl), cv::Point2f(xr, yr));
	ifstream f(text);
	double x, y; unsigned long long t1, t;
	f >> x >> y >> t;
	linf line = m.addline(Scalar(255, 0, 0), 2, cv::Point2f(y, x));
	while (!f.eof()) {
		f >> x >> y >> t1;
		m.addlistdraw(line, y, x, ltypes::LINE_AND_DOT);
		m.drawnow();
		imshow("maps", m.getimg());
		waitKey(t1 - t);
		t = t1;
	}
	waitKey(0);
	return(0);
}