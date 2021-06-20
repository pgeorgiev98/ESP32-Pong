#ifndef WIFI_H_INCLUDED
#define WIFI_H_INCLUDED

#include "array.h"
#include "gui.h"
#include "surface.h"

namespace Wifi {
}

namespace GUI {

	class ConnectToWifiPage : public Page {
	public:
		void init() override {
			surface.reset();
		}

		/*
		void onPress(Point p) override {
		}

		void onRelease() override {
		}

		void onDrag(Point p) override {
		}
		*/

	private:
		int m_networkCount;
	};

}

#endif
