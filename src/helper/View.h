#ifndef MB_VIEW_H
#define MB_VIEW_H

#include <cstdint>

namespace mb {
namespace helpers {

inline void printPackage(const uint8_t* package, int length) {
	for (int i = 0; i < length; i++) {
		printf("<%.2X>", package[i]);
	}
}

inline void printPackage(const char* package, int length) {
	for (int i = 0; i < length; i++) {
		printf("<%.2X>", package[i]);
	}
}

} // helpers
} // mb

#endif // MB_VIEW_H