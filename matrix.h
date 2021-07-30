#ifndef __MATRIX_H__
#define __MATRIX_H__

#include "vector.h"

namespace My
{
	template<typename T> struct getValueType;

	template <typename T>
	class MatrixBaseT
	{
	public:
		typedef typename getValueType<T>::type	ValueType;
		MatrixBaseT(){}
		template <typename MATRIX>
		MatrixBaseT(const MATRIX &x)
		{
			assign(x);
		}
		
		void clear()
		{
			for (int i(0); i < T::Rows; i++)
				for (int j(0); j < T::Cols; j++)
					at(i, j) = ValueType(0);
		}
		void loadIdentity()
		{
			for (int i(0); i < T::Rows; i++)
				for (int j(0); j < T::Cols; j++)
					at(i, j) = (i == j)?ValueType(1):ValueType(0);
		}
		const ValueType &at(int i, int j) const
		{
			return static_cast<const T *>(this)->at(i, j);
		}
		typename ValueType &at(int i, int j)
		{
			return static_cast<T *>(this)->at(i, j);
		}
		//void operator = (const T &x)
		//{
		//}
		template <typename MATRIX>
		void assign(const MATRIX &x)
		{
			for (int i(0); i < T::Rows; i++)
				for (int j(0); j < T::Cols; j++)
					at(i, j) = x.at(i, j);
		}
		template <typename MATRIX1, typename MATRIX2>
		void multiply(const MATRIX1 &a, MATRIX2 &b)
		{
			assert(a.cols() == b.rows());
			clear();
			for (int j(0); j < (int)b.cols(); j++)
				for (int i(0); i < (int)a.rows(); i++)
					for (int k(0); k < (int)a.cols(); k++)
						at(i, j) += a.at(i, k) * b.at(k, j);
		}

		void print(std::ostream &os) const
		{
			os << "[\n";
			for (int i(0); i < T::Rows; i++)
			{
				os << "[";
				for (int j(0); j < T::Cols; j++)
				{
					if (j > 0)
						os << "\t";
					os << at(i, j);
				}
				os << "]\n";
			}
			os << "]\n";
		}
	};

	
	template <typename T, int ROWS, int COLS=ROWS>
	class MatrixT : public MatrixBaseT< MatrixT<T, ROWS, COLS> >
	{
	protected:
		T m[ROWS][COLS];
	public:
		typedef typename T	ValueType;
		static const int Rows = ROWS;
		static const int Cols = COLS;
		MatrixT(){}
		MatrixT(const MatrixT<T, ROWS - 1> &o)
		{
			loadIdentity();
			for (int i(0); i < o.size(); i++)
			{
				for (int j(0); j < o.size(); j++)
					m[i][j] = o[i][j];
				m[i][size() - 1] = o[i][o.size() - 1];
			}
			for (int j(0); j < o.size(); j++)
				m[size() - 1][j] = o[o.size() - 1][j];
			m[size() - 1][size() - 1] = o[o.size() - 1][o.size() - 1];
		}

