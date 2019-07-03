#include "MapDrawer.h"
#include <thread>
void ThreadMapDrawer::draw(){
	while (1) {
		wait();
		mut1.lock();
		for (int i = 0; i < lorder.size(); i++)	drawrealline(lorder[i].id, lorder[i].pt.x, lorder[i].pt.y, ltypes::LINE_AND_DOT);
		lorder.clear();
		mut1.unlock();
	}
}

void ThreadMapDrawer::notify(){
	std::lock_guard<std::mutex> lock(mut2);
	++ct;
	condition.notify_one();
}

void ThreadMapDrawer::wait() {
	std::unique_lock<std::mutex> lock(mut2);
	while (!ct) // Handle spurious wake-ups.
		condition.wait(lock);
	--ct;
}

ThreadMapDrawer::ThreadMapDrawer(cv::Mat & src, cv::Point2f toplpos, cv::Point2f botrpos) : MapDrawer(src, toplpos, botrpos)
{
	std::thread th(&ThreadMapDrawer::draw, this);
	th.detach();
}

linf ThreadMapDrawer::addineT(cv::Scalar col, int thk, cv::Point2f fpos)
{
	return addline(col, thk, fpos);
}

void ThreadMapDrawer::addlistdraw(linf line, float xc, float yc, int type){
	struct tnf t; t.pt = cv::Point2f(xc, yc);
	t.id = line;
	mut1.lock();
	lorder.push_back(t);
	mut1.unlock();
}

void ThreadMapDrawer::drawnow(){
	notify();
}

cv::Mat ThreadMapDrawer::getimg()
{
	mut1.lock();
	cv::Mat  t = _getimg();
	mut1.unlock();
	return t;
}
