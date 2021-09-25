#pragma once
#include <exception>

template <typename T>
class CircularBuffer {
private:
	size_t size;
	mutable int begin_;
	mutable int end_;
	bool isEmpty;
	mutable T* data;
public:
	CircularBuffer(size_t size) : size(size), begin_(0), end_(0), isEmpty(true) {
		data = new T[size];
	}

	T first() {
		return data[begin_];
	}

	T last() {
		return data[end_];
	}

	T operator [] (int i) const {
		if (isEmpty || i > abs(begin_ - end_)) {
			throw std::exception("out of range");
		}
		return data[(begin_ + i) % size];
	}

	T& operator [] (int i) {
		if (isEmpty || i > abs(begin_ - end_)) {
			throw std::exception("out of range");
		}
		return data[(begin_ + i) % size];
	}

	void addFirst(T value) {
		if (isEmpty) {
			data[0] = value;
			isEmpty = false;
		}
		else {
			begin_ = (begin_ - 1 + size) % size;
			data[begin_] = value;
		}
	}

	void addLast(T value) {
		if (isEmpty) {
			data[0] = value;
			isEmpty = false;
		}
		else {
			end_ = (end_ + 1) % size;
			data[end_] = value;
		}
	}

	void delFirst() {
		if (begin_ == end_) {
			isEmpty = true;
			begin_ = 0;
			end_ = 0;
		}
		begin_ = (begin_ + 1) % size;
	}

	void delLast() {
		if (begin_ == end_) {
			isEmpty = true;
			begin_ = 0;
			end_ = 0;
		}
		end_ = (end_ - 1 + size) % size;
	}

	void changeCapacity(size_t new_size) {
		T* new_data = new T[new_size];
		for (size_t i = 0; i < size; ++i) {
			new_data[i] = data[i];
		}
		delete[] data;
		size = new_size;
		data = new_data;
	}

	T* begin() const {
		//begin_ = 0;
		return data;
	}

	T* end() const {
		//end_ = size - 1;
		return data + size;
	}
};