		int size() const { assert(ROWS == COLS); return ROWS; }
		inline int rows() const { return ROWS; }
		inline int cols() const { return COLS; }
		const T &at(int i, int j) const { return m[i][j]; }
		T &at(int i, int j){ return m[i][j]; }
		/*void print(std::ostream &os) const
		{
			for (int i(0); i < ROWS; i++)
			{
				os << "[";
				for (int j(0); j < COLS; j++)
				{
					if (j > 0)
						os << ",";
					os << m[i][j];
				}
				os << "]";
			}
		}*/
		MatrixT operator *(const MatrixT &x) const
		{
			const MatrixT &m(*this);
			MatrixT t;
			t.clear();
			for (int j(0); j < COLS; j++)
			{
				for (int i(0); i < ROWS; i++)
				{
					for (int k(0); k < COLS; k++)
					{
						t[i][j] += m[i][k] * x[k][j];
					}
				}
			}
			return t;
		}
		PointT<T, ROWS> operator *(const PointT<T, ROWS> &x) const
		{
			PointT<T, ROWS> t(0);
			for (int i(0); i < ROWS; i++)
				for (int j(0); j < COLS; j++)
					t[i] += m[i][j] * x[j];
			return t;
		}
		VectorT<T, ROWS> operator *(const VectorT<T, ROWS> &x) const
		{
			VectorT<T, ROWS> t(0);
			for (int i(0); i < ROWS; i++)
				for (int j(0); j < COLS; j++)
					t[i] += m[i][j] * x[j];
			return t;
		}
		T (&operator [](const int i))[ROWS]{ return m[i]; }
		const T (&operator [](const int i)const)[ROWS]{ return m[i]; }
		void translate(VectorT<T, ROWS> dv)
		{
			MatrixT &m(*this);
			for (int i(0); i < size() - 1; i++)
				m[i][size() - 1] += dv[i];
		}
		void translate(T dx, T dy, T dz)
		{
			MatrixT &m(*this);
			MatrixT t;
			t.loadIdentity();
			t.translate(VectorT<T, ROWS>(dx, dy, dz, 0));
			m = t*m;
		}
		void translate(T dx, T dy)
		{
			MatrixT &m(*this);
			MatrixT t;
			t.loadIdentity();
			t.translate(VectorT<T, ROWS>(dx, dy, 0));
			m = t*m;
		}
		void rotate(T deg, int axis)
		{
			loadIdentity();
			T cs(cos(DEG2RAD(deg)));
			T sn(sin(DEG2RAD(deg)));
			if (axis == 0)//x
			{
				m[1][1] = cs;
				m[1][2] = -sn;
				m[2][1] = sn;
				m[2][2] = cs;
			}
			else if (axis == 1)//y
			{
				m[0][0] = cs;
				m[0][2] = sn;
				m[2][0] = -sn;
				m[2][2] = cs;
			}
			else
			{
				m[0][0] = cs;
				m[0][1] = -sn;
				m[1][0] = sn;
				m[1][1] = cs;
			}
		}
		void rotate(float deg)
		{
			MatrixT &m(*this);
			VectorT<T, ROWS> v(m.col(ROWS - 1));
			v[ROWS-1] = 0;
			m.translate(-v);
			MatrixT m2;
			m2.rotate(deg, 2);
			m = m*m2;
			m.translate(v);
		}
		void scale(VectorT<T, ROWS> v)
		{
			loadIdentity();
			m[0][0] = v[0];
			m[1][1] = v[1];
			m[2][2] = v[2];
		}
		void scale(T dx, T dy, T dz)
		{
			MatrixT &m(*this);
			MatrixT t;
			t.scale(VectorT<T, ROWS>(dx, dy, dz));
			m = t*m;
		}
		VectorT<T, COLS> row(int i)
		{
			VectorT<T, COLS> t;
			for (int j(0); j < COLS; j++)
				t[j] = m[i][j];
			return t;
		}
		VectorT<T, ROWS> col(int j)
		{
			VectorT<T, ROWS> t;
			for (int i(0); i < ROWS; i++)
				t[i] = m[i][j];
			return t;
		}
	};

	template<typename T, int ROWS, int COLS> 
	struct getValueType<MatrixT<T, ROWS, COLS> >
	{ typedef T type; };

	typedef MatrixT<float, 3>	Mat3f;
	
	template <typename MATRIX, int ROWS, int COLS=ROWS>
	class SubMatrix : public MatrixBaseT< SubMatrix<MATRIX, ROWS, COLS> >
	{
		Point2i		m_start;
		//Size<int>	m_size;
		MATRIX &m_x;
	public:
		//typedef typename MatrixBaseT< SubMatrix<T, ROWS, COLS> >	Base;
		static const int Rows = ROWS;
		static const int Cols = COLS;
		typedef typename MATRIX::ValueType	ValueType;
		SubMatrix(MATRIX &x, const Point2i &start)//, const Size<int> &size)
			: m_x(x),
			m_start(start)//,
			//m_size(size)
		{
		}
		//unsigned rows() const { return m_size.height(); }
		//unsigned cols() const { return m_size.width(); }
		const ValueType &at(int i, int j) const { return m_x.at(m_start[1] + i, m_start[0] + j); }
		ValueType &at(int i, int j){ return m_x.at(m_start[1] + i, m_start[0] + j); }

/*		SubMatrix & operator=(const Base &o)
		{
			return *this;
		}*/
	};

	template<typename MATRIX, int ROWS, int COLS> 
	struct getValueType<SubMatrix<MATRIX, ROWS, COLS> >
	{ typedef typename MATRIX::ValueType type; };



}//namespace My

#endif//__MATRIX_H__
