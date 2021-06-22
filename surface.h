#ifndef SURFACE_H_INCLUDE
#define SURFACE_H_INCLUDE

#include "display.h"
#include "array.h"
#include "geometry.h"

class Surface {
public:
	static Rect globalRect() {
		return Rect{0, 0, WIDTH, HEIGHT};
	}

	struct Shape {
		using DrawFunc = void (*)(void *, Rect);

		void *data;
		DrawFunc drawFunc;
		Rect rect;
		bool isVisible;
		bool isSolid;

		void draw(Rect rect) {
			drawFunc(data, rect);
		}
	};

	/*
	struct RectShape {
		static void draw(void *data, Rect rect) {
			RectShape *r = static_cast<RectShape *>(data);
			r->rect.draw(rect);
		}

		RectShape(Color color) : color(color) {}

		void init() {
		}

		int shapeID;
		Color color;
	};
	*/

	Surface() {}

	void reset() {
		m_shapes.clear();
		m_invalidZones.clear();
		lcd.clrScr();
	}

	const Rect &rectForShape(int shapeID) const {
		return m_shapes[shapeID].rect;
	}

	void invalidateRect(Rect rect) {
		// TODO: Deduplicate invalid zones
		m_invalidZones.append(rect);
	}

	int createShape(Shape shape) {
		int id = m_shapes.size();
		m_shapes.append(shape);
		// TODO: Deduplicate invalid zones
		if (shape.isVisible)
			m_invalidZones.append(shape.rect);
		return id;
	}

	void moveShape(int shapeID, int dx, int dy) {
		Shape &shape = m_shapes[shapeID];
		Rect &rect = shape.rect;
		Rect newRect = rect;
		newRect.move(dx, dy);
		// TODO: Deduplicate invalid zones
		if (shape.isVisible) {
			if (shape.isSolid)
				Rect::getDifference(rect, newRect, m_invalidZones);
			else
				Rect::getUnion(rect, newRect, m_invalidZones);
		}
		rect = newRect;
	}

	void setShapePos(int shapeID, int x, int y) {
		const Rect &rect = m_shapes[shapeID].rect;
		moveShape(shapeID, x - rect.x1, y - rect.y1);
	}

	void setShapeVisibility(int shapeID, bool isVisible) {
		Shape &shape = m_shapes[shapeID];
		if (shape.isVisible != isVisible) {
			shape.isVisible = isVisible;
			// TODO: Deduplicate invalid zones
			m_invalidZones.append(shape.rect);
		}
	}

	void draw() {
		lcd.setColor(0, 0, 0);
		for (int i = 0; i < m_invalidZones.size(); ++i)
			m_invalidZones[i].draw();

		for (int i = 0; i < m_shapes.size(); ++i)
			for (int j = 0; j < m_invalidZones.size(); ++j)
				if (m_shapes[i].isVisible)
					m_shapes[i].draw(m_invalidZones[j]);

		m_invalidZones.clear();
	}

	void update() {
		invalidateRect(globalRect());
	}

	void clear() {
		m_shapes.clear();
		m_invalidZones.clear();
	}

private:
	Array<Shape, 256> m_shapes;
	Array<Rect, 256> m_invalidZones;
};

extern Surface surface;

#endif
