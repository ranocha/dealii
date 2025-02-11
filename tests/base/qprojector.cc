// ------------------------------------------------------------------------
//
// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright (C) 2005 - 2024 by the deal.II authors
//
// This file is part of the deal.II library.
//
// Part of the source code is dual licensed under Apache-2.0 WITH
// LLVM-exception OR LGPL-2.1-or-later. Detailed license information
// governing the source code and code contributions can be found in
// LICENSE.md and CONTRIBUTING.md at the top level directory of deal.II.
//
// ------------------------------------------------------------------------


// Test projection onto lines

#include <deal.II/base/geometry_info.h>
#include <deal.II/base/qprojector.h>
#include <deal.II/base/quadrature_lib.h>

#include <deal.II/grid/reference_cell.h>

#include "../tests.h"

template <int dim>
void
check_line(Quadrature<1> &quadrature)
{
  Point<dim> p1;
  Point<dim> p2;
  p1[0] = 1.;
  p2[0] = 7.;
  if (dim > 1)
    {
      p1[1] = 3;
      p2[1] = -5.;
    }
  if (dim > 2)
    {
      p1[2] = 0;
      p2[2] = 10.;
    }
  Quadrature<dim> q = QProjector<dim>::project_to_line(
    ReferenceCells::get_hypercube<dim>(), quadrature, p1, p2);
  double s = 0.;

  for (unsigned int k = 0; k < q.size(); ++k)
    {
      deallog << k << '\t' << q.point(k) << std::endl;
      s += q.weight(k);
    }
  deallog << "length: " << s << std::endl;
}

template <int dim>
void
check_face(Quadrature<1> &q1)
{
  deallog << "Checking dim " << dim << " 1d-points " << q1.size() << std::endl;

  Quadrature<dim - 1> subquadrature(q1);
  for (const unsigned int f : GeometryInfo<dim>::face_indices())
    {
      deallog << "Face " << f << std::endl;

      Quadrature<dim> quadrature =
        QProjector<dim>::project_to_face(ReferenceCells::get_hypercube<dim>(),
                                         subquadrature,
                                         f);
      for (unsigned int k = 0; k < quadrature.size(); ++k)
        deallog << quadrature.point(k) << std::endl;
    }

  for (const unsigned int f : GeometryInfo<dim>::face_indices())
    for (unsigned int s = 0; s < GeometryInfo<dim>::max_children_per_face; ++s)
      {
        deallog << "Face " << f << " subface " << s << std::endl;

        Quadrature<dim> quadrature =
          QProjector<dim>::project_to_face(ReferenceCells::get_hypercube<dim>(),
                                           subquadrature,
                                           f);
        for (unsigned int k = 0; k < quadrature.size(); ++k)
          deallog << quadrature.point(k) << std::endl;
      }
}

template <int dim>
void
check_faces(Quadrature<1> &q1)
{
  const unsigned int nq = q1.size();

  deallog << "Checking dim " << dim << " 1d-points " << nq << std::endl;


  Quadrature<dim - 1> subquadrature(q1);
  const unsigned int  nqs = subquadrature.size();

  Quadrature<dim> faces =
    QProjector<dim>::project_to_all_faces(ReferenceCells::get_hypercube<dim>(),
                                          subquadrature);

  for (const unsigned int f : GeometryInfo<dim>::face_indices())
    {
      deallog << "Face " << f << " orientation false" << std::endl;

      unsigned int offset = QProjector<dim>::DataSetDescriptor::face(
        ReferenceCells::get_hypercube<dim>(), f, false, false, false, nqs);

      for (unsigned int k = 0; k < nqs; ++k)
        deallog << faces.point(offset + k) << std::endl;

      deallog << "Face " << f << " orientation true" << std::endl;

      offset = QProjector<dim>::DataSetDescriptor::face(
        ReferenceCells::get_hypercube<dim>(), f, true, false, false, nqs);

      for (unsigned int k = 0; k < nqs; ++k)
        deallog << faces.point(offset + k) << std::endl;
    }

  /*
  Quadrature<dim> subs =
  QProjector<dim>::project_to_all_subfaces(subquadrature);


  for (unsigned int f=0;f<GeometryInfo<dim>::faces_per_cell;++f)
  for (unsigned int s=0;s<GeometryInfo<dim>::max_children_per_face;++s)
  {

  deallog << "Face " << f << " subface " << s
  << " orientation false"
  << std::endl;

  unsigned int
  offset = QProjector<dim>::DataSetDescriptor::subface(f, s, false, nqs);

  for (unsigned int k=0;k<nqs;++k)
  deallog << faces.point(offset+k) << std::endl;

  deallog << "Face " << f << " subface " << s
  << " orientation true"
  << std::endl;

  offset = QProjector<dim>::DataSetDescriptor::subface(f, s, true, nqs);

  for (unsigned int k=0;k<nqs;++k)
  deallog << faces.point(offset+k) << std::endl;
  }
  */
}


void
check(Quadrature<1> &q)
{
  deallog << std::endl;
  deallog.push("line");
  check_line<1>(q);
  check_line<2>(q);
  check_line<3>(q);
  deallog.pop();
  deallog.push("face");
  check_face<1>(q);
  check_face<2>(q);
  check_face<3>(q);
  deallog.pop();
  deallog.push("all");
  check_faces<2>(q);
  check_faces<3>(q);
  deallog.pop();
}

int
main()
{
  initlog();
  deallog << std::setprecision(2);


  Quadrature<1> none(0);
  check(none);

  QGauss<1> midpoint(1);
  check(midpoint);

  QTrapezoid<1> trapez;
  check(trapez);

  QSimpson<1> simpson;
  check(simpson);

  QMilne<1> milne;
  check(milne);
}
