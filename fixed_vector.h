#pragma once

	template<typename T, const int MaxSize> class FixedVector {

	private:

		T ts[MaxSize];
		int currentSize;

	public:

		FixedVector() {
			currentSize = 0;
		}

		void push_back(const T& t) {
			ts[currentSize++] = t;
		}

		const T& operator[](const int index) const {
			return ts[index];
		}

		int size() const {
			return currentSize;
		}
	};