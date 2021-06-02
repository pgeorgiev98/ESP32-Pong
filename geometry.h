#ifndef GEOMETRY_H_INCLUDED
#define GEOMETRY_H_INCLUDED

struct Point {
	int x, y;

	int distanceSquared(const Point &other) const {
		int dx = x - other.x;
		int dy = y - other.y;
		return dx * dx + dy * dy;
	}
};

struct Rect {
	// TODO: These ints can be 16 bit to save memory
	int x1, y1, x2, y2;
	bool visible;

	int width() const {
		return x2 - x1;
	}

	int height() const {
		return y2 - y1;
	}

	bool operator==(const Rect &r) const {
		return x1 == r.x1 && y1 == r.y1 && x2 == r.x2 && y2 == r.y2;
	}

	bool operator!=(const Rect &r) const {
		return !(*this == r);
	}

	void move(int dx, int dy) {
		x1 += dx; x2 += dx;
		y1 += dy; y2 += dy;
	}

	void draw() const {
		lcd.fillRect(x1, y1, x2, y2);
	}

	void draw(const Rect &area) const {
		Rect toDraw;
		if (Rect::getIntersection(*this, area, toDraw))
			toDraw.draw();
	}

	bool intersects(const Rect &r) const {
		return !(x1 >= r.x2 || r.x1 >= x2 || y1 >= r.y2 || r.y1 >= y2);
	}

	template<typename Array>
	static bool getDifference(const Rect &r1, const Rect &r2, Array &outArray) {
		if (!r1.intersects(r2)) {
			outArray.append(r1);
			outArray.append(r2);
			return false;
		}

		if (r1.x1 != r2.x1) { // Left
			if (r1.x1 <= r2.x1)
				outArray.append(Rect{r1.x1, r1.y1, r2.x1, r1.y2, false});
			else
				outArray.append(Rect{r2.x1, r2.y1, r1.x1, r2.y2, false});
		}

		if (r1.x2 != r2.x2) { // Right
			if (r1.x2 <= r2.x2)
				outArray.append(Rect{r1.x2, r2.y1, r2.x2, r2.y2, false});
			else
				outArray.append(Rect{r2.x2, r1.y1, r1.x2, r1.y2, false});
		}

		int left = max(r1.x1, r2.x1);
		int right = min(r1.x2, r2.x2);

		if (r1.y1 != r2.y1) // Bottom
			outArray.append(Rect{left, min(r1.y1, r2.y1), right, max(r1.y1, r2.y1), false});

		if (r1.y2 != r2.y2) // Top
			outArray.append(Rect{left, min(r1.y2, r2.y2), right, max(r1.y2, r2.y2), false});

		return true;
	}

	static bool getIntersection(const Rect &r1, const Rect &r2, Rect &outRect) {
		if (r1.intersects(r2)) {
			outRect = Rect{
				max(r1.x1, r2.x1),
				max(r1.y1, r2.y1),
				min(r1.x2, r2.x2),
				min(r1.y2, r2.y2),
				false
			};
			return true;
		} else {
			return false;
		}
	}

	template<typename Array>
	static void getUnion(const Rect &r1, const Rect &r2, Array &outArray) {
		// A little hack: the union is actually the difference + the intersection
		// TODO: Here we have a duplicate call to intersects()
		getDifference(r1, r2, outArray);
		Rect r;
		if (getIntersection(r1, r2, r))
			outArray.append(r);
	}
};


#endif
