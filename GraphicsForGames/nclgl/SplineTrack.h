#pragma once
#include <vector>
#include <iostream>
#include "Vector3.h"

class Spline {
public:
	Spline(void) {
		for (int i = 0; i < 4; ++i) {
			segments[i] = Vector3();
			points[i] = Vector3();
		}
	}

	Spline(Vector3 p0, Vector3 p1, Vector3 p2, Vector3 p3) {
		points[0] = p0;
		points[1] = p1;
		points[2] = p2;
		points[3] = p3;
		calcSegments(points[0], points[1], points[2], points[3]);
	}

	~Spline(void) {}

	float alpha = 0.5f;
	float tension = 0.0f;

	Vector3 segments[4];
	Vector3 points[4];

	void calcSegments(Vector3 p0, Vector3 p1, Vector3 p2, Vector3 p3) {
		float t0 = 0.0f;
		float t1 = t0 + pow(max(Vector3::Distance(p0, p1),0.00001f), alpha);
		float t2 = t1 + pow(max(Vector3::Distance(p1, p2), 0.00001f), alpha);
		float t3 = t2 + pow(max(Vector3::Distance(p2, p3), 0.00001f), alpha);

		Vector3 m1 = ((p1 - p0) / (t1 - t0) - (p2 - p0) / (t2 - t0) + (p2 - p1) / (t2 - t1)) * (1.0f - tension) * (t2 - t1);
		Vector3 m2 = ((p2 - p1) / (t2 - t1) - (p3 - p1) / (t3 - t1) + (p3 - p2) / (t3 - t2)) * (1.0f - tension) * (t2 - t1);

		segments[0] = (p1 - p2) * 2.0f + m1 + m2;
		segments[1] = (p1 - p2) * -3.0f - m1 -m1 - m2;
		segments[2] = m1;
		segments[3] = p1;
	}

	void setPoint(int i, Vector3 p) {
		points[i] = p;
		calcSegments(points[0], points[1], points[2], points[3]);
	}

	Vector3 getPoint(int i) {
		return points[i];
	}

	Vector3 pointAt(float t) {
		return (segments[0] * t * t * t) + (segments[1] * t * t) + (segments[2] * t) + segments[3];
	}
};

class SplineTrack {
public:

	SplineTrack() {};

	SplineTrack(Vector3 p0, Vector3 p1) {
		splineList.push_back(Spline(p0,p0,p1,p1));
		complete = false;
	}

	std::vector<Spline> splineList;

	~SplineTrack(void) {}

	void addPoint(Vector3 point) {
		if (complete) {
			splineList.pop_back();
			splineList.back().setPoint(3, point);
			splineList.push_back(Spline(splineList.back().getPoint(1), splineList.back().getPoint(2), point, splineList.front().getPoint(1)));
			splineList.front().setPoint(0, splineList.back().getPoint(2));
			splineList.push_back(Spline(splineList.back().getPoint(1), splineList.back().getPoint(2), splineList.front().getPoint(1), splineList.front().getPoint(2)));
		}
		else {
			splineList.back().setPoint(3, point);
			splineList.push_back(Spline(splineList.back().getPoint(1), splineList.back().getPoint(2), point, point));
		}
	}

	bool getComplete() {
		return complete;
	}

	void setComplete(bool c) {
		complete = c;
		
		Vector3 start = splineList.front().getPoint(1);
		Vector3 end = splineList.back().getPoint(2);

		if (complete) {
			splineList.back().setPoint(3, start);
			splineList.front().setPoint(0, end);
			splineList.push_back(Spline(splineList.back().getPoint(1), end, start, splineList.front().getPoint(2)));
		}
		else {
			splineList.pop_back();
			splineList.back().setPoint(3, splineList.back().getPoint(2));
			splineList.front().setPoint(0, start);
		}
	}

	Vector3 pointAt(float t) {
		while (t > 1.0f) {
			t -= 1.0f;
		}
		while (t < 0.0f) {
			t += 1.0f;
		}

		int index = 0;
		float splineLen = 1.0f / (float)splineList.size();
		while (t > splineLen) {
			t -= splineLen;
			index++;
		}
		return splineList[index].pointAt(t * (float)splineList.size());
	}
protected:
	bool complete;
};

