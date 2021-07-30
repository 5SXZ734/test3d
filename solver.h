#ifndef __SOLVER_H__
#define __SOLVER_H__

#include "vector.h"

namespace My {

	template <typename T>
	class SolverT
	{
		typename T	&m;
		typedef typename T::ValueType	ValueType;
	public:
		SolverT(T &_m) : m(_m){}
		void forwardEliminate(int steps = 0)
		{
			int i0(0);
			for (int j0(0); j0 < m.rows(); j0++)//for each diag position
			{
				if (m.at(i0, j0) != ValueType(0))
				{
					if (reduceRow(i0, m.at(i0, j0)))
						if (--steps == 0)
							return;
				}
				bool bModified(false);
				for (int i(i0 + 1); i < m.rows(); i++)
				{
					if (m.at(i, j0) != ValueType(0))
					{
						if (m.at(i0, j0) == ValueType(0))
						{
							swapRows(i0, i);
							if (--steps == 0)
								return;
							reduceRow(i0, m.at(i0, j0));
						}
						else
						{
							subtractRow(i, i0, m.at(i, j0));
						}
						bModified = true;
						if (--steps == 0)
							return;
					}
				}
				if (m.at(i0, j0) != ValueType(0))
					i0++;
				if (bModified && --steps == 0)
					break;
			}
		}

		void backwardEliminate(int steps = 0)
		{
			//int N(m_a.size());

			int i0(m.rows() - 1);
			for (int j0(m.rows()-1); j0 >= 0; j0--)//for each diag position
			{
				if (m.at(i0, j0) != ValueType(0))
				{
					if (reduceRow(i0, m.at(i0, j0)))
						if (--steps == 0)
							return;
				}
				bool bModified(false);
				for (int i(i0 - 1); i >= 0; i--)
				{
					if (m.at(i, j0) != ValueType(0))
					{
						if (m.at(i0, j0) == ValueType(0))
						{
							swapRows(i0, i);
							if (--steps == 0)
								return;
							reduceRow(i0, m.at(i0, j0));
						}
						else
						{
							subtractRow(i, i0, m.at(i, j0));
						}
						bModified = true;
						if (--steps == 0)
							return;
					}
				}
				if (m.at(i0, j0) != ValueType(0))
					i0--;
				if (bModified && --steps == 0)
					break;
			}
		}

		bool reduceRow(int i0, ValueType d)
		{
			assert(d != ValueType(0));
			if (d == ValueType(1))
				return false;
			int j2(m.cols());
			for (int j(0); j < j2; j++)
				m.at(i0, j) /= d;
			return true;
		}

		void subtractRow(int a0, int b0, ValueType k)//a=a-b*k
		{
			for (int j(0); j < m.cols(); j++)
				m.at(a0, j) = m.at(a0, j) - m.at(b0, j)*k;
		}

		void swapRows(int a, int b)
		{
			for (int j(0); j < m.cols(); j++)
				std::swap(m.at(a, j), m.at(b, j));
		}
	};

	template <typename MATRIX>
	MATRIX inverse(const MATRIX &m0)
	{
		typedef MatrixT<MATRIX::ValueType, MATRIX::Rows, MATRIX::Cols*2> AugMatrix;

		AugMatrix m;
		SubMatrix<AugMatrix, MATRIX::Rows, MATRIX::Cols> m1(m, Point2i(0, 0));//original - now is identity
		m1.assign(m0);

		SubMatrix<AugMatrix, MATRIX::Rows, MATRIX::Cols> m2(m, Point2i(MATRIX::Cols, 0));//reversed matrix
		m2.loadIdentity();

		SolverT<AugMatrix> S(m);
		S.forwardEliminate();
		S.backwardEliminate();

		MATRIX t;
		t.assign(m2);
		return t;
	}

}//namespace

#endif//__SOLVER_H__
