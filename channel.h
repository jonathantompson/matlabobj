#pragma once

#include <vec2.h>
#include <vec3.h>
#include <vec4.h>

#include "tri.h"

#include <string>
#include <vector>

namespace Geometry {

	class Mesh;

	class Channel {

		const std::string name;
		const Mesh* const owner;

	public:

		Channel(const std::string& name, const Mesh* const owner) : name(name), owner(owner) {
		}
		
		virtual ~Channel() {
		}

		const std::string& getName() const {
			return name;
		}

		virtual int getSize() const = 0;
		virtual int getMemoryUsage() const = 0;
		virtual std::string convertToString() const = 0;
		const Mesh* getOwner() const {
			return owner;
		}
	};

	template<typename T> class BaseChannel : public Channel {

	private:

		std::vector<T> values;

	public:

		BaseChannel(const std::string& name, const Mesh* const owner) : Channel(name, owner) {
		}

		void reserve(const int size) {
			values.resize(size);
		}

		virtual int getSize() const override {
			return int(values.size());
		}

		std::vector<T>& getValues() {
			return values;
		}

		const std::vector<T>& getValues() const {
			return values;
		}

		const T* getData() const {
			return values.size() > 0 ? &values[0] : NULL;
		}
		
		void add(const T& t) {
			values.push_back(t);
		}
		
		T& getAt(const int index) {
			return values[index];
		}

		const T& getAt(const int index) const {
			return values[index];
		}
/*
		T& operator[](const int index) {
			return getAt(index);
		}

		const T& operator[](const int index) const {
			return getAt(index);
		}
*/		
		void setAt(const int index, const T& t) {
			values[index] = t;
		}

		int getMemoryUsage() const {
			return sizeof(T) * int(values.size());
		}

		virtual std::string convertToString() const override {
			return "";
		}
	};

	typedef BaseChannel<int> IntChannel;
	typedef BaseChannel<float> FloatChannel;
// 	typedef BaseChannel<Vec2h> Vec2hChannel;
	typedef BaseChannel<Vec2f> Vec2fChannel;
// 	typedef BaseChannel<Vec3h> Vec3hChannel;
	typedef BaseChannel<Vec3f> Vec3fChannel;	
	typedef BaseChannel<Vec4f> Vec4fChannel;
// 	typedef BaseChannel<Vec4h> Vec4hChannel;

	typedef BaseChannel<Edge> EdgeChannel;
	typedef BaseChannel<Tri> TriChannel;
	typedef BaseChannel<Tetra> TetraChannel;

	// A channel that maps 1:1 to a complex.
	// A single value per-vertex, per-edge, per-face or per-tertra.
	// Use all the BaseChanels<> as realizations of this.
	class FlatChannel : public Channel {

	public:

		FlatChannel(const std::string& name, const Mesh* const owner) : Channel(name, owner) {
		}

		virtual int getSize() const override { 
			return 0;
		}

		virtual int getMemoryUsage() const override {
			return 0;
		}

		virtual std::string convertToString() const override {
			return "";
		}
	};

	typedef FlatChannel FlatTriChannel;
	typedef FlatChannel FlatEdgeChannel;
	typedef FlatChannel FlatTetraChannel;
};
