#ifndef SURFACE_H_INCLUDE
#define SURFACE_H_INCLUDE

#include "display.h"
#include "array.h"
#include "geometry.h"

class Surface {
public:
	Surface() {}

	void reset() {
		m_shapes.clear();
		m_invalidZones.clear();
		lcd.clrScr();
	}

	const Rect &rectForShape(int shapeID) const {
		return m_shapes[shapeID];
	}

	int createRect(int x1, int y1, int x2, int y2, bool visible) {
		int id = m_shapes.size();
		Rect r = Rect{x1, y1, x2, y2, visible};
		m_shapes.append(r);
		// TODO: Deduplicate invalid zones
		if (visible)
			m_invalidZones.append(r);
		return id;
	}

	void moveShape(int shapeID, int x, int y) {
		Rect &rect = m_shapes[shapeID];
		Rect newRect = rect;
		newRect.move(x, y);
		// TODO: Deduplicate invalid zones
		if (rect.visible)
			Rect::getDifference(rect, newRect, m_invalidZones);
		rect = newRect;
	}

	void setRectPos(int rectID, int x, int y) {
		Rect &rect = m_shapes[rectID];
		moveShape(rectID, x - rect.x1, y - rect.y1);
	}

	void setShapeVisibility(int shapeID, bool visible) {
		Rect &rect = m_shapes[shapeID];
		if (rect.visible != visible) {
			rect.visible = visible;
			// TODO: Deduplicate invalid zones
			m_invalidZones.append(rect);
		}
	}

	void draw() {
		lcd.setColor(0, 0, 0);
		for (int i = 0; i < m_invalidZones.size(); ++i)
			m_invalidZones[i].draw();

		lcd.setColor(255, 0, 0);
		for (int i = 0; i < m_shapes.size(); ++i)
			for (int j = 0; j < m_invalidZones.size(); ++j)
				if (m_shapes[i].visible)
					m_shapes[i].draw(m_invalidZones[j]);

		m_invalidZones.clear();
	}

private:
	Array<Rect, 256> m_shapes;
	Array<Rect, 256> m_invalidZones;
};

extern Surface surface;

#endif
