#pragma once

namespace Geometry {
	
	template<int dimension> class Simplex {

	public:

		int indices[dimension];

		Simplex() {
		}

		explicit Simplex(const int* const indices) {
			for( int i = 0; i < dimension; i++) {
				this->indices[i] = indices[i];
			}			
		}

		const int& operator[](const int index) const {
			assert(index < dimension);
			assert(index >= 0);
			return indices[index];
		}
		
		int& operator[](const int index) {
			assert(index < dimension);
			assert(index >= 0);
			return indices[index];
		}
	};

	class Tri : public Simplex<3> {
	
	public:

		Tri(const int index0 = 0, const int index1 = 0, const int index2 = 0) {
			indices[0] = index0;
			indices[1] = index1;
			indices[2] = index2;
		}

		explicit Tri(const int* const indices) : Simplex<3>(indices) {
		}
	};
	
	// Specialize for constructors
	typedef Simplex<1> Point;
	typedef Simplex<2> Edge;	
	typedef Simplex<4> Tetra;
};
